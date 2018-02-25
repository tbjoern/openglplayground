#version 150 core

in vec3 Color;

out vec4 out_color;

void main()
{
    out_color = vec4(Color, 1.0);
}
