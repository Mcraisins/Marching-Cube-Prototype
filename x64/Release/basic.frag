#version 330 core

in vec3 fragpos;
in vec3 fragnormal;
in vec3 fragtangent;
in vec3 fragbitangent;

out vec4 color;

uniform vec3 viewpos;
uniform vec3 usercolor;
uniform float shininess;

struct DirLight {
    vec3 direction, ambient, diffuse, specular;
	bool on;
};
struct PointLight {
    vec3 position, ambient, diffuse, specular;
    float constant, linear, quadratic;
	bool on;
};
uniform DirLight dirLight;
uniform PointLight pointLight;

vec3 dLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 pLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 dLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir =	normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = normalize(reflect(-lightDir, normal)); //normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    return (ambient + diffuse + specular);
}

vec3 pLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal); //normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0; //(light.constant + light.linear * distance + light.quadratic * (distance * distance));   
	
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 newcolor = usercolor;
	vec3 newnormal = normalize(fragnormal);
	vec3 viewdir = normalize(viewpos - newnormal);

	vec3 result;

	if(dirLight.on)
	result += newcolor * dLight(dirLight, newnormal, viewdir);
	if(pointLight.on)
	result += newcolor * pLight(pointLight, newnormal, fragpos, viewdir);

	//newnormal = normalize(fragnormal) * -1;
	//viewdir = normalize(viewpos - newnormal) * -1;

	//if(dirLight.on)
	//result += newcolor * dLight(dirLight, newnormal, viewdir);
	//if(pointLight.on)
	//result += newcolor * pLight(pointLight, newnormal, fragpos, viewdir);

    color = vec4(result, 1.0f);
}