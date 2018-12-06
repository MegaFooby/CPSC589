// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Co-Authors:
//			Jeremy Hart, University of Calgary
//			John Hall, University of Calgary
// Date:    December 2015
// ==========================================================================
// CPSC 453 Assignment 1: Boilerplate modified to show several patterns
// Author: Cameron Davies 30003456
// Date: January 2018
// Some code is loosely based on Jeremy Hart's Dragon Curve
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include <stdlib.h>
#include <math.h>
#include "Camera.h"

using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

#define PI_F 3.14159265359f

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geoShader);

int iterations = 0;
char shape = 0;

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
GLuint InitializeShaders(string vert, string frag)
{
	// load shader source from files
	string vertexSource = LoadSource(vert);
	string fragmentSource = LoadSource(frag);
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// check for OpenGL errors and return false if error occurred
	return program;
}

GLuint InitializeShaders(string vert, string frag, string geo)
{
	// load shader source from files
	string vertexSource = LoadSource(vert);
	string fragmentSource = LoadSource(frag);
	string geoSource = LoadSource(geo);
	if (vertexSource.empty() || fragmentSource.empty() || geoSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint geometry = CompileShader(GL_GEOMETRY_SHADER, geoSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment, geometry);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);

	// check for OpenGL errors and return false if error occurred
	return program;
}

// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

struct Geometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  textureBuffer;
	GLuint  colourBuffer;
	GLuint  vertexArray;
	GLsizei elementCount;

	// initialize object names to zero (OpenGL reserved value)
	Geometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
	{}
};

bool InitializeVAO(Geometry *geometry){

	const GLuint VERTEX_INDEX = 0;
	const GLuint COLOUR_INDEX = 1;

	//Generate Vertex Buffer Objects
	// create an array buffer object for storing our vertices
	glGenBuffers(1, &geometry->vertexBuffer);

	// create another one for storing our colours
	glGenBuffers(1, &geometry->colourBuffer);

	//Set up Vertex Array Object
	// create a vertex array object encapsulating all our vertex attributes
	glGenVertexArrays(1, &geometry->vertexArray);
	glBindVertexArray(geometry->vertexArray);

	// associate the position array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glVertexAttribPointer(
		VERTEX_INDEX,		//Attribute index 
		3, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec3),		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(VERTEX_INDEX);

	// associate the colour array with the vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glVertexAttribPointer(
		COLOUR_INDEX,		//Attribute index 
		3, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec3), 		//Stride - can use 0 if tightly packed
		0);					//Offset to first element
	glEnableVertexAttribArray(COLOUR_INDEX);

	// unbind our buffers, resetting to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return !CheckGLErrors();
}

// create buffers and fill with geometry data, returning true if successful
bool LoadGeometry(Geometry *geometry, vec3 *vertices, vec3 *colours, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount, vertices, GL_STATIC_DRAW);

	// create another one for storing our colours
	glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*geometry->elementCount, colours, GL_STATIC_DRAW);

	//Unbind buffer to reset to default state
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// check for OpenGL errors and return false if error occurred
	return !CheckGLErrors();
}

// deallocate geometry-related objects
void DestroyGeometry(Geometry *geometry)
{
	// unbind and destroy our vertex array object and associated buffers
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &geometry->vertexArray);
	glDeleteBuffers(1, &geometry->vertexBuffer);
	glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(Geometry *geometry, GLuint program)
{
	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);
	glBindVertexArray(geometry->vertexArray);
	//rendering lines or triangles based on the shape
	glDrawArrays(GL_LINES, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

void RenderScene(Geometry *geometry, GLuint program, vec3 color, Camera* camera, mat4 perspectiveMatrix, GLenum rendermode)
{

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram(program);

	int vp [4];
	glGetIntegerv(GL_VIEWPORT, vp);
	//int width = vp[2];
	//int height = vp[3];


	//Bind uniforms
	GLint uniformLocation = glGetUniformLocation(program, "Colour");
	glUniform3f(uniformLocation, color.r, color.g, color.b); 

	mat4 modelViewProjection = perspectiveMatrix*camera->viewMatrix();
	uniformLocation = glGetUniformLocation(program, "modelViewProjection");
	glUniformMatrix4fv(uniformLocation, 1, false, glm::value_ptr(modelViewProjection));

	glBindVertexArray(geometry->vertexArray);
	glDrawArrays(rendermode, 0, geometry->elementCount);

	// reset state to default (no shader or geometry bound)
	glBindVertexArray(0);
	glUseProgram(0);

	// check for an report any OpenGL errors
	CheckGLErrors();
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}

// handles keyboard input events
int press = 1;
bool clear = false;
bool render_model = false;
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if(key == GLFW_KEY_1) {
			press = 1;
		} else if(key == GLFW_KEY_2) {
			press = 2;
		} else if(key == GLFW_KEY_3) {
			press = 3;
		} else if(key == GLFW_KEY_4) {
			render_model = true;
			press = 4;
		} else if(key == GLFW_KEY_C) {
			clear = true;
		}
	}
}

void get_open_curve(vector<vec3>* points, vector<vec3>* colours, vector<vec2>* inp, vec3 inc) {
	points->clear();
	colours->clear();
	if(inp->size() == 0) return;
	points->push_back(vec3(inp->at(0).x, inp->at(0).y, 0));
	colours->push_back(inc);
	for(unsigned int i = 1; i < inp->size(); i++) {
		points->push_back(vec3(inp->at(i).x, inp->at(i).y, 0));
		points->push_back(vec3(inp->at(i).x, inp->at(i).y, 0));
		colours->push_back(inc);
		colours->push_back(inc);
	}
	points->pop_back();
	colours->pop_back();
}

void get_closed_curve(vector<vec3>* points, vector<vec3>* colours, vector<vec2>* inp, vec3 inc) {
	points->clear();
	colours->clear();
	if(inp->size() == 0) return;
	points->push_back(vec3(inp->at(0).x, inp->at(0).y, 0));
	colours->push_back(inc);
	for(unsigned int i = 1; i < inp->size(); i++) {
		points->push_back(vec3(inp->at(i).x, inp->at(i).y, 0));
		points->push_back(vec3(inp->at(i).x, inp->at(i).y, 0));
		colours->push_back(inc);
		colours->push_back(inc);
	}
	points->push_back(vec3(inp->at(0).x, inp->at(0).y, 0));
	colours->push_back(inc);
}

Camera* cameraPoint;
float* scrollsens;
void ScrollCallback(GLFWwindow* window, double x, double y) {
	cameraPoint->radius -= *scrollsens * y;
	if(cameraPoint->radius < 0.f) {
		cameraPoint->radius = 0.f;
	}
}
// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 512, height = 512;
	window = glfwCreateWindow(width, height, "CPSC 453 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwMakeContextCurrent(window);

	//Intialize GLAD
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	// call function to load and compile shader programs
	GLuint program = InitializeShaders("shaders/vertex.glsl", "shaders/fragment.glsl");
	GLuint program3d = InitializeShaders("shaders/vertex3d.glsl", "shaders/fragment3d.glsl", "shaders/geometry3d.glsl");
	if (program == 0 || program3d == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	vector<vec2> points;
	vec3 p1_colour = vec3(1, 1, 0);
	
	vector<vec2> points2;
	vec3 p2_colour = vec3(0, 1, 0);
	
	vector<vec2> points3;
	vec3 p3_colour = vec3(0, 1, 1);
	
	vector<vec3> pointsm;
	vec3 pm_colour = vec3(1, 1, 1);
	
	vector<vec3> colours;
	
	//3D shit
	mat4 perspectiveMatrix = glm::perspective(PI_F*.4f, float(width)/float(height), .1f, 50.f);//mat4(1.f);	//Fill in with Perspective Matrix
	Camera cam = Camera(1.f);
	cameraPoint = &cam;
	vec2 lastCursorPos;
	float cursorSensitivity = PI_F/200.f;	//PI/hundred pixels
	float movementSpeed = 0.01f;
	float scrollSpeed = 0.05f;
	scrollsens = &scrollSpeed;
	vec3 light = cam.pos;//vec3(-1, 1, 0);
	GLint cameraGL = glGetUniformLocation(program3d, "cameraPos");
	GLint lightGL = glGetUniformLocation(program, "light");
	
	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, pointsm.data(), colours.data(), pointsm.size()))
		cout << "Failed to load geometry" << endl;


	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window)) {
		// clear screen to a dark grey colour
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if(clear) {
			clear = false;
			if(press == 1) {
				points.clear();
			} else if(press == 2) {
				points2.clear();
			} else if(press == 3) {
				points3.clear();
			}
		}
		
		if(press == 1 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			points.push_back(vec2(xpos/256-1, -(ypos/256-1)));
		}
		if(press == 2 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			points2.push_back(vec2(xpos/256-1, -(ypos/256-1)));
		}
		if(press == 3 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			points3.push_back(vec2(xpos/256-1, -(ypos/256-1)));
		}
		
		//create the model
		if(render_model) {
			render_model = false;
			points3.clear();
			colours.clear();
			vector<vector<vec3>> model_points;
			
			
			//make points into triangles
			for(unsigned int i = 0; i < model_points.size()-1; i++) {
				for(unsigned int j = 0; j < model_points[i].size()-1; j++) {//cout << model_points.size() << " " << model_points[i].size() << " " << i << " " << j << endl;
					pointsm.push_back(model_points[i][j]);
					pointsm.push_back(model_points[i][j+1]);
					pointsm.push_back(model_points[i+1][j]);
					
					pointsm.push_back(model_points[i+1][j]);
					pointsm.push_back(model_points[i][j+1]);
					pointsm.push_back(model_points[i+1][j+1]);
					
					colours.push_back(pm_colour);
					colours.push_back(pm_colour);
					colours.push_back(pm_colour);
					colours.push_back(pm_colour);
					colours.push_back(pm_colour);
					colours.push_back(pm_colour);
				}
			}
		}
		
		if(press == 1 || press == 2) {
			vector<vec3> rline1, rline2;
			get_open_curve(&rline1, &colours, &points, p1_colour);
			LoadGeometry(&geometry, rline1.data(), colours.data(), rline1.size());
			RenderScene(&geometry, program);
			get_open_curve(&rline2, &colours, &points2, p2_colour);
			LoadGeometry(&geometry, rline2.data(), colours.data(), rline2.size());
			// call function to draw our scene
			RenderScene(&geometry, program);
		} else if(press == 3) {
			vector<vec3> render_line;
			get_open_curve(&render_line, &colours, &points3, p3_colour);
			LoadGeometry(&geometry, render_line.data(), colours.data(), render_line.size());
			// call function to draw our scene
			RenderScene(&geometry, program);
		} else if(press == 4) {
			////////////////////////
			//Camera interaction
			////////////////////////
			//Translation
			vec3 movement(0.f);
			
			if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				movement.z += 1.f;
			if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				movement.z -= 1.f;
			if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				movement.x += 1.f;
			if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				movement.x -= 1.f;
			cam.move(movement*movementSpeed);
			light = cam.pos;
			
			//Rotation
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			vec2 cursorPos(xpos, ypos);
			vec2 cursorChange = cursorPos - lastCursorPos;
			lastCursorPos = cursorPos;
			
			if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				cam.rotateHorizontal(-cursorChange.x*cursorSensitivity);
				cam.rotateVertical(-cursorChange.y*cursorSensitivity);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(program3d);
			glUniform3fv(cameraGL, 1, &(cam.pos.x));
			glUniform3fv(lightGL, 1, &(light.x));
			LoadGeometry(&geometry, pointsm.data(), colours.data(), pointsm.size());
			RenderScene(&geometry, program3d, vec3(1, 0, 0), &cam, perspectiveMatrix, GL_TRIANGLES);
		}

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// clean up allocated resources before exit
	DestroyGeometry(&geometry);
	glUseProgram(0);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "Goodbye!" << endl;
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}

GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geoShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);
	if (geoShader) glAttachShader(programObject, geoShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}
