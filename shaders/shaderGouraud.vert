#version 140

attribute vec3 vPositionModel; // in object space
attribute vec3 vNormalModel; // in object space

uniform mat4 model; //model 
uniform mat4 view; //camera
uniform mat4 projection;
uniform vec3 eye;

uniform bool spotlight;

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



varying vec3 color;

void main()
{
	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
	//convert vPositionModel and vNormalModel into world space

	vec4 worldCoordvPositionModel = model * vec4(vPositionModel, 1.0);
	vec4 nn = inverse(transpose(model)) * vec4(vNormalModel, 1.0);
	vec3 N = normalize(nn.xyz/nn.w);

	vec3 pos3d = vec3(worldCoordvPositionModel);

	//inverse transpose of model * normal to get the normal from world space  (transpose and inverse model)

	vec3 E = normalize(eye - pos3d);
	vec3 I = vec3(0.0f, 0.0f, 0.0f);
	if (!spotlight){
		for (int i=0; i < NUM_LIGHTS; i++){
			vec3 L =  normalize(lights[i].position - pos3d);
			vec3 R = normalize(2 * (dot(L, N)) * N - L);
			I += lights[i].color * ((kd * max(0, dot(L, N))) + (ks * pow(max(0, dot(R, E)), s)));
		}
		I = ka + I;
	}else{
	


		float spotCoef;
		vec3 Anorm = vec3(0.0f, 0.0f, 0.0f);
		vec3 A = -1* lights[0].position;
			
		Anorm = normalize(A);

		
		
		
		for (int i=0; i < NUM_LIGHTS; i++){

			vec3 L =  normalize(lights[i].position - pos3d);
			vec3 R = normalize(2 * (dot(L, N)) * N - L);
			vec3 negativeL = normalize(-1*L);
			if (dot(negativeL, Anorm) < 0.8){
				spotCoef = 0;
			}else{
				spotCoef = pow(dot(negativeL, Anorm), 3);
			}
			I = I + lights[i].color * ((kd *  dot(L, N)) + (ks * pow(dot(R, E), 3))) * spotCoef;
		}

		I = ka*Anorm + I;
	}

	color = I;
		
}

//vposition 

//r is combination

//Li normalized vertex connecting light to a vertex
//N is normal direction for a vertex
//Ri is reflected light directions corresponding to Li. Compute L from N following equation in slides
//E is direction towards current position of the camera. Compute by finding the normalized vertex from the camera (eye) to vertex
//inverse of transposed matrix glsl
//k is number of lights
