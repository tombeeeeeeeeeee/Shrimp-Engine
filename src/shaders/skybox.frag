#version 330 core

in vec3 direction;
out screenColor;

uniform samplerCube uTexEnv;

void main()
{
        screenColor = texture(uTexEnv, direction);
}