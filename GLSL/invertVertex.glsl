#version 410

in highp vec2 vertex;
in highp vec2 texCoord;

out highp vec2 texc;

void main(void)
{
    texc = texCoord;
    gl_Position = vec4(vertex, 0.0, 1.0);
}
