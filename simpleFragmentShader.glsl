#version 430 core

in vec3 exPosition;
in vec3 exNormal;
layout (std140) uniform Light {
	vec4 lightPosition;
	vec4 lightColor;
};
uniform vec4 color;

out vec4 outColor;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * vec3(lightColor);
		
	vec3 norm = normalize(exNormal);
	vec3 lightDirection = normalize(vec3(lightPosition) - exPosition);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = diff * vec3(lightColor);
	outColor=vec4(ambient+diffuse,1.0f) * color;
}