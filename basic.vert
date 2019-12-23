#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;

out vec3 fragpos;
out vec3 fragnormal;
out vec3 fragtangent;
out vec3 fragbitangent;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragpos = position;
	fragnormal = normal;
	fragtangent = tangent;
	fragbitangent = bitangent;

    gl_Position = projection * view * vec4(fragpos, 1.0);
}