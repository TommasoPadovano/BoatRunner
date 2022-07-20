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

vec3 Lambert_Hemispheric_Color(vec3 N, vec3 V, vec3 Cd, vec3 Ca, float gamma) {
	// Lambert Diffuse + Hemispheric
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

	vec3 H1 = normalize(gubo.lightDir + V);
	float clamped = clamp(dot(N, H1), 0, 1);
	float powered = pow(clamped, gamma);
	vec3 blinn = Cd * powered;

	
	return (first_term + blinn + second_term);
}

vec3 Lambert_Ambient_Color(vec3 N, vec3 V, vec3 Cd, vec3 Ca, float gamma) {
	// Lambert Diffuse + Ambient
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

	vec3 H1 = normalize(gubo.lightDir + V);
	float clamped = clamp(dot(N, H1), 0, 1);
	float powered = pow(clamped, gamma);
	vec3 blinn = Cd * powered;

	
	return (first_term + blinn + (gubo.AmbColor * Ca));
}

vec3 OrenNayar_Ambient_Color(vec3 N, vec3 V, vec3 Cd, vec3 Ca, float sigma) {
	// Oren Nayar Diffuse + Ambient
	// No Specular
	// One directional light (lightDir0 and lightColor0)
	//
	// Parameters are:
	//
	// vec3 N : normal vector
	// vec3 V : view direction
	// vec3 Cd : main color (diffuse color)
	// vec3 Ca : ambient color
	// float sigma : roughness of the material

	float teta_i = acos(dot(gubo.lightDir,N));
	float teta_r= acos(dot(V,N));
	
	float alpha = max(teta_i, teta_r);
	float beta = min(teta_i, teta_r);

	float A = (1.0f - 0.5f*(pow(sigma, 2.0f) / (pow(sigma, 2.0f) + 0.33f) ));
	float B = 0.45f * (pow(sigma, 2.0f) / (pow(sigma, 2.0f) + 0.09f) );

	vec3 v_i = normalize(gubo.lightDir - (dot(gubo.lightDir, N)*N));
	vec3 v_r = normalize(V - (dot(V, N)*N));
	
	float G = max(0.0f, dot(v_i, v_r));
	vec3 L = Cd * clamp(dot(gubo.lightDir, N), 0, 1);
	
	vec3 f = L * ( A + (B*G*sin(alpha)*tan(beta)));
	return ((f * gubo.lightColor) + (gubo.AmbColor * Ca));
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos.xyz - fragViewDir);

	//float AmbFact = 0.25;
	
	vec3 DifCol = texture(texSampler, fragTexCoord).rgb;

	//vec3 CompColor = Lambert_Hemispheric_Color(Norm, EyeDir, DifCol, DifCol, 200.0f);
	vec3 CompColor = OrenNayar_Ambient_Color(Norm, EyeDir, DifCol/10, DifCol, 0.03f);
	//vec3 CompColor = Lambert_Ambient_Color(Norm, EyeDir, DifCol, DifCol, 200.0f);

	outColor = vec4(CompColor, 1.0f);
}