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
#include <math.h>
#include <vector>

using namespace std;
using namespace glm;
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tcsShader, GLuint tesShader);

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
GLuint InitializeShaders()
{
	// load shader source from files
	string vertexSource = LoadSource("shaders/vertex.glsl");
	string fragmentSource = LoadSource("shaders/fragment.glsl");
	string tcsSource = LoadSource("shaders/tessControl.glsl");
	string tesSource = LoadSource("shaders/tessEval.glsl");
	if (vertexSource.empty() || fragmentSource.empty()) return false;

	// compile shader source into shader objects
	GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	GLuint tcs = CompileShader(GL_TESS_CONTROL_SHADER, tcsSource);
	GLuint tes = CompileShader(GL_TESS_EVALUATION_SHADER, tesSource);

	// link shader program
	GLuint program = LinkProgram(vertex, fragment, tcs, tes);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(tcs);
	glDeleteShader(tes);

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
		4, 					//# of components
		GL_FLOAT, 			//Type of component
		GL_FALSE, 			//Should be normalized?
		sizeof(vec4),		//Stride - can use 0 if tightly packed
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
bool LoadGeometry(Geometry *geometry, vec4 *vertices, vec3 *colours, int elementCount)
{
	geometry->elementCount = elementCount;

	// create an array buffer object for storing our vertices
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*geometry->elementCount, vertices, GL_STATIC_DRAW);

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
	glDrawArrays(GL_PATCHES, 0, geometry->elementCount);

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

char press = 0;
// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GL_TRUE);
		
		if(key == GLFW_KEY_KP_ADD) {
			press = 1;
		}
		else if(key == GLFW_KEY_KP_SUBTRACT) {
			press = 2;
		}
		else if(key == GLFW_KEY_A) {
			press = -2;
		}
	}
}
float scroll = 0.f;
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	scroll = yoffset;
}

void getPoints(vector<vec4> *toRender, vector<vec3> *colours, vector<vec4> *points, int order, vec3 colour) {
	toRender->clear();
	colours->clear();
	vector<float> knots;
	//float knot = 0;
	unsigned int i;
	for(i = 0; i < points->size()+order; i++) {
		if((int)i < order) {
			knots.push_back(0.f);
		} else if(i >= points->size()) {
			knots.push_back(1.f);
		} else {
			knots.push_back((i-order+1)/(float)(points->size()-order+1));
		}
		/*if((int)i >= order && i < points->size()) {
			knot += 1.f/(float)(order-1);
		}
		knots.push_back(knot);*/
		//cout << knots[i] << "\t";
	}
	//cout << endl;
	/*for(unsigned int j = 0; j < points->size(); j++) {
		for(i = i; i < points->size(); i++) {
			toRender->push_back(vec4((*points)[i].x, (*points)[i].y, knots[i], (*points)[i].z));
			colours->push_back(colour);
		}
		for(i = points->size(); i < points->size()+order; i++) {
			toRender->push_back(vec4(0, 0, knots[i], 0));
			colours->push_back(colour);
		}
	}*/
	for(i = 0; i < points->size(); i++) {
		toRender->push_back(vec4((*points)[i].x, (*points)[i].y, knots[i], (*points)[i].w));
		colours->push_back(colour);
	}
	for(i = points->size(); i < points->size()+order; i++) {
		toRender->push_back(vec4(0, 0, knots[i], 0));
		colours->push_back(colour);
	}
}

float deBoor(int i, float u, int order, float knots[]) {
	if(order == 1) {
		if(knots[i] <= u && u < knots[i+1]) {
			return 1.f;
		} else {
			return 0.f;
		}
	}
	float value = 0;
	float den = knots[i+order-1]-knots[i];
	if(den != 0)
	value += (u-knots[i])/(den) * deBoor(i, u, order-1, knots);
	den = knots[i+order]-knots[i+1];
	if(den != 0)
	value += (knots[i+order]-u)/(den) * deBoor(i+1, u, order-1, knots);
	return value;
}

void geopoints(vector<vec4> *toRender, vector<vec3> *colours, vector<vec4> *points, vector<float> *knots, float u, int order, vec3 colour) {
	order--;
	if(order == 1) return;
	for(unsigned int i = 0; i < points->size()-1; i++) {
		float N = deBoor(i, u, order, knots->data());
		toRender->push_back((*points)[i] * N);
		N = deBoor(i+1, u, order, knots->data());
		toRender->push_back((*points)[i+1] * N);
	}
	geopoints(toRender, colours, points, knots, u, order, colour);
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
	glfwWindowHint(GLFW_SAMPLES, 4);
	int width = 1024, height = 1024;
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
	GLuint program = InitializeShaders();
	if (program == 0) {
		cout << "Program could not initialize shaders, TERMINATING" << endl;
		return -1;
	}
	
	vector<vec4> points;
	vector<vec4> toRender;
	vector<vec3> colours;
	vector<vec3> pointcolours;
	vec3 pointcolour = vec3(0.f, .3f, 1.f);
	int order = 3;
	float increment = 0;
	vec3 colour = vec3(.9f, .9f, .9f);
	vector<vec4> geo;
	vector<vec3> geocolours;
	vec3 geocolour = vec3(.9f, .0f, .0f);
	
	points.push_back(vec4(-.5f, -.25f, 0.f, 1.f));
	points.push_back(vec4(-.5f, 0.f, .25f, 1.f));
	points.push_back(vec4(.0f, .25f, .5f, 1.f));
	points.push_back(vec4(.5f, 0.f, .75f, 1.f));
	points.push_back(vec4(.5f, -.25f, 1.f, 1.f));
	pointcolours.push_back(pointcolour);
	pointcolours.push_back(pointcolour);
	pointcolours.push_back(pointcolour);
	pointcolours.push_back(pointcolour);
	pointcolours.push_back(pointcolour);
	
	getPoints(&toRender, &colours, &points, order, colour);
	
	glUseProgram(program);
	GLint orderGL = glGetUniformLocation(program, "order");
	glUniform1i(orderGL, order);
	GLint circlesGL = glGetUniformLocation(program, "circles");
	glUniform1i(circlesGL, 0);
	GLint pointsGL = glGetUniformLocation(program, "points");
	glUniform1i(pointsGL, points.size());
	// call function to create and fill buffers with geometry data
	Geometry geometry;
	if (!InitializeVAO(&geometry))
		cout << "Program failed to intialize geometry!" << endl;

	if(!LoadGeometry(&geometry, toRender.data(), colours.data(), toRender.size()))
		cout << "Failed to load geometry" << endl;
	
	glPatchParameteri(GL_PATCH_VERTICES, points.size()+order);

	bool LMBpressed = false;
	int selected = -1;

	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos = (xpos - (width/2))/(width/2);
		ypos = -(ypos - (height/2))/(height/2);
		int closest = 0;
		float distance = pow(xpos - points[0].x, 2) + pow(ypos - points[0].y, 2);
		for(unsigned int i = 1; i < points.size(); i++) {
			float temp = pow(xpos - points[i].x, 2) + pow(ypos - points[i].y, 2);
			if(temp < distance) {
				distance = temp;
				closest = i;
			}
		}
		if(!LMBpressed && distance < .001f && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {//select point
			if(selected >= 0) {
				pointcolours[selected] = pointcolour;
			}
			selected = closest;
			LMBpressed = true;
		}
		
		if(selected >= 0 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {//move point
			points[selected].x = xpos;
			points[selected].y = ypos;
			getPoints(&toRender, &colours, &points, order, colour);
		}
		
		if(selected < 0 && !LMBpressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {//element added
			LMBpressed = true;
			//points.insert(position, vec2(xpos, ypos)); //create intuitive way to find position
			points.push_back(vec4(xpos, ypos, points.size(), 1.f));
			pointcolours.push_back(pointcolour);
			//get points here
			getPoints(&toRender, &colours, &points, order, colour);
		}
		
		if(scroll != 0.f) {
			if(pow(xpos - points[selected].x, 2) + pow(ypos - points[selected].y, 2) < .001f) {
				points[selected].w += scroll/10.f;
				if(points[selected].w <= 0.f) {
					points[selected].w -= scroll/10.f;
				}
				getPoints(&toRender, &colours, &points, order, colour);
			}
			scroll = 0.f;
		}
		
		if(selected >= 0 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {//deselect point
			pointcolours[selected] = pointcolour;
			selected = -1;
		}
		
		if(selected >= 0 && glfwGetKey(window, GLFW_KEY_DELETE) && points.size() > 1) {//element removed
			pointcolours[selected] = pointcolour;
			points.erase(points.begin()+selected); //create intuitive way to find position
			pointcolours.pop_back();
			selected = -1;
			//get points here
			getPoints(&toRender, &colours, &points, order, colour);
		}
		
		if(!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)) {//LMB reset
			LMBpressed = false;
		}
		if(selected >= 0) {//makes sure the selected point is highlighted
			pointcolours[selected] = vec3(0.f, 1.f, 0.f);
		}
		
		if(press == 1) {//increase order
			if(++order > (int)points.size())
				order--;
			glUseProgram(program);
			glUniform1i(orderGL, order);
			press = 0;
			getPoints(&toRender, &colours, &points, order, colour);
		}
		if(press == 2) {//decrease order
			if(--order == 1)
				order++;
			glUseProgram(program);
			glUniform1i(orderGL, order);
			press = 0;
			getPoints(&toRender, &colours, &points, order, colour);
		}
		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {//increase parameter increment
			increment += 0.01f;
			if(increment > 1.f)
				increment -= 0.01f;
		}
		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {//decrease parameter increment
			increment -= 0.01f;
			if(increment < 1)
				increment += 0.01f;
		}
		
		// call function to draw our scene
		// clear screen to a dark grey colour
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);
		glPatchParameteri(GL_PATCH_VERTICES, points.size()+order);
		glUniform1i(circlesGL, 0);
		LoadGeometry(&geometry, toRender.data(), colours.data(), toRender.size());
		RenderScene(&geometry, program);
		
		glUseProgram(program);
		glPatchParameteri(GL_PATCH_VERTICES, 1);
		glUniform1i(circlesGL, 1);
		LoadGeometry(&geometry, points.data(), pointcolours.data(), points.size());
		RenderScene(&geometry, program);
		if(press == -2) {
			press++;
			vector<float> knots;
			unsigned int i;
			for(i = 0; i < points.size()+order; i++) {
				if((int)i < order) {
					knots.push_back(0.f);
				} else if(i >= points.size()) {
					knots.push_back(1.f);
				} else {
					knots.push_back((i-order+1)/(float)(points.size()-order+1));
				}
			}
			geo.clear();
			geocolours.clear();
			geopoints(&geo, &geocolours, &points, &knots, increment, order, geocolour);
		}
		if(press == -1) {
			glUseProgram(program);
			glPatchParameteri(GL_PATCH_VERTICES, 2);
			glUniform1i(circlesGL, 2);
			LoadGeometry(&geometry, geo.data(), geocolours.data(), geo.size());
			RenderScene(&geometry, program);
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
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint tcsShader, GLuint tesShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);
	if (tcsShader) glAttachShader(programObject, tcsShader);
	if (tesShader) glAttachShader(programObject, tesShader);

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
