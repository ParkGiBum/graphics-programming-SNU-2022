#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube textureID;
uniform samplerCube textureID_night;
uniform float mixture;
void main()
{   
    if(FragColor.a < 0.5){discard;}
    else{FragColor = mix(texture(textureID_night,TexCoords),texture(textureID,TexCoords),mixture);}
}