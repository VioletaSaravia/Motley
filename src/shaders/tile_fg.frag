#version 300 es
precision highp float;

in vec2 fragTexCoord;
in vec4 fragColor; 

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    if (texture(texture0, fragTexCoord).r == 0.0f) discard;
    finalColor = fragColor;
}