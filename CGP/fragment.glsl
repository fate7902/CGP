#version 330

in vec3 ex_Color; //--- 버텍스 세이더에게서 전달 받음
out vec4 gl_FragColor; //--- 색상 출력

void main(void)
{
	gl_FragColor = vec4(ex_Color,1.0);
}