#version 140

uniform vec3 eye;



varying vec3 pos3d;
varying vec3 N;

void main()
{
	vec3 normal = normalize(N);

	vec3 E = normalize(eye - pos3d);
	vec3 color = vec3(0.0f, 0.0f, 0.0f);

	float dotProd = dot(normal, E);
	

	if (dotProd >= -0.26f && dotProd <= 0.26f){
		color = vec3(1.0f, 1.0f, 1.0f);
	}else{
		color = vec3(0.0f, 0.0f, 0.0f);
	}

	gl_FragColor = vec4(color, 1.0f);
}

