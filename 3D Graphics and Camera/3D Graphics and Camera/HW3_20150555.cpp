#define _CRT_SECURE_NO_WARNINGS
#define EPSILON 0.01

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix

// Begin of shader setup
#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram, h_ShaderProgram_simple, h_ShaderProgram_TXPS; // handles to shader programs
GLint loc_ModelViewProjectionMatrix, loc_primitive_color, loc_ModelViewProjectionMatrix_simple; // indices of uniform variables

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

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix, ViewProjectionMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;
glm::mat4 ModelViewProjectionMatrix_SPIDER, ModelViewProjectionMatrix_TIGER, ModelViewProjectionMatrix_BUS;
glm::mat4 ModelViewProjectionMatrix_BEN, ModelViewProjectionMatrix_IRONMAN;
glm::mat4 ModelMatrix_SPIDER_to_EYE, ModelMatrix_TIGER_to_EYE, ModelMatrix_BUS_to_EYE;
glm::mat4 ModelMatrix_BEN_to_EYE, ModelMatrix_IRONMAN_to_EYE;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2
// End of shader setup

// for animation
unsigned int timestamp_scene = 0; // the global clock in the scene
int flag_tiger_animation, flag_polygon_fill;
int cur_frame_tiger = 0, cur_frame_ben = 0, cur_frame_wolf, cur_frame_spider = 0;
float rotation_angle_tiger = 0.0f, rotation_angle_cow = 0.0f, rotation_angle_bus = 0.0f, rotation_angle_spider = 0.0f, rotation_angle_ben = 0.0f, rotation_angle_wolf = 0.0f;
float ben_pos = -6.0f, ben_pos_speed = 0.1f, wolf_pos = 6.0f, wolf_pos_speed = 0.1f;
float ben_pos_y = 3.0f, wolf_pos_y = 4.0f;
int ben_pos_period = 121, wolf_pos_period = 121, num_bens = 1, num_wolfs = 1;
bool ben_pos_fl = FALSE, wolf_pos_fl = TRUE;

// Begin of geometry setup
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION 0

GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_axes(void) {
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}

#define N_OBJECTS 6
#define OBJECT_SQUARE16 0
#define OBJECT_DRAGON 1
#define OBJECT_COW 2
#define OBJECT_TANK 3
#define OBJECT_BUS 4
#define OBJECT_IRONMAN 5

GLuint object_VBO[N_OBJECTS], object_VAO[N_OBJECTS];
int object_n_triangles[N_OBJECTS];
GLfloat* object_vertices[N_OBJECTS];

// tiger object
#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat* tiger_vertices[N_TIGER_FRAMES];

// ben object
#define N_BEN_FRAMES 30
GLuint ben_VBO, ben_VAO;
int ben_n_triangles[N_BEN_FRAMES];
int ben_vertex_offset[N_BEN_FRAMES];
GLfloat* ben_vertices[N_BEN_FRAMES];

// wolf object
#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];

// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];

int read_triangular_mesh(GLfloat** object, int bytes_per_primitive, const char* filename) {
	int n_triangles;
	FILE* fp;

	fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float*)malloc(n_triangles * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void set_up_object(int object_ID, const char* filename, int n_bytes_per_vertex) {
	object_n_triangles[object_ID] = read_triangular_mesh(&object_vertices[object_ID],
		3 * n_bytes_per_vertex, filename);
	// Error checking is needed here.

	// Initialize vertex buffer object.
	glGenBuffers(1, &object_VBO[object_ID]);

	glBindBuffer(GL_ARRAY_BUFFER, object_VBO[object_ID]);
	glBufferData(GL_ARRAY_BUFFER, object_n_triangles[object_ID] * 3 * n_bytes_per_vertex,
		object_vertices[object_ID], GL_STATIC_DRAW);

	// As the geometry data exists now in graphics memory, ...
	free(object_vertices[object_ID]);

	// Initialize vertex array object.
	glGenVertexArrays(1, &object_VAO[object_ID]);
	glBindVertexArray(object_VAO[object_ID]);

	glBindBuffer(GL_ARRAY_BUFFER, object_VBO[object_ID]);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_square(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_SQUARE16, "Data/Square16_triangles_vnt.geom", 8 * sizeof(float));
}

void prepare_wolf(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, wolf_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_WOLF_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/wolf/wolf_%02d_vnt.geom", i);
		wolf_n_triangles[i] = read_triangular_mesh(&wolf_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		wolf_n_total_triangles += wolf_n_triangles[i];

		if (i == 0)
			wolf_vertex_offset[i] = 0;
		else
			wolf_vertex_offset[i] = wolf_vertex_offset[i - 1] + 3 * wolf_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &wolf_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glBufferData(GL_ARRAY_BUFFER, wolf_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_WOLF_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, wolf_vertex_offset[i] * n_bytes_per_vertex,
			wolf_n_triangles[i] * n_bytes_per_triangle, wolf_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_WOLF_FRAMES; i++)
		free(wolf_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &wolf_VAO);
	glBindVertexArray(wolf_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_ben(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, ben_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_BEN_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/ben/ben_vn%d%d.geom", i / 10, i % 10);
		ben_n_triangles[i] = read_triangular_mesh(&ben_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		ben_n_total_triangles += ben_n_triangles[i];

		if (i == 0)
			ben_vertex_offset[i] = 0;
		else
			ben_vertex_offset[i] = ben_vertex_offset[i - 1] + 3 * ben_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &ben_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ben_VBO);
	glBufferData(GL_ARRAY_BUFFER, ben_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_BEN_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, ben_vertex_offset[i] * n_bytes_per_vertex,
			ben_n_triangles[i] * n_bytes_per_triangle, ben_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_BEN_FRAMES; i++)
		free(ben_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &ben_VAO);
	glBindVertexArray(ben_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ben_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_spider(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_triangular_mesh(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];

		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_tiger(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/tiger/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_triangular_mesh(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &tiger_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_dragon(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_DRAGON, "Data/static_objects/dragon_vnt.geom", 8 * sizeof(float));
}

void prepare_cow(void) {
	// 6 = 3 for vertex, and 3 for normal
	set_up_object(OBJECT_COW, "Data/static_objects/cow_v.geom", 3 * sizeof(float));
}

void prepare_tank(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_TANK, "Data/static_objects/tank_vnt.geom", 8 * sizeof(float));
}

void prepare_bus(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_BUS, "Data/static_objects/bus_vnt.geom", 8 * sizeof(float));
}

void prepare_ironman(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_IRONMAN, "Data/static_objects/ironman_vnt.geom", 8 * sizeof(float));
}

void draw_object(int object_ID, float r, float g, float b) {
	glUniform3f(loc_primitive_color, r, g, b);
	glBindVertexArray(object_VAO[object_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * object_n_triangles[object_ID]);
	glBindVertexArray(0);
}

void draw_tiger() {
	glFrontFace(GL_CW);
	glUniform3f(loc_primitive_color, 255 / 255.0f, 153 / 255.0f, 51 / 255.0f);
	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);
}

void draw_ben(void) {
	glFrontFace(GL_CW);
	glUniform3f(loc_primitive_color, 0 / 255.0f, 255 / 255.0f, 255 / 255.0f);
	glBindVertexArray(ben_VAO);
	glDrawArrays(GL_TRIANGLES, ben_vertex_offset[cur_frame_ben], 3 * ben_n_triangles[cur_frame_ben]);
	glBindVertexArray(0);
}

void draw_wolf(void) {
	glFrontFace(GL_CW);
	glUniform3f(loc_primitive_color, 153 / 255.0f, 76 / 255.0f, 0 / 255.0f);
	glBindVertexArray(wolf_VAO);
	glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
	glBindVertexArray(0);
}

void draw_spider(void) {
	glFrontFace(GL_CW);
	glUniform3f(loc_primitive_color, (rand() % 50) / 255.0f, (rand() % 50) / 255.0f, (rand() % 50) / 255.0f);
	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);
}

// End of geometry setup

// Begin of Callback function definitions
int flag_fill_floor = 0;

#define CAM_ROT_SENSITIVITY 0.15f

typedef struct _camera {
	glm::vec3 prp, vrp, vup;
	float fov_y, aspect_ratio, near_clip, far_clip, zoom_factor;
	int move;
} camera;

camera camera_wv;
unsigned int left_mousepressed = 0;
int prevx, prevy;

enum _cameraType {
	VIEW_WORLD, VIEW_OPP_WORLD, VIEW_SPIDER, VIEW_TOP, VIEW_MOVE
} camera_type;

void set_ViewMatrix_for_world_viewer(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (left_mousepressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera_wv.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY * delx * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4_tmp = glm::translate(mat4_tmp, -camera_wv.vrp);

		camera_wv.prp = glm::vec3(mat4_tmp * glm::vec4(camera_wv.prp, 1.0f));
		camera_wv.vup = glm::vec3(mat4_tmp * glm::vec4(camera_wv.vup, 0.0f));

		vec3_tmp = glm::cross(camera_wv.vup, camera_wv.vrp - camera_wv.prp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera_wv.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY * dely * TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera_wv.vrp);

		camera_wv.prp = glm::vec3(mat4_tmp * glm::vec4(camera_wv.prp, 1.0f));
		camera_wv.vup = glm::vec3(mat4_tmp * glm::vec4(camera_wv.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

void set_ViewMatrix_for_opposite_world_viewer(int x, int y) {
	camera_wv.prp = glm::vec3(-25.0f, 25.0f, -25.0f);
	camera_wv.vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_wv.vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewMatrix_for_top_viewer(int x, int y) {
	camera_wv.prp = glm::vec3(0.0f, 30.0f, 0.0f);
	camera_wv.vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_wv.vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewMatrix_for_spider(void) {
	glm::mat4 Matrix_CAMERA_spider_inverse;

	Matrix_CAMERA_spider_inverse = ModelViewProjectionMatrix_SPIDER * ModelMatrix_SPIDER_to_EYE;

	ViewMatrix = glm::affineInverse(Matrix_CAMERA_spider_inverse);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewMatrix_for_user_camera(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (left_mousepressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera_wv.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY * delx * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4_tmp = glm::translate(mat4_tmp, -camera_wv.vrp);

		camera_wv.prp = glm::vec3(mat4_tmp * glm::vec4(camera_wv.prp, 1.0f));
		camera_wv.vup = glm::vec3(mat4_tmp * glm::vec4(camera_wv.vup, 0.0f));

		vec3_tmp = glm::cross(camera_wv.vup, camera_wv.vrp - camera_wv.prp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera_wv.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY * dely * TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera_wv.vrp);

		camera_wv.prp = glm::vec3(mat4_tmp * glm::vec4(camera_wv.prp, 1.0f));
		camera_wv.vup = glm::vec3(mat4_tmp * glm::vec4(camera_wv.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
	else{
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;
		mat4_tmp = glm::translate(glm::mat4(1.0f), glm::vec3(delx, dely, 0.0f));
		camera_wv.prp = glm::vec3(mat4_tmp * glm::vec4(camera_wv.prp, 1.0f));
		ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

float speed = 1.0;
float bus_radius = 5.0f;
bool clockwise = TRUE;
float tank_dist = 40.0f;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//DRAW TANK
	for(int i = 0; i < 3; i++){
		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(-1.0f, 0.0f, -0.5f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, (100.0f + tank_dist * i) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_TANK, 152 / 255.0f, 255 / 255.0f, 152 / 255.0f); // Mint Green
	}

	//DRAW SPIDER
	if (clockwise) ModelViewProjectionMatrix_SPIDER = glm::rotate(glm::mat4(1.0f), -rotation_angle_spider * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	else ModelViewProjectionMatrix_SPIDER = glm::rotate(glm::mat4(1.0f), rotation_angle_spider * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix_SPIDER = glm::translate(ModelViewProjectionMatrix_SPIDER, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix_SPIDER = glm::rotate(ModelViewProjectionMatrix_SPIDER, -180.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	if (camera_type == VIEW_SPIDER) set_ViewMatrix_for_spider();
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelViewProjectionMatrix_SPIDER;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_spider();
	ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(0.0f, -0.05f, 0.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes(); // draw MC axes of body
	glLineWidth(1.0f);


	//DRAW BUS
	if (clockwise) {
		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, -rotation_angle_bus * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(bus_radius, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
	}
	else {
		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, rotation_angle_bus * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(bus_radius, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
	}
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_BUS, 232 / 255.0f, 173 / 255.0f, 170 / 255.0f); // Rose

	//DRAW TIGER
	if (clockwise) {
		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, -rotation_angle_tiger * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(bus_radius, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.007f, 0.007f, 0.007f));
	}
	else {
		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, rotation_angle_tiger * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(bus_radius, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.007f, 0.007f, 0.007f));
	}
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_tiger();

	//DRAW COW
	for (int i = 0; i < 3; i++) {
		ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, (120.0f * i * TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		if(i%2) ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(6.0f, 0.0f, 0.0f));
		else ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(4.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, (rotation_angle_cow * TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_object(OBJECT_COW, 152 / 255.0f, 255 / 255.0f, 152 / 255.0f); // Mint Green
	}

	//DRAW BEN
	for (int i = 0; i < num_bens; i++) {
		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(ben_pos_y + i * 0.8f, 0.0f, ben_pos + i * 2.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_ben();
	}

	//DRAW WOLF
	for (int i = 0; i < num_wolfs; i++) {
		ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(wolf_pos_y - i * 1.7f, 0.0f, wolf_pos - i * 3.0f));
		ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_wolf();
	}

	//DRAW DRAGON
	ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix,
		rotation_angle_tiger * TO_RADIAN, glm::vec3(1.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(5.0f, 0.0f, 2.0f));
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
	ModelViewProjectionMatrix = ModelViewProjectionMatrix
		* glm::mat4(-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_DRAGON, 65 / 255.0f, 163 / 255.0f, 23 / 255.0f); // Lime Green

	//DRAW IRONMAN
	ModelViewProjectionMatrix = glm::rotate(ViewProjectionMatrix, -rotation_angle_tiger * TO_RADIAN, glm::vec3(1.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = glm::translate(ModelViewProjectionMatrix, glm::vec3(0.0f, 3.0f, 0.0f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -90.0f * TO_RADIAN,
		glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_IRONMAN, 255 / 255.0f, 229 / 255.0f, 180 / 255.0f); // Peach

	//DRAW FLOOR
	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix, glm::vec3(-6.0f, 0.0f, -6.0f));
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(12.0f, 12.0f, 12.0f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix,
		90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(3.0f);
	draw_axes();
	glLineWidth(1.0f);
	if (flag_fill_floor == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	draw_object(OBJECT_SQUARE16, 130 / 255.0f, 201 / 255.0f, 250 / 255.0f); // Light Sky Blue
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(3.0f);
	draw_axes();
	glLineWidth(1.0f);

	glutSwapBuffers();
}

//CAMERA 5: CAMERA MOVES FREELY
void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP: /*move prp wrt -n ++*/
		camera_wv.prp.z += 1.0f;
		ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case GLUT_KEY_DOWN: /*move prp wrt -n --*/
		camera_wv.prp.z -= 1.0f;
		ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case GLUT_KEY_LEFT: /*zoom in*/
		camera_wv.zoom_factor *= 1.05f;
		if (camera_wv.zoom_factor > 5.0f)
			camera_wv.zoom_factor = 5.0f;
		ProjectionMatrix = glm::perspective(camera_wv.zoom_factor * camera_wv.fov_y * TO_RADIAN, camera_wv.aspect_ratio, camera_wv.near_clip, camera_wv.far_clip);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	case GLUT_KEY_RIGHT: /*zoom out*/
		camera_wv.zoom_factor *= 0.95f;
		if (camera_wv.zoom_factor < 0.1f)
			camera_wv.zoom_factor = 0.1f;
		ProjectionMatrix = glm::perspective(camera_wv.zoom_factor * camera_wv.fov_y * TO_RADIAN, camera_wv.aspect_ratio, camera_wv.near_clip, camera_wv.far_clip);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		break;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 27) { // ESC key
		glutLeaveMainLoop(); // incur destuction callback for cleanups.
	}
	switch (key) {
	case 'q': /*speed up rotation*/
		if (speed >= 6) speed = 1;
		else speed += 2;
		break;
	case 'w': /*increase tank distance*/
		if (tank_dist >= 60.0f) tank_dist = 40.0f;
		else tank_dist += 10.0f;
		break;
	case 'e': /*anti-clockwise*/
		clockwise = !clockwise;
		break;
	case 'r': /*bigger radius*/
		if (bus_radius + 0.5 >= 6) bus_radius = 6;
		else bus_radius += 0.5;
		break;
	case 't': /*smaller radius*/
		if (bus_radius - 0.5 <= 3.5) bus_radius = 3.5;
		else bus_radius -= 0.5;
		break;
	case 'y': /*move ben*/
		if (ben_pos_y >= 6.0f) ben_pos_y -= 12.0f;
		else ben_pos_y += 1.0f;
		break;
	case 'u': /*move wolf*/
		if (wolf_pos_y >= 6.0f) wolf_pos_y -= 12.0f;
		else wolf_pos_y += 1.0f;
		break;
	case 'i': /*multi bens*/
		if (num_bens >= 6) num_bens = 1;
		else num_bens += 1;
		break;
	case 'o': /*multi wolfs*/
		if (num_wolfs >= 3) num_wolfs = 1;
		else num_wolfs += 1;
		break;

	// CAMERA 1: initialize the camera position and orientation
	case 'a':
		camera_type = VIEW_WORLD;
		camera_wv.prp = glm::vec3(25.0f, 25.0f, 25.0f);
		camera_wv.vrp = glm::vec3(0.0f, 0.0f, 0.0f);
		camera_wv.vup = glm::vec3(0.0f, 1.0f, 0.0f);
		glutMotionFunc(set_ViewMatrix_for_world_viewer);
		glutSpecialFunc(special);
		break;
	// CAMERA 2: set opposite camera position and orientation
	case 's':
		camera_type = VIEW_OPP_WORLD;
		glutMotionFunc(set_ViewMatrix_for_opposite_world_viewer);
		break;
	// CAMERA 3: set top view
	case 'd':
		camera_type = VIEW_TOP;
		glutMotionFunc(set_ViewMatrix_for_top_viewer);
		break;
	// CAMERA 4: set spider's point of view
	case 'f':
		camera_type = VIEW_SPIDER;
		break;
	// CAMERA 5: moving camera
	case 'g':
		camera_type = VIEW_MOVE;
		camera_wv.prp = glm::vec3(5.0f, 5.0f, 25.0f);
		camera_wv.vrp = glm::vec3(0.0f, 0.0f, 0.0f);
		camera_wv.vup = glm::vec3(0.0f, 1.0f, 0.0f);
		glutMotionFunc(set_ViewMatrix_for_user_camera);
		glutSpecialFunc(special);
		break;
	}
	glutPostRedisplay();
}

// PROJECTION TRANSFORMATION: ZOOM IN & OUT
void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		prevx = x, prevy = y;
		left_mousepressed = 1;
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		prevx = x, prevy = y;
		left_mousepressed = 0;
	}
}

void reshape(int width, int height) {
	float aspect_ratio;
	glViewport(0, 0, width, height);

	camera_wv.aspect_ratio = (float)width / height;
	ProjectionMatrix = glm::perspective(camera_wv.zoom_factor * 15.0f * TO_RADIAN, camera_wv.aspect_ratio, 1.0f, 1000.0f);

	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

void timer_scene(int timestamp_scene) {
	rotation_angle_tiger = (float)(timestamp_scene + 30 % 360);
	rotation_angle_bus = (float)(timestamp_scene % 360);
	rotation_angle_spider = (float)((int)(speed * timestamp_scene) % 360);
	rotation_angle_cow = (float)((int)(2 * timestamp_scene) % 360);
	ben_pos = -6.0f + (ben_pos_speed) * (float)(timestamp_scene % ben_pos_period);
	wolf_pos = 6.0f - (wolf_pos_speed) * (float)(timestamp_scene % wolf_pos_period);

	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	cur_frame_tiger = timestamp_scene % N_TIGER_FRAMES;
	cur_frame_ben = timestamp_scene % N_BEN_FRAMES;
	cur_frame_wolf = timestamp_scene % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;	glutPostRedisplay();

	if ((int)rotation_angle_cow == 180)
		glutTimerFunc(2000, timer_scene, (timestamp_scene + 1) % INT_MAX);
	else
		glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(N_OBJECTS, object_VAO);
	glDeleteBuffers(N_OBJECTS, object_VBO);
}
// End of callback function definitions

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousepress);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void initialize_OpenGL(void) {
	camera_type = VIEW_WORLD;

	camera_wv.prp = glm::vec3(25.0f, 25.0f, 25.0f);
	camera_wv.vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_wv.vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);

	camera_wv.move = 0;
	camera_wv.fov_y = 15.0f, camera_wv.aspect_ratio = 1.0f; camera_wv.near_clip = 1.0f; camera_wv.far_clip = 1000.0f;
	camera_wv.zoom_factor = 1.0f;

	// the transformation that moves the driver's camera frame from car body's MC to driver seat
	//ModelMatrix_TIGER_to_EYE = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -90.0f, 60.0f));
	//ModelMatrix_TIGER_to_EYE = glm::rotate(ModelMatrix_TIGER_to_EYE,
	//	TO_RADIAN * 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	ModelMatrix_SPIDER_to_EYE = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.05f, 0.0f));
	ModelMatrix_SPIDER_to_EYE = glm::rotate(ModelMatrix_SPIDER_to_EYE,
		TO_RADIAN * 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	//ModelMatrix_BUS_to_EYE = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 3.0f, 12.0f));
	//ModelMatrix_BUS_to_EYE = glm::rotate(ModelMatrix_BUS_to_EYE,
	//	TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	//ModelMatrix_IRONMAN_to_EYE = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.05f, 0.0f));
	//ModelMatrix_IRONMAN_to_EYE = glm::rotate(ModelMatrix_IRONMAN_to_EYE,
	//	TO_RADIAN * 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	glClearColor(100 / 255.0f, 100 / 255.0f, 100 / 255.0f, 1.0f); // Gray
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_square();
	prepare_dragon();
	prepare_cow();
	prepare_tank();
	prepare_bus();
	prepare_ironman();
	prepare_tiger();
	prepare_ben();
	prepare_wolf();
	prepare_spider();

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

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  HW3: Understanding 3D Viewing Pipeline\n  NAME: 20150555 Minhyuk Nam\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char* argv[]) {
	char program_name[64] = "CSE4170 HW3 20150555 MINHYUK NAM";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'b', 's', 't', 'w', 'o', 'x', 'y', 'z', 'ESC', KEY-UP, KEY-DOWN",
		"    - Mouse used: Left/Right Butten Click"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
