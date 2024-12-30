#version 300 es
precision mediump float;

in vec2 fragTexCoord;
in vec4 fragColor; 

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    //vec2 fragCoord = gl_FragCoord.xy / vec2(800, 450);
    if (texture(texture0, fragTexCoord).r == 0.0f) discard;
    finalColor = fragColor - vec4(0, 0, 0, 0.1f);
}