#version 410
#extension GL_EXT_geometry_shader4 : enable
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 colour[];
in vec3 light_Vec[];
in vec3 camera_Vec[];

out vec3 normal;
out vec3 frag_colour;
out vec3 lightVec;
out vec3 cameraVec;


void main() {
	lightVec = (1.f/3.f)*(light_Vec[0] + light_Vec[1] + light_Vec[2]);
	cameraVec = (1.f/3.f)*(camera_Vec[0] + camera_Vec[1] + camera_Vec[2]);
	frag_colour = (1.f/3.f)*(colour[0] + colour[1] + colour[2]);
	vec3 P0 = gl_in[0].gl_Position.xyz;
	vec3 P1 = gl_in[1].gl_Position.xyz;
	vec3 P2 = gl_in[2].gl_Position.xyz;
	
	vec3 V0 = P0 - P1;
	vec3 V1 = P2 - P1;
	
	vec3 N = cross(V1, V0);
	normal = normalize(N);
	if(normal.z < 0) normal = -1*normal;
	
	for(int i = 0; i < gl_VerticesIn; i++) {
		gl_Position = gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();
}
//https://www.khronos.org/opengl/wiki/Geometry_Shader_Examples
/*#version 410
#extension GL_ARB_geometry_shader4 : enable
// Uniform
uniform mat4 ProjectionMatrix;
// Varying
varying in vec2 VTexCoord0[3];    // [3] because this makes a triangle
varying in vec3 VHalfVector0[3];
varying in vec3 VEyeNormal[3];
varying in vec3 VEyeVertex[3];
varying out vec2 TexCoord0;
varying out vec3 HalfVector0;
varying out vec3 EyeNormal;

void main()
{
	int i;
	vec3 newVertex;
	// Pass through the original vertex
	for(i=0; i<gl_VerticesIn; i++)
	{
		gl_Position = gl_PositionIn[i];
		TexCoord0 = VTexCoord0[i];
		HalfVector0 = VHalfVector0[i];
		EyeNormal = VEyeNormal[i];

		EmitVertex();
	}

	EndPrimitive();

	// Push the vertex out a little using the normal
	for(i=0; i<gl_VerticesIn; i++)
	{
		newVertex = VEyeNormal[i] + VEyeVertex[i];
		gl_Position = ProjectionMatrix * vec4(newVertex, 1.0);
		TexCoord0 = VTexCoord0[i];
		HalfVector0 = VHalfVector0[i];
		EyeNormal = VEyeNormal[i];

		EmitVertex();
	}

	EndPrimitive();
}
*/
