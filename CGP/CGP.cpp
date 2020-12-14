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
#include <math.h>

// 정의는 이곳에 해주세요
#define PI 3.141592
#define MAX_WALL 56
#define MAX_ZOMBIE 34
#define MAX_ZOMBIEMOVE 100
#define MAX_PARTICLE 25
#define MAX_DISTANCE 16
#define MAX_ATTACK 5
#define MAX_ITEM 11
#define LIFE 3
#define POWER 1

GLuint vertexShader;
GLuint fragmentShader;
GLuint ShaderProgramID;
GLchar* vertexSource, * fragmentSource;
GLfloat rColor, gColor, bColor;
GLfloat g_window_w, g_window_h;
GLchar errorLog[512];
GLint width, height;
GLuint s_program;
GLuint vao[5], vbo[2];

// 사용할 구조체 여기에 선언해주세요
struct Collision
{
	GLfloat left_x, right_x;
	GLfloat bottom_z, top_z;
	GLboolean draw; // 그릴지 여부
};

struct Particle
{
	GLfloat posX, posY, posZ;
	GLfloat dx, dy, dz; // 움직일 거리
	GLfloat r, g, b;
	GLint count; // 20시 삭제
	GLboolean draw;
	void sound()
	{

	}
};

struct Item
{
	GLfloat posZ, posX;
	GLint type; // 1 - 열쇠 2 - 체력 3 - 공격력 업
	GLboolean draw; // 그릴지 여부
	GLfloat r, g, b;
};

struct Sword
{
	GLfloat rotate;
	GLfloat posX, posZ;
	GLint state; // 0 - 프리 1 - 발사중
	GLint temp_atk;
	GLfloat dx, dz;
	void sound() 
	{

	}
};

struct Zombieset
{
	GLfloat posZ, posY, posX;
	GLint concept_state; // 1 - 정지 2 - 배회 3 - 추적
	GLint state; // 1 - 앞 2 - 왼 3 - 뒤 4 - 오
};

struct Zombie
{
	GLfloat posX, posY, posZ;
	GLfloat mr; // 팔, 다리 회전각
	GLfloat dmr; // 회전 속도
	GLfloat ds; // 이동 속도
	GLint state; // 1 - 앞 2 - 왼 3 - 뒤 4 - 오
	GLfloat state_rotation; // state에 따른 회전각
	GLint concept_state; // 1 - 정지 2 - 배회 3 - 추적
	GLint count; // 위치변화 횟수
	GLint life;
	GLboolean draw;
};

struct Sword sword[MAX_ATTACK] = { 0, };
struct Particle particle[2][MAX_PARTICLE] = { 0, };
struct Collision collision[MAX_WALL] =
{ 
	{5.f,6.f,0.f,32.f,GL_TRUE},{9.f,26.f,0.f,1.f,GL_TRUE},{25.f,26.f,0.f,32.f,GL_TRUE},{6.f,22.f,16.f,17.f,GL_TRUE},
	{9.f,25.f,20.f,21.f,GL_TRUE},{14.f,22.f,4.f,5.f,GL_TRUE},{17.f,25.f,12.f,13.f,GL_TRUE},{9.f,17.f,28.f,29.f,GL_TRUE},
	{9.f,13.f,8.f,9.f,GL_TRUE},{18.f,22.f,24.f,25.f,GL_TRUE},{21.f,25.f,28.f,29.f,GL_TRUE},{1.f,16.f,32.f,33.f,GL_TRUE},
	{20.f,32.f,32.f,33.f,GL_TRUE},{0.f,1.f,32.f,64.f,GL_TRUE},{32.f,33.f,32.f,64.f,GL_TRUE},{13.f,14.f,4.f,16.f,GL_TRUE},
	{21.f,22.f,5.f,9.f,GL_TRUE},{17.f,18.f,8.f,12.f,GL_TRUE},{9.f,10.f,9.f,13.f,GL_TRUE},{13.f,14.f,21.f,25.f,GL_TRUE},
	{9.f,10.f,24.f,28.f,GL_TRUE},{17.f,18.f,24.f,29.f,GL_TRUE},{16.f,17.f,29.f,37.f,GL_TRUE},{20.f,21.f,33.f,41.f,GL_TRUE},
	{20.f,21.f,44.f,52.f,GL_TRUE},{28.f,29.f,36.f,44.f,GL_TRUE},{13.f,14.f,52.f,60.f,GL_TRUE},{4.f,5.f,53.f,61.f,GL_TRUE},
	{24.f,25.f,36.f,49.f,GL_TRUE},{8.f,9.f,37.f,48.f,GL_TRUE},{28.f,29.f,48.f,61.f,GL_TRUE},{4.f,5.f,36.f,39.f,GL_TRUE},
	{4.f,5.f,43.f,48.f,GL_TRUE},{12.f,13.f,41.f,45.f,GL_TRUE},{16.f,17.f,45.f,49.f,GL_TRUE},{17.f,18.f,53.f,56.f,GL_TRUE},
	{24.f,25.f,57.f,61.f,GL_TRUE},{8.f,9.f,60.f,64.f,GL_TRUE},{20.f,21.f,61.f,65.f,GL_TRUE},{29.f,32.f,36.f,37.f,GL_TRUE},
	{1.f,5.f,39.f,40.f,GL_TRUE},{16.f,20.f,44.f,45.f,GL_TRUE},{25.f,29.f,44.f,45.f,GL_TRUE},{5.f,10.f,56.f,57.f,GL_TRUE},
	{29.f,32.f,56.f,57.f,GL_TRUE},{8.f,16.f,36.f,37.f,GL_TRUE},{12.f,20.f,40.f,41.f,GL_TRUE},{4.f,16.f,48.f,49.f,GL_TRUE},
	{17.f,25.f,52.f,53.f,GL_TRUE},{17.f,25.f,56.f,57.f,GL_TRUE},{4.f,13.f,52.f,53.f,GL_TRUE},{13.f,21.f,60.f,61.f,GL_TRUE},
	{0.f,17.f,64.f,65.f,GL_TRUE},{21.f,33.f,64.f,65.f,GL_TRUE},{17.f,20.f,32.f,33.f,GL_TRUE},{17.f,20.f,64.f,65.f,GL_TRUE}
};
struct Zombieset zombieset[MAX_ZOMBIE] =
{
	{14.5f,1.f,2.f,1,1},{14.5f,1.f,3.f,1,1},{23.f,1.f,16.5f,1,2},{24.f,1.f,16.5f,1,2},
	{7.5f,1.f,18.f,1,3},{7.5f,1.f,19.f,1,3},{15.f,1.f,22.5f,1,4},{16.f,1.f,22.5f,1,4},
	{19.5f,1.f,26.f,1,3},{19.5f,1.f,27.f,1,3},{4.5f,1.f,34.f,2,3},{4.5f,1.f,35.f,2,3},
	{24.5f,1.f,34.f,1,1},{24.5f,1.f,35.f,1,1},{20.5f,1.f,42.f,1,1},{20.5f,1.f,43.f,1,1},
	{6.f,1.f,43.5f,1,2},{7.f,1.f,43.5f,1,2},{12.5f,1.f,46.f,1,1},{12.5f,1.f,47.f,1,1},
	{28.5f,1.f,46.f,1,1},{28.5f,1.f,47.f,1,1},{15.5f,1.f,50.f,1,4},{15.5f,1.f,51.f,1,4},
	{24.5f,1.f,50.f,1,1},{24.5f,1.f,51.f,1,1},{2.f,1.f,52.5f,2,2},{3.f,1.f,52.5f,2,2},
	{11.f,1.f,56.5f,1,4},{12.f,1.f,56.5f,1,4},{13.5f,1.f,62.5f,1,1},{13.5f,1.f,63.5f,1,1},
	{24.5f,1.f,62.5f,2,3},{24.5f,1.f,63.5f,2,3}
};
struct Item item[MAX_ITEM] = {
	{11.5f,10.5f,1,GL_FALSE,1.f,1.f,0.f},{14.5f,30.5f,1,GL_FALSE,1.f,1.f,0.f},{14.5f,34.5f,1,GL_FALSE,1.f,1.f,0.f},
	{30.5f,34.5f,1,GL_FALSE,1.f,1.f,0.f},{2.5f,37.5f,1,GL_FALSE,1.f,1.f,0.f},{30.5f,38.5f,1,GL_FALSE,1.f,1.f,0.f},
	{6.5f,46.5f,1,GL_FALSE,1.f,1.f,0.f},{6.5f,54.5f,1,GL_FALSE,1.f,1.f,0.f},{30.5f,58.5f,1,GL_FALSE,1.f,1.f,0.f},
	{18.5f,36.5f,2,GL_TRUE,0.f,1.f,0.f},{26.5f,42.5f,3,GL_TRUE,0.5f,0.f,1.f}
};
struct Zombie zombie[MAX_ZOMBIE] = { 0, };

// 추가 변수 여기에 선언해주세요
GLfloat rotate = 0.f; // 회전 값
GLfloat ds = 0.1f; // 이동크기값
GLfloat posX = 0.f, posY = 2.f, posZ = 0.f; // 초기 생성 위치 값
GLfloat realX, realY, realZ; // 실제 좌표 값
GLfloat realbody = 0.3f; // 카메라가 주인공이기에 가상의 두께 값
GLfloat distance; // 좀비와 플레이어의 거리
GLboolean camera_set = GL_FALSE;
GLboolean col = GL_FALSE; // 충돌 여부판단
GLint ws_state = 0, ad_state = 0; // 0 - 정지 1 - 앞/좌 2 - 뒤/우
GLint rotate_state = 0; // 0 - 정지 1 - 좌 2 - 우
GLint atk_count = 0, temp_atk = 0;
GLint life = LIFE, power = POWER, gaurd = 0; // 생명, 공격력, 피격 후 무적 시간
GLfloat goal_r = 0.f, goal_g = 0.f, goal_b = 0.f;
GLfloat firstMouseX = 0; // 최초 마우스 위치
GLint right_button = -1; // 시점이동 중단용 우클릭

// 여기에 도형 좌표 해주세요
GLfloat line[6][3] = {
	{-65.0f,0.0f,0.0f},{65.0f,0.0f,0.0f},
	{0.0f,65.0f,0.0f},{0.0f,-65.0f,0.0f},
	{0.0f,0.0f,-65.0f},{0.0f,0.0f,65.0f}
};
GLfloat tp[18][3] = {
	{0.f,0.25f,0.f},{-0.25f,-0.25f,-0.25f},{-0.25f,-0.25f,0.25f},
	{0.f,0.25f,0.f},{-0.25f,-0.25f,0.25f},{0.25f,-0.25f,0.25f},
	{0.f,0.25f,0.f},{0.25f,-0.25f,0.25f},{0.25f,-0.25f,-0.25f},
	{0.f,0.25f,0.f},{0.25f,-0.25f,-0.25},{-0.25f,-0.25f,-0.25f},
	{-0.25f,-0.25f,0.25f},{-0.25f,-0.25f,-0.25f},{0.25f,-0.25f,0.25f},
	{0.25f,-0.25f,0.25f},{-0.25f,-0.25f,-0.25f},{0.25f,-0.25f,-0.25f}
};
GLfloat cube[36][3] = {
   {0.0f,0.0f,0.0f}, // triangle 1 : begin
   {0.0f,0.0f, 1.0f},
   {0.0f, 1.0f, 1.0f}, // triangle 1 : end
   {1.0f, 1.0f,0.0f}, // triangle 2 : begin
   {0.0f,0.0f,0.0f},
   {0.0f, 1.0f,0.0f}, // triangle 2 : end
   {1.0f,0.0f, 1.0f},
   {0.0f,0.0f,0.0f},
   {1.0f,0.0f,0.0f},
   {1.0f, 1.0f,0.0f},
   {1.0f,0.0f,0.0f},
   {0.0f,0.0f,0.0f},
   {0.0f,0.0f,0.0f},
   {0.0f, 1.0f, 1.0f},
   {0.0f, 1.0f,0.0f},
   {1.0f,0.0f, 1.0f},
   {0.0f,0.0f, 1.0f},
   {0.0f,0.0f,0.0f},
   {0.0f, 1.0f, 1.0f},
   {0.0f,0.0f, 1.0f},
   {1.0f,0.0f, 1.0f},
   {1.0f, 1.0f, 1.0f},
   {1.0f,0.0f,0.0f},
   {1.0f, 1.0f,0.0f},
   {1.0f,0.0f,0.0f},
   {1.0f, 1.0f, 1.0f},
   {1.0f,0.0f, 1.0f},
   {1.0f, 1.0f, 1.0f},
   {1.0f, 1.0f,0.0f},
   {0.0f, 1.0f,0.0f},
   {1.0f, 1.0f, 1.0f},
   {0.0f, 1.0f,0.0f},
   {0.0f, 1.0f, 1.0f},
   {1.0f, 1.0f, 1.0f},
   {0.0f, 1.0f, 1.0f},
   {1.0f,0.0f, 1.0f}
};
GLfloat zombie_body[36][3] = {
	{-0.25,1.0,0.25},{-0.25,0.0,0.25},{0.25,0.0,0.25},
	{-0.25,1.0,0.25},{0.25,0.0,0.25},{0.25,1.0,0.25}, // 앞면
	{ 0.25,1.0,0.25},{0.25,0.0,0.25},{0.25,0.0,-0.25},
	{0.25,1.0,0.25},{0.25,0.0,-0.25},{0.25,1.0,-0.25}, // 오른면
	{0.25,1.0,-0.25},{0.25,0.0,-0.25},{-0.25,0.0,-0.25},
	{0.25,1.0, -0.25},{-0.25,0.0,-0.25},{-0.25,1.0,-0.25}, // 뒷면
	{-0.25,1.0,-0.25},{-0.25,0.0,-0.25},{-0.25,0.0,0.25},
	{-0.25,1.0,-0.25},{-0.25,0.0,0.25},{-0.25,1.0,0.25}, // 왼쪽면
	{-0.25,1.0,-0.25},{-0.25,1.0,0.25},{0.25,1.0,0.25},
	{-0.25,1.0,-0.25},{0.25,1.0,0.25},{0.25,1.0,-0.25}, // 윗면
	{-0.25,0.0,0.25},{-0.25,0.0,-0.25},{0.25,0.0,0.25},
	{0.25,0.0,0.25},{-0.25,0.0,-0.25},{0.25,0.0,-0.25} // 아랫면
};

// 추가 함수 여기에 선언해주세요
GLdouble GetRadian(GLfloat angle)
{
	return  angle * PI / 180.f;
}

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid KeyBoard(unsigned char key, int x, int y);
GLvoid KeyUp(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Timer(int value);

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
	glGenVertexArrays(5, vao);

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

	// zombie
	glBindVertexArray(vao[2]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), zombie_body, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// floor
	glBindVertexArray(vao[3]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(GLfloat), cube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 검관련
	glBindVertexArray(vao[4]);
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 54 * sizeof(GLfloat), tp, GL_STATIC_DRAW);
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
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glUseProgram(s_program);

	// 카메라 위치
	// 상공
	glm::vec3 cameraPos = glm::vec3(0.0f, 40.0f, 0.0f);
	glm::vec3 cameraTarget = glm::vec3(realZ, realY, realX); // 카메라가 바라보는 곳
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
	glm::mat4 rotatePos = glm::mat4(1.0f);
	if (!camera_set) // 상공시점
	{
		transPos = glm::translate(transPos, glm::vec3(-realZ, realX, realY));		
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		view = transPos * view;
	}
	else // 주인공 시점
	{
		rotatePos = glm::rotate(rotatePos, glm::radians(rotate), glm::vec3(0.0f, 1.0f, 0.0f));
		transPos = glm::translate(transPos, glm::vec3(-realX, realY - 2.f, realZ));
		view = glm::lookAt(cameraPos2, cameraTarget2, cameraUp2);
		view = rotatePos * transPos * view;
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
	CS = glm::scale(CS, glm::vec3(32.0f, 6.0f, 1.0));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 5.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[1]);
	colorLocation = glGetUniformLocation(s_program, "color");
	glUniform3f(colorLocation, 0.3, 0.3, 0.3);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// wall2
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 16.0f));
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
	CS = glm::scale(CS, glm::vec3(32.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 16.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 16.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 4.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 4.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 4.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 15.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 12.0f));
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
	CS = glm::scale(CS, glm::vec3(32.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(32.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(12.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(5.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(8.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(8.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(8.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(8.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(8.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(8.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(13.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(11.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(13.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(3.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(5.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(3.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(4.0f, 6.0f, 1.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 3.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 4.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 4.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 4.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 5.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 3.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 12.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 9.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 8.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 17.0f));
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
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 12.0f));
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
	glBindVertexArray(vao[3]);
	colorLocation = glGetUniformLocation(s_program, "color");
	glUniform3f(colorLocation, 0.5, 0.4, 0.4);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/*
	//roof56
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(65.0f, 1.0f, 33.0f));
	MX = glm::translate(MX, glm::vec3(0.0f, 6.0f, 0.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	*/

	//door1
	
	if (collision[54].draw)
	{
		colorLocation = glGetUniformLocation(s_program, "color");
		glUniform3f(colorLocation, 0.0, 0.0, 0.0);
		CT = glm::mat4(1.0f);
		MX = glm::mat4(1.0f);
		CS = glm::mat4(1.0f);
		CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 3.0f));
		MX = glm::translate(MX, glm::vec3(32.0f, 0.0f, 17.0f));
		CT = MX * CS;
		modelLocation = glGetUniformLocation(s_program, "modelTransform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
		glBindVertexArray(vao[3]);		
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//door2	
	colorLocation = glGetUniformLocation(s_program, "color");
	glUniform3f(colorLocation, goal_r, goal_g, goal_b);
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 3.0f));
	MX = glm::translate(MX, glm::vec3(64.0f, 0.0f, 17.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//door start
	colorLocation = glGetUniformLocation(s_program, "color");
	glUniform3f(colorLocation, goal_r, goal_g, goal_b);
	CT = glm::mat4(1.0f);
	MX = glm::mat4(1.0f);
	CS = glm::mat4(1.0f);
	CS = glm::scale(CS, glm::vec3(1.0f, 6.0f, 3.0f));
	MX = glm::translate(MX, glm::vec3(0.0f, 0.0f, 6.0f));
	CT = MX * CS;
	modelLocation = glGetUniformLocation(s_program, "modelTransform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(CT));
	glBindVertexArray(vao[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// zombie
	glm::mat4 Ty = glm::mat4(1.0f);
	glm::mat4 Ty1 = glm::mat4(1.0f);
	glm::mat4 Ty2 = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Tz = glm::mat4(1.0f);
	glm::mat4 TS = glm::mat4(1.0f);
	glm::mat4 TDO = glm::mat4(1.0f);
	glm::mat4 RS = glm::mat4(1.0f);
	glm::mat4 Rx = glm::mat4(1.0f);
	glm::mat4 RR1 = glm::mat4(1.0f);
	glm::mat4 RR2 = glm::mat4(1.0f);
	glm::mat4 TM = glm::mat4(1.0f);
	glm::mat4 TJy = glm::mat4(1.0f);
	for (int i = 0; i < MAX_ZOMBIE; i++)
	{
		if (zombie[i].draw)
		{
			// 좀비 머리 그리기
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, 0.7, 0.7, 0.7);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Ty = glm::mat4(1.0f);
			RS = glm::mat4(1.0f);
			TM = glm::mat4(1.0f);
			TM = glm::translate(TM, glm::vec3(zombie[i].posX, zombie[i].posY, zombie[i].posZ));
			RS = glm::rotate(RS, glm::radians(zombie[i].state_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			Ty = glm::translate(Ty, glm::vec3(0.0f, 1.0f, 0.0f));
			TS = glm::scale(TS, glm::vec3(1.0f, 0.4f, 0.5f));
			TT = TM * RS * Ty * TS;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// 좀비 코 그리기
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, 0.5, 0.5, 0.5);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Ty = glm::mat4(1.0f);
			Tz = glm::mat4(1.0f);
			Tz = glm::translate(Tz, glm::vec3(0.0f, 0.0f, 0.15f));
			Ty = glm::translate(Ty, glm::vec3(0.0f, 1.2f, 0.0f));
			TS = glm::scale(TS, glm::vec3(0.3f, 0.1f, 0.2f));
			TT = TM * RS * Tz * Ty * TS;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);


			// 좀비 몸 그리기
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, 1.0, 0.0, 0.0);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Ty = glm::mat4(1.0f);
			Ty = glm::translate(Ty, glm::vec3(0.0f, 0.5f, 0.0f));
			TS = glm::scale(TS, glm::vec3(1.5f, 0.6f, 0.7f));
			TT = TM * RS * Ty * TS;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// 좀비 왼다리 그리기
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, 1.0, 1.0, 0.0);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Tx = glm::mat4(1.0f);
			Rx = glm::mat4(1.0f);
			Ty1 = glm::mat4(1.0f);
			Ty2 = glm::mat4(1.0f);
			Ty1 = glm::translate(Ty1, glm::vec3(0.0f, 0.3f, 0.0f));
			Ty2 = glm::translate(Ty2, glm::vec3(0.0f, -0.3f, 0.0f));
			Rx = glm::rotate(Rx, glm::radians(zombie[i].mr + 180.f), glm::vec3(1.0f, 0.0f, 0.0f));
			Tx = glm::translate(Tx, glm::vec3(-0.2f, 1.0f, 0.0f));
			TS = glm::scale(TS, glm::vec3(0.3f, 0.6f, 0.2f));
			TT = TM * RS * Tx * TS * Ty2 * Rx * Ty1;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// 좀비 오른다리 그리기
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, 1.0, 0.0, 1.0);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Tx = glm::mat4(1.0f);
			Ty1 = glm::mat4(1.0f);
			Ty2 = glm::mat4(1.0f);
			Rx = glm::mat4(1.0f);
			Ty1 = glm::translate(Ty1, glm::vec3(0.0f, 0.3f, 0.0f));
			Ty2 = glm::translate(Ty2, glm::vec3(0.0f, -0.3f, 0.0f));
			Tx = glm::translate(Tx, glm::vec3(0.2f, 1.0f, 0.0f));
			Rx = glm::rotate(Rx, glm::radians(-zombie[i].mr + 180.f), glm::vec3(1.0f, 0.0f, 0.0f));
			TS = glm::scale(TS, glm::vec3(0.3f, 0.6f, 0.2f));
			TT = TM * RS * Tx * TS * Ty2 * Rx * Ty1;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// 좀비 왼팔 그리기
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, 1.0, 1.0, 0.0);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Ty = glm::mat4(1.0f);
			Tx = glm::mat4(1.0f);
			Ty1 = glm::mat4(1.0f);
			Ty2 = glm::mat4(1.0f);
			Rx = glm::mat4(1.0f);
			Ty1 = glm::translate(Ty1, glm::vec3(0.0f, 0.3f, 0.0f));
			Ty2 = glm::translate(Ty2, glm::vec3(0.0f, -0.3f, 0.0f));
			Tx = glm::translate(Tx, glm::vec3(-0.4f, 0.0f, 0.0f));
			Ty = glm::translate(Ty, glm::vec3(0.0f, 1.4f, 0.0f));
			TS = glm::scale(TS, glm::vec3(0.4f, 0.6f, 0.2f));
			Rx = glm::rotate(Rx, glm::radians(-zombie[i].mr + 180.f), glm::vec3(1.0f, 0.0f, 0.0f));
			TT = TM * RS * Tx * Ty * TS * Ty2 * Rx * Ty1;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// 좀비 오른팔 그리기
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, 1.0, 0.0, 1.0);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Ty = glm::mat4(1.0f);
			Tx = glm::mat4(1.0f);
			Ty1 = glm::mat4(1.0f);
			Ty2 = glm::mat4(1.0f);
			Rx = glm::mat4(1.0f);
			Ty1 = glm::translate(Ty1, glm::vec3(0.0f, 0.3f, 0.0f));
			Ty2 = glm::translate(Ty2, glm::vec3(0.0f, -0.3f, 0.0f));
			Tx = glm::translate(Tx, glm::vec3(0.4f, 0.0f, 0.0f));
			Ty = glm::translate(Ty, glm::vec3(0.0f, 1.4f, 0.0f));
			TS = glm::scale(TS, glm::vec3(0.4f, 0.6f, 0.2f));
			Rx = glm::rotate(Rx, glm::radians(zombie[i].mr + 180.f), glm::vec3(1.0f, 0.0f, 0.0f));
			TT = TM * RS * Tx * Ty * TS * Ty2 * Rx * Ty1;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// 검 그리기
	if (atk_count != 0)
	{
		for (int i = 0; i < MAX_ATTACK; i++)
		{
			if (sword[i].state == 1)
			{
				colorLocation = glGetUniformLocation(s_program, "color");
				glUniform3f(colorLocation, 0.0, 0.0, 0.0);
				TT = glm::mat4(1.0f);
				TS = glm::mat4(1.0f);
				Ty1 = glm::mat4(1.0f);
				Ty2 = glm::mat4(1.0f);
				Ty = glm::mat4(1.0f);
				TM = glm::mat4(1.0f);
				RR1 = glm::mat4(1.0f);
				RR2 = glm::mat4(1.0f);
				Ty1 = glm::translate(Ty1, glm::vec3(-2.0f, 0.0f, 0.0f));
				Ty2 = glm::translate(Ty2, glm::vec3(2.0f, 0.0f, 0.0f));
				RR1 = glm::rotate(RR1, glm::radians(-90.f), glm::vec3(0.0f, 0.0f, 1.0f));
				RR2 = glm::rotate(RR2, glm::radians(sword[i].rotate), glm::vec3(0.0f, 1.0f, 0.0f));
				TM = glm::translate(TM, glm::vec3(sword[i].posX, 1.5f, sword[i].posZ));
				Ty = glm::translate(Ty, glm::vec3(0.0f, 1.6f, 0.0f));
				TS = glm::scale(TS, glm::vec3(0.2f, 0.2f, 0.2f));
				TT = TM * Ty2 * RR2 * Ty1 * RR1 * Ty * TS;
				modelLocation = glGetUniformLocation(s_program, "modelTransform");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
				glBindVertexArray(vao[2]);
				glDrawArrays(GL_TRIANGLES, 0, 36);

				colorLocation = glGetUniformLocation(s_program, "color");
				glUniform3f(colorLocation, 0.0, 0.0, 0.0);
				TT = glm::mat4(1.0f);
				TS = glm::mat4(1.0f);
				Ty = glm::mat4(1.0f);
				Ty = glm::translate(Ty, glm::vec3(0.0f, 1.8f, 0.0f));
				TS = glm::scale(TS, glm::vec3(0.2f, 0.1f, 0.6f));
				TT = TM * Ty2 * RR2 * Ty1 * RR1 * Ty * TS;
				modelLocation = glGetUniformLocation(s_program, "modelTransform");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
				glBindVertexArray(vao[2]);
				glDrawArrays(GL_TRIANGLES, 0, 36);

				colorLocation = glGetUniformLocation(s_program, "color");
				glUniform3f(colorLocation, 0.6, 0.6, 0.6);
				TT = glm::mat4(1.0f);
				TS = glm::mat4(1.0f);
				Ty = glm::mat4(1.0f);
				Ty = glm::translate(Ty, glm::vec3(0.0f, 1.9f, 0.0f));
				TS = glm::scale(TS, glm::vec3(0.1f, 0.4f, 0.3f));
				TT = TM * Ty2 * RR2 * Ty1 * RR1 * Ty * TS;
				modelLocation = glGetUniformLocation(s_program, "modelTransform");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
				glBindVertexArray(vao[2]);
				glDrawArrays(GL_TRIANGLES, 0, 36);

				colorLocation = glGetUniformLocation(s_program, "color");
				glUniform3f(colorLocation, 0.6, 0.6, 0.6);
				TT = glm::mat4(1.0f);
				TS = glm::mat4(1.0f);
				Ty = glm::mat4(1.0f);
				Ty = glm::translate(Ty, glm::vec3(0.0f, 2.38f, 0.0f));
				TS = glm::scale(TS, glm::vec3(0.1f, 0.3f, 0.3f));
				TT = TM * Ty2 * RR2 * Ty1 * RR1 * Ty * TS;
				modelLocation = glGetUniformLocation(s_program, "modelTransform");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
				glBindVertexArray(vao[4]);
				glDrawArrays(GL_TRIANGLES, 0, 18);
			}
		}
	}

	// 아이템 그리기
	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (item[i].draw)
		{
			colorLocation = glGetUniformLocation(s_program, "color");
			glUniform3f(colorLocation, item[i].r, item[i].g, item[i].b);
			TT = glm::mat4(1.0f);
			TS = glm::mat4(1.0f);
			Ty = glm::mat4(1.0f);
			TM = glm::mat4(1.0f);
			TM = glm::translate(TM, glm::vec3(item[i].posX, 0.0f, item[i].posZ));
			Ty = glm::translate(Ty, glm::vec3(0.0f, 1.f, 0.0f));
			TS = glm::scale(TS, glm::vec3(1.0f, 0.5f, 1.0f));
			TT = TM * Ty * TS;
			modelLocation = glGetUniformLocation(s_program, "modelTransform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// 파티클 그리기
	for (int i = 0; i < 2; i++)
	{
		if (particle[i][0].draw)
		{
			for (int j = 0; j < MAX_PARTICLE; j++)
			{
				colorLocation = glGetUniformLocation(s_program, "color");
				glUniform3f(colorLocation, particle[i][j].r, particle[i][j].g, particle[i][j].b);
				TT = glm::mat4(1.0f);
				TS = glm::mat4(1.0f);
				Ty = glm::mat4(1.0f);
				TM = glm::mat4(1.0f);
				TM = glm::translate(TM, glm::vec3(particle[i][j].posX, particle[i][j].posY, particle[i][j].posZ));
				Ty = glm::translate(Ty, glm::vec3(0.0f, 1.f, 0.0f));
				TS = glm::scale(TS, glm::vec3(0.05f, 0.05f, 0.05f));
				TT = TM * Ty * TS;
				modelLocation = glGetUniformLocation(s_program, "modelTransform");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TT));
				glBindVertexArray(vao[2]);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				particle[i][j].posX += particle[i][j].dx;
				particle[i][j].posY += particle[i][j].dy;
				particle[i][j].posZ += particle[i][j].dz;
				particle[i][j].count--;
				if (particle[i][j].count <= 0)
				{
					particle[i][j].draw = GL_FALSE;
				}
			}
		}
	}

	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w' | 'W':
		ws_state = 1;
		break;
	case 'a' | 'A':
		ad_state = 1;
		break;
	case 's' | 'S':
		ws_state = 2;
		break;
	case 'd' | 'D':
		ad_state = 2;
		break;
	case 'q' | 'Q': // 왼쪽 회전
		rotate_state = 1;
		break;
	case 'e' | 'E': // 우측 회전
		rotate_state = 2;
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
	case 'j' | 'J':
		if (atk_count < MAX_ATTACK)
		{
			for (int i = 0; i < MAX_ATTACK; i++)
			{
				if (sword[i].state == 0)
				{
					sword[i].posX = realZ - 2.2f;
					sword[i].posZ = realX;
					sword[i].rotate = -rotate;
					sword[i].state = 1;
					sword[i].dx = (ds * sin(GetRadian(90 - rotate)));
					sword[i].dz = -(ds * cos(GetRadian(90 + rotate)));
					atk_count++;
					i = MAX_ATTACK;
				}
			}
		}
		break;
	case 'p' | 'P':
		life = -1;
		break;
	case 'g' | 'G':	
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

GLvoid KeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w' | 'W':
	case 's' | 'S':
		ws_state = 0;
		break;
	case 'a' | 'A':	
	case 'd' | 'D':
		ad_state = 0;
		break;
	case 'q' | 'Q':
	case 'e' | 'E':
		rotate_state = 0;
		break;
	}
	glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
	firstMouseX = (GLfloat)((x - (GLfloat)width / 2.0) * (GLfloat)(1.0 / (GLfloat)(width / 2.0)));
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (atk_count < MAX_ATTACK)
		{
			for (int i = 0; i < MAX_ATTACK; i++)
			{
				if (sword[i].state == 0)
				{
					sword[i].posX = realZ - 2.2f;
					sword[i].posZ = realX;
					sword[i].rotate = -rotate;
					sword[i].state = 1;
					sword[i].dx = (ds * sin(GetRadian(90 - rotate)));
					sword[i].dz = -(ds * cos(GetRadian(90 + rotate)));
					atk_count++;
					i = MAX_ATTACK;
				}
			}
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (firstMouseX < 0)
		{
			rotate -= (-firstMouseX / (GLfloat)0.05) * (GLfloat)2;
		}
		else
		{
			rotate += (firstMouseX / (GLfloat)0.05) * (GLfloat)2;
		}
		//right_button *= -1;
	}
}

GLvoid Motion(int x, int y)
{
	if (right_button == 1) {
		GLfloat tempvalue = (GLfloat)((x - (GLfloat)width / 2.0) * (GLfloat)(1.0 / (GLfloat)(width / 2.0)));
		if (tempvalue < 0)
			rotate_state = 1;
		else if (tempvalue > 0)
			rotate_state = 2;
	}
	else
		rotate_state = 0;
}

GLvoid Timer(int value) {
	if (life > 0)
	{
		// 공격
		if (atk_count > 0)
		{
			temp_atk = 0;
			for (int i = 0; i < MAX_ATTACK; i++)
			{
				if (sword[i].state == 1)
				{
					sword[i].posX += sword[i].dx;
					sword[i].posZ += sword[i].dz;
					sword[i].temp_atk++;

					//충돌 처리 공간
					for (int j = 0; j < MAX_ZOMBIE; j++)
					{
						if (zombie[j].draw)
						{
							distance = pow(((sword[i].posX + 2.f) - zombie[j].posX), 2) + pow((sword[i].posZ - zombie[j].posZ), 2);
							if (distance <= 1)
							{
								zombie[j].life -= power;
								if (zombie[j].life <= 0)
								{
									zombie[j].draw = GL_FALSE;
									for (int k = 0; k < 2; k++)
									{
										if (!particle[k][0].draw)
										{
											for (int l = 0; l < MAX_PARTICLE; l++)
											{
												particle[k][l].draw = GL_TRUE;
												particle[k][l].count = 20;
												particle[k][l].posX = zombie[j].posX;
												particle[k][l].posY = zombie[j].posY;
												particle[k][l].posZ = zombie[j].posZ;
												particle[k][l].dx = (rand() % 2 == 0) ? rand() % 11 * 0.005f : rand() % 11 * -0.005f;
												particle[k][l].dy = rand() % 11 * 0.005f;
												particle[k][l].dz = (rand() % 2 == 0) ? rand() % 11 * 0.005f : rand() % 11 * -0.005f;
												switch (rand() % 3)
												{
												case 0:
													particle[k][l].r = 1.f;
													particle[k][l].g = 0.f;
													particle[k][l].b = 0.f;
													break;
												case 1:
													particle[k][l].r = 1.f;
													particle[k][l].g = 1.f;
													particle[k][l].b = 0.f;
													break;
												case 2:
													particle[k][l].r = 0.5f;
													particle[k][l].g = 0.5f;
													particle[k][l].b = 0.5f;
													break;
												}
											}
										}
									}
								}
								j = MAX_ZOMBIE;
								sword[i].temp_atk = 25;
							}
						}
					}

					if (sword[i].temp_atk > 20)
					{
						sword[i] = { 0, };
						atk_count--;
					}

					temp_atk++;
					if (sword[i].state == 0)
					{
						if (temp_atk == atk_count + 1)
						{
							i = MAX_ATTACK;
						}
					}
					else
					{
						if (temp_atk == atk_count)
						{
							i = MAX_ATTACK;
						}
					}
				}
			}
		}

		// 플레이어 이동
		if (ws_state != 0 && camera_set)
		{
			switch (ws_state)
			{
			case 1: // 앞
				for (int i = 0; i < MAX_WALL; i++)
				{
					if (collision[i].draw)
					{
						if (realZ + (ds * sin(GetRadian(90 - rotate))) > collision[i].bottom_z - realbody && realZ + (ds * sin(GetRadian(90 - rotate))) < collision[i].top_z + realbody &&
							realX + (ds * cos(GetRadian(90 - rotate))) > collision[i].left_x - realbody && realX + (ds * cos(GetRadian(90 - rotate))) < collision[i].right_x + realbody)
						{
							if (i == MAX_WALL - 1)
							{
								life = -1;
							}
							col = GL_TRUE;
						}
						if (col)
						{
							i = MAX_WALL;
						}
					}
				}
				if (!col)
				{
					realZ += (ds * sin(GetRadian(90 - rotate)));
					realX += (ds * cos(GetRadian(90 - rotate)));
				}
				else
				{
					col = GL_FALSE;
				}
				break;
			case 2: // 뒤
				for (int i = 0; i < MAX_WALL; i++)
				{
					if (collision[i].draw)
					{
						if (realZ - (ds * sin(GetRadian(90 - rotate))) > collision[i].bottom_z - realbody && realZ - (ds * sin(GetRadian(90 - rotate))) < collision[i].top_z + realbody &&
							realX - (ds * cos(GetRadian(90 - rotate))) > collision[i].left_x - realbody && realX - (ds * cos(GetRadian(90 - rotate))) < collision[i].right_x + realbody)
						{
							if (i == MAX_WALL - 1)
							{
								life = -1;
							}
							col = GL_TRUE;
						}
						if (col)
						{
							i = MAX_WALL;
						}
					}
				}
				if (!col)
				{
					realZ -= (ds * sin(GetRadian(90 - rotate)));
					realX -= (ds * cos(GetRadian(90 - rotate)));
					col = GL_FALSE;
				}
				else
				{
					col = GL_FALSE;
				}
				break;
			default:
				break;
			}
		}

		if (ad_state != 0 && camera_set)
		{
			switch (ad_state)
			{
			case 1: // 좌
				for (int i = 0; i < MAX_WALL; i++)
				{
					if (collision[i].draw)
					{
						if (realZ - (ds * sin(GetRadian(-rotate))) > collision[i].bottom_z - realbody && realZ - (ds * sin(GetRadian(-rotate))) < collision[i].top_z + realbody &&
							realX - (ds * cos(GetRadian(-rotate))) > collision[i].left_x - realbody && realX - (ds * cos(GetRadian(-rotate))) < collision[i].right_x + realbody)
						{
							if (i == MAX_WALL - 1)
							{
								life = -1;
							}
							col = GL_TRUE;
						}
						if (col)
						{
							i = MAX_WALL;
						}
					}
				}
				if (!col)
				{
					realZ -= (ds * sin(GetRadian(-rotate)));
					realX -= (ds * cos(GetRadian(-rotate)));
					col = GL_FALSE;
				}
				else
				{
					col = GL_FALSE;
				}
				break;
			case 2: // 우
				for (int i = 0; i < MAX_WALL; i++)
				{
					if (collision[i].draw)
					{
						if (realZ + (ds * sin(GetRadian(-rotate))) > collision[i].bottom_z - realbody && realZ + (ds * sin(GetRadian(-rotate))) < collision[i].top_z + realbody &&
							realX + (ds * cos(GetRadian(-rotate))) > collision[i].left_x - realbody && realX + (ds * cos(GetRadian(-rotate))) < collision[i].right_x + realbody)
						{
							if (i == MAX_WALL - 1)
							{
								life = -1;
							}
							col = GL_TRUE;
						}
						if (col)
						{
							i = MAX_WALL;
						}
					}
				}
				if (!col)
				{
					realZ += (ds * sin(GetRadian(-rotate)));
					realX += (ds * cos(GetRadian(-rotate)));
					col = GL_FALSE;
				}
				else
				{
					col = GL_FALSE;
				}
				break;
			default:
				break;
			}
		}

		if (rotate_state != 0)
		{
			switch (rotate_state)
			{
			case 1:
				rotate -= 3.f;
				break;
			case 2:
				rotate += 3.f;
				break;
			default:
				break;
			}
		}

		// 아이템 충돌 검사
		if (camera_set)
		{
			for (int i = 0; i < MAX_ITEM; i++)
			{
				if (item[i].draw)
				{
					distance = pow((realZ - item[i].posX), 2) + pow((realX - item[i].posZ), 2);
					if (distance <= 1)
					{
						item[i].draw = GL_FALSE;
						switch (item[i].type)
						{
						case 1:
							if (i < 2)
							{
								collision[54].draw = GL_FALSE;
							}
							else
							{
								goal_r = 1.f;
								goal_g = 0.8f;
								goal_b = 0.f;
							}
							break;
						case 2:
							life += 1;
							break;
						case 3:
							power += 1;
							break;
						}
						i = MAX_ITEM;
					}
				}
			}
		}

		// 좀비 움직임
		for (int i = 0; i < MAX_ZOMBIE; i++)
		{
			if (zombie[i].draw)
			{
				switch (zombie[i].concept_state)
				{
				case 1: // 정지
					break;
				case 2: // 배회
					switch (zombie[i].state)
					{
					case 1:
						zombie[i].posZ -= zombie[i].ds;
						zombie[i].count += 1;
						if (zombie[i].count >= MAX_ZOMBIEMOVE)
						{
							zombie[i].state = 3;
							zombie[i].state_rotation = 0.f;
							zombie[i].count = 0;
						}
						break;
					case 2:
						zombie[i].posX -= zombie[i].ds;
						zombie[i].count += 1;
						if (zombie[i].count >= MAX_ZOMBIEMOVE)
						{
							zombie[i].state = 4;
							zombie[i].state_rotation = 90.f;
							zombie[i].count = 0;
						}
						break;
					case 3:
						zombie[i].posZ += zombie[i].ds;
						zombie[i].count += 1;
						if (zombie[i].count >= MAX_ZOMBIEMOVE)
						{
							zombie[i].state = 1;
							zombie[i].state_rotation = 180.f;
							zombie[i].count = 0;
						}
						break;
					case 4:
						zombie[i].posX += zombie[i].ds;
						zombie[i].count += 1;
						if (zombie[i].count >= MAX_ZOMBIEMOVE)
						{
							zombie[i].state = 2;
							zombie[i].state_rotation = -90.f;
							zombie[i].count = 0;
						}
						break;
					}
					break;
				case 3: // 추적
					if (pow((realZ - zombie[i].posX), 2) > pow((realX - zombie[i].posZ), 2) && realZ > zombie[i].posX)
					{
						zombie[i].state = 4;
						zombie[i].state_rotation = 90.f;
					}
					else if (pow((realZ - zombie[i].posX), 2) > pow((realX - zombie[i].posZ), 2) && realZ < zombie[i].posX)
					{
						zombie[i].state = 2;
						zombie[i].state_rotation = -90.f;
					}
					else if (pow((realZ - zombie[i].posX), 2) < pow((realX - zombie[i].posZ), 2) && realX > zombie[i].posZ)
					{
						zombie[i].state = 3;
						zombie[i].state_rotation = 0.f;
					}
					else if (pow((realZ - zombie[i].posX), 2) < pow((realX - zombie[i].posZ), 2) && realX < zombie[i].posZ)
					{
						zombie[i].state = 1;
						zombie[i].state_rotation = 180.f;
					}

					if (realZ > zombie[i].posX + zombie[i].ds)
					{
						zombie[i].posX += zombie[i].ds;
					}
					else if (realZ < zombie[i].posX - zombie[i].ds)
					{
						zombie[i].posX -= zombie[i].ds;
					}

					if (realX > zombie[i].posZ + zombie[i].ds)
					{
						zombie[i].posZ += zombie[i].ds;
					}
					else if (realX < zombie[i].posZ - zombie[i].ds)
					{
						zombie[i].posZ -= zombie[i].ds;
					}
					break;
				case 4: // 초기 위치로 복귀
					distance = pow((zombieset[i].posX - zombie[i].posX), 2) + pow((zombieset[i].posZ - zombie[i].posZ), 2);
					if (distance <= 1)
					{
						zombie[i].posX = zombieset[i].posX;
						zombie[i].posZ = zombieset[i].posZ;
						zombie[i].concept_state = zombieset[i].concept_state;
						zombie[i].state = zombieset[i].state;
						switch (zombie[i].state)
						{
						case 1:
							zombie[i].state_rotation = 180.f;
							break;
						case 2:
							zombie[i].state_rotation = -90.f;
							break;
						case 3:
							zombie[i].state_rotation = 0.f;
							break;
						case 4:
							zombie[i].state_rotation = 90.f;
							break;
						}
					}
					else
					{
						if (pow((zombieset[i].posX - zombie[i].posX), 2) > pow((zombieset[i].posZ - zombie[i].posZ), 2) && zombieset[i].posX > zombie[i].posX)
						{
							zombie[i].state = 4;
							zombie[i].state_rotation = 90.f;
						}
						else if (pow((zombieset[i].posX - zombie[i].posX), 2) > pow((zombieset[i].posZ - zombie[i].posZ), 2) && zombieset[i].posX < zombie[i].posX)
						{
							zombie[i].state = 2;
							zombie[i].state_rotation = -90.f;
						}
						else if (pow((zombieset[i].posX - zombie[i].posX), 2) < pow((zombieset[i].posZ - zombie[i].posZ), 2) && zombieset[i].posZ > zombie[i].posZ)
						{
							zombie[i].state = 3;
							zombie[i].state_rotation = 0.f;
						}
						else if (pow((zombieset[i].posX - zombie[i].posX), 2) < pow((zombieset[i].posZ - zombie[i].posZ), 2) && zombieset[i].posZ < zombie[i].posZ)
						{
							zombie[i].state = 1;
							zombie[i].state_rotation = 180.f;
						}

						if (zombieset[i].posX > zombie[i].posX + zombie[i].ds)
						{
							zombie[i].posX += zombie[i].ds;
						}
						else if (zombieset[i].posX < zombie[i].posX - zombie[i].ds)
						{
							zombie[i].posX -= zombie[i].ds;
						}

						if (zombieset[i].posZ > zombie[i].posZ + zombie[i].ds)
						{
							zombie[i].posZ += zombie[i].ds;
						}
						else if (zombieset[i].posZ < zombie[i].posZ - zombie[i].ds)
						{
							zombie[i].posZ -= zombie[i].ds;
						}
					}
					break;
				}

				// 팔다리 회전애니메이션
				zombie[i].mr += zombie[i].dmr;
				if (zombie[i].mr >= 45.f || zombie[i].mr <= -45.f) {
					zombie[i].dmr *= -1.f;
				}

				// 유저와의 거리 측정
				distance = pow((realZ - zombie[i].posX), 2) + pow((realX - zombie[i].posZ), 2);
				if (distance <= MAX_DISTANCE && zombie[i].concept_state != 3)
				{
					zombie[i].concept_state = 3;
				}
				else if (distance > MAX_DISTANCE && zombie[i].concept_state == 3)
				{
					zombie[i].concept_state = 4;
				}

				if (distance <= 0.5f && gaurd <= 0)
				{
					gaurd = 50;
					life--;
				}
			}
		}
		gaurd--;
	}
	else
	{
		// 게임 오버 및 클리어시
		rotate = 0.f;
		camera_set = GL_FALSE;
		col = GL_FALSE; // 충돌 여부판단
		ws_state = 0;
		ad_state = 0;
		rotate_state = 0;
		atk_count = 0;
		temp_atk = 0;
		life = LIFE;
		power = POWER;
		gaurd = 0;
		firstMouseX = 0;
		right_button = -1;
		goal_r = goal_g = goal_b = 0.f;		

		// 아이템 초기화
		srand((unsigned int)time(NULL));
		GLint temp1 = rand() % 2;
		GLint temp2 = rand() % 7 + 2;
		for (int i = 0; i < MAX_ITEM; i++)
		{
			if (i == temp1 || i == temp2)
			{
				item[i].draw = GL_TRUE;
			}
			else if(i<9)
			{
				item[i].draw = GL_FALSE;
			}
			else
			{
				item[i].draw = GL_TRUE;
			}
		}

		// 벽 초기화
		for (int i = 0; i < MAX_WALL; i++)
		{
			collision[i].draw = GL_TRUE;
		}

		// 좀비 초기화
		for (int i = 0; i < MAX_ZOMBIE; i++)
		{
			if (i < 10)
			{
				zombie[i].life = 2;
			}
			else
			{
				zombie[i].life = 4;
			}
			zombie[i].ds = 0.05f;
			zombie[i].mr = (GLfloat)(rand() % 16) * 3.f;
			zombie[i].dmr = 3.f;
			zombie[i].concept_state = zombieset[i].concept_state;
			zombie[i].count = 0;
			zombie[i].posX = zombieset[i].posX;
			zombie[i].posY = zombieset[i].posY;
			zombie[i].posZ = zombieset[i].posZ;
			zombie[i].state = zombieset[i].state;
			zombie[i].draw = GL_TRUE;
			switch (zombie[i].state)
			{
			case 1:
				zombie[i].state_rotation = 180.f;
				break;
			case 2:
				zombie[i].state_rotation = -90.f;
				break;
			case 3:
				zombie[i].state_rotation = 0.f;
				break;
			case 4:
				zombie[i].state_rotation = 90.f;
				break;
			}
		}

		realX = posX;
		realY = posY;
		realZ = posZ;
	}
	
	glutPostRedisplay();
	glutTimerFunc(50, Timer, 1);
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	// 키 셋
	printf("공격은 한번에 최대 5회\n");
	printf("플레이어 시점에서만 위치이동 가능\n");
	printf("게임 클리어 및 게임 오버시 자동 재시작\n");
	printf("wasd/WASD - 앞뒤좌우\n");
	printf("q/Q - 좌측 회전\te/E - 우측 회전\n");
	printf("j/J(마우스 좌클릭) - 공격\n");
	printf("t/T - 시점 변화(현재 위치 상공뷰)\n");
	printf("p/P - 재시작\tg/G - 종료\n");

	srand((unsigned int)time(NULL));
	// 화면 사이즈 조절
	width = 1000;
	height = 1000;

	// 키 박스 설정
	GLint temp1 = rand() % 2;
	GLint temp2 = rand() % 7 + 2;
	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (i == temp1 || i == temp2)
		{
			item[i].draw = GL_TRUE;
		}
	}

	// 좀비 초기화
	for (int i = 0; i < MAX_ZOMBIE; i++)
	{
		if (i < 10)
		{
			zombie[i].life = 2;
		}
		else
		{
			zombie[i].life = 4;
		}
		zombie[i].ds = 0.05f;
		zombie[i].mr = (GLfloat)(rand() % 16) * 3.f;
		zombie[i].dmr = 3.f;
		zombie[i].concept_state = zombieset[i].concept_state;
		zombie[i].count = 0;
		zombie[i].posX = zombieset[i].posX;
		zombie[i].posY = zombieset[i].posY;
		zombie[i].posZ = zombieset[i].posZ;
		zombie[i].state = zombieset[i].state;
		zombie[i].draw = GL_TRUE;
		switch (zombie[i].state)
		{
		case 1:
			zombie[i].state_rotation = 180.f;
			break;
		case 2:
			zombie[i].state_rotation = -90.f;
			break;
		case 3:
			zombie[i].state_rotation = 0.f;
			break;
		case 4:
			zombie[i].state_rotation = 90.f;
			break;
		}
	}

	realX = posX;
	realY = posY;
	realZ = posZ;

	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(500, 20); // 윈도우의 위치 지정
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
	glutMouseFunc(Mouse);
	glutPassiveMotionFunc(Motion);
	glutTimerFunc(50, Timer, 1);
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}