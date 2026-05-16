#version 330 core

out vec3 FragColour;
in vec3 ourColour;

void main()
{
    FragColour = ourColour;
}