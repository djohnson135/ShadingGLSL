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
	//vec3 pos3d = normalize(pos3d);
	//vec3 N = normalize(N);

	vec3 normalizedPos3d = normalize(pos3d);
	vec3 normal = normalize(N);

	vec3 E = normalize(eye - normalizedPos3d);
	vec3 I = vec3(0.0f, 0.0f, 0.0f);

	for (int i=0; i < NUM_LIGHTS; i++){
		vec3 L =  normalize(lights[i].position - normalizedPos3d);
		vec3 R = normalize(2 * (dot(L, normal)) * normal - L);
		I += lights[i].color * ((kd * max(0, dot(L, normal))) + (ks * pow(max(0, dot(R, E)), s)));
	}
	I = ka + I;
	gl_FragColor = vec4(I, 1.0f);
}

