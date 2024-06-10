#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 ourPos;
uniform bool isStriped;

void main()
{
    if(isStriped){
        float theta = atan(ourPos.x, 0.5f-ourPos.y);
        float cosValue = cos(theta * 50.0f) * 0.5f + 0.5f;
        FragColor = vec4(cosValue, cosValue, cosValue, 1.0f);
    }
    else{
        FragColor = vec4(ourColor, 1.0f);
    }
}