#version 410
#extension GL_EXT_geometry_shader4 : enable

//minimal geometry shader, just to add the mesh wireframe in the fragment shader

///we expect to receive triangles from the tessellation evaluation shader
layout(triangles) in;
//we're going to output one triangle per input triangle
layout(triangle_strip, max_vertices = 3) out;

//inputs for each triangle, received from the tessellation evaluation shader

in vec3 teNormal[3];
// in vec3 teToEye[3];
in vec3 teColor[3];

/*
Implicitly defined:
in gl_PerVertex
{
	vec4 gl_Position;
} gl_in[3];
*/

//pass-though output variables for each vertex

//out vec4 gl_Position; //implicit
out vec3 gsViewNormal;
// out vec3 gsToEye;
out vec3 gsColor;
// out vec3 gsBaryCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	for (uint i = 0; i < 3; i++)
	{
		gl_Position = projection * view * gl_in[i].gl_Position;
		gsViewNormal = teNormal[i];
		// gsToEye = teToEye[i];
		gsColor = teColor[i];
		// gsBaryCoords = barycentricCoords[i];

		EmitVertex();//add the current vertex to the current triangle strip
	}

	EndPrimitive();//end the current triangle strip
}