#version 150 core

in vec2 Texture;

in vec3 Color;

out vec4 out_color;

// uniform sampler2D eliteTex;
uniform sampler2D kittenTex;

uniform float time;

void main()
{
    float xcoord = Texture.x;
    float ycoord = Texture.y;
    if(Texture.y > 0.5f) {
        float sin_distort = (sin(Texture.y * 50.f + time) + sin(Texture.y* 80.f)) / 30.f;
        sin_distort *= (ycoord - 0.5f);
        xcoord += sin_distort;
        ycoord = 1.f - ycoord;
    }
    vec2 texture_distorted = vec2(xcoord, ycoord);
    // vec4 colElite = texture(eliteTex, Texture);
    vec4 colKitten = texture(kittenTex, texture_distorted);
    out_color = colKitten;
}
