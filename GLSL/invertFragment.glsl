#version 410

in highp vec2 texc;

uniform sampler2D texture;

out vec4 frameOut;

void main(void)
{
    vec4 color = texture2D(texture, texc);
    vec4 colorInverted = vec4(vec3(1.0) - color.rgb, color.a);

    frameOut = colorInverted;
}
