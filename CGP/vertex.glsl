#version 330
//--- in_Position: attribute index 0
//--- in_Color: attribute index 1

layout (location = 0) in vec3 in_Position; //--- 위치 속성
layout (location = 1) in vec3 in_Color; //--- 색상 속성
uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projection;
out vec3 ex_Color; // 프래그먼트 세이더에게 전달

void main(void)
{
	gl_Position = projection * viewTransform * modelTransform * vec4 (in_Position, 1.0);	
	ex_Color = in_Color;
}