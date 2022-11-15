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
	vec3 I = vec3(0.0f, 0.0f, 0.0f);

	for (int i=0; i < NUM_LIGHTS; i++){
		vec3 L =  normalize(lights[i].position - newPos);
		vec3 R = normalize(2 * (dot(L, Norm)) * Norm - L);
		I += lights[i].color * ((kd * max(0, dot(L, Norm))) + (ks * pow(max(0, dot(R, E)), s)));
	}
	I = ka + I;
	gl_FragColor = vec4(I, 1.0f);
}

