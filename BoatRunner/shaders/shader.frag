#version 450

layout(set= 0, binding = 0) uniform globalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 lightDir0; 
	vec3 lightColor0; 
	vec3 lightDir1;
	vec3 lightColor1;
	vec3 lightDir2;
	vec3 lightColor2;
	vec3 lightDir3;
	vec3 lightColor3;

	vec3 AmbColor;	
	vec3 TopColor;	
	vec3 DzColor;
	vec3 DxColor;

	vec3 eyePos;
	vec4 selector;
} gubo;

layout(set= 1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 Case2_Color(vec3 N, vec3 V, vec3 Cd, vec3 Ca) {
	// Lambert Diffuse + Hemispheric
	// No Specular
	// One directional light (lightDir0 and lightColor0)
	// Hemispheric light oriented along the y-axis
	//
	// Parameters are:
	//
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 Cd : main color (diffuse color)
	// vec3 Ca : ambient color

	vec3 HemiDir = vec3(0.0f, 1.0f, 0.0f);
	
	vec3 lambert = Cd * max(dot(gubo.lightDir0, N), 0.0f);
	
	vec3 first_term = lambert * gubo.lightColor0;
	
	vec3 x = ((dot(N, gubo.lightDir0) + 1)* gubo.TopColor)/2;
	vec3 y = ((1 - dot(N, gubo.lightDir0))* gubo.AmbColor)/2; 
	vec3 second_term = (x + y) * Ca;
	
	return (first_term + second_term);
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos.xyz - fragViewDir);

	float AmbFact = 0.025;
	
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb * gubo.selector.w +
				  vec3(1,1,1) * (1-gubo.selector.w);

	vec3 CompColor = gubo.selector.y * Case2_Color(Norm, EyeDir, DifCol, DifCol);
	
	outColor = vec4(CompColor, 1.0f);
}