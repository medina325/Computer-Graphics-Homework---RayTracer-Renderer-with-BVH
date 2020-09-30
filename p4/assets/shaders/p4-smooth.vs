#version 330 core

uniform mat3 u_normalMatrix;
uniform mat4 u_vpMatrix = mat4(1);

uniform mat4 u_transform;
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec4 P;
out vec3 N;

void main()
{
	P = u_transform * position;
  N = normalize(u_normalMatrix * normal);

  gl_Position = u_vpMatrix * P;
}