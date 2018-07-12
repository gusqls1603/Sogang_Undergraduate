#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
#include <time.h>
#include <math.h>
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0


//////////////////////////////////////////////// ////////////////////////////////////////////////
int game_speed = 50;
int gameover_counter = 0;
int gameover_flag = 0;
int jump_flag = 0;
float jump_time_counter = 0;
float jump_height = 0;
int shake_hand = 0;
int shake_hand2 = 0;
int end_sequence = 0;
int randomseed = 0;
int randomseed2 = 0;
float SENSITIVITY;
int dead_flag = 0;
int peng_clock = 0;
float cliff_scale_ratio = 0;
float cliff_scale_ratio2 = 0;
float cliff_scale_ratio3 = 0;
float cliff_speed_ratio = 0.5;
float cliff_speed_ratio2 = 0.5;
float cliff_speed_ratio3 = 0.5;
int fall_flag = 0;
int fall_length = 0;
int restart_flag = 0;
int restart_counter = 0;
//////////////////////////////////////////////// ////////////////////////////////////////////////


int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.0f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.0f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.0f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.0f;
	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.0f; axes[1][0] = win_width / 2.0f;
	axes[2][1] = -win_height / 2.0f;
	axes[3][1] = win_height / 2.0f;
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
	// y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}


#define HAT_LEAF 0
#define HAT_BODY 1
#define HAT_STRIP 2
#define HAT_BOTTOM 3

GLfloat hat_leaf[4][2] = { { 3.0, 20.0 },{ 3.0, 28.0 },{ 9.0, 32.0 },{ 9.0, 24.0 } };
GLfloat hat_body[4][2] = { { -19.5, 2.0 },{ 19.5, 2.0 },{ 15.0, 20.0 },{ -15.0, 20.0 } };
GLfloat hat_strip[4][2] = { { -20.0, 0.0 },{ 20.0, 0.0 },{ 19.5, 2.0 },{ -19.5, 2.0 } };
GLfloat hat_bottom[4][2] = { { 25.0, 0.0 },{ -25.0, 0.0 },{ -25.0, -4.0 },{ 25.0, -4.0 } };

GLfloat hat_color[4][3] = {
	{ 167 / 255.0f, 255 / 255.0f, 55 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f },
{ 255 / 255.0f, 40 / 255.0f, 33 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f }
};
GLuint VBO_hat, VAO_hat;

void prepare_hat() {
	GLsizeiptr buffer_size = sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip) + sizeof(hat_bottom);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hat_leaf), hat_leaf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf), sizeof(hat_body), hat_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body), sizeof(hat_strip), hat_strip);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip), sizeof(hat_bottom), hat_bottom);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_hat);
	glBindVertexArray(VAO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_hat() {
	glBindVertexArray(VAO_hat);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_LEAF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 8);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_STRIP]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 12);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 16);

	glBindVertexArray(0);
}


#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 },{ -8.0, -9.0 },{ -11.0, 8.0 },{ -6.0, 10.0 },{ -3.0, 7.0 },{ 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 },{ 0.0, 9.0 },{ 3.0, 7.0 },{ 6.0, 10.0 },{ 11.0, 8.0 },{ 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 },{ -3.0, 7.0 },{ -6.0, 10.0 },{ -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 },{ 4.0, 11.0 },{ 6.0, 10.0 },{ 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 },{ 4.0, 1.0 },{ 4.0, 3.0 },{ 7.0, 3.0 },{ 7.0, 1.0 },{ 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 },{ 1.0, 6.0 },{ 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 },{ 1.0, 3.0 },{ 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 },{ 1.0, 0.0 },{ 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 },{ 1.0, -3.0 },{ 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3) + sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}


#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 }, { -20.0, 15.0 }, { -20.0, 20.0 }, { 0.0, 23.0 }, { 20.0, 20.0 }, { 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 }, { -11.0, -12.0 }, { -12.0, -7.0 }, { 0.0, -10.0 }, { 12.0, -7.0 }, { 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 }, { -6.0, 0.0 }, { -6.0, 22.0 }, { 6.0, 22.0 }, { 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 }, { -7.0, 24.0 }, { -7.0, 21.0 }, { 7.0, 21.0 }, { 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 }, { -18.0, 3.0 }, { -16.0, 10.0 }, { -18.0, 20.0 }, { -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 }, { 18.0, 3.0 }, { 16.0, 10.0 }, { 18.0, 20.0 }, { 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

#define AIRPLANE_ROTATION_RADIUS 200.0f
int shirt_clock = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2)+sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}


//////////////////////////////////////////////// ////////////////////////////////////////////////
#define FIN 0
GLfloat fin[4][2] = { { 10.0, 0.0 },{ 10.0, -100.0 },{ -10.0, -100.0 },{ -10.0, 0.0 } };
GLfloat fin_color[1][3] = {
	{ 248 / 255.0f, 246 / 255.0f, 248 / 255.0f }
};
GLuint VBO_fin, VAO_fin;
void prepare_fin() {
	GLsizeiptr buffer_size = sizeof(fin);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_fin);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_fin);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fin), fin);

	// init vertex array obj
	glGenVertexArrays(1, &VAO_fin);
	glBindVertexArray(VAO_fin);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_fin);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_fin() {
	glBindVertexArray(VAO_fin);

	glUniform3fv(loc_primitive_color, 1, fin_color[FIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
}
float fin_x = 0, fin_y = 0;


//////////////////////////////////////////////// ////////////////////////////////////////////////
#define CLIFF 0

GLfloat cliff[6][2] = { { 6.0, 0.0 },{ 4.0, 1.0 } ,{ 5.0, 2.0 },{ -5.0, 2.0 },{ -4.0, 1.0 },{ -6.0, 0.0 } };

GLfloat cliff_color[1][3] = {
	{ 30 / 255.0f, 44 / 255.0f, 253 / 255.0f }
};

GLuint VBO_cliff, VAO_cliff;

void prepare_cliff() {
	GLsizeiptr buffer_size = sizeof(cliff);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_cliff);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cliff);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cliff), cliff);

	// init vertex array obj
	glGenVertexArrays(1, &VAO_cliff);
	glBindVertexArray(VAO_cliff);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cliff);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cliff() {
	glBindVertexArray(VAO_cliff);

	glUniform3fv(loc_primitive_color, 1, cliff_color[CLIFF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glBindVertexArray(0);
}

GLuint VBO_cliff2, VAO_cliff2;

void prepare_cliff2() {
	GLsizeiptr buffer_size = sizeof(cliff);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_cliff2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cliff2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cliff), cliff);

	// init vertex array obj
	glGenVertexArrays(1, &VAO_cliff2);
	glBindVertexArray(VAO_cliff2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cliff2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cliff2() {
	glBindVertexArray(VAO_cliff2);

	glUniform3fv(loc_primitive_color, 1, cliff_color[CLIFF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glBindVertexArray(0);
}

GLuint VBO_cliff3, VAO_cliff3;

void prepare_cliff3() {
	GLsizeiptr buffer_size = sizeof(cliff);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_cliff3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cliff3);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cliff), cliff);

	// init vertex array obj
	glGenVertexArrays(1, &VAO_cliff3);
	glBindVertexArray(VAO_cliff3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cliff3);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cliff3() {
	glBindVertexArray(VAO_cliff3);

	glUniform3fv(loc_primitive_color, 1, cliff_color[CLIFF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glBindVertexArray(0);
}


//////////////////////////////////////////////// ////////////////////////////////////////////////
#define CLOUD1 0
#define CLOUD2 1
#define CLOUD3 2
#define CLOUD4 3
#define CLOUD5 4
#define CLOUD6 5

GLfloat cloud1[4][2] = { { 0.0, 0.0 },{ -57.0, 13.0 } ,{ -5.0, 21.0 },{ 52.0, 0.0 } };
GLfloat cloud2[3][2] = { { -57.0, 13.0 }, { -91.0, 46.0 }, { -5.0, 21.0 } };
GLfloat cloud3[3][2] = { { -91.0, 46.0 },{ -66.0, 73.0 },{ -5.0, 21.0 } };
GLfloat cloud4[8][2] = { { -5.0, 21.0 },{ -66.0, 73.0 } ,{ -25.0, 88.0 },{ 15.0, 92.0 },{ 60.0, 92.0 },{ 107.0, 45.0 },{ 57.0, 53.0 },{ 25.0, 28.0 } };
GLfloat cloud5[3][2] = { { 107.0, 45.0 },{ 57.0, 53.0 },{ 25.0, 28.0 } };
GLfloat cloud6[3][2] = { { 107.0, 45.0 },{ 52.0, 0.0 },{ -5.0, 21.0 } };

GLfloat cloud_color[6][3] = {
	{ 189 / 255.0f, 190 / 255.0f, 192 / 255.0f },
	{ 209 / 255.0f, 210 / 255.0f, 212 / 255.0f },
	{ 231 / 255.0f, 232 / 255.0f, 234 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 241 / 255.0f, 243 / 255.0f, 242 / 255.0f },
	{ 231 / 255.0f, 232 / 255.0f, 236 / 255.0f }
};

GLuint VBO_cloud, VAO_cloud;

void prepare_cloud() {
	GLsizeiptr buffer_size = sizeof(cloud1) + sizeof(cloud2) + sizeof(cloud3) + sizeof(cloud4) + sizeof(cloud5) + sizeof(cloud6);

	glGenBuffers(1, &VBO_cloud);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cloud);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cloud1), cloud1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cloud1), sizeof(cloud2), cloud2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cloud1) + sizeof(cloud2), sizeof(cloud3), cloud3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cloud1) + sizeof(cloud2) + sizeof(cloud3), sizeof(cloud4), cloud4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cloud1) + sizeof(cloud2) + sizeof(cloud3) + sizeof(cloud4), sizeof(cloud5), cloud5);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cloud1) + sizeof(cloud2) + sizeof(cloud3) + sizeof(cloud4) + sizeof(cloud5), sizeof(cloud6), cloud6);

	glGenVertexArrays(1, &VAO_cloud);
	glBindVertexArray(VAO_cloud);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cloud);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cloud() {
	glBindVertexArray(VAO_cloud);

	glUniform3fv(loc_primitive_color, 1, cloud_color[CLOUD1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, cloud_color[CLOUD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 3);

	glUniform3fv(loc_primitive_color, 1, cloud_color[CLOUD3]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 3);

	glUniform3fv(loc_primitive_color, 1, cloud_color[CLOUD4]);
	glDrawArrays(GL_TRIANGLE_FAN, 10, 8);

	glUniform3fv(loc_primitive_color, 1, cloud_color[CLOUD5]);
	glDrawArrays(GL_TRIANGLE_FAN, 18, 3);

	glUniform3fv(loc_primitive_color, 1, cloud_color[CLOUD6]);
	glDrawArrays(GL_TRIANGLE_FAN, 21, 3);

	glBindVertexArray(0);
}

//////////////////////////////////////////////// ////////////////////////////////////////////////
#define VOLCANO1 0
#define VOLCANO2 1
#define VOLCANO3 2

GLfloat volcano1[4][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 },{ 0.0, 0.0 } };
GLfloat volcano2[4][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 },{ 0.0, 0.0 } };
GLfloat volcano3[4][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 },{ 0.0, 0.0 } };

GLfloat volcano_color[3][3] = {
	{ 247 / 255.0f, 248 / 255.0f, 251 / 255.0f }, // initial volcano color (be changed after a trigger)
	{ 247 / 255.0f, 248 / 255.0f, 251 / 255.0f }, // initial volcano color (be changed after a trigger)
	{ 247 / 255.0f, 248 / 255.0f, 251 / 255.0f } // initial volcano color (be changed after a trigger)
};

GLuint VBO_vol, VAO_vol;

void prepare_vol() {
	GLsizeiptr buffer_size = sizeof(volcano1) + sizeof(volcano2) + sizeof(volcano3);

	glGenBuffers(1, &VBO_vol);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_vol);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(volcano1), volcano1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(volcano1), sizeof(volcano2), volcano2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(volcano1) + sizeof(volcano2), sizeof(volcano3), volcano3);

	glGenVertexArrays(1, &VAO_vol);
	glBindVertexArray(VAO_vol);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_vol);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_vol() {
	glBindVertexArray(VAO_vol);

	glUniform3fv(loc_primitive_color, 1, volcano_color[VOLCANO1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, volcano_color[VOLCANO2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, volcano_color[VOLCANO3]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glBindVertexArray(0);
}

//////////////////////////////////////////////// ////////////////////////////////////////////////
#define SEA_LEFT 0
#define SEA_RIGHT 1
#define SEA_SKY 2
#define SEA_GLACIER1 3
#define SEA_GLACIER2 4

GLfloat sea_left[3][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 } };
GLfloat sea_right[3][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 } };
GLfloat sea_sky[4][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 },{ 0.0, 0.0 } };
GLfloat sea_glacier1[6][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 },{ 0.0, 0.0 },{ 0.0, 0.0 },{ 0.0, 0.0 } };
GLfloat sea_glacier2[4][2] = { { 0.0, 0.0 },{ 0.0, 0.0 } ,{ 0.0, 0.0 },{ 0.0, 0.0 } };

GLfloat sea_color[5][3] = {
	{63 / 255.0f, 74 / 255.0f, 255 / 255.0f}, // left sea
	{63 / 255.0f, 74 / 255.0f, 255 / 255.0f }, // right sea
	{87 / 255.0f, 197 / 255.0f, 228 / 255.0f}, // sky
	{247 / 255.0f, 248 / 255.0f, 251 / 255.0f }, // glacier1
	{247 / 255.0f, 248 / 255.0f, 251 / 255.0f } // glacier2
};

GLuint VBO_sea, VAO_sea;

void prepare_sea() {
	GLsizeiptr buffer_size = sizeof(sea_left) + sizeof(sea_right) + sizeof(sea_sky) + sizeof(sea_glacier1) + sizeof(sea_glacier2);

	glGenBuffers(1, &VBO_sea);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sea);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sea_left), sea_left);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sea_left), sizeof(sea_right), sea_right);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sea_left) + sizeof(sea_right), sizeof(sea_sky), sea_sky);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sea_left) + sizeof(sea_right) + sizeof(sea_sky), sizeof(sea_glacier1), sea_glacier1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sea_left) + sizeof(sea_right) + sizeof(sea_sky) + sizeof(sea_glacier1), sizeof(sea_glacier2), sea_glacier2);

	glGenVertexArrays(1, &VAO_sea);
	glBindVertexArray(VAO_sea);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sea);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sea() {
	glBindVertexArray(VAO_sea);

	glUniform3fv(loc_primitive_color, 1, sea_color[SEA_LEFT]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, sea_color[SEA_RIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 3);

	glUniform3fv(loc_primitive_color, 1, sea_color[SEA_SKY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 4);

	glUniform3fv(loc_primitive_color, 1, sea_color[SEA_GLACIER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 10, 6);

	glUniform3fv(loc_primitive_color, 1, sea_color[SEA_GLACIER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glBindVertexArray(0);
}

//////////////////////////////////////////////// ////////////////////////////////////////////////
#define PENG_BODY 0
#define COCKTAIL_NECK 1
#define COCKTAIL_LIQUID 2
#define COCKTAIL_REMAIN 3
#define COCKTAIL_STRAW 4
#define COCKTAIL_DECO 5

int peng_rotate = 0; // 0, 10, 20, 30
float peng_x = 0.0f, peng_y = 0.0f;

GLfloat pengbody[19][2] = { {15,0}, {32,10}, {40,30}, {41,46}, {55,45}, {38,56}, {35,70}, {28,87}, {15,100}, {0, 105},
						 {-15,100},{ -28,87 },{ -35,70 },{ -38,56 },{ -55,45 },{ -41,46 },{ -40,30 },{ -32,10 }, {-15,0} };
GLfloat pengjump[19][2] = { { 15,0 },{ 32,10 },{ 40,30 },{ 41,46 },{ 55,45 },{ 38,56 },{ 35,70 },{ 28,87 },{ 15,100 },{ 0, 105 },
						{ -15,100 },{ -28,87 },{ -35,70 },{ -38,56 },{ -55,60 },{ -41,46 },{ -40,30 },{ -32,10 },{ -15,0 } };
GLfloat neck[6][2] = { { 49.0, 45.0 },{ 49.0, 46.0 },{ 54.0, 57.0 },{ 56.0, 57.0 },{ 61.0, 46.0 },{ 61.0, 45.0 } };
GLfloat liquid[6][2] = { { 54.0, 57.0 },{ 46.0, 61.0 },{ 43.0, 64.0 },{ 67.0, 64.0 },{ 64.0, 61.0 },{ 56.0, 57.0 } };
GLfloat remain[4][2] = { { 43.0, 64.0 },{ 43.0, 67.0 },{ 67.0, 67.0 },{ 67.0, 64.0 } };
GLfloat straw[4][2] = { { 62.0, 64.0 },{ 67.0, 69.0 },{ 69.0, 69.0 },{ 64.0, 64.0 } };
GLfloat deco[8][2] = { { 67.0, 69.0 },{ 65.0, 71.0 },{ 65.0, 73.0 },{ 67.0, 75.0 },{ 69.0, 75.0 },{ 71.0, 73.0 },{ 71.0, 71.0 },{ 69.0, 69.0 } };

GLfloat peng_color[6][3] = {
	{ 2 / 255.0f, 0 / 255.0f, 6 / 255.0f },  // body
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};


// penguin with cocktail
GLuint VBO_peng, VAO_peng, VBO_pengjump, VAO_pengjump;

void prepare_peng() {
	GLsizeiptr buffer_size = sizeof(pengbody) + sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw) + sizeof(deco);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_peng);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_peng);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(neck), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw), sizeof(deco), deco);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw) + sizeof(deco), sizeof(pengbody), pengbody);

	// init vertex array obj
	glGenVertexArrays(1, &VAO_peng);
	glBindVertexArray(VAO_peng);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_peng);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void prepare_pengjump() {
	GLsizeiptr buffer_size = sizeof(pengjump) + sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw) + sizeof(deco);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_pengjump);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pengjump);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(neck), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw), sizeof(deco), deco);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw) + sizeof(deco), sizeof(pengjump), pengjump);

	// init vertex array obj
	glGenVertexArrays(1, &VAO_pengjump);
	glBindVertexArray(VAO_pengjump);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pengjump);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_peng() {
	glBindVertexArray(VAO_peng);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glUniform3fv(loc_primitive_color, 1, peng_color[PENG_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 19);

	glBindVertexArray(0);
}
void draw_pengjump() {
	glBindVertexArray(VAO_pengjump);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, peng_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glUniform3fv(loc_primitive_color, 1, peng_color[PENG_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 19);

	glBindVertexArray(0);
}


// penguin without cocktail
GLfloat pengbody2[19][2] = { { 15,-50 },{ 32,-40 },{ 40,-20 },{ 41,-4 },{ 55,-5 },{ 38,6 },{ 35,20 },{ 28,37 },{ 15,50 },{ 0, 55 },
							{ -15,50 },{ -28,37 },{ -35,20 },{ -38,6 },{ -55,-5 },{ -41,-4 },{ -40,-20 },{ -32,-40 },{ -15,-50 } };
GLfloat pengjump2[19][2] = { { 15,-50 },{ 32,-40 },{ 40,-20 },{ 41,-4 },{ 55,10 },{ 38,6 },{ 35,20 },{ 28,37 },{ 15,50 },{ 0, 55 },
							{ -15,50 },{ -28,37 },{ -35,20 },{ -38,6 },{ -55,10 },{ -41,-4 },{ -40,-20 },{ -32,-40 },{ -15,-50 } };

GLuint VBO_peng2, VAO_peng2, VBO_pengjump2, VAO_pengjump2;

void prepare_peng2() {
	GLsizeiptr buffer_size = sizeof(pengbody2);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_peng2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_peng2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pengbody2), pengbody2);

	// init vertex array obj
	glGenVertexArrays(1, &VAO_peng2);
	glBindVertexArray(VAO_peng2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_peng2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void prepare_pengjump2() {
	GLsizeiptr buffer_size = sizeof(pengjump2);

	// init vertex buffer obj
	glGenBuffers(1, &VBO_pengjump2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pengjump2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer obj mem

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pengjump2), pengjump2);
	
	// init vertex array obj
	glGenVertexArrays(1, &VAO_pengjump2);
	glBindVertexArray(VAO_pengjump2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pengjump2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_peng2() {
	glBindVertexArray(VAO_peng2);

	glUniform3fv(loc_primitive_color, 1, peng_color[PENG_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 19);

	glBindVertexArray(0);
}
void draw_pengjump2() {
	glBindVertexArray(VAO_pengjump2);

	glUniform3fv(loc_primitive_color, 1, peng_color[PENG_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 19);

	glBindVertexArray(0);
}


//////////////////////////////////////////////// ////////////////////////////////////////////////

int l1=0, l2=0, l3=0, l4=0; // 0 : 없음 (80%), 1 : 구름(15%), 2 : 비행기(4%) 3 : 셔츠 (1%)
int l1_x = 0, l2_x = 0, l3_x = 0, l4_x = 0;
float l1_y, l2_y, l3_y, l4_y;
int line1() {
	int p;
	if (l1 == 0) {
		if (gameover_flag == 1) return 0; // 이스터에그 진입 시 추가 물체 생성 안함
		p = rand() % 100;
		if (p < 15) l1 = 1;
		else if (p >= 15 && p < 19) l1 = 2;
		else if (p >= 19 && p < 20) l1 = 3;
		else l1 = 0;
	}
	if (l1 == 0) l1_x = 0;
	else if(l1 == 1) l1_x -= 12;
	else if (l1 == 2) l1_x -= 24;
	else l1_x -= 18;

	return l1_x;
}
int line2() {
	int p;
	if (l2 == 0) {
		if (gameover_flag == 1) return 0;
		p = rand() % 100;
		if (p >= 25 && p < 40) l2 = 1;
		else if (p >= 41 && p < 44) l2 = 2;
		else if (p >= 44 && p < 45) l2 = 3;
		else l2 = 0;
	}
	if (l2 == 0) l2_x = 0;
	else if (l2 == 1) l2_x -= 6;
	else if (l2 == 2) l2_x -= 16;
	else l2_x -= 12;

	return l2_x;
}
int line3() {
	int p;
	if (l3 == 0) {
		if (gameover_flag == 1) return 0;
		p = rand() % 100;
		if (p >= 50 && p < 65) l3 = 1;
		else if (p >= 66 && p < 69) l3 = 2;
		else if (p >= 69 && p < 70) l3 = 3;
		else l3 = 0;
	}
	if (l3 == 0) l3_x = 0;
	else if (l3 == 1) l3_x += 6;
	else if (l3 == 2) l3_x += 16;
	else l3_x += 12;

	return l3_x;
}
int line4() {
	int p;
	if (l4 == 0) {
		if (gameover_flag == 1) return 0;
		p = rand() % 100;
		if (p >= 75 && p < 90) l4 = 1;
		else if (p >= 91 && p < 94) l4 = 2;
		else if (p >= 94 && p < 95) l4 = 3;
		else l4 = 0;
	}
	if (l4 == 0) l4_x = 0;
	else if (l4 == 1) l4_x += 12;
	else if (l4 == 2) l4_x += 24;
	else l4_x += 18;

	return l4_x;
}


int e1 = 0; // 0 : 없음(90%), 1 : 모자(8%), 2 : 펭귄(2%)
float e1_x = 0;
float e1_y, a;
float exp_line1() {
	int p;
	if (e1 == 0) {
		if (gameover_flag == 1) return 0;
		p = rand() % 100;
		randomseed = rand() % 3; // 날아가는 트랙 3가지
		randomseed2 = rand() % 3; // 33% 확률로 밑에서 위로 날아감
		if (randomseed2 == 0) {
			randomseed2 = (-1);
		}
		else {
			randomseed2 = 1;
		}
		if (p < 8) e1 = 1;
		else if (p >= 8 && p < 10) e1 = 2;
		else e1 = 0;
	}
	if (e1 == 0) e1_x = -(win_width / 3.0f);
	else if (e1 == 1) e1_x += 30;
	else e1_x += 50;

	return e1_x;
}


int c1 = 0, c2 = 0, c3 = 0; // 0 : 없음(50%), 1 : 절벽(50%)
int c1_x = 0, c2_x = 0, c3_x = 0;
float c1_y, c2_y, c3_y;
int c_line1() {
	int p;
	if (c1 == 0) {
		if (gameover_flag == 1) return 0;
		p = rand() % 100;
		if (p < 50) c1 = 1;
		else c1 = 0;
	}
	if (c1 == 0) c1_x = 0;
	else c1_x -= (3 + (win_width/100.0f)*cliff_speed_ratio);

	return c1_x;
}
int c_line2() {
	int p;
	if (c2 == 0) {
		if (gameover_flag == 1) return 0;
		p = rand() % 100;
		if (p >= 33 && p < 83) c2 = 1;
		else c2 = 0;
	}
	if (c2 == 0) c2_x = 0;
	else c2_x -= (3 + (win_width / 100.0f)*cliff_speed_ratio2);

	return c2_x;
}
int c_line3() {
	int p;
	if (c3 == 0) {
		if (gameover_flag == 1) return 0;
		p = rand() % 100;
		if (p >= 50) c3 = 1;
		else c3 = 0;
	}
	if (c3 == 0) c3_x = 0;
	else c3_x += (3 + (win_width / 100.0f)*cliff_speed_ratio3);

	return c3_x;
}



// 남은 할 일
// 0. 장애물 추가
// 1. 눈 모델 추가
// 2. 눈 내리는 움직임 추가

void display(void) {
	int i;
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);
	
	// background scene
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_sea();


	if (fall_flag == 0) {
		// cliff
		c1_x = c_line1();
		if (c1_x != 0) { // 장애물 등장
			c1_y = (((win_height / 22.0f) * 2.5) / ((win_width / 20.0f) * 1)) * (float)c1_x + (win_height / 22.0f) * 4;
			if (c1_y < (win_height / 22.0f) * 3) {
				cliff_speed_ratio += (game_speed / 2000.0f);
//				cliff_speed_ratio += (2000.0f / game_speed);
				cliff_scale_ratio += (win_width / 100.0f); // 장애물 스케일 비율

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(c1_x, c1_y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio) / 10, (cliff_scale_ratio) / 10, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_cliff();

				if (c1_y < -(win_height / 1.7f)) {
					c1 = 0;
					cliff_scale_ratio = 0;
					cliff_speed_ratio = 0.5;
				}

				// 펭귄 추락 여부 체크
				if (jump_flag == 0 && c1_y < -win_height / 2.5f && c1_y + (cliff_scale_ratio) / 5 > -win_height / 2.5f && restart_flag == 0 && gameover_flag == 0) {
					if (c1_x - (cliff_scale_ratio) / 2 < peng_x - 30 && c1_x + (cliff_scale_ratio) / 2 > peng_x + 30)
						fall_flag = 1;
				}
			}
		}
		c2_x = c_line2();
		if (c2_x != 0) { // 장애물 등장
			c2_y = (((win_height / 22.0f) * 2.5) / ((win_width / 20.0f) * 1)) * (float)c2_x + (win_height / 22.0f) * 4;
			if (c2_y < (win_height / 22.0f) * 3) {
				cliff_speed_ratio2 += (game_speed / 2000.0f);
//				cliff_speed_ratio2 += (2000.0f / game_speed);
				cliff_scale_ratio2 += (win_width / 100.0f); // 장애물 스케일 비율

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, c2_y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio2) / 10, (cliff_scale_ratio2) / 10, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_cliff2();

				if (c2_y < -(win_height / 1.7f)) {
					c2 = 0;
					cliff_scale_ratio2 = 0;
					cliff_speed_ratio2 = 0.5;
				}

				// 펭귄 추락 여부 체크
				if (jump_flag == 0 && c2_y < -win_height / 2.5f && c2_y + (cliff_scale_ratio2) / 5 > -win_height / 2.5f && restart_flag == 0 && gameover_flag == 0) {
					if (0 - (cliff_scale_ratio2) / 2 < peng_x - 30 && 0 + (cliff_scale_ratio2) / 2 > peng_x + 30)
						fall_flag = 2;
				}
			}
		}
		c3_x = c_line3();
		if (c3_x != 0) { // 장애물 등장
			c3_y = (((win_height / 22.0f) * 2.5) / ((win_width / 20.0f) * 1))*(-1) * (float)c3_x + (win_height / 22.0f) * 4;
			if (c3_y < (win_height / 22.0f) * 3) {
				cliff_speed_ratio3 += (game_speed / 2000.0f);
//				cliff_speed_ratio3 += (2000.0f / game_speed);
				cliff_scale_ratio3 += (win_width / 100.0f); // 장애물 스케일 비율

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(c3_x, c3_y, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio3) / 10, (cliff_scale_ratio3) / 10, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_cliff3();

				if (c3_y < -(win_height / 1.7f)) {
					c3 = 0;
					cliff_scale_ratio3 = 0;
					cliff_speed_ratio3 = 0.5;
				}

				// 펭귄 추락 여부 체크
				if (jump_flag == 0 && c3_y < -win_height / 2.5f && c3_y + (cliff_scale_ratio3) / 5 > -win_height / 2.5f && restart_flag == 0 && gameover_flag == 0) {
					if (c3_x - (cliff_scale_ratio3) / 2 < peng_x - 30 && c3_x + (cliff_scale_ratio3) / 2 > peng_x + 30)
						fall_flag = 3;
				}
			}
		}
	}
	else { // when fall flag = 1
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(c1_x, c1_y, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio) / 10, (cliff_scale_ratio) / 10, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cliff();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, c2_y, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio2) / 10, (cliff_scale_ratio2) / 10, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cliff2();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(c3_x, c3_y, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio3) / 10, (cliff_scale_ratio3) / 10, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cliff3();
	}



	// penguin
	if (jump_flag==0) { 
		peng_y = -win_height / 2.5f;
		if (gameover_flag == 0 && restart_flag == 0) { // walk
			if (peng_rotate == 10 || peng_rotate == 15) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(peng_x, peng_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (-5)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			}
			else if (peng_rotate == 30 || peng_rotate == 35) {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(peng_x, peng_y, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (5)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			}
			else {
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(peng_x, peng_y, 0.0f));
			}
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_peng();
		}
		else if (gameover_flag == 0 && fall_flag != 0) {
			peng_y = -win_height / 2.5f - fall_length; // fall_length는 평소에 0

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(peng_x, peng_y, 0.0f));

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_peng();
		}
		else if (restart_flag == 0) { // die
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(peng_x, peng_y, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, peng_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_peng();
		}
	}
	else { // jump
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(peng_x, peng_y + jump_height, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (shake_hand == 0) {
			draw_pengjump();
			shake_hand = 1;
		}
		else {
			draw_peng();
			shake_hand = 0;
		}
	}
	
	// 펭귄 앞 가리개?
	if (fall_flag != 0) {
		if (fall_flag == 1) {
			fin_x = c1_x;
			fin_y = c1_y;

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(fin_x, fin_y, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio) / 10, (cliff_scale_ratio) / 10, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_fin();
		}
		if (fall_flag == 2) {
			fin_x = 0;
			fin_y = c2_y;

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(fin_x, fin_y, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio2) / 10, (cliff_scale_ratio2) / 10, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_fin();
		}
		if (fall_flag == 3) {
			fin_x = c3_x;
			fin_y = c3_y;

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(fin_x, fin_y, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3((cliff_scale_ratio3) / 10, (cliff_scale_ratio3) / 10, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_fin();
		}
	}

	// objects in sky

	l1_x = line1();
	if (l1_x != 0) {
			l1_y = -(2.0 / 3.0)*(float)l1_x;
			if (l1_y >= (win_height / 22.0f) * 3.2) {
				if (l1 == 1) { // 구름
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l1_x, l1_y, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.6 * l1_y / (win_height / 2.0f), 0.6 * l1_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cloud();
				}
				else if (l1 == 2) { // 비행기
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l1_x, l1_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, 236 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2 * l1_y / (win_height / 2.0f), 2 * l1_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_airplane();
				}
				else { // 셔츠
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l1_x, l1_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, (shirt_clock)* TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_shirt();
				}

				if (l1_y >= (win_height / 2.0f))
					l1 = 0;
			}
		}
	l2_x = line2();
	if (l2_x != 0) {
			l2_y = -(3.0 / 2.0)*(float)l2_x;
			if (l2_y >= (win_height / 22.0f) * 3.2) {
				if (l2 == 1) { // 구름
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l2_x, l2_y, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.6 * l2_y / (win_height / 2.0f), 0.6 * l2_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cloud();
				}
				else if (l2 == 2) { // 비행기
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l2_x, l2_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, 214 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2 * l2_y / (win_height / 2.0f), 2 * l2_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_airplane();
				}
				else {
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l2_x, l2_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, (2 * shirt_clock)* TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_shirt();
				}

				if (l2_y >= (win_height / 2.0f))
					l2 = 0;
			}
		}
	l3_x = line3();
	if (l3_x != 0) {
			l3_y = (3.0 / 2.0)*(float)l3_x;
			if (l3_y >= (win_height / 22.0f) * 3.2) {
				if (l3 == 1) { // 구름
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l3_x, l3_y, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.6 * l3_y / (win_height / 2.0f), 0.6 * l3_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cloud();
				}
				else if (l3 == 3) { // 비행기
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l3_x, l3_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, 146 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2 * l3_y / (win_height / 2.0f), 2 * l3_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_airplane();
				}
				else {
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l3_x, l3_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, ((-1)*shirt_clock)* TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_shirt();
				}

				if (l3_y >= (win_height / 2.0f))
					l3 = 0;
			}
		}
	l4_x = line4();
	if (l4_x != 0) {
			l4_y = (2.0 / 3.0)*(float)l4_x;
			if (l4_y >= (win_height / 22.0f) * 3.2) {
				if (l4 == 1) { // 구름
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l4_x, l4_y, 0.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.6 * l4_y / (win_height / 2.0f), 0.6 * l4_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cloud();
				}
				else if (l4 == 3) { // 비행기
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l4_x, l4_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, 124 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2 * l4_y / (win_height / 2.0f), 2 * l4_y / (win_height / 2.0f), 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_airplane();
				}
				else {
					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(l4_x, l4_y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, ((-2) * shirt_clock)* TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_shirt();
				}

				if (l4_y >= (win_height / 2.0f))
					l4 = 0;
			}
		}



	// objects in scene
	e1_x = exp_line1();
	if (e1 != 0) {

		e1_y = (((win_height/1.0f) / ((win_width * win_width)/1.0f)) * (18.0 / 11.0)) * (e1_x - (win_width / 2.0f)) * (e1_x - (win_width / 2.0f)) - (win_height / 22.0f) * 6;

		if (e1_x <= (win_width / 2.0f)*1.2) {
			if (e1 == 1) { // 모자
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(e1_x, randomseed2 * (e1_y - randomseed * (win_height/22.0f)*6), 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (shirt_clock)*2* TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_hat();
			}
			if (e1 == 2) { // 펭귄
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(e1_x, randomseed2 * (e1_y - randomseed * (win_height / 22.0f) * 6), 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (shirt_clock)*4* TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

				if (shake_hand2 == 0) {
					draw_pengjump2();
					shake_hand2 = 1;
				}
				else {
					draw_peng2();
					shake_hand2 = 0;
				}
			}
		}
		else {
			e1 = 0;
			e1_x = -(win_width / 3.0f);
		}

	}


	// volcano scene
	if (gameover_flag == 1) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_vol();
	}

	glFlush();	
}   

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 32: // SPACE key
		if(gameover_flag==0 && fall_flag == 0)
			jump_flag = 1;
		else if (fall_flag != 0) {
			fall_flag = 0;
			fall_length = 0;
		}
		break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		if(peng_x - SENSITIVITY >= -(win_width / 2.0f) +(win_height / 22.0f) * 7 && gameover_flag == 0 && fall_flag == 0)
			peng_x -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		if (peng_x + SENSITIVITY <= (win_width / 2.0f) -(win_height / 22.0f) * 7 && gameover_flag == 0 && fall_flag == 0)
			peng_x += SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (gameover_flag == 0 && fall_flag == 0) {
			if (game_speed == 10)
				game_speed += 10;
			else if (game_speed + 30 <= 200)
				game_speed += 30;
		}
		centery -= SENSITIVITY;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if (gameover_flag == 0 && fall_flag == 0) {
			if (game_speed - 30 >= 20)
				game_speed -= 30;
			else if (game_speed == 20)
				game_speed -= 10;
		}
		centery += SENSITIVITY;
		glutPostRedisplay();
		break;
	}
}

// not used
int leftbuttonpressed = 0;
void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
}

// not used
void motion(int x, int y) {
	static int delay = 0;
	static float tmpx = 0.0, tmpy = 0.0;
	float dx, dy;
	if (leftbuttonpressed) {
		centerx =  x - win_width/2.0f, centery = (win_height - y) - win_height/2.0f;
		if (delay == 8) {	
			dx = centerx - tmpx;
			dy = centery - tmpy;
	  
			if (dx > 0.0) {
				rotate_angle = atan(dy / dx) + 90.0f*TO_RADIAN;
			}
			else if (dx < 0.0) {
				rotate_angle = atan(dy / dx) - 90.0f*TO_RADIAN;
			}
			else if (dx == 0.0) {
				if (dy > 0.0) rotate_angle = 180.0f*TO_RADIAN;
				else  rotate_angle = 0.0f;
			}
			tmpx = centerx, tmpy = centery; 
			delay = 0;
		}
		glutPostRedisplay();
		delay++;
	}
} 
	
void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void timer(int value) {
	srand(time(NULL));
	shirt_clock = (shirt_clock + 20) % 720;
	SENSITIVITY = (win_width / 20.0f);
	if (peng_x < -(win_width / 2.0f) + (win_height / 22.0f) * 7)
		peng_x = -(win_width / 2.0f) + (win_height / 22.0f) * 7;
	if (peng_x > (win_width / 2.0f) - (win_height / 22.0f) * 7)
		peng_x = (win_width / 2.0f) - (win_height / 22.0f) * 7;

	sea_left[0][0] = -(win_width / 20.0f); sea_left[0][1] = (win_height / 22.0f) * 3;
	sea_left[1][0] = -(win_width / 2.0f); sea_left[1][1] = (win_height / 22.0f) * 3;
	sea_left[2][0] = -(win_width / 2.0f); sea_left[2][1] = -(win_height / 22.0f) * 7;
	sea_right[0][0] = (win_width / 20.0f); sea_right[0][1] = (win_height / 22.0f) * 3;
	sea_right[1][0] = (win_width / 2.0f); sea_right[1][1] = (win_height / 22.0f) * 3;
	sea_right[2][0] = (win_width / 2.0f); sea_right[2][1] = -(win_height / 22.0f) * 7;
	sea_sky[0][0] = -(win_width / 2.0f); sea_sky[0][1] = win_height / 2.0f;
	sea_sky[1][0] = -(win_width / 2.0f); sea_sky[1][1] = (win_height / 22.0f) * 3;
	sea_sky[2][0] = (win_width / 2.0f); sea_sky[2][1] = (win_height / 22.0f) * 3;
	sea_sky[3][0] = (win_width / 2.0f); sea_sky[3][1] = win_height / 2.0f;
	sea_glacier1[0][0] = -(win_width / 80.0f) * 13; sea_glacier1[0][1] = (win_height / 44.0f) * 6;
	sea_glacier1[1][0] = -(win_width / 80.0f) * 16; sea_glacier1[1][1] = (win_height / 44.0f) * 7;
	sea_glacier1[2][0] = -(win_width / 80.0f) * 21; sea_glacier1[2][1] = (win_height / 44.0f) * 9;
	sea_glacier1[3][0] = -(win_width / 80.0f) * 28; sea_glacier1[3][1] = (win_height / 88.0f) * 19;
	sea_glacier1[4][0] = -(win_width / 80.0f) * 32; sea_glacier1[4][1] = (win_height / 88.0f) * 19;
	sea_glacier1[5][0] = -(win_width / 80.0f) * 37; sea_glacier1[5][1] = (win_height / 44.0f) * 6;
	sea_glacier2[0][0] = (win_width / 80.0f) * 14; sea_glacier2[0][1] = (win_height / 44.0f) * 6;
	sea_glacier2[1][0] = (win_width / 80.0f) * 17; sea_glacier2[1][1] = (win_height / 44.0f) * 8;
	sea_glacier2[2][0] = (win_width / 80.0f) * 30; sea_glacier2[2][1] = (win_height / 44.0f) * 8;
	sea_glacier2[3][0] = (win_width / 80.0f) * 32; sea_glacier2[3][1] = (win_height / 44.0f) * 6;
	prepare_sea();

	if (fall_flag != 0) {
		restart_flag = 1;
		game_speed = 50;
		fall_length += 45; // 추락속도조절
	}
	if (restart_flag == 1 && fall_flag == 0) {
		restart_counter++;
		if (restart_counter == 20) {
			restart_flag = 0;
			restart_counter = 0;
			peng_x = 0;
		}
	}

	if (gameover_flag == 0) {
		if (jump_flag == 0) {
			if (game_speed == 200) {
				peng_rotate = 0;
				gameover_counter++;
			}
			else {
				peng_rotate += 5;
				if (peng_rotate == 40)
					peng_rotate = 0;
				gameover_counter = 0;
			}
		}
		else {
			jump_height = (-(jump_time_counter - 0.5)*(jump_time_counter - 0.5) + 0.25) * 300.0;
			jump_time_counter = jump_time_counter + (game_speed / 1000.0);
//			printf("%f\n", jump_time_counter);
			if (jump_time_counter >= 1.0) {
				jump_flag = 0;
				jump_time_counter = 0;
			}
		}

		if (gameover_counter == 10) { // 이스터에그 진입
			gameover_flag = 1;
			game_speed = 50;
		}
	}
	else { // 종말
		if (end_sequence <= 260) end_sequence++;
		if (sea_color[2][0] > 0 / 255.0f) sea_color[2][0] = sea_color[2][0] - 4 / 255.0f;
		if (sea_color[2][1] > 0 / 255.0f) sea_color[2][1] = sea_color[2][1] - 4 / 255.0f;
		if (sea_color[2][2] > 0 / 255.0f) sea_color[2][2] = sea_color[2][2] - 4 / 255.0f;

		volcano1[0][0] = -(win_width / 80.0f) * 21; volcano1[0][1] = (win_height / 44.0f) * 9;
		volcano1[1][0] = -(win_width / 80.0f) * 28; volcano1[1][1] = (win_height / 88.0f) * 19;
		volcano1[2][0] = -(win_width / 80.0f) * 32; volcano1[2][1] = (win_height / 88.0f) * 19;
		volcano1[3][0] = -(win_width / 80.0f) * (37.0 - (30.0 / 7.0)); volcano1[3][1] = (win_height / 44.0f) * 9;
		volcano2[0][0] = -(win_width / 80.0f) * 16; volcano2[0][1] = (win_height / 44.0f) * 7;
		volcano2[1][0] = -(win_width / 80.0f) * 21; volcano2[1][1] = (win_height / 44.0f) * 9;
		volcano2[2][0] = -(win_width / 80.0f) * (37.0 - (30.0 / 7.0)); volcano2[2][1] = (win_height / 44.0f) * 9;
		volcano2[3][0] = -(win_width / 80.0f) * (37.0 - (10.0 / 7.0)); volcano2[3][1] = (win_height / 44.0f) * 7;
		volcano3[0][0] = -(win_width / 80.0f) * 13; volcano3[0][1] = (win_height / 44.0f) * 6;
		volcano3[1][0] = -(win_width / 80.0f) * 16; volcano3[1][1] = (win_height / 44.0f) * 7;
		volcano3[2][0] = -(win_width / 80.0f) * (37.0 - (10.0 / 7.0)); volcano3[2][1] = (win_height / 44.0f) * 7;
		volcano3[3][0] = -(win_width / 80.0f) * 37; volcano3[3][1] = (win_height / 44.0f) * 6;

		if (end_sequence == 80) { volcano_color[0][0] = 255 / 255.0f; volcano_color[0][1] = 72 / 255.0f; volcano_color[0][2] = 5 / 255.0f; }
		if (end_sequence == 100) { volcano_color[1][0] = 255 / 255.0f; volcano_color[1][1] = 72 / 255.0f; volcano_color[1][2] = 5 / 255.0f; }
		if (end_sequence == 120) { volcano_color[2][0] = 255 / 255.0f; volcano_color[2][1] = 72 / 255.0f; volcano_color[2][2] = 5 / 255.0f; }
		prepare_vol();
		
		if (end_sequence >= 130) {
			if (sea_color[0][0] < 167 / 255.0f) { sea_color[0][0] += 3 / 255.0f; sea_color[1][0] += 3 / 255.0f; }
			if (sea_color[0][1] > 16 / 255.0f) { sea_color[0][1] -= 3 / 255.0f; sea_color[1][1] -= 3 / 255.0f; }
			if (sea_color[0][2] > 28 / 255.0f) { sea_color[0][2] -= 3 / 255.0f; sea_color[1][2] -= 3 / 255.0f; }
		}

		if (end_sequence >= 200) { // 펭귄 쓰러짐
			dead_flag = 1;
			if (peng_clock > -90) peng_clock-=10;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(game_speed, timer, 0);
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

// 배경색
void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(248 / 255.0f, 246 / 255.0f, 248 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_peng();
	prepare_pengjump();
	prepare_cloud();
	prepare_shirt();
	prepare_hat();
	prepare_peng2();
	prepare_pengjump2();
	prepare_cliff();
	prepare_cliff2();
	prepare_cliff3();
	prepare_fin();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program(); 
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

    error = glewInit();
	if (error != GLEW_OK) { 
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple2DTransformation_GLSL_3.0";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC', four arrows, 'SPACE BAR'",
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200*0.95, 800*0.95);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop ();
}


