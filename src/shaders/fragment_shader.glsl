#version 150 core

in vec2 Texture;

in vec3 Color;

out vec4 out_color;

uniform sampler2D tex;

void main()
{
    out_color = texture(tex, Texture) * vec4(Color, 1.0);
}
