#include "GeometryUtils.h"
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cstdlib>

#include "../Renderer.h"

namespace GeometryUtils {

float length2(const glm::vec2& a, const glm::vec2& b) {
   glm::vec2 temp = a - b;
   return glm::dot(temp, temp);
}

bool findPolygonUnion(const std::vector<std::vector<glm::vec2>>& polygons, PolyTreeD& output) {
   ClipperD clipper;
   for (const auto& polygon : polygons) {
      if (!polygon.empty()) {
         PathsD subject;
         PathD  path;
         path.reserve(polygon.size());
         for (const auto& point : polygon) {
            path.emplace_back(static_cast<double>(point.x), static_cast<double>(point.y));
         }
         subject.push_back(path);
         clipper.AddSubject(subject);
      }
   }
   return clipper.Execute(ClipType::Union, FillRule::Positive, output);
}

PathsD FlattenPolyPathD(const PolyPathD& polyPath) {
   PathsD paths;

   // Lambda function for recursive traversal
   std::function<void(const PolyPathD&)> traverse = [&](const PolyPathD& node) {
      // Add the current node's polygon to the paths
      paths.emplace_back(node.Polygon());

      // Recursively traverse each child
      for (auto it = node.begin(); it != node.end(); ++it) {
         if (*it) { // Check if the child exists
            traverse(*(*it));
         }
      }
   };

   // Start traversal from the root node
   traverse(polyPath);

   paths = SimplifyPaths(paths, 0.025);

   return paths;
}


// Helper function to compute intersection between a ray and a segment
std::optional<glm::vec2> RaySegmentIntersect(const glm::vec2& ray_origin, double dx, double dy, const glm::vec2& a,
                                             const glm::vec2& b) {
   double r_px = ray_origin.x;
   double r_py = ray_origin.y;
   double r_dx = dx;
   double r_dy = dy;

   double s_px = a.x;
   double s_py = a.y;
   double s_dx = b.x - a.x;
   double s_dy = b.y - a.y;

   double denominator = r_dx * s_dy - r_dy * s_dx;

   if (std::fabs(denominator) < 1e-10) {
      // Lines are parallel
      return std::nullopt;
   }

   double t = ((s_px - r_px) * s_dy - (s_py - r_py) * s_dx) / denominator;
   double u = ((s_px - r_px) * r_dy - (s_py - r_py) * r_dx) / denominator;

   if (t >= 0 && u >= 0 && u <= 1) {
      // Intersection point
      glm::vec2 intersection_point;
      intersection_point.x = r_px + t * r_dx;
      intersection_point.y = r_py + t * r_dy;
      return intersection_point;
   } else {
      return std::nullopt;
   }
}

float distancePointToLineSegment(const glm::vec2& point, const glm::vec2& lineStart, const glm::vec2& lineEnd) {
   glm::vec2 line         = lineEnd - lineStart;
   float     lineLengthSq = glm::dot(line, line);

   if (lineLengthSq == 0.0f) {
      return glm::distance(point, lineStart);
   }

   float     t          = std::max(0.0f, std::min(1.0f, glm::dot(point - lineStart, line) / lineLengthSq));
   glm::vec2 projection = lineStart + t * line;

   return glm::distance(point, projection);
}

std::optional<glm::vec2> RayIntersect(const glm::vec2& ray_origin, double dx, double dy, const PathsD& scene) {
   float                    closest_distance = std::numeric_limits<float>::max();
   std::optional<glm::vec2> closest_intersection;
   for (const auto& polygon : scene) {
      for (size_t i = 0; i < polygon.size(); ++i) {
         const auto& pa = polygon[i];
         const auto& pb = polygon[(i + 1) % polygon.size()];

         auto intersection_opt = RaySegmentIntersect(ray_origin, dx, dy, {pa.x, pa.y}, {pb.x, pb.y});
         if (intersection_opt) {
            auto current_distance = length2(*intersection_opt, ray_origin);
            if (current_distance > 0.01) {
               if (current_distance < closest_distance) {
                  closest_distance     = current_distance;
                  closest_intersection = intersection_opt;
                  std::cout << "closest intersection: " << closest_intersection->x << ", " << closest_intersection->y
                            << " with length2: " << current_distance << std::endl;
               }
            }
         }
      }
   }
   std::cout << "returning closest intersection: " << closest_intersection->x << ", " << closest_intersection->y
             << " with length2: " << closest_distance << std::endl;
   return closest_intersection;
}

// Function to compute intersection between two line segments
std::optional<glm::vec2> LineSegmentIntersect(const glm::vec2& line1_start, const glm::vec2& line1_end,
                                              const glm::vec2& line2_start, const glm::vec2& line2_end) {
   double x1 = line1_start.x;
   double y1 = line1_start.y;
   double x2 = line1_end.x;
   double y2 = line1_end.y;

   double x3 = line2_start.x;
   double y3 = line2_start.y;
   double x4 = line2_end.x;
   double y4 = line2_end.y;

   double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

   if (std::fabs(denominator) < 1e-10) {
      // Lines are parallel
      return std::nullopt;
   }

   double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
   double u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / denominator;

   if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
      // Intersection point
      glm::vec2 intersection_point;
      intersection_point.x = x1 + t * (x2 - x1);
      intersection_point.y = y1 + t * (y2 - y1);
      return intersection_point;
   } else {
      return std::nullopt;
   }
}


std::optional<glm::vec2> LineIntersect(const glm::vec2& line1_start, const glm::vec2& line1_end, const PathsD& scene) {
   float                    closest_distance = std::numeric_limits<float>::max();
   std::optional<glm::vec2> closest_intersection;
   for (const auto& polygon : scene) {
      for (size_t i = 0; i < polygon.size(); ++i) {
         const auto& pa = polygon[i];
         const auto& pb = polygon[(i + 1) % polygon.size()];

         auto intersection_opt = LineSegmentIntersect(line1_start, line1_end, {pa.x, pa.y}, {pb.x, pb.y});
         if (intersection_opt) {
            auto current_distance = length2(*intersection_opt, line1_start);
            if (current_distance > 0.01) {
               if (current_distance < closest_distance) {
                  closest_distance     = current_distance;
                  closest_intersection = intersection_opt;
               }
            }
         }
      }
   }
   return closest_intersection;
}

bool isPointObstructed(const glm::vec2& position, const glm::vec2& point, const PathsD& scene) {
   auto intersection_opt = LineIntersect(position, point, scene);
   if (intersection_opt) {
      return length2(intersection_opt.value(), position) < length2(point, position);
   }
   return false;
}

bool adjacentInVectorCircular(size_t a, size_t b, size_t size) {
   return a == b || abs(static_cast<int>(a) - static_cast<int>(b)) == 1 || (a == 0 && b == size - 1);
}
enum class Side { LEFT, RIGHT, ON_LINE };

Side pointSide(const glm::vec2& point, const glm::vec2& linePoint, const glm::vec2& lineDirection) {
   glm::vec2 pointVector  = point - linePoint;
   float     crossProduct = lineDirection.x * pointVector.y - lineDirection.y * pointVector.x;

   if (crossProduct > 0) {
      return Side::LEFT;
   } else if (crossProduct < 0) {
      return Side::RIGHT;
   } else {
      return Side::ON_LINE;
   }
}

PathD ComputeVisibilityPolygon(const glm::vec2& position, const PathsD& obstacles) {
   enum class PointType { Start, End, Middle };

   // Struct to hold a point along with its angle and type
   struct TaggedPoint {
      PointD    point;
      double    angle;
      PointType end;
   };

   std::vector<TaggedPoint> all_points;

   for (size_t i = 0; i < obstacles.size(); i++) {
      const auto&              polygon = obstacles[i];
      std::vector<TaggedPoint> tagged_points;

      for (size_t j = 0; j < polygon.size(); j++) {
         const auto& pt = polygon[j];
         glm::vec2   vertex(pt.x, pt.y);
         double      dx    = vertex.x - position.x;
         double      dy    = vertex.y - position.y;
         double      angle = atan2(dy, dx);

         auto prev = polygon[(j - 1 + polygon.size()) % polygon.size()];
         auto next = polygon[(j + 1) % polygon.size()];

         Side side1 = pointSide(vertex, {prev.x, prev.y}, {dx, dy});
         Side side2 = pointSide(vertex, {next.x, next.y}, {dx, dy});

         if (side1 == Side::RIGHT && side2 == Side::RIGHT) {
            tagged_points.emplace_back(pt, angle, PointType::Start);
         } else if (side1 == Side::LEFT && side2 == Side::LEFT) {
            tagged_points.emplace_back(pt, angle, PointType::End);
         } else {
            tagged_points.emplace_back(pt, angle, PointType::Middle);
         }
      }

      all_points.insert(all_points.end(), tagged_points.begin(), tagged_points.end());
   }

   // Sort all points globally by angle
   std::sort(all_points.begin(), all_points.end(),
             [](const TaggedPoint& a, const TaggedPoint& b) { return a.angle < b.angle; });

   // filter out obstructed points
   std::vector<TaggedPoint> filtered_points;
   for (const auto& point : all_points) {
      if (!isPointObstructed(position, {point.point.x, point.point.y}, obstacles)) {
         filtered_points.push_back(point);
      }
   }

   std::vector<glm::vec2> visibility_polygon;
   if (filtered_points.empty()) {
      return PathD();
   }

   for (int i = 0; i < filtered_points.size(); i++) {
      const auto& point  = filtered_points[i];
      const auto  vertex = glm::vec2(point.point.x, point.point.y);

      std::optional<glm::vec2> extendedPoint;
      if (point.end != PointType::Middle) {
         glm::vec2 direction = glm::normalize(vertex - position);
         extendedPoint       = RayIntersect(vertex, direction.x, direction.y, obstacles);
         if (length2(*extendedPoint, vertex) < 0.5) {
            std::cout << "vertex super close to extended: " << length2(*extendedPoint, vertex) << std::endl;
         }
      }

      if (point.end == PointType::Start) {
         if (extendedPoint) {
            visibility_polygon.push_back(*extendedPoint);
         }
      }

      visibility_polygon.emplace_back(point.point.x, point.point.y);

      if (point.end == PointType::End) {
         if (extendedPoint) {
            visibility_polygon.push_back(*extendedPoint);
         }
      }
   }

   PathD path;
   for (const auto& point : visibility_polygon) {
      path.emplace_back(point.x, point.y);
   }

   return path;
}

} // namespace GeometryUtils
