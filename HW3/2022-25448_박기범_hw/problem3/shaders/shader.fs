#version 330 core

in vec3 worldPos;
in vec3 Normal;

uniform vec3 worldLightPos;
uniform vec3 worldCameraPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
	vec3 ambient = 0.2 * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(worldLightPos - worldPos);
	vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor;

	vec3 viewDir = normalize(worldCameraPos - worldPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 specular = 0.5 * pow(max(dot(viewDir, reflectDir), 0.0), 100) * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}
