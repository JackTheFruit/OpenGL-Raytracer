#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

struct Light {
	int type;
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Light dirLight;
uniform Light pointLights[4];
uniform Light spotLight;

//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
//{
//vec3 lightDir = normalize(-light.direction);
//// diffuse shading
//float diff = max(dot(normal, lightDir), 0.0);
//// specular shading
//vec3 reflectDir = reflect(-lightDir, normal);
//float spec = pow(max(dot(viewDir, reflectDir), 0.0),
//material.shininess);
//// combine results
//vec3 ambient = light.ambient * vec3(texture(material.diffuse,
//TexCoords));
//vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,
//TexCoords));
//vec3 specular = light.specular * spec * vec3(texture(material.specular,
//TexCoords));
//return (ambient + diffuse + specular);
//}
//
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3
//viewDir)
//{
//vec3 lightDir = normalize(light.position - fragPos);
//// diffuse shading
//float diff = max(dot(normal, lightDir), 0.0);
//// specular shading
//vec3 reflectDir = reflect(-lightDir, normal);
//float spec = pow(max(dot(viewDir, reflectDir), 0.0),
//material.shininess);
//// attenuation
//float distance = length(light.position - fragPos);
//float attenuation = 1.0 / (light.constant + light.linear * distance +
//light.quadratic * (distance * distance));
//// combine results
//vec3 ambient = light.ambient * vec3(texture(material.diffuse,
//TexCoords));
//vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,
//TexCoords));
//vec3 specular = light.specular * spec * vec3(texture(material.specular,
//TexCoords));
//ambient *= attenuation;
//diffuse *= attenuation;
//specular *= attenuation;
//return (ambient + diffuse + specular);
//}

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir;
	if(light.type == 0)
		lightDir = normalize(-light.direction);
	else
		lightDir = normalize(light.position - FragPos);
	
	float theta = dot(lightDir, normalize(-light.direction));
	if((theta > light.outerCutOff)||(light.type != 2))
	{
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0f);

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, Normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

		vec3 ambient = texture(material.diffuse,TexCoords).rgb * light.ambient;
		vec3 diffuse = light.diffuse * (diff * texture(material.diffuse,TexCoords).rgb);
		vec3 specular = light.specular * (spec * texture(material.specular,TexCoords).rgb);

		if(light.type == 1) {
			float distance = length(light.position - fragPos);
			float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
			
			ambient *= attenuation;
			diffuse *= attenuation;
			specular *= attenuation;
		}
		if(light.type == 2) {
			float epsilon = light.cutOff - light.outerCutOff;
			float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

			diffuse *= intensity;
			specular *= intensity;
		}

		return(ambient + diffuse + specular);
	}
	else // use ambient light so scene isn’t black outside the spotlight.
		return(light.ambient * vec3(texture(material.diffuse,TexCoords)));
}

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	// phase 1: Directional lighting
	vec3 result = CalcLight(dirLight, norm, FragPos,viewDir);
	// phase 2: Point lights
	for(int i = 0; i < 4; i++)
		result += CalcLight(pointLights[i], norm, FragPos, viewDir);
	// phase 3: Spot light
	result += CalcLight(spotLight, norm, FragPos, viewDir);
	FragColor = vec4(result, 1.0);
}