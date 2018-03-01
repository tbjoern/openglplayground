#version 150 core

in vec2 texture;

in vec2 position;
in vec3 color;

out vec3 Color;
out vec2 Texture;

void main()
{
    Color = color;
    Texture = texture;
    gl_Position = vec4(position, 0.0, 1.0);
}
