#pragma once

#include <vector>
#include <functional>
#include "clipper2/clipper.h"
#include <glm/glm.hpp>

namespace GeometryUtils {
using namespace Clipper2Lib;

/**
 * @brief Finds the union of multiple polygons.
 *
 * @param polygons A vector of polygons, where each polygon is represented as a vector of glm::vec2 points.
 * @param output A PolyTreeD object to store the resulting union.
 * @return true if the union operation was successful, false otherwise.
 */
bool findPolygonUnion(const std::vector<std::vector<glm::vec2>>& polygons, PolyTreeD& output);

/**
 * @brief Flattens a hierarchical PolyPathD into a simple PathsD structure.
 *
 * @param polyPath The root PolyPathD to flatten.
 * @return PathsD A flattened PathsD containing all polygons from the hierarchy.
 */
PathsD FlattenPolyPathD(const PolyPathD& polyPath);

/**
 * @brief Computes the visibility polygon from a given position and obstacles.
 *
 * @param position The player's position as glm::vec2.
 * @param obstacles The obstacles represented as PathsD (vector of paths).
 * @return PathD The visibility polygon as a vector of points.
 */
PathD ComputeVisibilityPolygon(const glm::vec2& position, const PathsD& obstacles);
PathD ComputeVisibilityPolygon(const PointD& pos, const PathsD& obstacles);
} // namespace GeometryUtils
