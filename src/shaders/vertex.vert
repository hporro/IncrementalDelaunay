#version 330 core
layout (location = 0) in vec2 aPos;

uniform float a;
uniform float pox;
uniform float poy;

void main()
{
    float posx = (aPos.x);
    float posy = (aPos.y);
    gl_Position = vec4(posx, posy, 0, 1.0);
}