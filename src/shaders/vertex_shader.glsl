#version 150 core

in vec2 texture;

in vec2 position;
in vec3 color;

out vec3 Color;
out vec2 Texture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time_v;

void main()
{
    Color = color;
    Texture = texture;
    gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
}
