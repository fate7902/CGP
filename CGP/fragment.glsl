#version 330

out vec4 gl_FragColor; //--- ���� ���
uniform vec3 color;

void main(void)
{
	gl_FragColor = vec4(color,1.0);
}