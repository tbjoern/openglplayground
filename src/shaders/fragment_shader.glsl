#version 150 core

in vec2 Texture;

in vec3 Color;

out vec4 out_color;

uniform sampler2D eliteTex;
uniform sampler2D kittenTex;

uniform float time;

void main()
{
    vec4 colElite = texture(eliteTex, Texture);
    vec4 colKitten = texture(kittenTex, Texture);
    out_color = mix(colElite, colKitten, time);
}
