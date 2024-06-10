#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
   // Fill in the blank
   
   FragColor = (texture(texture1, TexCoord));
   if(FragColor.a < 0.5){discard;}
}