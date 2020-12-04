#version 330

out vec4 gl_FragColor; //--- 색상 출력
uniform vec3 color;

void main(void)
{
	gl_FragColor = vec4(color,1.0);
}