#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model; // Model matrix	
uniform mat4 view; // View matrix
uniform mat4 projection; // Projection matrix

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = aNormal;

	// Set the position of the fragment in clip space
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}