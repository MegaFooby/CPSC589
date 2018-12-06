#include "Geometry.h"
#include <iostream>

#include <cmath>

Geometry::Geometry() {
	drawMode = GL_TRIANGLES;
	vao = 0;
	vertexBuffer = 0;
	colourBuffer = 0;
	modelMatrix = glm::mat4(1.f);
}

double Geometry::delta(std::vector<double> &uVector, double iterator, int index1, int index2){
    if(index2==1)
    {
        if(uVector[index1]<=iterator && iterator<uVector[index1+1])
            return 1;
        return 0;
    }
    double delted;
    delted = ((iterator-uVector[index1])/(uVector[index1+index2-1]-uVector[index1])*delta(uVector, iterator, index1, index2-1)) + ((uVector[index1+index2]-iterator)/(uVector[index1+index2]-uVector[index1+1])*delta(uVector, iterator, index1+1, index2-1));
    return delted;
}

Geometry Geometry::E_delta_1(std::vector<int> polyx, std::vector<int> polyy, int factor)
{
    Geometry curve;
    int n, d;
    d = factor;
    std::vector<int> xcopy;
    xcopy.swap(polyx);
    std::vector<int> ycopy;
    ycopy.swap(polyy);
    for(int index = 0; index < d; index ++){
        xcopy.insert(xcopy.begin(), xcopy[0]);
        ycopy.insert(ycopy.begin(), ycopy[0]);
        xcopy.insert(xcopy.end(), xcopy[xcopy.size()-1]);
        ycopy.insert(ycopy.end(), ycopy[ycopy.size()-1]);
    }
    n = xcopy.size();
    std::vector<double> uVec;
    int i;
    for(i=0;i<n+d;i++)
    {
        uVec.push_back(((double)i)/(n+d-1));
    }
    double x, y, basis, u;
    for(u=0;u<=1;u+=0.0001)
    {
        if (u >= (uVec[d-1]) && u <= (uVec[n])){
            x = 0;
            y = 0;
            for(i=0;i<xcopy.size();i++)
            {
                basis = Geometry::delta(uVec, u, i, d);
                x += basis*xcopy[i];
                y += basis*ycopy[i];
            }
            curve.verts.push_back(glm::vec3(x, y, 0));
            curve.colours.push_back(glm::vec3(1.f, 0.f, 0.f));
        }
    }
    curve.drawMode = GL_LINE_STRIP;
    return curve;
}


Geometry Geometry::makeCircle(float radius, float uInc) {

	Geometry circle;

	for (double u = 0.0; u <= 2.0 * M_PI; u += uInc) {
		circle.verts.push_back(radius * glm::vec3(cos(u), sin(u), 0));
		circle.colours.push_back(glm::vec3(1.f, 1.f, 1.f));
	}
	circle.drawMode = GL_LINE_STRIP;
	return circle;
}
Geometry Geometry::makeSmolCircle(float radius, float uInc, double x, double y) {
    
    Geometry circle;
    
    for (double u = 0.0; u <= 2.0 * M_PI; u += uInc) {
        glm::vec3 circlePoint =  radius * glm::vec3(cos(u), sin(u) , 0);
        circlePoint = circlePoint + glm::vec3(x, y , 0);
//        std::cout << circlePoint.x  << std::endl;
        circle.verts.push_back(circlePoint);
        circle.colours.push_back(glm::vec3(0.f, 0.f, 0.f));
    }
    circle.drawMode = GL_LINE_STRIP;
    return circle;
}


Geometry Geometry::makeHypocycloid(float smolRadius, float bigRadius, int cycle, float rotation, float scale, float uInc){
    Geometry hypo;
    
    for (double u = 0.0; u <= cycle * 2.0 * M_PI; u += uInc) {
        float test1 = (bigRadius - smolRadius) * cos(u);
        float test2 =(smolRadius * cos(((bigRadius- smolRadius)/ smolRadius)* u));
        float x = scale * (test1 + test2);
        float y = scale * (((bigRadius - smolRadius)* sin(u)) - (smolRadius * sin(((bigRadius- smolRadius)/ smolRadius)* u)));
        glm::mat3 rot = glm::mat3(cos(rotation) , sin(rotation), 0,
                                  -sin(rotation), cos(rotation), 0,
                                  0,0,1);
        glm::vec3 vec = rot *  glm::vec3(x, y, 0);
        hypo.verts.push_back(vec);
        hypo.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
    }
    hypo.drawMode = GL_LINE_STRIP;
    return hypo;
}
