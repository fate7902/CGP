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

// 정의는 이곳에 해주세요
#define MAX_WALL 54

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

// 사용할 구조체 여기에 선언해주세요
struct Collision
{
	GLfloat left_x, right_x;
	GLfloat bottom_z, top_z;
};

struct Collision collision[54] =
{ {5.f,6.f,0.f,32.f},{9.f,26.f,0.f,1.f},{25.f,26.f,0.f,32.f},{6.f,22.f,16.f,17.f},
	{9.f,25.f,20.f,21.f},{14.f,22.f,4.f,5.f},{17.f,25.f,12.f,13.f},{9.f,17.f,28.f,29.f},
	{9.f,13.f,8.f,9.f},{18.f,22.f,24.f,25.f},{21.f,25.f,28.f,29.f},{1.f,16.f,32.f,33.f},
	{20.f,32.f,32.f,33.f},{0.f,1.f,32.f,64.f},{32.f,33.f,32.f,64.f},{13.f,14.f,4.f,16.f},
	{21.f,22.f,5.f,9.f},{17.f,18.f,8.f,12.f},{9.f,10.f,9.f,13.f},{13.f,14.f,21.f,25.f},
	{9.f,10.f,24.f,28.f},{17.f,18.f,24.f,29.f},{16.f,17.f,29.f,37.f},{20.f,21.f,33.f,41.f},
	{20.f,21.f,44.f,52.f},{28.f,29.f,36.f,44.f},{13.f,14.f,52.f,60.f},{4.f,5.f,53.f,61.f},
	{24.f,25.f,36.f,49.f},{8.f,9.f,37.f,48.f},{28.f,29.f,48.f,61.f},{4.f,5.f,36.f,39.f},
	{4.f,5.f,43.f,48.f},{12.f,13.f,41.f,45.f},{16.f,17.f,45.f,49.f},{17.f,18.f,53.f,56.f},
	{24.f,25.f,57.f,61.f},{8.f,9.f,60.f,64.f},{20.f,21.f,61.f,65.f},{29.f,32.f,36.f,37.f},
	{1.f,5.f,39.f,40.f},{16.f,20.f,44.f,45.f},{25.f,29.f,44.f,45.f},{5.f,10.f,56.f,57.f},
	{29.f,32.f,56.f,57.f},{8.f,16.f,36.f,37.f},{12.f,20.f,40.f,41.f},{4.f,16.f,48.f,49.f},
	{17.f,25.f,52.f,53.f},{17.f,25.f,56.f,57.f},{4.f,13.f,52.f,53.f},{13.f,21.f,60.f,61.f},
	{0.f,17.f,64.f,65.f},{21.f,33.f,64.f,65.f}
};

// 추가 변수 여기에 선언해주세요
GLfloat dx = 0.f, dy = 0.f, dz = 0.f; // 이동값
GLfloat ds = 0.01f; // 이동크기값
GLfloat posX = 0.f, posY = 2.f, posZ = 0.f; // 초기 생성 위치 값
GLboolean camera_set = GL_FALSE;
GLuint state = 0; // 0 - 정지 1 - 앞 2 - 뒤 3 - 좌 4 - 우

// 추가 함수 여기에 선언해주세요
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

	// wall
	glBindVertexArray(vao[1]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// floor
	glBindVertexArray(vao[2]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
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
	// 상공
	glm::vec3 cameraPos = glm::vec3(0.0f, 50.0f, 0.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // 카메라가 바라보는 곳	
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); //--- 카메라 방향 벡터
	glm::vec3 up = glm::vec3(0.0f, 0.0f,-1.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	// 주인공
	glm::vec3 cameraPos2 = glm::vec3(posX, posY, posZ);
	glm::vec3 cameraTarget2 = glm::vec3(50.0f, 2.0f, 0.0f);
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
	unsigned int colorLocation = glGetUniformLocation(s_program, "color"); // 색상인자 매칭
	glUniform3f(colorLocation, 1.0, 0.0, 0.0); // 색상값 주기
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINES, 0, 6);

	//큐브 그리기
	// wall1
	glm::mat4 CT = glm::mat4(1.0f);
	glm::mat4 MX = glm::mat4(1.0f);
	glm::mat4 CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(32.0f, 10.0f, 1.0));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 5.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	colorLocation = glGetUniformLocation(s_program, "color");
	glUniform3f(colorLocation, 0.2, 0.4, 0.7);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// wall2
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 16.0f));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 9.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");	
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// wall3
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(32.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 25.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall4
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 16.0f));
	MX = glm::translate(MX, glm::vec3(16.0f, 0.0f, 6.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall5
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 16.0f));
	MX = glm::translate(MX, glm::vec3(20.0f, 0.0f, 9.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall6
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(4.0f, 0.0f, 14.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall7
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(12.0f, 0.0f, 17.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall8
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(28.0f, 0.0f, 9.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall9	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 4.0f));
	MX = glm::translate(MX, glm::vec3(8.0f, 0.0f, 9.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall10	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 4.0f));
	MX = glm::translate(MX, glm::vec3(24.0f, 0.0f, 18.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall11	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 4.0f));
	MX = glm::translate(MX, glm::vec3(28.0f, 0.0f, 21.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall12	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 15.0f));
	MX = glm::translate(MX, glm::vec3(32.0f, 0.0f, 1.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall13	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 12.0f));
	MX = glm::translate(MX, glm::vec3(32.0f, 0.0f, 20.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall14	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(32.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(32.0f, 0.0f, 0.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall15
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(32.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(32.0f, 0.0f, 32.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall16	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(12.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(4.0f, 0.0f, 13.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall17	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(5.0f, 0.0f, 21.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall18	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(8.0f, 0.0f, 17.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall19	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(9.0f, 0.0f, 9.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall20	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(21.0f, 0.0f, 13.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall21	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(24.0f, 0.0f, 9.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall22	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(5.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(24.0f, 0.0f, 17.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall23	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(8.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(29.0f, 0.0f, 16.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall24	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(8.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(33.0f, 0.0f, 20.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall25	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(8.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(44.0f, 0.0f, 20.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall26	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(8.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(36.0f, 0.0f, 28.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall27	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(8.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(52.0f, 0.0f, 13.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall28	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(8.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(53.0f, 0.0f, 4.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall29	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(13.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(36.0f, 0.0f, 24.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall30	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(11.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(37.0f, 0.0f, 8.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall31	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(13.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(48.0f, 0.0f, 28.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall32	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(3.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(36.0f, 0.0f, 4.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall33	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(5.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(43.0f, 0.0f, 4.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall34	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(41.0f, 0.0f, 12.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall35	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(45.0f, 0.0f, 16.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall36	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(3.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(53.0f, 0.0f, 17.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall37	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(57.0f, 0.0f, 24.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall38	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(60.0f, 0.0f, 8.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall39	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(4.0f, 10.0f, 1.0f));
	MX = glm::translate(MX, glm::vec3(61.0f, 0.0f, 20.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall40	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 3.0f));
	MX = glm::translate(MX, glm::vec3(36.0f, 0.0f, 29.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall41	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 4.0f));
	MX = glm::translate(MX, glm::vec3(39.0f, 0.0f, 1.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall42	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 4.0f));
	MX = glm::translate(MX, glm::vec3(44.0f, 0.0f, 16.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall43	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 4.0f));
	MX = glm::translate(MX, glm::vec3(44.0f, 0.0f, 25.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall44	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 5.0f));
	MX = glm::translate(MX, glm::vec3(56.0f, 0.0f, 5.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall45	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 3.0f));
	MX = glm::translate(MX, glm::vec3(56.0f, 0.0f, 29.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall46	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(36.0f, 0.0f, 8.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall47	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(40.0f, 0.0f, 12.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall48	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 12.0f));
	MX = glm::translate(MX, glm::vec3(48.0f, 0.0f, 4.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall49	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(52.0f, 0.0f, 17.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall50	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(56.0f, 0.0f, 17.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall51	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 9.0f));
	MX = glm::translate(MX, glm::vec3(52.0f, 0.0f, 4.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall52	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 8.0f));
	MX = glm::translate(MX, glm::vec3(60.0f, 0.0f, 13.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall53	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 17.0f));
	MX = glm::translate(MX, glm::vec3(64.0f, 0.0f, 0.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//wall54	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 10.0f, 12.0f));
	MX = glm::translate(MX, glm::vec3(64.0f, 0.0f, 21.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//floor55	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(65.0f, 1.0f, 33.0f));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 0.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[2]);
	colorLocation = glGetUniformLocation(s_program, "color");
	glUniform3f(colorLocation, 0.5, 0.4, 0.4);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/*
	//roof56	
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(65.0f, 1.0f, 33.0f));
	MX = glm::translate(MX, glm::vec3(0.0f, 10.0f, 0.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	*/

	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w' | 'W':
		state = 1;
		break;
	case 'a' | 'A':
		state = 3;
		break;
	case 's' | 'S':
		state = 2;
		break;
	case 'd' | 'D':
		state = 4;
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

GLvoid KeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w' | 'W':
	case 'a' | 'A':
	case 's' | 'S':
	case 'd' | 'D' :
		state = 0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	// 플레이어 이동
	if (state != 0) 
	{
		for (int i = 0; i < MAX_WALL; i++) 
		{
			switch (state)
			{
			case 1: // 앞
				if (!(posZ + dz + ds > collision[i].bottom_z && posZ + dz + ds < collision[i].top_z &&
					posX + dx > collision[i].left_x && posX + dx < collision[i].right_x))
				{
					dz += ds;
				}				
				break;
			case 2: // 뒤
				dz -= ds;
				break;
			case 3: // 좌
				dx += ds;
				break;
			case 4: // 우
				dx -= ds;
				break;
			default:
				break;
			}
		}
	}
	glutPostRedisplay();
	glutTimerFunc(50, Timer, 1);
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand((unsigned int)time(NULL));
	// 화면 사이즈 조절
	width = 1000;
	height = 1000;

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
	glutKeyboardUpFunc(KeyUp);
	glutTimerFunc(50, Timer, 1);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}