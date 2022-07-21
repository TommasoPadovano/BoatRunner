#version 450

layout(set= 1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	const vec3  diffColor = texture(texSampler, fragTexCoord).rgb;
	const vec3  specColor = vec3(1.0f, 1.0f, 1.0f);
	const float specPower = 150.0f;
	const vec3  L = normalize(vec3(0.0f, 20.0f, 25.0f) - fragViewDir);
	
	vec3 N = normalize(fragNorm);
	vec3 R = -reflect(L, N);
	vec3 V = normalize(fragViewDir);
	
	// Lambert diffuse
	//vec3 diffuse  = diffColor * max(dot(N,L), 0.0f) * 1.5;
	// Phong specular
	//vec3 specular = specColor * pow(max(dot(R,V), 0.0f), specPower);
	// Hemispheric ambient
	vec3 ambient  = vec3(0.7f,0.7f, 0.7f) * diffColor;
	
	outColor = vec4(clamp(ambient, vec3(0.0f), vec3(1.0f)), 1.0f);
}