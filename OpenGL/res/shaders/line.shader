#shader vertex
#version 330 core

// Vertex attributes
layout(location = 0) in vec2 a_Position;

// Uniforms
uniform vec2 u_StartPos;       // Start position of the line
uniform vec2 u_EndPos;         // End position of the line
uniform float u_Width;         // Width of the line

uniform mat4 u_MVP;            // Model-View-Projection matrix

void main()
{
    // Calculate the direction vector from start to end
    vec2 direction = normalize(u_EndPos - u_StartPos);
    
    // Calculate the perpendicular vector for width offset
    vec2 perpendicular = vec2(-direction.y, direction.x);
    
    // Scale the perpendicular vector by width and vertex y position
    vec2 offset = perpendicular * a_Position.y * u_Width;
    
    // Interpolate between start and end positions based on vertex x position
    vec2 interpolatedPos = mix(u_StartPos, u_EndPos, a_Position.x);
    
    // Apply the offset to get the final position
    vec2 finalPos = interpolatedPos + offset;
    
    // Transform the final position using the MVP matrix
    gl_Position = u_MVP * vec4(finalPos, 0.0, 1.0);
}

#shader fragment
#version 330 core

// Output color
layout(location = 0) out vec4 color;

// Uniform for line color
uniform vec4 u_Color;

void main()
{
    color = u_Color;
}
