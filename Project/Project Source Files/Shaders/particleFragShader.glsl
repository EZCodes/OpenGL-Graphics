#version 330 core
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D particle;

void main()
{
    color = ParticleColor;
}  