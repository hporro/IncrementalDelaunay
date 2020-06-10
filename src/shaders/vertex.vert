#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 p0;
uniform vec2 p1;
uniform vec2 p2;
uniform vec2 p3;

uniform vec2 offset;

uniform float zoom;

void main()
{
    float a = p1.x - p0.x;
    float b = p3.y - p0.y;

    a = max(a,b);

    float posx = ((aPos.x - p0.x)/a - 0.5)*2*zoom + offset.x;
    float posy = ((aPos.y - p0.y)/a - 0.5)*2*zoom + offset.y;
    gl_Position = vec4(posx, posy, 0, 1.0);
}