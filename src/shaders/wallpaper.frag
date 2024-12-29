#version 330

in vec2 fragTexCoord;
in vec4 fragColor; 

uniform vec3 color1;
uniform vec3 color2;
uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    finalColor = vec4(mix(
        color2, 
        color1, 
        texture(texture0, fragTexCoord).r), 1);
}
