#version 330 core

in vec3 outPos;

out vec4 FragColor;

void main()
{
   FragColor = vec4(outPos.x,outPos.y,outPos.z,1.0f);
}