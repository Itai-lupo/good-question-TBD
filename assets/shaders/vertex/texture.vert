#version 460 core


layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out layout(location = 0) vec2 TexCoord;

layout (std140, binding = 0) uniform Matrices
{
    vec2 cords;
};

void main()
{
    gl_Position = (vec4(cords, 0.0, 0.0) + vec4(pos, 1.0));
    TexCoord =  texCoord;
} 
