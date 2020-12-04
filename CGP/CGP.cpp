#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"glew32")
#pragma comment(lib,"freeglut")

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

// 추가 변수 여기에 선언해주세요
GLfloat dx = 0.f, dy = 0.f, dz = 0.f;
GLboolean camera_set = GL_FALSE;

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
GLfloat floorcolors[36][3] = { 0 };

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

	// wall
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

	// floor
	glBindVertexArray(vao[2]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), floorcolors, GL_STATIC_DRAW);
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
	rColor = gColor = bColor = 1.0f;
	//--- 변경된 배경색 설정
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(s_program);

	// 카메라 위치
	glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 0.0f); // 상공	
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // 상공카메라가 바라보는 곳	
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); //--- 카메라 방향 벡터
	glm::vec3 up = glm::vec3(0.0f, 0.0f,-1.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	glm::vec3 cameraPos2 = glm::vec3(0.0f, 2.0f, 0.0f); // 주인공
	glm::vec3 cameraTarget2 = glm::vec3(50.0f, 2.0f, 0.0f); // 주인공카메라가 바라보는 곳
	glm::vec3 cameraUp2 = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 transPos = glm::mat4(1.0f);
	if (!camera_set) // 상공시점
	{
		transPos = glm::translate(transPos, glm::vec3(dx, dy, dz));
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		view = transPos * view;
	}
	else // 주인공 시점
	{
		transPos = glm::translate(transPos, glm::vec3(dx, dy, dz));
		view = glm::lookAt(cameraPos2, cameraTarget2, cameraUp2);
		view = transPos * view;
	}	
	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	// 원근 투영
	unsigned int projecLocation = glGetUniformLocation(s_program, "projectionTransform");
	glm::mat4 pTransform = glm::mat4(1.0f);
	pTransform = glm::perspective(glm::radians(120.0f), (float)g_window_w / (float)g_window_h, 0.1f, 50.0f);
	glUniformMatrix4fv(projecLocation, 1, GL_FALSE, &pTransform[0][0]);

	// 축 그리기
	glm::mat4 TT = glm::mat4(1.0f);
	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINES, 0, 6);

	//큐브 그리기
	// wall1
	glm::mat4 CT = glm::mat4(1.0f);
	glm::mat4 MX = glm::mat4(1.0f);
	glm::mat4 CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(32.0f, 10.0f, 1.0));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 5.0f)); //--- model 행렬에 이동 변환 적용
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	// wall2
	glm::mat4 CT2 = glm::mat4(1.0f);
	glm::mat4 MX2 = glm::mat4(1.0f);
	glm::mat4 CS2 = glm::mat4(1.0f);
	CS2 = glm::scale(CS2, glm::vec3(1.0f, 10.0f, 16.0f));
	MX2 = glm::translate(MX2, glm::vec3(0.0f, 0.0f, 9.0f)); //--- model 행렬에 이동 변환 적용
	CT2 = MX2 * CS2;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT2)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	// wall3
	glm::mat4 CT3 = glm::mat4(1.0f);
	glm::mat4 MX3 = glm::mat4(1.0f);
	glm::mat4 CS3 = glm::mat4(1.0f);
	CS3 = glm::scale(CS3, glm::vec3(32.0f, 10.0f, 1.0f));
	MX3 = glm::translate(MX3, glm::vec3(0.0f, 0.0f, 25.0f)); //--- model 행렬에 이동 변환 적용
	CT3 = MX3 * CS3;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT3)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall4
	glm::mat4 CT4 = glm::mat4(1.0f);
	glm::mat4 MX4 = glm::mat4(1.0f);
	glm::mat4 CS4 = glm::mat4(1.0f);
	CS4 = glm::scale(CS4, glm::vec3(1.0f, 10.0f, 16.0f));
	MX4 = glm::translate(MX4, glm::vec3(16.0f, 0.0f, 6.0f)); //--- model 행렬에 이동 변환 적용
	CT4 = MX4 * CS4;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT4)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall5
	glm::mat4 CT5 = glm::mat4(1.0f);
	glm::mat4 MX5 = glm::mat4(1.0f);
	glm::mat4 CS5 = glm::mat4(1.0f);
	CS5 = glm::scale(CS5, glm::vec3(1.0f, 10.0f, 16.0f));
	MX5 = glm::translate(MX5, glm::vec3(20.0f, 0.0f, 9.0f)); //--- model 행렬에 이동 변환 적용
	CT5 = MX5 * CS5;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT5)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall6
	glm::mat4 CT6 = glm::mat4(1.0f);
	glm::mat4 MX6 = glm::mat4(1.0f);
	glm::mat4 CS6 = glm::mat4(1.0f);
	CS6 = glm::scale(CS6, glm::vec3(1.0f, 10.0f, 8.0f));
	MX6 = glm::translate(MX6, glm::vec3(4.0f, 0.0f, 14.0f)); //--- model 행렬에 이동 변환 적용
	CT6 = MX6 * CS6;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT6)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall7
	glm::mat4 CT7 = glm::mat4(1.0f);
	glm::mat4 MX7 = glm::mat4(1.0f);
	glm::mat4 CS7 = glm::mat4(1.0f);
	CS7 = glm::scale(CS7, glm::vec3(1.0f, 10.0f, 8.0f));
	MX7 = glm::translate(MX7, glm::vec3(12.0f, 0.0f, 17.0f)); //--- model 행렬에 이동 변환 적용
	CT7 = MX7 * CS7;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT7)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall8
	glm::mat4 CT8 = glm::mat4(1.0f);
	glm::mat4 MX8 = glm::mat4(1.0f);
	glm::mat4 CS8 = glm::mat4(1.0f);
	CS8 = glm::scale(CS8, glm::vec3(1.0f, 10.0f, 8.0f));
	MX8 = glm::translate(MX8, glm::vec3(28.0f, 0.0f, 9.0f)); //--- model 행렬에 이동 변환 적용
	CT8 = MX8 * CS8;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT8)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall9	
	glm::mat4 CT9 = glm::mat4(1.0f);
	glm::mat4 MX9 = glm::mat4(1.0f);
	glm::mat4 CS9 = glm::mat4(1.0f);
	CS9 = glm::scale(CS9, glm::vec3(1.0f, 10.0f, 4.0f));
	MX9 = glm::translate(MX9, glm::vec3(8.0f, 0.0f, 9.0f)); //--- model 행렬에 이동 변환 적용
	CT9 = MX9 * CS9;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT9)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall10	
	glm::mat4 CT10 = glm::mat4(1.0f);
	glm::mat4 MX10 = glm::mat4(1.0f);
	glm::mat4 CS10 = glm::mat4(1.0f);
	CS10 = glm::scale(CS10, glm::vec3(1.0f, 10.0f, 4.0f));
	MX10 = glm::translate(MX10, glm::vec3(24.0f, 0.0f, 18.0f)); //--- model 행렬에 이동 변환 적용
	CT10 = MX10 * CS10;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT10)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall11	
	glm::mat4 CT11 = glm::mat4(1.0f);
	glm::mat4 MX11 = glm::mat4(1.0f);
	glm::mat4 CS11 = glm::mat4(1.0f);
	CS11 = glm::scale(CS11, glm::vec3(1.0f, 10.0f, 4.0f));
	MX11 = glm::translate(MX11, glm::vec3(28.0f, 0.0f, 21.0f)); //--- model 행렬에 이동 변환 적용
	CT11 = MX11 * CS11;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT11)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall12	
	glm::mat4 CT12 = glm::mat4(1.0f);
	glm::mat4 MX12 = glm::mat4(1.0f);
	glm::mat4 CS12 = glm::mat4(1.0f);
	CS12 = glm::scale(CS12, glm::vec3(1.0f, 10.0f, 15.0f));
	MX12 = glm::translate(MX12, glm::vec3(32.0f, 0.0f, 1.0f)); //--- model 행렬에 이동 변환 적용
	CT12 = MX12 * CS12;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT12)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall13	
	glm::mat4 CT13 = glm::mat4(1.0f);
	glm::mat4 MX13 = glm::mat4(1.0f);
	glm::mat4 CS13 = glm::mat4(1.0f);
	CS13 = glm::scale(CS13, glm::vec3(1.0f, 10.0f, 12.0f));
	MX13 = glm::translate(MX13, glm::vec3(32.0f, 0.0f, 20.0f)); //--- model 행렬에 이동 변환 적용
	CT13 = MX13 * CS13;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT13)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall14	
	glm::mat4 CT14 = glm::mat4(1.0f);
	glm::mat4 MX14 = glm::mat4(1.0f);
	glm::mat4 CS14 = glm::mat4(1.0f);
	CS14 = glm::scale(CS14, glm::vec3(32.0f, 10.0f, 1.0f));
	MX14 = glm::translate(MX14, glm::vec3(32.0f, 0.0f, 0.0f)); //--- model 행렬에 이동 변환 적용
	CT14 = MX14 * CS14;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT14)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall15
	glm::mat4 CT15 = glm::mat4(1.0f);
	glm::mat4 MX15 = glm::mat4(1.0f);
	glm::mat4 CS15 = glm::mat4(1.0f);
	CS15 = glm::scale(CS15, glm::vec3(32.0f, 10.0f, 1.0f));
	MX15 = glm::translate(MX15, glm::vec3(32.0f, 0.0f, 32.0f)); //--- model 행렬에 이동 변환 적용
	CT15 = MX15 * CS15;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT15)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall16	
	glm::mat4 CT16 = glm::mat4(1.0f);
	glm::mat4 MX16 = glm::mat4(1.0f);
	glm::mat4 CS16 = glm::mat4(1.0f);
	CS16 = glm::scale(CS16, glm::vec3(12.0f, 10.0f, 1.0f));
	MX16 = glm::translate(MX16, glm::vec3(4.0f, 0.0f, 13.0f)); //--- model 행렬에 이동 변환 적용
	CT16 = MX16 * CS16;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT16)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall17	
	glm::mat4 CT17 = glm::mat4(1.0f);
	glm::mat4 MX17 = glm::mat4(1.0f);
	glm::mat4 CS17 = glm::mat4(1.0f);
	CS17 = glm::scale(CS17, glm::vec3(4.0f, 10.0f, 1.0f));
	MX17 = glm::translate(MX17, glm::vec3(5.0f, 0.0f, 21.0f)); //--- model 행렬에 이동 변환 적용
	CT17 = MX17 * CS17;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT17)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall18	
	glm::mat4 CT18 = glm::mat4(1.0f);
	glm::mat4 MX18 = glm::mat4(1.0f);
	glm::mat4 CS18 = glm::mat4(1.0f);
	CS18 = glm::scale(CS18, glm::vec3(4.0f, 10.0f, 1.0f));
	MX18 = glm::translate(MX18, glm::vec3(8.0f, 0.0f, 17.0f)); //--- model 행렬에 이동 변환 적용
	CT18 = MX18 * CS18;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT18)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall19	
	glm::mat4 CT19 = glm::mat4(1.0f);
	glm::mat4 MX19 = glm::mat4(1.0f);
	glm::mat4 CS19 = glm::mat4(1.0f);
	CS19 = glm::scale(CS19, glm::vec3(4.0f, 10.0f, 1.0f));
	MX19 = glm::translate(MX19, glm::vec3(9.0f, 0.0f, 9.0f)); //--- model 행렬에 이동 변환 적용
	CT19 = MX19 * CS19;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT19)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall20	
	glm::mat4 CT20 = glm::mat4(1.0f);
	glm::mat4 MX20 = glm::mat4(1.0f);
	glm::mat4 CS20 = glm::mat4(1.0f);
	CS20 = glm::scale(CS20, glm::vec3(4.0f, 10.0f, 1.0f));
	MX20 = glm::translate(MX20, glm::vec3(21.0f, 0.0f, 13.0f)); //--- model 행렬에 이동 변환 적용
	CT20 = MX20 * CS20;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT20)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall21	
	glm::mat4 CT21 = glm::mat4(1.0f);
	glm::mat4 MX21 = glm::mat4(1.0f);
	glm::mat4 CS21 = glm::mat4(1.0f);
	CS21 = glm::scale(CS21, glm::vec3(4.0f, 10.0f, 1.0f));
	MX21 = glm::translate(MX21, glm::vec3(24.0f, 0.0f, 9.0f)); //--- model 행렬에 이동 변환 적용
	CT21 = MX21 * CS21;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT21)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall22	
	glm::mat4 CT22 = glm::mat4(1.0f);
	glm::mat4 MX22 = glm::mat4(1.0f);
	glm::mat4 CS22 = glm::mat4(1.0f);
	CS22 = glm::scale(CS22, glm::vec3(5.0f, 10.0f, 1.0f));
	MX22 = glm::translate(MX22, glm::vec3(24.0f, 0.0f, 17.0f)); //--- model 행렬에 이동 변환 적용
	CT22 = MX22 * CS22;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT22)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall23	
	glm::mat4 CT23 = glm::mat4(1.0f);
	glm::mat4 MX23 = glm::mat4(1.0f);
	glm::mat4 CS23 = glm::mat4(1.0f);
	CS23 = glm::scale(CS23, glm::vec3(8.0f, 10.0f, 1.0f));
	MX23 = glm::translate(MX23, glm::vec3(29.0f, 0.0f, 16.0f)); //--- model 행렬에 이동 변환 적용
	CT23 = MX23 * CS23;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT23)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall24	
	glm::mat4 CT24 = glm::mat4(1.0f);
	glm::mat4 MX24 = glm::mat4(1.0f);
	glm::mat4 CS24 = glm::mat4(1.0f);
	CS24 = glm::scale(CS24, glm::vec3(8.0f, 10.0f, 1.0f));
	MX24 = glm::translate(MX24, glm::vec3(33.0f, 0.0f, 20.0f)); //--- model 행렬에 이동 변환 적용
	CT24 = MX24 * CS24;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT24)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall25	
	glm::mat4 CT25 = glm::mat4(1.0f);
	glm::mat4 MX25 = glm::mat4(1.0f);
	glm::mat4 CS25 = glm::mat4(1.0f);
	CS25 = glm::scale(CS25, glm::vec3(8.0f, 10.0f, 1.0f));
	MX25 = glm::translate(MX25, glm::vec3(44.0f, 0.0f, 20.0f)); //--- model 행렬에 이동 변환 적용
	CT25 = MX25 * CS25;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT25)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall26	
	glm::mat4 CT26 = glm::mat4(1.0f);
	glm::mat4 MX26 = glm::mat4(1.0f);
	glm::mat4 CS26 = glm::mat4(1.0f);
	CS26 = glm::scale(CS26, glm::vec3(8.0f, 10.0f, 1.0f));
	MX26 = glm::translate(MX26, glm::vec3(36.0f, 0.0f, 28.0f)); //--- model 행렬에 이동 변환 적용
	CT26 = MX26 * CS26;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT26)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall27	
	glm::mat4 CT27 = glm::mat4(1.0f);
	glm::mat4 MX27 = glm::mat4(1.0f);
	glm::mat4 CS27 = glm::mat4(1.0f);
	CS27 = glm::scale(CS27, glm::vec3(8.0f, 10.0f, 1.0f));
	MX27 = glm::translate(MX27, glm::vec3(52.0f, 0.0f, 13.0f)); //--- model 행렬에 이동 변환 적용
	CT27 = MX27 * CS27;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT27)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall28	
	glm::mat4 CT28 = glm::mat4(1.0f);
	glm::mat4 MX28 = glm::mat4(1.0f);
	glm::mat4 CS28 = glm::mat4(1.0f);
	CS28 = glm::scale(CS28, glm::vec3(8.0f, 10.0f, 1.0f));
	MX28 = glm::translate(MX28, glm::vec3(53.0f, 0.0f, 4.0f)); //--- model 행렬에 이동 변환 적용
	CT28 = MX28 * CS28;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT28)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall29	
	glm::mat4 CT29 = glm::mat4(1.0f);
	glm::mat4 MX29 = glm::mat4(1.0f);
	glm::mat4 CS29 = glm::mat4(1.0f);
	CS29 = glm::scale(CS29, glm::vec3(13.0f, 10.0f, 1.0f));
	MX29 = glm::translate(MX29, glm::vec3(36.0f, 0.0f, 24.0f)); //--- model 행렬에 이동 변환 적용
	CT29 = MX29 * CS29;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT29)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall30	
	glm::mat4 CT30 = glm::mat4(1.0f);
	glm::mat4 MX30 = glm::mat4(1.0f);
	glm::mat4 CS30 = glm::mat4(1.0f);
	CS30 = glm::scale(CS30, glm::vec3(11.0f, 10.0f, 1.0f));
	MX30 = glm::translate(MX30, glm::vec3(37.0f, 0.0f, 8.0f)); //--- model 행렬에 이동 변환 적용
	CT30 = MX30 * CS30;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT30)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall31	
	glm::mat4 CT31 = glm::mat4(1.0f);
	glm::mat4 MX31 = glm::mat4(1.0f);
	glm::mat4 CS31 = glm::mat4(1.0f);
	CS31 = glm::scale(CS31, glm::vec3(13.0f, 10.0f, 1.0f));
	MX31 = glm::translate(MX31, glm::vec3(48.0f, 0.0f, 28.0f)); //--- model 행렬에 이동 변환 적용
	CT31 = MX31 * CS31;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT31)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall32	
	glm::mat4 CT32 = glm::mat4(1.0f);
	glm::mat4 MX32 = glm::mat4(1.0f);
	glm::mat4 CS32 = glm::mat4(1.0f);
	CS32 = glm::scale(CS32, glm::vec3(3.0f, 10.0f, 1.0f));
	MX32 = glm::translate(MX32, glm::vec3(36.0f, 0.0f, 4.0f)); //--- model 행렬에 이동 변환 적용
	CT32 = MX32 * CS32;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT32)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall33	
	glm::mat4 CT33 = glm::mat4(1.0f);
	glm::mat4 MX33 = glm::mat4(1.0f);
	glm::mat4 CS33 = glm::mat4(1.0f);
	CS33 = glm::scale(CS33, glm::vec3(5.0f, 10.0f, 1.0f));
	MX33 = glm::translate(MX33, glm::vec3(43.0f, 0.0f, 4.0f)); //--- model 행렬에 이동 변환 적용
	CT33 = MX33 * CS33;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT33)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall34	
	glm::mat4 CT34 = glm::mat4(1.0f);
	glm::mat4 MX34 = glm::mat4(1.0f);
	glm::mat4 CS34 = glm::mat4(1.0f);
	CS34 = glm::scale(CS34, glm::vec3(4.0f, 10.0f, 1.0f));
	MX34 = glm::translate(MX34, glm::vec3(41.0f, 0.0f, 12.0f)); //--- model 행렬에 이동 변환 적용
	CT34 = MX34 * CS34;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT34)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall35	
	glm::mat4 CT35 = glm::mat4(1.0f);
	glm::mat4 MX35 = glm::mat4(1.0f);
	glm::mat4 CS35 = glm::mat4(1.0f);
	CS35 = glm::scale(CS35, glm::vec3(4.0f, 10.0f, 1.0f));
	MX35 = glm::translate(MX35, glm::vec3(45.0f, 0.0f, 16.0f)); //--- model 행렬에 이동 변환 적용
	CT35 = MX35 * CS35;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT35)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall36	
	glm::mat4 CT36 = glm::mat4(1.0f);
	glm::mat4 MX36 = glm::mat4(1.0f);
	glm::mat4 CS36 = glm::mat4(1.0f);
	CS36 = glm::scale(CS36, glm::vec3(3.0f, 10.0f, 1.0f));
	MX36 = glm::translate(MX36, glm::vec3(53.0f, 0.0f, 17.0f)); //--- model 행렬에 이동 변환 적용
	CT36 = MX36 * CS36;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT36)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall37	
	glm::mat4 CT37 = glm::mat4(1.0f);
	glm::mat4 MX37 = glm::mat4(1.0f);
	glm::mat4 CS37 = glm::mat4(1.0f);
	CS37 = glm::scale(CS37, glm::vec3(4.0f, 10.0f, 1.0f));
	MX37 = glm::translate(MX37, glm::vec3(57.0f, 0.0f, 24.0f)); //--- model 행렬에 이동 변환 적용
	CT37 = MX37 * CS37;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT37)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall38	
	glm::mat4 CT38 = glm::mat4(1.0f);
	glm::mat4 MX38 = glm::mat4(1.0f);
	glm::mat4 CS38 = glm::mat4(1.0f);
	CS38 = glm::scale(CS38, glm::vec3(4.0f, 10.0f, 1.0f));
	MX38 = glm::translate(MX38, glm::vec3(60.0f, 0.0f, 8.0f)); //--- model 행렬에 이동 변환 적용
	CT38 = MX38 * CS38;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT38)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall39	
	glm::mat4 CT39 = glm::mat4(1.0f);
	glm::mat4 MX39 = glm::mat4(1.0f);
	glm::mat4 CS39 = glm::mat4(1.0f);
	CS39 = glm::scale(CS39, glm::vec3(4.0f, 10.0f, 1.0f));
	MX39 = glm::translate(MX39, glm::vec3(61.0f, 0.0f, 20.0f)); //--- model 행렬에 이동 변환 적용
	CT39 = MX39 * CS39;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT39)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall40	
	glm::mat4 CT40 = glm::mat4(1.0f);
	glm::mat4 MX40 = glm::mat4(1.0f);
	glm::mat4 CS40 = glm::mat4(1.0f);
	CS40 = glm::scale(CS40, glm::vec3(1.0f, 10.0f, 3.0f));
	MX40 = glm::translate(MX40, glm::vec3(36.0f, 0.0f, 29.0f)); //--- model 행렬에 이동 변환 적용
	CT40 = MX40 * CS40;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT40)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall41	
	glm::mat4 CT41 = glm::mat4(1.0f);
	glm::mat4 MX41 = glm::mat4(1.0f);
	glm::mat4 CS41 = glm::mat4(1.0f);
	CS41 = glm::scale(CS41, glm::vec3(1.0f, 10.0f, 4.0f));
	MX41 = glm::translate(MX41, glm::vec3(39.0f, 0.0f, 1.0f)); //--- model 행렬에 이동 변환 적용
	CT41 = MX41 * CS41;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT41)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall42	
	glm::mat4 CT42 = glm::mat4(1.0f);
	glm::mat4 MX42 = glm::mat4(1.0f);
	glm::mat4 CS42 = glm::mat4(1.0f);
	CS42 = glm::scale(CS42, glm::vec3(1.0f, 10.0f, 4.0f));
	MX42 = glm::translate(MX42, glm::vec3(44.0f, 0.0f, 16.0f)); //--- model 행렬에 이동 변환 적용
	CT42 = MX42 * CS42;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT42)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall43	
	glm::mat4 CT43 = glm::mat4(1.0f);
	glm::mat4 MX43 = glm::mat4(1.0f);
	glm::mat4 CS43 = glm::mat4(1.0f);
	CS43 = glm::scale(CS43, glm::vec3(1.0f, 10.0f, 4.0f));
	MX43 = glm::translate(MX43, glm::vec3(44.0f, 0.0f, 25.0f)); //--- model 행렬에 이동 변환 적용
	CT43 = MX43 * CS43;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT43)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall44	
	glm::mat4 CT44 = glm::mat4(1.0f);
	glm::mat4 MX44 = glm::mat4(1.0f);
	glm::mat4 CS44 = glm::mat4(1.0f);
	CS44 = glm::scale(CS44, glm::vec3(1.0f, 10.0f, 5.0f));
	MX44 = glm::translate(MX44, glm::vec3(56.0f, 0.0f, 5.0f)); //--- model 행렬에 이동 변환 적용
	CT44 = MX44 * CS44;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT44)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall45	
	glm::mat4 CT45 = glm::mat4(1.0f);
	glm::mat4 MX45 = glm::mat4(1.0f);
	glm::mat4 CS45 = glm::mat4(1.0f);
	CS45 = glm::scale(CS45, glm::vec3(1.0f, 10.0f, 3.0f));
	MX45 = glm::translate(MX45, glm::vec3(56.0f, 0.0f, 29.0f)); //--- model 행렬에 이동 변환 적용
	CT45 = MX45 * CS45;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT45)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall46	
	glm::mat4 CT46 = glm::mat4(1.0f);
	glm::mat4 MX46 = glm::mat4(1.0f);
	glm::mat4 CS46 = glm::mat4(1.0f);
	CS46 = glm::scale(CS46, glm::vec3(1.0f, 10.0f, 8.0f));
	MX46 = glm::translate(MX46, glm::vec3(36.0f, 0.0f, 8.0f)); //--- model 행렬에 이동 변환 적용
	CT46 = MX46 * CS46;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT46)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall47	
	glm::mat4 CT47 = glm::mat4(1.0f);
	glm::mat4 MX47 = glm::mat4(1.0f);
	glm::mat4 CS47 = glm::mat4(1.0f);
	CS47 = glm::scale(CS47, glm::vec3(1.0f, 10.0f, 8.0f));
	MX47 = glm::translate(MX47, glm::vec3(40.0f, 0.0f, 12.0f)); //--- model 행렬에 이동 변환 적용
	CT47 = MX47 * CS47;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT47)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall48	
	glm::mat4 CT48 = glm::mat4(1.0f);
	glm::mat4 MX48 = glm::mat4(1.0f);
	glm::mat4 CS48 = glm::mat4(1.0f);
	CS48 = glm::scale(CS48, glm::vec3(1.0f, 10.0f, 12.0f));
	MX48 = glm::translate(MX48, glm::vec3(48.0f, 0.0f, 4.0f)); //--- model 행렬에 이동 변환 적용
	CT48 = MX48 * CS48;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT48)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall49	
	glm::mat4 CT49 = glm::mat4(1.0f);
	glm::mat4 MX49 = glm::mat4(1.0f);
	glm::mat4 CS49 = glm::mat4(1.0f);
	CS49 = glm::scale(CS49, glm::vec3(1.0f, 10.0f, 8.0f));
	MX49 = glm::translate(MX49, glm::vec3(52.0f, 0.0f, 17.0f)); //--- model 행렬에 이동 변환 적용
	CT49 = MX49 * CS49;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT49)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall50	
	glm::mat4 CT50 = glm::mat4(1.0f);
	glm::mat4 MX50 = glm::mat4(1.0f);
	glm::mat4 CS50 = glm::mat4(1.0f);
	CS50 = glm::scale(CS50, glm::vec3(1.0f, 10.0f, 8.0f));
	MX50 = glm::translate(MX50, glm::vec3(56.0f, 0.0f, 17.0f)); //--- model 행렬에 이동 변환 적용
	CT50 = MX50 * CS50;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT50)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall51	
	glm::mat4 CT51 = glm::mat4(1.0f);
	glm::mat4 MX51 = glm::mat4(1.0f);
	glm::mat4 CS51 = glm::mat4(1.0f);
	CS51 = glm::scale(CS51, glm::vec3(1.0f, 10.0f, 9.0f));
	MX51 = glm::translate(MX51, glm::vec3(52.0f, 0.0f, 4.0f)); //--- model 행렬에 이동 변환 적용
	CT51 = MX51 * CS51;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT51)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall52	
	glm::mat4 CT52 = glm::mat4(1.0f);
	glm::mat4 MX52 = glm::mat4(1.0f);
	glm::mat4 CS52 = glm::mat4(1.0f);
	CS52 = glm::scale(CS52, glm::vec3(1.0f, 10.0f, 8.0f));
	MX52 = glm::translate(MX52, glm::vec3(60.0f, 0.0f, 13.0f)); //--- model 행렬에 이동 변환 적용
	CT52 = MX52 * CS52;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT52)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall53	
	glm::mat4 CT53 = glm::mat4(1.0f);
	glm::mat4 MX53 = glm::mat4(1.0f);
	glm::mat4 CS53 = glm::mat4(1.0f);
	CS53 = glm::scale(CS53, glm::vec3(1.0f, 10.0f, 17.0f));
	MX53 = glm::translate(MX53, glm::vec3(64.0f, 0.0f, 0.0f)); //--- model 행렬에 이동 변환 적용
	CT53 = MX53 * CS53;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT53)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//wall54	
	glm::mat4 CT54 = glm::mat4(1.0f);
	glm::mat4 MX54 = glm::mat4(1.0f);
	glm::mat4 CS54 = glm::mat4(1.0f);
	CS54 = glm::scale(CS54, glm::vec3(1.0f, 10.0f, 12.0f));
	MX54 = glm::translate(MX54, glm::vec3(64.0f, 0.0f, 21.0f)); //--- model 행렬에 이동 변환 적용
	CT54 = MX54 * CS54;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT54)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	//floor55	
	glm::mat4 CT55 = glm::mat4(1.0f);
	glm::mat4 MX55 = glm::mat4(1.0f);
	glm::mat4 CS55 = glm::mat4(1.0f);
	CS55 = glm::scale(CS55, glm::vec3(65.0f, 1.0f, 33.0f));
	MX55 = glm::translate(MX55, glm::vec3(0.0f, 0.0f, 0.0f)); //--- model 행렬에 이동 변환 적용
	CT55 = MX55 * CS55;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT55)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기

	/*
	//roof56	
	glm::mat4 CT56 = glm::mat4(1.0f);
	glm::mat4 MX56 = glm::mat4(1.0f);
	glm::mat4 CS56 = glm::mat4(1.0f);
	CS56 = glm::scale(CS56, glm::vec3(65.0f, 1.0f, 33.0f));
	MX56 = glm::translate(MX56, glm::vec3(0.0f, 10.0f, 0.0f)); //--- model 행렬에 이동 변환 적용
	CT56 = MX56 * CS56;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	//--- 버텍스 세이더에서 modelTransform 변수 위치 가져오기
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT56)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36); //--- 도형 그리기
	*/

	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w' | 'W':
		dz += 1.f;
		break;
	case 'a' | 'A':
		dx += 1.f;
		break;
	case 's' | 'S':
		dz -= 1.f;
		break;
	case 'd' | 'D':
		dx -= 1.f;
		break;
	case 't' | 'T': // 카메라 시점 바꾸기
		if (!camera_set) 
		{
			camera_set = GL_TRUE; // 주인공 시점
		}
		else
		{
			camera_set = GL_FALSE; // 상공 시점
		}
		break;
	case 'q':
	case 'Q':
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
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
		cubecolors[i][0] = 0.2f;
		cubecolors[i][1] = 0.4f;
		cubecolors[i][2] = 0.7f;
	}

	for (int i = 0; i < 36; ++i) {
		floorcolors[i][0] = 0.5f;
		floorcolors[i][1] = 0.4f;
		floorcolors[i][2] = 0.4f;
	}

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(20, 20); // 윈도우의 위치 지정
	glutInitWindowSize(width, height); // 윈도우의 크기 지정
	glutCreateWindow("CGP"); // 윈도우 생성(윈도우 이름)

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