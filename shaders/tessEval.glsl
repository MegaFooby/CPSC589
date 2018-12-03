#version 410
#define PI 3.14159
layout(isolines) in;

in vec3 teColour[];
//in gl_in[];

out vec3 Colour;

uniform int order;
uniform int circles;
uniform int points;
uniform vec3 colour;

/*float deBoor1(int i, float u) {
	if(gl_in[i].gl_Position.z <= u && u < gl_in[i+1].gl_Position.z) {
		return 1.f;
	} else {
		return 0.f;
	}
}

float deBoor2(int i, float u) {
	if(gl_in[i].gl_Position.z <= u && u < gl_in[i+1].gl_Position.z) {
		return (u - gl_in[i].gl_Position.z)/(gl_in[i+1].gl_Position.z - gl_in[i].gl_Position.z);
	} else if(gl_in[i+1].gl_Position.z <= u && u < gl_in[i+2].gl_Position.z) {
		return (gl_in[i+2].gl_Position.z - u)/(gl_in[i+2].gl_Position.z - gl_in[i+1].gl_Position.z);
	} else {
		return 0.f;
	}
}

float deBoor3(int i, float u) {
	if(gl_in[i].gl_Position.z <= u && u < gl_in[i+1].gl_Position.z) {
		return ((u - gl_in[i].gl_Position.z)/(gl_in[i+2].gl_Position.z - gl_in[i].gl_Position.z)) * ((u - gl_in[i].gl_Position.z)/(gl_in[i+1].gl_Position.z - gl_in[i].gl_Position.z));
	} else if(gl_in[i+1].gl_Position.z <= u && u < gl_in[i+2].gl_Position.z) {
		return ((u - gl_in[i].gl_Position.z)/(gl_in[i+2].gl_Position.z - gl_in[i].gl_Position.z)) * ((gl_in[i+2].gl_Position.z - u)/(gl_in[i+2].gl_Position.z - gl_in[i+1].gl_Position.z))
		 + ((gl_in[i+3].gl_Position.z - u)/(gl_in[i+3].gl_Position.z - gl_in[i+1].gl_Position.z)) * ((u - gl_in[i+1].gl_Position.z)/(gl_in[i+2].gl_Position.z - gl_in[i+1].gl_Position.z));
	} else if(gl_in[i+1].gl_Position.z <= u && u < gl_in[i+2].gl_Position.z) {
		return ((gl_in[i+3].gl_Position.z - u)/(gl_in[i+3].gl_Position.z - gl_in[i+1].gl_Position.z)) * ((gl_in[i+3].gl_Position.z - u)/(gl_in[i+3].gl_Position.z - gl_in[i+2].gl_Position.z));
	} else {
		return 0.f;
	}
}

float deBoor4(int i, float u) {
	const int r = 4;
	float value1 = 0, value2 = 0;
	float den1 = (gl_in[i+r-1].gl_Position.z - gl_in[i].gl_Position.z), den2 = (gl_in[i+r].gl_Position.z - gl_in[i+1].gl_Position.z);
	if((value1 = deBoor3(i, u)) > 0 && (den1 > .001f) && (den1 < -.001f))
		value1 += ((u - gl_in[i].gl_Position.z)/den1);
	if((value2 = deBoor3(i+1, u)) > 0 && (den2 > .001f) && (den2 < -.001f))
		value1 += value2 * ((gl_in[i+r].gl_Position.z - u)/den2);
	return value1;
}*/

float bico(int n, int v) {
	float num = 1, den = 1;
	for(int i = n; i > 1; i--) {
		num *= i;
	}
	for(int i = v; i > 1; i--) {
		den *= i;
	}
	for(int i = n-v; i > 1; i--) {
		den *= i;
	}
	return num/den;
}

float deBoor1(int i, float u) {
	if(gl_in[i].gl_Position.z <= u && u < gl_in[i+1].gl_Position.z) {
		return 1.f;
	} else {
		return 0.f;
	}
}

float deBoor2(int i, float u) {
	int order = 2;
	float value = 0;
	float den = gl_in[i+order-1].gl_Position.z-gl_in[i].gl_Position.z;
	if(den != 0)
	value += (u-gl_in[i].gl_Position.z)/(den) * deBoor1(i, u);
	den = gl_in[i+order].gl_Position.z-gl_in[i+1].gl_Position.z;
	if(den != 0)
	value += (gl_in[i+order].gl_Position.z-u)/(den) * deBoor1(i+1, u);
	return value;
}

float deBoor3(int i, float u) {
	int order = 3;
	float value = 0;
	float den = gl_in[i+order-1].gl_Position.z-gl_in[i].gl_Position.z;
	if(den != 0)
	value += (u-gl_in[i].gl_Position.z)/(den) * deBoor2(i, u);
	den = gl_in[i+order].gl_Position.z-gl_in[i+1].gl_Position.z;
	if(den != 0)
	value += (gl_in[i+order].gl_Position.z-u)/(den) * deBoor2(i+1, u);
	return value;
}

float deBoor4(int i, float u) {
	int order = 4;
	float value = 0;
	float den = gl_in[i+order-1].gl_Position.z-gl_in[i].gl_Position.z;
	if(den != 0)
	value += (u-gl_in[i].gl_Position.z)/(den) * deBoor3(i, u);
	den = gl_in[i+order].gl_Position.z-gl_in[i+1].gl_Position.z;
	if(den != 0)
	value += (gl_in[i+order].gl_Position.z-u)/(den) * deBoor3(i+1, u);
	return value;
}

void main()
{
	float u = gl_TessCoord.x;
	if(circles == 1) {
		u *= 2*PI;
		vec2 position = vec2(cos(u), sin(u))*.03f*gl_in[0].gl_Position.w;
		gl_Position = vec4(position + gl_in[0].gl_Position.xy, 0, 1);
		Colour = teColour[0];
		return;
	}
	if(circles == 2) {
		vec2 position = u*gl_in[0].gl_Position.xy + (1-u)*gl_in[1].gl_Position.xy;
		gl_Position = vec4(position, 0, 1);
		Colour = teColour[0];
		return;
	}
	//scale u so I can draw it in multiple segments
	//u = (gl_in[order].gl_Position.z - gl_in[0].gl_Position.z)*u + gl_in[0].gl_Position.z;//+gl_in[points].gl_Position.x;
	//u *= gl_in[points].gl_Position.z;
	vec2 position = vec2(0, 0);
	float sum = 0.f;
	//if(u == 1.f) u-=.0001;
	for(int i = 0; i < points; i++) {
		//float N = gl_in[i].gl_Position.z * bico(order-1, i)*(pow(u, i) * pow((1-u), order-i-1));//bezier
		float N =1;// gl_in[i].gl_Position.w;//b-spline
		switch(order) {
			case 1: N *= deBoor1(i, u); break;
			case 2: N *= deBoor2(i, u); break;
			case 3: N *= deBoor3(i, u); break;
			case 4: N *= deBoor4(i, u); break;
		}
		position += gl_in[i].gl_Position.xy * N;//point times xi where the sum of x0 to xn is 1
		sum += N;
	}
	//position /= sum;
	gl_Position = vec4(position, 0, 1);
	//gl_Position = vec4(u-.5f, sum-.5f, 0, 1);
	//gl_Position = vec4(u-.5f, gl_in[3].gl_Position.x, 0, 1);//deBoor2(1, u)-.5f, 0, 1);
	/*if(position.x > 1 || position.x < -1 || position.y > 1 || position.y < -1) {
		gl_Position = vec4(u-.5f, -.75f, 0, 1);
	}*/
	Colour = teColour[0];

}
