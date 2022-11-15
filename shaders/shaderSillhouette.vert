#version 140

attribute vec3 vPositionModel; // in object space
attribute vec3 vNormalModel; // in object space

uniform mat4 model; //model 
uniform mat4 view; //camera
uniform mat4 projection;


varying vec3 pos3d;
varying vec3 N;

void main()
{
	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
	//convert vPositionModel and vNormalModel into world space

	vec4 worldCoordvPositionModel = model * vec4(vPositionModel, 1.0);
	vec4 nn = inverse(transpose(model)) * vec4(vNormalModel, 1.0);
	vec3 N = normalize(nn.xyz/nn.w);

	// vec3 N = vec3(worldCoordinateNormal);
	vec3 pos3d = vec3(worldCoordvPositionModel);
		
}
