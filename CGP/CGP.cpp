#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"glew32")
#pragma comment(lib,"freeglut")

// TEST YOOHOO
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>

GLuint vertexShader;
GLuint fragmentShader;
GLuint ShaderProgramID;
GLchar* vertexSource, * fragmentSource;
GLfloat rColor, gColor, bColor;
GLfloat g_window_w, g_window_h;
GLchar errorLog[512];
GLint width, height;
GLuint s_program;
GLuint vao[3], vbo[2];

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid Timer(int value);

// 여기에 도형 좌표 해주세요
GLfloat line[6][3] = {
	{-65.0f,0.0f,0.0f},{65.0f,0.0f,0.0f},
	{0.0f,65.0f,0.0f},{0.0f,-65.0f,0.0f},
	{0.0f,0.0f,-65.0f},{0.0f,0.0f,65.0f}
};
GLfloat line_colors[6][3] = {
	{1.0,0.0,0.0},{1.0,0.0,0.0},
	{1.0,0.0,0.0},{1.0,0.0,0.0},
	{1.0,0.0,0.0},{1.0,0.0,0.0}
};
GLfloat cube[36][3] = {
	{-0.0f,-0.0f,-0.0f}, // triangle 1 : begin
	{-0.0f,-0.0f, 1.0f },
	{-0.0f, 1.0f, 1.0f}, // triangle 1 : end
	{1.0f, 1.0f,-0.0f}, // triangle 2 : begin
	{-0.0f,-0.0f,-0.0f},
	{ -0.0f, 1.0f,-0.0f}, // triangle 2 : end
	{ 1.0f,-0.0f, 1.0f},
	{-0.0f,-1.0f,-0.0f},
	{ 1.0f,-0.0f,-0.0f},
	{ 1.0f, 1.0f,-0.0f},
	{ 1.0f,-0.0f,-0.0f},
	{ -0.0f,-0.0f,-0.0f},
	{ -0.0f,-0.0f,-0.0f},
	{-0.0f, 1.0f, 1.0f},
	{-0.0f, 1.0f,-0.0f},
	{ 1.0f,-0.0f, 1.0f},
	{ -0.0f,-0.0f, 1.0f},
	{-0.0f,-0.0f,-0.0f},
	{-0.0f, 1.0f, 1.0f},
	{ -0.0f,-0.0f, 1.0f},
	{  1.0f,-0.0f, 1.0f},
	{ 1.0f, 1.0f, 1.0f},
	{ 1.0f,-0.0f,-0.0f},
	{ 1.0f, 1.0f,-0.0f},
	{ 1.0f,-0.0f,-0.0f},
	{1.0f, 1.0f, 1.0f},
	{1.0f,-0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f},
	{ 1.0f, 1.0f,-0.0f},
	{ -0.0f, 1.0f,-0.0f},
	{ 1.0f, 1.0f, 1.0f},
	{ -0.0f, 1.0f,-0.0f},
	{ -0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f},
	{ -0.0f, 1.0f, 1.0f},
	{ 1.0f,-0.0f, 1.0f}
};

GLfloat cubecolors[36][3]{ 0 };

GLfloat rycount = 30.0f;

char* filetobuf(const char* file) {

	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl"); // 버텍스세이더 읽어오기

	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기

	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void InitBuffer()
{
	glGenVertexArrays(3, vao);

	// x,y,z 축	
	glBindVertexArray(vao[0]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), line_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vao[1]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), cubecolors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void InitShader()
{
	make_vertexShaders();
	make_fragmentShaders();
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexShader);
	glAttachShader(s_program, fragmentShader);
	glLinkProgram(s_program);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(s_program);
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	rColor = gColor = bColor = 0.0f;
	//--- 변경된 배경색 설정
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(s_program);

	// 카메라 위치
	glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 0.0f);

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라가 바라보는 곳
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); //--- 카메라 방향 벡터

	glm::vec3 up = glm::vec3(0.0f, 0.0f,-1.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// 원근 투영
	unsigned int projecLocation = glGetUniformLocation(s_program, "projectionTransform");
	glm::mat4 pTransform = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(120.0f), (float)g_window_w / (float)g_window_h, 0.1f, 50.0f);
	//pTransform = glm::translate(pTransform, glm::vec3(0.0f, 0.0f, -10.0f));
	//pTransform = glm::rotate(pTransform, glm::radians(45.f), glm::vec3(1.0f, 0.0f, 0.0f));
	//pTransform = glm::rotate(pTransform, glm::radians(45.f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(projecLocation, 1, GL_FALSE, &pTransform[0][0]);

	// 축 그리기
	glm::mat4 TT = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINES, 0, 6);

	//큐브 그리기
	glm::mat4 CT = glm::mat4(1.0f);
	glm::mat4 MX = glm::mat4(1.0f);
	glm::mat4 RY = glm::mat4(1.0f);
	glm::mat4 CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(32.0f, 0.0f, 1.0));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 5.0f)); //--- model 행렬에 이동 변환 적용
	RY = glm::rotate(RY, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	glm::mat4 CT2 = glm::mat4(1.0f);
	glm::mat4 MX2 = glm::mat4(1.0f);
	glm::mat4 RY2 = glm::mat4(1.0f);
	glm::mat4 CS2 = glm::mat4(1.0f);
	CS2 = glm::scale(CS2, glm::vec3(1.0f, 0.0f, 16.0f));
	MX2 = glm::translate(MX2, glm::vec3(0.0f, 0.0f, 10.0f)); //--- model 행렬에 이동 변환 적용
	RY2 = glm::rotate(RY2, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
	CT2 = MX2 * CS2;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT2)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q':
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
	InitShader();
	InitBuffer();
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	InitShader();
	InitBuffer();
	rycount += 2.0f;
	glutPostRedisplay();
	glutTimerFunc(50, Timer, 1);
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand((unsigned int)time(NULL));
	// 화면 사이즈 조절
	width = 1000;
	height = 1000;

	for (int i = 0; i < 36; ++i) {
		cubecolors[i][0] = 0.1f;
		cubecolors[i][1] = 0.8f;
		cubecolors[i][2] = 0.8f;
	}

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();

	glutDisplayFunc(drawScene); // 출력 함수의 지정	
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(KeyBoard);
	//glutTimerFunc(50, Timer, 1);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}