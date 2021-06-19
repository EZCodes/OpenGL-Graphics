#version 330 core
in vec3 vertex;

out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 position;
uniform vec4 color;
uniform float scale;

void main()
{
    ParticleColor = color;
    gl_Position = projection* view * vec4((vertex*scale) + position, 1.0);
}