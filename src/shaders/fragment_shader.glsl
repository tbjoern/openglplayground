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
    float threshold = 0.7f;
    if(Texture.y > threshold) {
        float sin_distort = (sin(Texture.y * 50.f + time) + sin(Texture.y* 80.f)) / 30.f;
        sin_distort *= log(1.f + ycoord - threshold) * 2;
        xcoord += sin_distort;
        ycoord = (ycoord - threshold) * -1.f + threshold;
    }
    vec2 texture_distorted = vec2(xcoord, ycoord);
    // vec4 colElite = texture(eliteTex, Texture);
    vec4 colKitten = texture(kittenTex, texture_distorted);
    out_color = colKitten;
}
