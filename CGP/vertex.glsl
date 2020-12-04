#version 330

layout (location = 0) in vec3 in_Position; //--- 위치 속성

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main(void)
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4 (in_Position, 1.0);
}