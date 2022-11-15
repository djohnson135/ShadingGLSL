#version 140




uniform vec3 eye;

struct lightStruct
{
	vec3 position;
	vec3 color;
};

#define NUM_LIGHTS 2

uniform lightStruct lights[NUM_LIGHTS];

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;




varying vec3 pos3d;
varying vec3 N;

void main()
{
	vec3 newPos = normalize(pos3d);
	vec3 Norm = normalize(N);

	vec3 E = normalize(eye - newPos);
	vec3 color = vec3(0.0f, 0.0f, 0.0f);

	float dotProd = dot(Norm, eye);
	

	if (dotProd >= 0.0f && dotProd <0.01f){
		color = vec3(1.0f, 1.0f, 1.0f);
	}else{
		color = vec3(1.0f, 1.0f, 1.0f);
	}

	gl_FragColor = vec4(color, 1.0f);
}

