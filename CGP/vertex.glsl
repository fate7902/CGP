#version 330

layout (location = 0) in vec3 in_Position; //--- ��ġ �Ӽ�
layout (location = 1) in vec3 in_Color; //--- ���� �Ӽ�

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

out vec3 ex_Color; // �����׸�Ʈ ���̴����� ����

void main(void)
{
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4 (in_Position, 1.0);	
	ex_Color = in_Color;
}