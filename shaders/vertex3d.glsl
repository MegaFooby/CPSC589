// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColour;

uniform mat4 modelViewProjection;
uniform vec3 light;
uniform vec3 cameraPos;
out vec3 colour;

//out vec3 normal;
out vec3 light_Vec;
out vec3 camera_Vec;

void main()
{
	// assign vertex position without modification
    gl_Position = modelViewProjection*vec4(VertexPosition, 1.0);
    colour = VertexColour;
    
    /*vec3 U = triangle[1] - triangle[0];
	vec3 V = triangle[2] - triangle[0];
	normal.x = (U.y*V.z) - (U.z*V.y);
	normal.y = (U.z*V.x) - (U.x*V.z);
	normal.z = (U.x*V.y) - (U.y*V.x);
	normal = normalize(normal);*/
    light_Vec = light - VertexPosition;
    camera_Vec = cameraPos - VertexPosition;
}
