#version 410

in vec4 positionAttribute;
in vec4 colorAttribute;

uniform mat4 matrix;

out vec4 color;

void main(void)
{
    color = colorAttribute;
    gl_Position = matrix * positionAttribute;
}
