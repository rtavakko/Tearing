#version 410

in highp vec2 texc;

uniform sampler2D texture;

out vec4 frameOut;

void main(void)
{
    frameOut = texture2D(texture, texc);
}
