// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
uniform vec3 Colour;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

in vec3 colour;

//in vec3 normal;
//in vec3 lightVec;
in vec3 cameraVec;

void main(void)
{
	// write colour output without modification
    FragmentColour = vec4(colour, 1);
	
	/*vec3 normLightVec = normalize(lightVec);
	vec3 reflection = 2*dot(normLightVec, normal) * normal - normLightVec;
	
	vec4 diffuse = FragmentColour*dot(normal, normLightVec);//*5.f/pow(length(lightVec), 2.f); //diffuse
	vec4 specular = FragmentColour*pow(max(0.f, dot(reflection, normalize(cameraVec))), 100);//*5.f/pow(length(lightVec), 2.f);
	vec4 ambient = FragmentColour*.1f;
	FragmentColour = ambient + diffuse + specular;*/
}
