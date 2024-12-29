#version 330

in vec2 fragTexCoord;
in vec4 fragColor; 

uniform vec3 color1;
uniform vec3 color2;
uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    vec4 color = texture(texture0, fragTexCoord);
    finalColor = vec4(color.r < 0.7 ? color1 : color2, 1);
}