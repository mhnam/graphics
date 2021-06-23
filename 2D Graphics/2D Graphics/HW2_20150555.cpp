#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
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

int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f;

// 2D ¹°Ã¼ Á¤ÀÇ ºÎºÐÀº objects.h ÆÄÀÏ·Î ºÐ¸®
// »õ·Î¿î ¹°Ã¼ Ãß°¡ ½Ã prepare_scene() ÇÔ¼ö¿¡¼­ ÇØ´ç ¹°Ã¼¿¡ ´ëÇÑ prepare_***() ÇÔ¼ö¸¦ ¼öÇàÇÔ.
// (ÇÊ¼ö´Â ¾Æ´Ï³ª ¿Ã¹Ù¸¥ ÄÚµùÀ» À§ÇÏ¿©) cleanup() ÇÔ¼ö¿¡¼­ ÇØ´ç resource¸¦ free ½ÃÅ´.
#include "objects.h"

unsigned int timestamp = 0;
void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void display(void) {
	glm::mat4 ModelMatrix;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	float frame_moving_speed = 0;
	if (timestamp % 2100 > 1800)
		frame_moving_speed = 0.0f;
	else if (timestamp % 2100 > 1700)
		frame_moving_speed = 950.0f - 9.5f * ((timestamp % 2100)- 1700);
	else if (timestamp % 2100 > 900)
		frame_moving_speed = 950.0;
	else if (timestamp % 2100 > 500)
//		frame_moving_speed = 0;
		frame_moving_speed = 700.0f / 300.0f * ((timestamp % 2100) - 500);

	float frame_moving_speed1 = 0;
	if (timestamp % 2100 > 650)
		frame_moving_speed1 = 0.5f * (timestamp - 650);

	float frame_moving_speed2 = 0;
	if (timestamp % 2100 > 850)
		frame_moving_speed2 = 200.0f + 0.5f * (timestamp - 850);

	float shake = 0;
	float shake1 = 0;
	float shake2 = 0;

	//small object shake
	float shake_rotation = 0.4f;

	if (timestamp % 2100 > 700)
		shake = 0;
	else if (timestamp % 2100 > 500)
		if ((timestamp % 20) < 5)
			shake = -(float)(timestamp % 20) * (shake_rotation / 5.0f);
		else if ((timestamp % 20) < 15)
			shake = -shake_rotation + ((timestamp % 20) - 5.0f) * (shake_rotation / 5.0f);
		else
			shake = shake_rotation - ((timestamp % 20) - 15.0f) * (shake_rotation / 5.0f);

	//big object shake
	shake_rotation = 0.05f;
	if (timestamp % 2100 > 700)
		shake1 = 0;
	else if (timestamp % 2100 > 500)
		if ((timestamp % 50) < 12.5)
			shake1 = -(float)(timestamp % 50) * (shake_rotation / 12.5f);
		else if ((timestamp % 50) < 37.5)
			shake1 = -shake_rotation + ((timestamp % 50) - 5.0f) * (shake_rotation / 12.5f);
		else
			shake1 = shake_rotation - ((timestamp % 50) - 15.0f) * (shake_rotation / 12.5f);

	//floating house shakes
	int house_shake_clock = timestamp % 10;
	if (timestamp % 2100 > 550)	shake2 = 0.0f;
	else if (timestamp % 2100 > 300) {
		if (house_shake_clock < 5)	shake2 = -1.0f * house_shake_clock;
		else if (house_shake_clock < 15) shake2 = -5.0f + 1.0f * house_shake_clock;
		else shake2 = 5.0f - 1.0f * house_shake_clock;
	}

	//floating house gets bigger
	float house_scale_speed = 0;
	if (timestamp % 2100 > 600)
		house_scale_speed = 6.0f;
	else if (timestamp % 2100 > 500)
		house_scale_speed = 0.05f * ((timestamp % 2100) - 480);

	//stars
	for (int i = 0; i < 5; i++) {
		float star_locationX = rand() % (int)win_width - win_width / 2.0f;
		float star_locationY = rand() % (int)win_height - win_height / 2.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(star_locationX, star_locationY, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_dot();
	}

	//starlike objects
	float starlike_clock = (timestamp % 360) * 1.8f; // -360 <= house_clock <= 360 
	float rotation_angle_starlike = starlike_clock * TO_RADIAN;
	float scaling_factor_starlike = (1.0f - fabs(sinf(rotation_angle_starlike)));

	float star_location[7][5] = { {0.15f,0.267f,0.623f,0.195f,0.685f},
		{0.36f,0.453f,0.876f,0.616f,0.608f},
		{0.71f,0.698f,0.903f,0.278f,0.185f},
		{0.455f,0.956f,0.626f,0.617f,0.459f},
		{0.57f,0.31f,0.884f,0.992f,0.884f},
		{0.9f,0.758f,0.432f,0.564f,0.389f},
		{0.313f,0.049f,0.238f,0.387f,0.071f}
	};

	for (int i = 0; i < 7; i++) {
		float star_locationX = (float)win_width * (star_location[i][0] - 0.5f);
		float star_locationY = (float)win_height * (star_location[i][1] - 0.5f);
		float star_location1X = (float)win_width * (star_location[6-i][1] - 0.5f);
		float star_location1Y = (float)win_height * (star_location[6-i][0] - 0.5f);

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(star_locationX, star_locationY - frame_moving_speed1, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_starlike, scaling_factor_starlike, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_starlike, glm::vec3(star_location[i][2], star_location[i][3], star_location[i][4]));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_hat();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(star_location1X, star_location1Y - frame_moving_speed2, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_starlike, scaling_factor_starlike, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_starlike, glm::vec3(star_location[i][2], star_location[i][3], star_location[i][4]));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cake();
	}


	//mountain
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 10.0f - frame_moving_speed, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(win_width/10.5f, win_height/5.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_mountain();

	//terrain
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -win_height / 4.0f - frame_moving_speed, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(win_width / 2.0f, win_height / 4.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_terrain();

	//road
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-win_width / 2.0f, -80.0f - frame_moving_speed, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(win_width / 3.0f, 30.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_road();

	int car_clock = timestamp;
	float scaling_factor_car = 1.5f;

	//cars left to right
	for (int i = 0; i < 16; i++) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(((float)(timestamp % 2100) - i * 80.0f) * 1.5f - win_width / 2.0f, -frame_moving_speed, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_car, scaling_factor_car, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, shake, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (i % 3 == 1)
			draw_car();
		else
			draw_car2();
	}

	//cars right to left
	for (int i = 0; i < 10; i++) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(win_width / 2.0f - ((float)(timestamp % 2100) - i * 110.0f) * 2.0f , -50.0f - frame_moving_speed, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_car, scaling_factor_car, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, shake, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(win_width / 2.0f - ((float)(timestamp % 2100) - 55.0f - i * 110.0f) * 2.1f, -50.0f - frame_moving_speed, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_car, scaling_factor_car, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, shake, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car2();
	}

	//houses and buildings
	float scaling_factor_house1 = 3.0f;
	for (int i = 0; i < 15; i++) {
		if ((i % 10 == 2) || (i % 10 == 5) || (i % 10 == 9))
			scaling_factor_house1 = 25.0f; //building
		else
			scaling_factor_house1 = 3.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)win_width / 2.0f + (float)i * 90.0f, 50.0f - frame_moving_speed, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_house1, scaling_factor_house1, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, shake1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if ((i % 10 == 2) || (i % 10 == 5) || (i % 10 == 9))
			draw_building(); //building
		else if ((i % 10 == 1) || (i % 10 == 3) || (i % 10 == 8))
			draw_house();
	}
	
	//sprinkles
	float dot_timer = 0.0f;
	float dot_sprinkle_cycle = 1.0f;
	float dot_sprinkle_maxdistX = 0.0f;
	float dot_sprinkle_maxdistY = 0.0f;
	float dot_sprinkle_minY = 0.0f;

	if (timestamp % 2100 > 350) {
		int max_sprinkle = (timestamp % 2100) - 350;
		dot_sprinkle_cycle = 10.0f;
		dot_sprinkle_maxdistX = 250 + max_sprinkle * 0.5f;
		dot_sprinkle_maxdistY = 10 + max_sprinkle * 0.5f;
		dot_timer = ((timestamp % 2100) % (int)dot_sprinkle_cycle);
		if (timestamp % 2100 > 600){ //going up
			max_sprinkle = 600 - 350;
			dot_sprinkle_maxdistX = 250 + max_sprinkle * 0.5f;
			dot_sprinkle_maxdistY = 10 + max_sprinkle * 0.5f;
		}
		else if (timestamp % 2100 > 1100) { //disappear
			max_sprinkle = 250 - ((timestamp % 2100) - 1100) * 2;
			dot_sprinkle_maxdistX = 250 + max_sprinkle * 0.5f;
			dot_sprinkle_maxdistY = 10 + max_sprinkle * 0.5f;
		}
		else if (timestamp % 2100 > 1600) { //going down
			max_sprinkle = 250 + ((timestamp % 2100) - 1600) * 0.5f;
			dot_sprinkle_maxdistX = 500 + max_sprinkle * 0.5f;
			dot_sprinkle_maxdistY = 10;
		}

		if (timestamp % 2100 < 600) {
			for (int i = 0; i < max_sprinkle; i++) {
				float dot_sprinkleX = ((rand() % (int)dot_sprinkle_maxdistX) - dot_sprinkle_maxdistX / 2.0f) / dot_sprinkle_cycle;
				float dot_sprinkleY = -((rand() % (int)dot_sprinkle_maxdistY)) / dot_sprinkle_cycle;
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dot_sprinkleX * dot_timer, (dot_sprinkleY * dot_timer) + 45.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_dot();
			}
		}
		else if (timestamp % 2100 < 750) {
			for (int i = 0; i < max_sprinkle; i++) {
				float dot_sprinkleX = ((rand() % (int)dot_sprinkle_maxdistX) - dot_sprinkle_maxdistX / 2.0f) / dot_sprinkle_cycle;
				float dot_sprinkleY = -((rand() % (int)dot_sprinkle_maxdistY)) / dot_sprinkle_cycle;
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((dot_sprinkleX * dot_timer) + ((timestamp % 2100) - 600) * -4.0f, (dot_sprinkleY * dot_timer) + 45.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -0.07f * ((timestamp % 2100) - 600) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_dot();
			}
		}
		else if (timestamp % 2100 < 1000) {
			for (int i = 0; i < max_sprinkle; i++) {
				float dot_sprinkleX = ((rand() % (int)dot_sprinkle_maxdistX) - dot_sprinkle_maxdistX / 2.0f) / dot_sprinkle_cycle;
				float dot_sprinkleY = -((rand() % (int)dot_sprinkle_maxdistY)) / dot_sprinkle_cycle;
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + (dot_sprinkleX * dot_timer) + ((timestamp % 2100) - 750) * 7.0f, (dot_sprinkleY * dot_timer) + 45.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, -10.5f + 0.1f * (timestamp - 750) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_dot();
			}
		}
		else if (timestamp % 2100 < 1200) {
			for (int i = 0; i < max_sprinkle; i++) {
				float dot_sprinkleX = ((rand() % (int)dot_sprinkle_maxdistX) - dot_sprinkle_maxdistX / 2.0f) / dot_sprinkle_cycle;
				float dot_sprinkleY = -((rand() % (int)dot_sprinkle_maxdistY)) / dot_sprinkle_cycle;
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + (dot_sprinkleX * dot_timer) + ((timestamp % 2100) - 1000) * 5.0f, (dot_sprinkleY * dot_timer) + 95.0f + ((timestamp % 2100) - 1000) * -0.35f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f - (3.0f/200) * ((timestamp % 2100) - 1000), 3.0f - (3.0f / 200) * ((timestamp % 2100) - 1000), 1.0f));
				ModelMatrix = glm::rotate(ModelMatrix, (-10.0f + 0.1f * ((timestamp % 2100) - 1000)) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_dot();
			}
		}
		else if (timestamp % 2100 > 1600 && timestamp % 2100 < 1900) {
			for (int i = 0; i < max_sprinkle; i++) {
				float dot_sprinkleX = ((rand() % (int)dot_sprinkle_maxdistX) - dot_sprinkle_maxdistX / 2.0f) / dot_sprinkle_cycle;
				float dot_sprinkleY = ((rand() % (int)dot_sprinkle_maxdistY)) / dot_sprinkle_cycle;
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dot_sprinkleX * dot_timer + 30.0f, (dot_sprinkleY * dot_timer) + 45.0f, 0.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_dot();
			}
		}
	}

	//floating house
	if (timestamp % 2100 < 600) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(shake2, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f + house_scale_speed, 3.0f + house_scale_speed, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}
	else if (timestamp % 2100 < 750) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(((timestamp % 2100) - 600) * -4.0f, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(9.0f, 9.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -0.07f * ((timestamp % 2100) - 600) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}
	else if (timestamp % 2100 < 1000) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + ((timestamp % 2100) - 750) * 7.0f, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(9.0f, 9.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (-10.5f + 0.1f * ((timestamp % 2100) - 750)) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}
	else if (timestamp % 2100 < 1200) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-600.0f + ((timestamp % 2100) - 1000) * +5.0f, 100.0f + ((timestamp % 2100) - 1000) * -0.35f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(9.0f - (6.0f / 200) * ((timestamp % 2100) - 1000), 9.0f - (6.0f / 200) * ((timestamp % 2100) - 1000), 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (-10.0f + 0.1f * ((timestamp % 2100) - 1000)) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}
	else if (timestamp % 2100 > 1400 && timestamp % 2100 < 1600) { //going down
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 500.0f - (timestamp - 1400) * 2.25f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.0f - (9.0f / 200) * (timestamp - 1400), 12.0f - (9.0f / 200) * (timestamp - 1400), 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}
	else if (timestamp % 2100 > 1600){ //final
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(shake2, 50.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}

	float scaling_factor_tree = 13.0f;
	for (int i = 0; i < 30; i++) {
		if(i%2)
			scaling_factor_tree = 18.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)win_width / 2.0f + (float)i * 90.0f, 20.0f - frame_moving_speed, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_tree, scaling_factor_tree, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, shake1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_tree();

		scaling_factor_tree = 20.0f;
		if (i % 2)
			scaling_factor_tree = 15.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)win_width / 2.0f + (float)i * 100.0f, -105.0f - frame_moving_speed, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_tree, scaling_factor_tree, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, shake1, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_tree();

	}

	int airplane_clock = timestamp % 1440; // 0 <= airplane_clock <= 719
	float airplane_cx = 100.0f;
	float airplane_cy = 100.0f;
	float airplane_rotation_radius = 200.0f;

	if (airplane_clock <= 360) { 
		airplane_rotation_radius = 150.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_rotation_radius, -airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-airplane_rotation_radius, airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock * 0.5f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (airplane_clock <= 720) {
		airplane_rotation_radius = 400.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-airplane_rotation_radius, airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -(airplane_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(airplane_rotation_radius, -airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock * 2.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

		if (airplane_clock <= 540)
			airplane_s_factor = (airplane_clock - 360.0f) / 180.0f + 1.0f;
		else 
			airplane_s_factor = (airplane_clock - 540.0f) / 180.0f + 2.0f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_s_factor, airplane_s_factor, 1.0f));
	}
	else if (airplane_clock <= 1080) {
		airplane_rotation_radius = 200.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_rotation_radius, -airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-airplane_rotation_radius, airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock * 1.5f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		if (airplane_clock <= 900)
			airplane_s_factor = -(airplane_clock - 720.0f) / 180.0f + 3.0f;
		else
			airplane_s_factor = (airplane_clock - 900.0f) / 180.0f + 2.0f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_s_factor, airplane_s_factor, 1.0f));

	}
	else {
		airplane_rotation_radius = 300.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-airplane_rotation_radius, airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -(airplane_clock)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(airplane_rotation_radius, -airplane_cy, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, airplane_clock * 4.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

		if (airplane_clock <= 1260)
			airplane_s_factor = -(airplane_clock - 1080.0f) / 180.0f + 3.0f;
		else
			airplane_s_factor = -(airplane_clock - 1260.0f) / 180.0f + 2.0f;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_s_factor, airplane_s_factor, 1.0f));
	}

	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
 	draw_airplane();
	
	glFlush();
}   

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

int leftbuttonpressed = 0;
void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
}

void motion(int x, int y) {
	if (leftbuttonpressed) {
		centerx =  x - win_width/2.0f, centery = (win_height - y) - win_height/2.0f;
		glutPostRedisplay();
	}
} 
	
void reshape(int width, int height) {
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	glDeleteVertexArrays(1, &VAO_house);
	glDeleteBuffers(1, &VBO_house);

	glDeleteVertexArrays(1, &VAO_road);
	glDeleteBuffers(1, &VBO_road);

	glDeleteVertexArrays(1, &VAO_car);
	glDeleteBuffers(1, &VBO_car);

	glDeleteVertexArrays(1, &VAO_car2);
	glDeleteBuffers(1, &VBO_car2);

	glDeleteVertexArrays(1, &VAO_tree);
	glDeleteBuffers(1, &VBO_tree);

	glDeleteVertexArrays(1, &VAO_building);
	glDeleteBuffers(1, &VBO_building);

	glDeleteVertexArrays(1, &VAO_dot);
	glDeleteBuffers(1, &VBO_dot);

	glDeleteVertexArrays(1, &VAO_mountain);
	glDeleteBuffers(1, &VBO_mountain);

	glDeleteVertexArrays(1, &VAO_terrain);
	glDeleteBuffers(1, &VBO_terrain);

	glDeleteVertexArrays(1, &VAO_hat);
	glDeleteBuffers(1, &VBO_hat);

	glDeleteVertexArrays(1, &VAO_cake);
	glDeleteBuffers(1, &VBO_cake);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
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

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_airplane();
	prepare_house();
	prepare_road();
	prepare_car();
	prepare_car2();
	prepare_tree();
	prepare_building();
	prepare_dot();
	prepare_mountain();
	prepare_terrain();
	prepare_hat();
	prepare_cake();
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
int main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 HW2 20150555 MINHYUK NAM";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC'"
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


