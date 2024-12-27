#version 330

in vec2 fragTexCoord;
in vec4 fragColor; 

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    //vec2 fragCoord = gl_FragCoord.xy / vec2(800, 450);
    if (texture(texture0, fragTexCoord).r == 0.0f) discard;
    finalColor = fragColor;
    //finalColor = vec4(gl_FragCoord.xy / vec2(1280, 720), 0, 1);
}