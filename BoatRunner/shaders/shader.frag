#version 450

layout(set= 0, binding = 0) uniform globalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 lightDir; 
	vec3 lightColor; 

	vec3 AmbColor;	
	vec3 TopColor;	

	vec3 eyePos;
} gubo;

layout(set= 1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 Case2_Color(vec3 N, vec3 V, vec3 Cd, vec3 Ca) {
	// Lambert Diffuse + Hemispheric
	// No Specular
	// One directional light (lightDir and lightColor)
	// Hemispheric light oriented along the y-axis
	//
	// Parameters are:
	//
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 Cd : main color (diffuse color)
	// vec3 Ca : ambient color

	vec3 HemiDir = vec3(0.0f, 1.0f, 0.0f);
	
	vec3 lambert = Cd * max(dot(gubo.lightDir, N), 0.0f);
	
	vec3 first_term = lambert * gubo.lightColor;
	
	vec3 x = ((dot(N, gubo.lightDir) + 1)* gubo.TopColor)/2;
	vec3 y = ((1 - dot(N, gubo.lightDir))* gubo.AmbColor)/2; 
	vec3 second_term = (x + y) * Ca;
	
	return (first_term + second_term);
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos.xyz - fragViewDir);

	float AmbFact = 0.25;
	
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb;

	vec3 CompColor = Case2_Color(Norm, EyeDir, DifCol, DifCol);
	
	outColor = vec4(CompColor, 1.0f);
}