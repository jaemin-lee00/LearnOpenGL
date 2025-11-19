#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 direction;
	float	cutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};


out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2	TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 lightDir = normalize(light.position - FragPos);
	
	// check if lighting is inside the spotlight cone
	float theta = dot(lightDir, normalize(-light.direction));

	if(theta > light.cutOff)
	{
		// ambient
		vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
		
		// Diffuse
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
		
		// specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
		
		// attenuation
		float distance = length(light.position - FragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
		
		//ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		
		vec3 result = ambient + diffuse + specular;
		FragColor = vec4(result, 1.0f);
	}
	else
	{
		// else, use ambient light only
		vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
		FragColor = vec4(ambient, 1.0f);
	}
}