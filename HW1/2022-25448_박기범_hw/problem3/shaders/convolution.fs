#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 ourPos;
uniform bool isStriped;

float kernel[9] = float[](
        -1, -2, -1,
        -2,  12, -2,
        -1, -2, -1);
    
void main()
{
    
        vec3 value;
        value = 0;
        
        value += (ourPos.x,ourPos.y)
        value += (ourPos.x-1.0/900.0,ourPos.y)
        value += (ourPos.x+1.0/900.0,ourPos.y)
        value += (ourPos.x,ourPos.y-1.0/600.0)
        value += (ourPos.x,ourPos.y+1.0/600.0)
        value += (ourPos.x-1.0/900.0,ourPos.y-1.0/900.0)
        value += (ourPos.x+1.0/900,ourPos.y+1.0/900.0)
        value += (ourPos.x+1.0/900,ourPos.y-1.0/900.0)
        value += (ourPos.x-1.0/900.0,ourPos.y+1.0/900.0)
        
        FragColor = vec4(value, 1.0f);
}