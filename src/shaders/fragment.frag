#version 330 core
out vec4 FragColor;

uniform vec3 aColor;

in float showing;

void main()
{
    FragColor = vec4(aColor.x, aColor.y, aColor.z, 1.0f);
    if(showing<1.0f)FragColor=vec4(0.0f,0.0f,0.0f,0.0f);
}