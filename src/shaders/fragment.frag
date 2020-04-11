#version 330 core
out vec4 FragColor;

uniform vec3 aColor;

void main()
{
    FragColor = vec4(aColor.x, aColor.y, aColor.z, 1.0f);
}