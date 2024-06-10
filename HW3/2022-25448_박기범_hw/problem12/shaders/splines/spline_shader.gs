#version 330 core
#define N 20
layout (lines_adjacency) in;
layout (line_strip, max_vertices = N) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 B;
uniform float opt  = 1.0;
uniform float loc  = 0.0f;
uniform bool cat  = false;
uniform bool B_S  = false;

void BezierLine(float t)
{
    vec4 T = vec4(t*t*t, t*t, t, 1);
    mat4 B = mat4(vec4(-1,3,-3,1), vec4(3,-6,3,0), vec4(-3,3,0,0), vec4(1,0,0,0));
    mat4 B2 = mat4(vec4(-1.0/6,3.0/6,-3.0/6,1.0/6), vec4(3.0/6,-6.0/6,3.0/6,0), vec4(-3.0/6.0,0,3.0/6.0,0), vec4(1.0/6.0,4.0/6.0,1.0/6.0,0));
    mat4 C = mat4(  vec4(-0.5,1.5,-1.5,0.5),vec4(1,-2.5,2.0,-0.5), vec4(-0.5,0.0,0.5,0),vec4(0,1,0,0));
    if(cat){
        B = C;
    }
    if(B_S){
        B = B2;
    }
    mat4x3 G;
    for(int i=0; i<4; i++){
        vec4 wp = model * gl_in[i].gl_Position;
        //vec4 wp =  gl_in[i].gl_Position;
        G[i][0] = (wp.x + loc);
        G[i][1] = wp.y;
        G[i][2] = wp.z;
    }
    fColor = vec3(t,1,1-t);//gs_in[0].color;
    vec4 worldV = vec4(G * B * T, 1.0f);
    gl_Position = projection * view * worldV;
    //gl_Position = worldV;
    //gl_Position.x += loc;
    EmitVertex();
}

void main() {    
    for(int i=0; i<N; i++){
        BezierLine(float(i)/(N-1));
    }
    EndPrimitive();
}