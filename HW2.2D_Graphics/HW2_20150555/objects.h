#pragma once
GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

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
	axes[0][0] = -win_width / 2.5f; axes[1][0] = win_width / 2.5f;
	axes[2][1] = -win_height / 2.5f;
	axes[3][1] = win_height / 2.5f;

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

float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

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

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 14.0 },{ 0.0, 26.0 },{ 12.0, 14.0 } };
GLfloat house_body[4][2] = { { -12.0, 0.0 },{ -12.0, 14.0 },{ 12.0, 14.0 },{ 12.0, 0.0 } };
GLfloat chimney[4][2] = { { 6.0, 20.0 },{ 6.0, 28.0 },{ 10.0, 28.0 },{ 10.0, 16.0 } };
GLfloat door[4][2] = { { -8.0, 0.0 },{ -8.0, 6.0 },{ -4.0, 6.0 },{ -4.0, 0.0 } };
GLfloat window[4][2] = { { 4.0, 8.0 },{ 4.0, 12.0 },{ 8.0, 12.0 },{ 8.0, 8.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//shirt
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

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//draw cocktail
#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 },{ -6.0, -11.0 },{ -1.0, 0.0 },{ 1.0, 0.0 },{ 6.0, -11.0 },{ 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 },{ -9.0, 4.0 },{ -12.0, 7.0 },{ 12.0, 7.0 },{ 9.0, 4.0 },{ 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 },{ -12.0, 10.0 },{ 12.0, 10.0 },{ 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 },{ 12.0, 12.0 },{ 14.0, 12.0 },{ 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 },{ 10.0, 14.0 },{ 10.0, 16.0 },{ 12.0, 18.0 },{ 14.0, 18.0 },{ 16.0, 16.0 },{ 16.0, 14.0 },{ 14.0, 12.0 } };

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};

GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw)
		+ sizeof(deco);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw),
		sizeof(deco), deco);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glBindVertexArray(VAO_cocktail);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 },{ -18.0, 0.0 },{ -13.0, 0.0 },{ -10.0, 8.0 },{ 10.0, 8.0 },{ 13.0, 0.0 },{ 18.0, 0.0 },{ 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 },{ -8.0, 6.0 },{ -2.0, 6.0 },{ -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 },{ 0.0, 6.0 },{ 8.0, 6.0 },{ 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 },{ -13.0, -8.0 },{ -13.0, -7.0 },{ -11.0, -4.0 },{ -7.0, -4.0 },{ -5.0, -7.0 },{ -5.0, -8.0 },{ -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 },{ 5.0, -8.0 },{ 5.0, -7.0 },{ 7.0, -4.0 },{ 11.0, -4.0 },{ 13.0, -7.0 },{ 13.0, -8.0 },{ 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 },{ -17.0, -2.0 },{ -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 },{ -17.0, -5.0 },{ -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1) + sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

// hat
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
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_STRIP]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glBindVertexArray(0);
}

// cake
#define CAKE_FIRE 0
#define CAKE_CANDLE 1
#define CAKE_BODY 2
#define CAKE_BOTTOM 3
#define CAKE_DECORATE 4

GLfloat cake_fire[4][2] = { { -0.5, 14.0 },{ -0.5, 13.0 },{ 0.5, 13.0 },{ 0.5, 14.0 } };
GLfloat cake_candle[4][2] = { { -1.0, 8.0 } ,{ -1.0, 13.0 },{ 1.0, 13.0 },{ 1.0, 8.0 } };
GLfloat cake_body[4][2] = { { 8.0, 5.0 },{ -8.0, 5.0 } ,{ -8.0, 8.0 },{ 8.0, 8.0 } };
GLfloat cake_bottom[4][2] = { { -10.0, 1.0 },{ -10.0, 5.0 },{ 10.0, 5.0 },{ 10.0, 1.0 } };
GLfloat cake_decorate[4][2] = { { -10.0, 0.0 },{ -10.0, 1.0 },{ 10.0, 1.0 },{ 10.0, 0.0 } };

GLfloat cake_color[5][3] = {
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 204 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 102 / 255.0f, 51 / 255.0f, 0 / 255.0f }
};

GLuint VBO_cake, VAO_cake;

void prepare_cake() {
	int size = sizeof(cake_fire);
	GLsizeiptr buffer_size = sizeof(cake_fire) * 5;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, size, cake_fire);
	glBufferSubData(GL_ARRAY_BUFFER, size, size, cake_candle);
	glBufferSubData(GL_ARRAY_BUFFER, size * 2, size, cake_body);
	glBufferSubData(GL_ARRAY_BUFFER, size * 3, size, cake_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, size * 4, size, cake_decorate);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cake);
	glBindVertexArray(VAO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cake() {
	glBindVertexArray(VAO_cake);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_FIRE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_CANDLE]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_DECORATE]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glBindVertexArray(0);
}

// sword

#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

//road
GLfloat road[4][2] = { {0.0, 0.0}, {0.0, 3.0}, {3.0, 3.0}, {3.0, 0.0} };
GLfloat line1[4][2] = { {0.2, 1.45}, {0.2, 1.55}, {0.3, 1.55}, {0.3, 1.45} };
GLfloat line2[4][2] = { {0.7, 1.45}, {0.7, 1.55}, {0.8, 1.55}, {0.8, 1.45} };
GLfloat line3[4][2] = { {1.2, 1.45}, {1.2, 1.55}, {1.3, 1.55}, {1.3, 1.45} };
GLfloat line4[4][2] = { {1.7, 1.45}, {1.7, 1.55}, {1.8, 1.55}, {1.8, 1.45} };
GLfloat line5[4][2] = { {2.2, 1.45}, {2.2, 1.55}, {2.3, 1.55}, {2.3, 1.45} };
GLfloat line6[4][2] = { {2.7, 1.45}, {2.7, 1.55}, {2.8, 1.55}, {2.8, 1.45} };
GLfloat road_color[7][3] = {
	{ 102 / 255.0f, 102 / 255.0f, 102 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 0 / 255.0f }
};

GLuint VBO_road, VAO_road;

#define ROAD_ROTATION_RADIUS 200.0f
float road_s_factor = 1.0f;

void prepare_road() {
	GLsizeiptr buffer_size = sizeof(road) + sizeof(line1) + sizeof(line2) + sizeof(line3)
		+ sizeof(line4) + sizeof(line5) + sizeof(line6);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_road);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(road), road);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road), sizeof(line1), line1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road) + sizeof(line1), sizeof(line2), line2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road) + sizeof(line1) + sizeof(line2), sizeof(line3), line3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road) + sizeof(line1) + sizeof(line2) + sizeof(line3),
		sizeof(line4), line4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road) + sizeof(line1) + sizeof(line2) + sizeof(line3)
		+ sizeof(line4), sizeof(line5), line5);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(road) + sizeof(line1) + sizeof(line2) + sizeof(line3)
		+ sizeof(line4) + sizeof(line5), sizeof(line6), line6);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_road);
	glBindVertexArray(VAO_road);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_road() { // Draw road in its MC.
	glBindVertexArray(VAO_road);

	glUniform3fv(loc_primitive_color, 1, road_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[3]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[4]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[5]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, road_color[6]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//terrain
GLfloat terrain[4][2] = { {-1.0, -1.0}, {-1.0, 1.0}, {1.0, 1.0}, {1.0, -1.0} };
GLfloat terrain_color[1][3] = { { 204 / 255.0f, 153 / 255.0f, 51 / 255.0f } };

GLuint VBO_terrain, VAO_terrain;

#define TERRAIN_ROTATION_RADIUS 200.0f
float terrain_s_factor = 1.0f;

void prepare_terrain() {
	GLsizeiptr buffer_size = sizeof(terrain);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_terrain);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_terrain);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(terrain), terrain);


	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_terrain);
	glBindVertexArray(VAO_terrain);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_terrain);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_terrain() { // Draw road in its MC.
	glBindVertexArray(VAO_terrain);

	glUniform3fv(loc_primitive_color, 1, terrain_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
}

//mountain
GLfloat mountain1[3][2] = { {-5.0, 0.0}, {-1.5, 3.5}, {1.5, 0.0} };
GLfloat mountain2[3][2] = { {-0.5, 0.0}, {2.5, 2.5}, {6.5, 0.0} };
GLfloat mountain_color[1][3] = { { 0 / 255.0f, 102 / 255.0f, 51 / 255.0f } };

GLuint VBO_mountain, VAO_mountain;

#define MOUNTAIN_ROTATION_RADIUS 200.0f
float mountain_s_factor = 1.0f;

void prepare_mountain() {
	GLsizeiptr buffer_size = sizeof(mountain1) + sizeof(mountain2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_mountain);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_mountain);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mountain1), mountain1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(mountain1), sizeof(mountain2), mountain2);


	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_mountain);
	glBindVertexArray(VAO_mountain);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_mountain);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_mountain() { // Draw road in its MC.
	glBindVertexArray(VAO_mountain);

	glUniform3fv(loc_primitive_color, 1, mountain_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, mountain_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 3);

	glBindVertexArray(0);
}

//tree
GLfloat leaf1[3][2] = { {-1.5, 1.0}, {1.5, 1.0}, {0.0, 4.0} };
GLfloat leaf2[3][2] = { {-1.5, 2.0}, {1.5, 2.0}, {0.0, 5.0} };
GLfloat stump[4][2] = { {-0.5, 1.0}, {-0.5, 0.0}, {0.5, 0.0}, {0.5, 1.0} };
GLfloat tree_color[2][3] = {
	{ 51 / 255.0f, 153 / 255.0f, 51 / 255.0f },
	{ 153 / 255.0f, 102 / 255.0f, 51 / 255.0f }
};

GLuint VBO_tree, VAO_tree;

#define TREE_ROTATION_RADIUS 200.0f
float tree_s_factor = 1.0f;

void prepare_tree() {
	GLsizeiptr buffer_size = sizeof(leaf1) + sizeof(leaf2) + sizeof(stump);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_tree);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_tree);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(leaf1), leaf1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf1), sizeof(leaf2), leaf2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf1) + sizeof(leaf2), sizeof(stump), stump);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_tree);
	glBindVertexArray(VAO_tree);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_tree);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_tree() { // Draw road in its MC.
	glBindVertexArray(VAO_tree);

	glUniform3fv(loc_primitive_color, 1, tree_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, tree_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 3);

	glUniform3fv(loc_primitive_color, 1, tree_color[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 4);

	glBindVertexArray(0);
}

//dot
GLfloat dot[1][2] = { {0.0, 0.0} };
GLuint VBO_dot, VAO_dot;

#define DOT_ROTATION_RADIUS 200.0f
float dot_s_factor = 1.0f;

void prepare_dot() {
	GLsizeiptr buffer_size = sizeof(dot);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_dot);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_dot);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(dot), dot);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_dot);
	glBindVertexArray(VAO_dot);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_dot);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_dot() { // Draw road in its MC.
	GLfloat dot_color[1][3] = { { (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX} };

	glBindVertexArray(VAO_dot);

	glUniform3fv(loc_primitive_color, 1, dot_color[0]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 0, 1);
	glPointSize(1.0);

	glBindVertexArray(0);
}

//building
GLfloat building[4][2] = { {-2.5, 0.0}, {2.5, 0.0}, {2.5, 3.5}, {-2.5, 3.5} };
GLfloat buildingdoor[4][2] = { {-0.5, 0.0}, {0.5, 0.0}, {0.5, 1.0}, {-0.5, 1.0} };
GLfloat window1[4][2] = { {-2.0, 0.5}, {-1.5, 0.5}, {-1.5, 1.0}, {-2.0, 1.0} };
GLfloat window2[4][2] = { {2.0, 0.5}, {1.5, 0.5}, {1.5, 1.0}, {2.0, 1.0} };
GLfloat window3[4][2] = { {-2.0, 1.5}, {-1.5, 1.5}, {-1.5, 2.0}, {-2.0, 2.0} };
GLfloat window4[4][2] = { {-1.0, 1.5}, {-0.5, 1.5}, {-0.5, 2.0}, {-1.0, 2.0} };
GLfloat window5[4][2] = { {1.0, 1.5}, {0.5, 1.5}, {0.5, 2.0}, {1.0, 2.0} };
GLfloat window6[4][2] = { {2.0, 1.5}, {1.5, 1.5}, {1.5, 2.0}, {2.0, 2.0} };
GLfloat window7[4][2] = { {-2.0, 2.5}, {-1.5, 2.5}, {-1.5, 3.0}, {-2.0, 3.0} };
GLfloat window8[4][2] = { {-1.0, 2.5}, {-0.5, 2.5}, {-0.5, 3.0}, {-1.0, 3.0} };
GLfloat window9[4][2] = { {1.0, 2.5}, {0.5, 2.5}, {0.5, 3.0}, {1.0, 3.0} };
GLfloat window10[4][2] = { {2.0, 2.5}, {1.5, 2.5}, {1.5, 3.0}, {2.0, 3.0} };

GLfloat building_color[12][3] = {
	{ 204 / 255.0f, 153 / 255.0f, 51 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 153 / 255.0f, 255 / 255.0f }
};

GLuint VBO_building, VAO_building;

#define BUILDING_ROTATION_RADIUS 200.0f
float building_s_factor = 1.0f;

void prepare_building() {
	GLsizeiptr buffer_size = sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3) + sizeof(window4) + sizeof(window5) + sizeof(window6) + sizeof(window7)
		+ sizeof(window8) + sizeof(window9) + sizeof(window10);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_building);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_building);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(building), building);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building), sizeof(buildingdoor), buildingdoor);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor), sizeof(window1), window1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1), sizeof(window2), window2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2),
		sizeof(window3), window3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3), sizeof(window4), window4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3) + sizeof(window4), sizeof(window5), window5);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3) + sizeof(window4) + sizeof(window5), sizeof(window6), window6);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3) + sizeof(window4) + sizeof(window5) + sizeof(window6), sizeof(window7), window7);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3) + sizeof(window4) + sizeof(window5) + sizeof(window6) + sizeof(window7), 
		sizeof(window8), window8);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3) + sizeof(window4) + sizeof(window5) + sizeof(window6) + sizeof(window7)
		+ sizeof(window8), sizeof(window9), window9);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(building) + sizeof(buildingdoor) + sizeof(window1) + sizeof(window2)
		+ sizeof(window3) + sizeof(window4) + sizeof(window5) + sizeof(window6) + sizeof(window7)
		+ sizeof(window8) + sizeof(window9), sizeof(window10), window10);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_building);
	glBindVertexArray(VAO_building);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_building);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_building() { // Draw road in its MC.
	glBindVertexArray(VAO_building);

	glUniform3fv(loc_primitive_color, 1, building_color[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 36, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 40, 4);

	glUniform3fv(loc_primitive_color, 1, building_color[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 44, 4);

	glBindVertexArray(0);
}

/******
//air balloon
GLfloat balloon1_st[2][2] = { { 0.0, 0.0 }, { 1.0, 0.3 } };
GLfloat balloon1[1][2] = { { 1.0, 0.3 } };
GLfloat balloon2_st[2][2] = { { 0.0, 0.0 }, { 0.8, 0.9 } };
GLfloat balloon2[1][2] = { { 0.8, 0.9 } };
GLfloat balloon3_st[2][2] = { { 0.0, 0.0 }, { 0.6, 0.5 } };
GLfloat balloon3[1][2] = { { 0.6, 0.5 } };
GLfloat balloon4_st[2][2] = { { 0.0, 0.0 }, { 0.3, 1.3 } };
GLfloat balloon4[1][2] = { { 0.3, 1.3 } };
GLfloat balloon5_st[2][2] = { { 0.0, 0.0 }, { 0.0, 1.7 } };
GLfloat balloon5[1][2] = { { 0.0, 1.7 } };
GLfloat balloon6_st[2][2] = { { 0.0, 0.0 }, { -0.4, 0.7 } };
GLfloat balloon6[1][2] = { { -0.4, 0.7 } };
GLfloat balloon7_st[2][2] = { { 0.0, 0.0 }, { -1.0, 0.5 } };
GLfloat balloon7[1][2] = { { -1.0, 0.5 } };

GLfloat balloon_color[8][3] = {
	{ 51 / 255.0f, 51 / 255.0f, 51 / 255.0f },  // string
	{ 255 / 255.0f, 255 / 255.0f, 51 / 255.0f },
	{ 102 / 255.0f, 255 / 255.0f, 102 / 255.0f },
	{ 0 / 255.0f, 153 / 255.0f, 153 / 255.0f },
	{ 51 / 255.0f, 153 / 255.0f, 0 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 204 / 255.0f },
	{ 255 / 255.0f, 102 / 255.0f, 51 / 255.0f },
	{ 51 / 255.0f, 51 / 255.0f, 204 / 255.0f }
};

GLuint VBO_balloon, VAO_balloon;

#define BALLOON_ROTATION_RADIUS 200.0f
float balloon_s_factor = 1.0f;

void prepare_balloon() {
	GLsizeiptr buffer_size = sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st) + sizeof(balloon4) + sizeof(balloon5_st)
		+ sizeof(balloon5) + sizeof(balloon6_st) + sizeof(balloon6) + sizeof(balloon7_st) + sizeof(balloon7);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_balloon);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_balloon);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(balloon1_st), balloon1_st);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st), sizeof(balloon1), balloon1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1), sizeof(balloon2_st), balloon2_st);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(window1), sizeof(balloon2), balloon2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2),
		sizeof(balloon3_st), balloon3_st);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st), sizeof(balloon3), balloon3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3), sizeof(balloon4_st), balloon4_st);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st), sizeof(balloon4), balloon4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st) + sizeof(balloon4), sizeof(balloon5_st), balloon5_st);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st) + sizeof(balloon4) + sizeof(balloon5_st),
		sizeof(balloon5), balloon5);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st) + sizeof(balloon4) + sizeof(balloon5_st)
		+ sizeof(balloon5), sizeof(balloon6_st), balloon6_st);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st) + sizeof(balloon4) + sizeof(balloon5_st)
		+ sizeof(balloon5) + sizeof(balloon6_st), sizeof(balloon6), balloon6);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st) + sizeof(balloon4) + sizeof(balloon5_st)
		+ sizeof(balloon5) + sizeof(balloon6_st) + sizeof(balloon6), sizeof(balloon7_st), balloon7_st);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(balloon1_st) + sizeof(balloon1) + sizeof(balloon2_st) + sizeof(balloon2)
		+ sizeof(balloon3_st) + sizeof(balloon3) + sizeof(balloon4_st) + sizeof(balloon4) + sizeof(balloon5_st)
		+ sizeof(balloon5) + sizeof(balloon6_st) + sizeof(balloon6) + sizeof(balloon7_st), sizeof(balloon7), balloon7);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_balloon);
	glBindVertexArray(VAO_balloon);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_balloon);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_balloon() { // Draw airplane in its MC.
	glBindVertexArray(VAO_balloon);

	glUniform3fv(loc_primitive_color, 1, balloon_color[0]);
	glDrawArrays(GL_LINES, 0, 2);

	glUniform3fv(loc_primitive_color, 1, balloon_color[1]);
	glPointSize(500.0);
	glDrawArrays(GL_POINTS, 3, 1);
//	glPointSize(1.0);

	glUniform3fv(loc_primitive_color, 1, balloon_color[0]);
	glDrawArrays(GL_LINES, 4, 2);

	glUniform3fv(loc_primitive_color, 1, balloon_color[2]);
	glPointSize(500.0);
	glDrawArrays(GL_POINTS, 6, 1);
//	glPointSize(1.0);

	glUniform3fv(loc_primitive_color, 1, balloon_color[0]);
	glDrawArrays(GL_LINES, 7, 2);

	glUniform3fv(loc_primitive_color, 1, balloon_color[3]);
	glPointSize(500.0);
	glDrawArrays(GL_POINTS, 9, 1);
//	glPointSize(1.0);

	glUniform3fv(loc_primitive_color, 1, balloon_color[0]);
	glDrawArrays(GL_LINES, 10, 2);

	glUniform3fv(loc_primitive_color, 1, balloon_color[4]);
	glPointSize(500.0);
	glDrawArrays(GL_POINTS, 12, 1);
//	glPointSize(1.0);

	glUniform3fv(loc_primitive_color, 1, balloon_color[0]);
	glDrawArrays(GL_LINES, 13, 2);

	glUniform3fv(loc_primitive_color, 1, balloon_color[5]);
	glPointSize(500.0);
	glDrawArrays(GL_POINTS, 15, 1);
//	glPointSize(1.0);

	glUniform3fv(loc_primitive_color, 1, balloon_color[0]);
	glDrawArrays(GL_LINES, 16, 2);

	glUniform3fv(loc_primitive_color, 1, balloon_color[6]);
	glPointSize(500.0);
	glDrawArrays(GL_POINTS, 18, 1);
//	glPointSize(1.0);

	glUniform3fv(loc_primitive_color, 1, balloon_color[0]);
	glDrawArrays(GL_LINES, 19, 2);

	glUniform3fv(loc_primitive_color, 1, balloon_color[7]);
	glPointSize(500.0);
	glDrawArrays(GL_POINTS, 21, 1);
//	glPointSize(1.0);

	glBindVertexArray(0);
}

//cloud

//sun
void drawCircle(float x, float y, float radius){
	GLfloat doublePi = 2.0f * 3.141592;
	GLfloat circleVerticesX[101];
	GLfloat circleVerticesY[101];
	GLfloat circleVerticesZ[101];
	GLfloat circle_color[1][3] = { { 51 / 255.0f, 51 / 255.0f, 51 / 255.0f } };
	//circleVerticesX[0] = x;
	//circleVerticesY[0] = y;
	//circleVerticesZ[0] = z;

	for (int i = 0; i < 101; i++){
		circleVerticesX[i] = (GLfloat) x + ((GLfloat) radius * cos(i * doublePi / 100));
		circleVerticesY[i] = (GLfloat) y + ((GLfloat) radius * sin(i * doublePi / 100));
		circleVerticesZ[i] = (GLfloat)0;
	}

	GLfloat allCircleVertices[101 * 3];

	for (int i = 0; i < 101; i++){
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 101);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void draw_sun() {
	drawCircle((float)win_width / 2, (float)win_height / 2, 120.0f);
	//circle(-0.25, 0.75, 0.15);
}

void drawFilledSun() {
	//static float angle;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0, 0, -10);
	int i, x, y;
	double radius = 0.30;
	//glColor3ub(253, 184, 19);     
	glColor3ub(255, 0, 0);
	double twicePi = 2.0 * 3.142;
	x = 0, y = 0;
	glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * twicePi / 20))), (y + (radius * sin(i * twicePi / 20)))
		);
	}
	glEnd(); //END
}
float theta;
float radius;
glColor3f(1, 0, 0);
glBegin(GL_POLYGON);
for (int i = 0; i < 360; i++) {
	glVertex2f(ax + radius * cos(i* TO_RADIAN), ay + radius * sin(i* TO_RADIAN));
}

******/