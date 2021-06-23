#define _CRT_SECURE_NO_WARNINGS
#define EPSILON 0.01
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

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

glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix, ViewProjectionMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;
glm::mat4 ModelViewMatrix_Bus, ModelViewMatrix_Spider, ModelViewMatrix_TIGER, ModelViewMatrix_BUS;
glm::mat4 ModelViewMatrix_BEN, ModelViewMatrix_IRONMAN;
glm::mat4 ModelMatrix_Spider_to_Eye, ModelMatrix_Bus_to_Eye, ModelMatrix_TIGER_to_EYE;
glm::mat4 ModelMatrix_BEN_to_EYE, ModelMatrix_IRONMAN_to_EYE;

// for animation
unsigned int timestamp_scene = 0; // the global clock in the scene
int flag_tiger_animation, flag_polygon_fill;
int cur_frame_tiger = 0, cur_frame_ben = 0, cur_frame_wolf, cur_frame_spider = 0;
float rotation_angle_tiger = 0.0f, rotation_angle_cow = 0.0f, rotation_angle_bus = 0.0f, rotation_angle_spider = 0.0f, rotation_angle_ben = 0.0f, rotation_angle_wolf = 0.0f;
float ben_pos = -6.0f, ben_pos_speed = 0.1f, wolf_pos = 6.0f, wolf_pos_speed = 0.1f;
float ben_pos_y = 3.0f, wolf_pos_y = 4.0f;
int ben_pos_period = 121, wolf_pos_period = 121, num_bens = 1, num_wolfs = 1;
bool ben_pos_fl = FALSE, wolf_pos_fl = TRUE;

///////////////////////////////
// Begin of shader setup
///////////////////////////////
#include "Shaders/LoadShaders.h"
#include "Light_and_Material.h"

#define NUMBER_OF_LIGHT_SUPPORTED 4

// for simple shaders
GLuint h_ShaderProgram; // handles to shader programs
GLint loc_ModelViewProjectionMatrix, loc_ModelViewMatrix, loc_ModelViewMatrixInvTrans, loc_primitive_color; // indices of uniform variables

// for Gouraud Shading shaders
GLint h_ShaderProgram_GS;
GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;
loc_light_Parameters loc_light_GS[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material_GS;
GLint loc_global_ambient_color_GS;

// for Phong Shading shaders
GLuint h_ShaderProgram_PS;
GLint loc_ModelViewProjectionMatrix_PS, loc_ModelViewMatrix_PS, loc_ModelViewMatrixInvTrans_PS;
loc_light_Parameters loc_light_PS[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material_PS;
GLint loc_global_ambient_color_PS;
GLint loc_texture_PS, loc_flag_texture_mapping_PS;
GLint loc_floor_effect, loc_floor_frequency, loc_floor_width;
GLint loc_blind_effect;

// for lights
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

// for texture
#define N_TEXTURES_USED 3
#define TEXTURE_ID_FLOOR 0
#define TEXTURE_ID_SPIDER 1
#define TEXTURE_ID_TANK 2
GLuint texture_names[N_TEXTURES_USED];
int flag_texture_mapping = 1;
int flag_draw_floor, flag_floor_effect;
float floor_frequency, floor_width;

void My_glTexImage2D_from_file(const char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);
}

void prepare_shader_program(void) {
	int i;
	char string[256];

	//simple shader
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");

	//gourad shader
	ShaderInfo shader_info_GS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light_GS[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light_GS[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light_GS[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light_GS[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light_GS[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light_GS[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light_GS[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light_GS[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light_GS[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}

	loc_material_GS.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material_GS.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material_GS.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material_GS.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material_GS.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");

	//phong shader
	ShaderInfo shader_info_PS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_PS = LoadShaders(shader_info_PS);
	loc_ModelViewProjectionMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light_PS[i].light_on = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light_PS[i].position = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light_PS[i].ambient_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light_PS[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light_PS[i].specular_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light_PS[i].spot_direction = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light_PS[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light_PS[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light_PS[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_PS, string);
	}

	loc_material_PS.ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.ambient_color");
	loc_material_PS.diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.diffuse_color");
	loc_material_PS.specular_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_color");
	loc_material_PS.emissive_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.emissive_color");
	loc_material_PS.specular_exponent = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_exponent");
	
	loc_texture_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_base_texture");
	loc_flag_texture_mapping_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_flag_texture_mapping");

	loc_floor_effect = glGetUniformLocation(h_ShaderProgram_PS, "floor_effect");
	loc_floor_frequency = glGetUniformLocation(h_ShaderProgram_PS, "floor_frequency");
	loc_floor_width = glGetUniformLocation(h_ShaderProgram_PS, "screen_width");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_blind_effect");
}

void initialize_lights_and_material(GLint h_ShaderProgram, GLint loc_global_ambient_color, loc_light_Parameters* loc_light, loc_Material_Parameters loc_material) {
	int i;

	glUseProgram(h_ShaderProgram);

	glUniform4f(loc_global_ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

// set up lights attributes
void set_up_lights(GLint h_ShaderProgram, loc_light_Parameters* loc_light) {
	glUseProgram(h_ShaderProgram);

	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 10.0f; 	// point light position in EC
	light[0].position[2] = 10.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.1f;
	light[0].ambient_color[2] = 0.1f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.55f; light[0].diffuse_color[1] = 0.35f;
	light[0].diffuse_color[2] = 0.35f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 1.0f; light[0].specular_color[1] = 0.85f;
	light[0].specular_color[2] = 0.85f; light[0].specular_color[3] = 1.0f;

	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);
	glUniform3fv(loc_light[0].spot_direction, 1, light[0].spot_direction);

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = 0.0f; light[1].position[1] = 30.0f; // spot light position in WC
	light[1].position[2] = 0.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.152f; light[1].ambient_color[1] = 0.152f;
	light[1].ambient_color[2] = 0.152f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.572f; light[1].diffuse_color[1] = 0.572f;
	light[1].diffuse_color[2] = 0.572f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.772f; light[1].specular_color[1] = 0.772f;
	light[1].specular_color[2] = 0.772f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 0.0f;
	light[1].spot_cutoff_angle = 5.0f;
	light[1].spot_exponent = 8.0f;

	glUniform1i(loc_light[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	
	// spot_light_MC: use light 2
	light[2].light_on = 1;
	light[2].position[0] = 0.0f; light[2].position[1] = 5.0f; // spot light position in WC
	light[2].position[2] = 7.0f; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 0.05f; light[2].ambient_color[1] = 0.05f;
	light[2].ambient_color[2] = 0.05f; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 0.572f; light[2].diffuse_color[1] = 0.572f;
	light[2].diffuse_color[2] = 0.572f; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 1.0f; light[2].specular_color[1] = 1.0f;
	light[2].specular_color[2] = 0.0f; light[2].specular_color[3] = 1.0f;

	light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = 0.0f; // spot light direction in WC
	light[2].spot_direction[2] = 1.0f;
	light[2].spot_cutoff_angle = 500.0f;
	light[2].spot_exponent = 8.0f;

	glUniform1i(loc_light[2].light_on, light[2].light_on);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);

	if (h_ShaderProgram == h_ShaderProgram_PS) {
		glUniform1i(loc_floor_effect, 0);
		glUniform1f(loc_floor_frequency, 6.0f);
		glUniform1f(loc_floor_width, 0.2f);

		glUniform1i(loc_blind_effect, 0);
	}
	
	glUseProgram(0);
}

///////////////////////////////
// Begin of geometry setup
///////////////////////////////

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION 0

//axes
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

// static objects
#define N_OBJECTS 5
#define OBJECT_DRAGON 0
#define OBJECT_COW 1
#define OBJECT_TANK 2
#define OBJECT_BUS 3
#define OBJECT_IRONMAN 4

GLuint object_VBO[N_OBJECTS], object_VAO[N_OBJECTS];
int object_n_triangles[N_OBJECTS];
GLfloat* object_vertices[N_OBJECTS];
Material_Parameters material_bus;
Material_Parameters material_tank;
Material_Parameters material_cow;

// animated objects
//// tiger object
#define N_TIGER_FRAMES 12
GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat* tiger_vertices[N_TIGER_FRAMES];
Material_Parameters material_tiger;

// floor object
#define TEX_COORD_EXTENT 1.0f
GLuint rectangle_VBO, rectangle_VAO;
GLfloat rectangle_vertices[6][8] = {  // vertices enumerated counterclockwise
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, TEX_COORD_EXTENT, 0.0f },
	{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, TEX_COORD_EXTENT, TEX_COORD_EXTENT },
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, TEX_COORD_EXTENT, TEX_COORD_EXTENT },
	{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, TEX_COORD_EXTENT }
};
Material_Parameters material_floor;

//// ben object
#define N_BEN_FRAMES 30
GLuint ben_VBO, ben_VAO;
int ben_n_triangles[N_BEN_FRAMES];
int ben_vertex_offset[N_BEN_FRAMES];
GLfloat* ben_vertices[N_BEN_FRAMES];
Material_Parameters material_ben;

//// wolf object
#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];
Material_Parameters material_wolf;

//// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];
Material_Parameters material_spider;

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

void set_up_object(int object_ID, const char* filename, int n_bytes_per_vertex, int texcoord) {
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
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(LOC_NORMAL);
	if (texcoord) {
		glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(LOC_TEXCOORD);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_floor(void) { // Draw coordinate axes.
	// Initialize vertex buffer object.
	glGenBuffers(1, &rectangle_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices[0][0], GL_STATIC_DRAW);

	//free(rectangle_vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &rectangle_VAO);
	glBindVertexArray(rectangle_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(LOC_NORMAL);
	//glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
	//glEnableVertexAttribArray(LOC_TEXCOORD);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material: emerald (see lecture note chp8 #51)
	material_floor.ambient_color[0] = 0.0215f;
	material_floor.ambient_color[1] = 0.1745f;
	material_floor.ambient_color[2] = 0.0215f;
	material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.07568f;
	material_floor.diffuse_color[1] = 0.61424f;
	material_floor.diffuse_color[2] = 0.07568f;
	material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.633f;
	material_floor.specular_color[1] = 0.727811f;
	material_floor.specular_color[2] = 0.633f;
	material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 76.8f;

	material_floor.emissive_color[0] = 0.0f;
	material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f;
	material_floor.emissive_color[3] = 1.0f;
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
	glEnableVertexAttribArray(LOC_TEXCOORD);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material: silver (see lecture note chp8 #51)
	material_spider.ambient_color[0] = 0.25f;
	material_spider.ambient_color[1] = 0.20725f;
	material_spider.ambient_color[2] = 0.20725f;
	material_spider.ambient_color[3] = 1.0f;

	material_spider.diffuse_color[0] = 1.0f;
	material_spider.diffuse_color[1] = 0.829f;
	material_spider.diffuse_color[2] = 0.829f;
	material_spider.diffuse_color[3] = 1.0f;

	material_spider.specular_color[0] = 0.296648f;
	material_spider.specular_color[1] = 0.296648f;
	material_spider.specular_color[2] = 0.296648f;
	material_spider.specular_color[3] = 1.0f;

	material_spider.specular_exponent = 11.3f;

	material_spider.emissive_color[0] = 0.1f;
	material_spider.emissive_color[1] = 0.1f;
	material_spider.emissive_color[2] = 0.0f;
	material_spider.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_SPIDER);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_SPIDER]);

	My_glTexImage2D_from_file("Data/pattern2_tex.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	glEnableVertexAttribArray(LOC_VERTEX);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(LOC_NORMAL);
	//glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	//glEnableVertexAttribArray(LOC_TEXCOORD);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material: bronze (see lecture note chp8 #51)
	material_tiger.ambient_color[0] = 0.2125f;
	material_tiger.ambient_color[1] = 0.1275f;
	material_tiger.ambient_color[2] = 0.0540f;
	material_tiger.ambient_color[3] = 1.0f;

	material_tiger.diffuse_color[0] = 0.714f;
	material_tiger.diffuse_color[1] = 0.4284f;
	material_tiger.diffuse_color[2] = 0.18144f;
	material_tiger.diffuse_color[3] = 1.0f;

	material_tiger.specular_color[0] = 0.393548f;
	material_tiger.specular_color[1] = 0.271906f;
	material_tiger.specular_color[2] = 0.166721f;
	material_tiger.specular_color[3] = 1.0f;

	material_tiger.specular_exponent = 25.6f;

	material_tiger.emissive_color[0] = 0.1f;
	material_tiger.emissive_color[1] = 0.1f;
	material_tiger.emissive_color[2] = 0.0f;
	material_tiger.emissive_color[3] = 1.0f;
}

void prepare_dragon(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_DRAGON, "Data/static_objects/dragon_vnt.geom", 8 * sizeof(float), 0);

	//material: emerald (see lecture note chp8 #51)
	material_cow.ambient_color[0] = 0.0215f;
	material_cow.ambient_color[1] = 0.1745f;
	material_cow.ambient_color[2] = 0.0215f;
	material_cow.ambient_color[3] = 1.0f;

	material_cow.diffuse_color[0] = 0.07568f;
	material_cow.diffuse_color[1] = 0.61424f;
	material_cow.diffuse_color[2] = 0.07568f;
	material_cow.diffuse_color[3] = 1.0f;

	material_cow.specular_color[0] = 0.633f;
	material_cow.specular_color[1] = 0.727811f;
	material_cow.specular_color[2] = 0.633f;
	material_cow.specular_color[3] = 1.0f;

	material_cow.specular_exponent = 76.8f;

	material_cow.emissive_color[0] = 0.0f;
	material_cow.emissive_color[1] = 0.0f;
	material_cow.emissive_color[2] = 0.0f;
	material_cow.emissive_color[3] = 1.0f;
}

void prepare_cow(void) {
	// 6 = 3 for vertex, and 3 for normal
	set_up_object(OBJECT_COW, "Data/static_objects/cow_vn.geom", 6 * sizeof(float), 0);
}

void prepare_tank(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_TANK, "Data/static_objects/tank_vnt.geom", 8 * sizeof(float), 1);

	material_tank.ambient_color[0] = 0.0215f;
	material_tank.ambient_color[1] = 0.1745f;
	material_tank.ambient_color[2] = 0.0215f;
	material_tank.ambient_color[3] = 1.0f;

	material_tank.diffuse_color[0] = 0.07568f;
	material_tank.diffuse_color[1] = 0.61424f;
	material_tank.diffuse_color[2] = 0.07568f;
	material_tank.diffuse_color[3] = 1.0f;

	material_tank.specular_color[0] = 0.633f;
	material_tank.specular_color[1] = 0.727811f;
	material_tank.specular_color[2] = 0.633f;
	material_tank.specular_color[3] = 1.0f;

	material_tank.specular_exponent = 76.8f;

	material_tank.emissive_color[0] = 0.0f;
	material_tank.emissive_color[1] = 0.0f;
	material_tank.emissive_color[2] = 0.0f;
	material_tank.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glActiveTexture(GL_TEXTURE0 + TEXTURE_ID_TANK);
	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_ID_TANK]);

	My_glTexImage2D_from_file("Data/pattern1_tex.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void prepare_bus(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_BUS, "Data/static_objects/bus_vnt.geom", 8 * sizeof(float), 0);

	//material: silver (see lecture note chp8 #51)
	material_bus.ambient_color[0] = 0.1745f;
	material_bus.ambient_color[1] = 0.01175f;
	material_bus.ambient_color[2] = 0.01175f;
	material_bus.ambient_color[3] = 1.0f;

	material_bus.diffuse_color[0] = 0.61424f;
	material_bus.diffuse_color[1] = 0.04136f;
	material_bus.diffuse_color[2] = 0.04136f;
	material_bus.diffuse_color[3] = 1.0f;

	material_bus.specular_color[0] = 0.727811f;
	material_bus.specular_color[1] = 0.626959f;
	material_bus.specular_color[2] = 0.626959f;
	material_bus.specular_color[3] = 1.0f;

	material_bus.specular_exponent = 76.8f;

	material_bus.emissive_color[0] = 0.1f;
	material_bus.emissive_color[1] = 0.1f;
	material_bus.emissive_color[2] = 0.0f;
	material_bus.emissive_color[3] = 1.0f;
}

void prepare_ironman(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_IRONMAN, "Data/static_objects/ironman_vnt.geom", 8 * sizeof(float), 0);
}

void draw_object(int object_ID, float r, float g, float b) {
	glUniform3f(loc_primitive_color, r, g, b);
	glBindVertexArray(object_VAO[object_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * object_n_triangles[object_ID]);
	glBindVertexArray(0);
}

void draw_floor(void) {
	glFrontFace(GL_CCW);

	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void initialize_floor(void) {
	flag_draw_floor = 1;
	flag_floor_effect = 0;
	floor_frequency = 6.0f;
	floor_width = 0.2f;
}

void draw_object_shaded(int object_ID) {
	glFrontFace(GL_CCW);

	glBindVertexArray(object_VAO[object_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * object_n_triangles[object_ID]);
	glBindVertexArray(0);
}

void draw_moving_object(GLuint VAO, int* vertex_offset, int* n_triangles, int cur_frame) {
	glFrontFace(GL_CW);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, vertex_offset[cur_frame], 3 * n_triangles[cur_frame]);
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

void set_material(Material_Parameters material, loc_Material_Parameters loc_material) {
	glUniform4fv(loc_material.ambient_color, 1, material.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material.specular_color);
	glUniform1f(loc_material.specular_exponent, material.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material.emissive_color);
}

///////////////////////////////
// Begin of Camera Setup
///////////////////////////////

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

	Matrix_CAMERA_spider_inverse = ModelViewMatrix_Spider * ModelMatrix_Spider_to_Eye;

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
	else {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;
		mat4_tmp = glm::translate(glm::mat4(1.0f), glm::vec3(delx, dely, 0.0f));
		camera_wv.prp = glm::vec3(mat4_tmp * glm::vec4(camera_wv.prp, 1.0f));
		ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

///////////////////////////////
// Begin of Callback function definitions
///////////////////////////////

float speed = 1.0;
float bus_radius = 5.0f;
bool clockwise = TRUE;
float tank_dist = 40.0f;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(h_ShaderProgram);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(50.0f, 50.0f, 50.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);
	
	//DRAW FLOOR
	glUseProgram(h_ShaderProgram_PS);
	set_material(material_floor, loc_material_PS);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-7.0f, 0.0f, 7.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(14.0f, 14.0f, 14.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	draw_floor();

	if (flag_draw_floor) {
		set_material(material_floor, loc_material_PS);
		glUniform1i(loc_floor_effect, flag_floor_effect);
		draw_floor();
		//glUniform1i(loc_floor_effect, 0);
	}

	//DRAW TIGER
	glUseProgram(h_ShaderProgram_GS);
	set_material(material_tiger, loc_material_GS);
	ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_tiger * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(bus_radius, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.007f, 0.007f, 0.007f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_moving_object(tiger_VAO, tiger_vertex_offset, tiger_n_triangles, cur_frame_tiger);

	//DRAW BUS
	ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_bus * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(bus_radius, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	// need to supply position in EC for shading
	glm::vec4 position_EC = ModelViewMatrix * glm::vec4(light[2].position[0], light[2].position[1],
		light[2].position[2], light[2].position[3]);
	glUniform4fv(loc_light_GS[2].position, 1, &position_EC[0]);
	// need to supply direction in EC for shading in this example shader
	glm::vec3 direction_EC = glm::normalize(glm::mat3(ModelViewMatrixInvTrans) * glm::vec3(light[2].spot_direction[0], light[2].spot_direction[1],
		light[2].spot_direction[2]));
	glUniform3fv(loc_light_GS[2].spot_direction, 1, &direction_EC[0]);
	
	set_material(material_bus, loc_material_GS);
	draw_object_shaded(OBJECT_BUS);

	//DRAW SPIDER
	glUseProgram(h_ShaderProgram_PS);
	set_material(material_spider, loc_material_PS);
	glUniform1i(loc_texture_PS, TEXTURE_ID_SPIDER);
	ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_spider * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -180.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	//if (camera_type == VIEW_SPIDER) set_ViewMatrix_for_spider();
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_moving_object(spider_VAO, spider_vertex_offset, spider_n_triangles, cur_frame_spider);

	/*for camera for the spider's eye view
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, -0.05f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes(); // draw MC axes of body
	glLineWidth(1.0f);
	*/
//	glUseProgram(h_ShaderProgram_simple);
//	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//	draw_axes();

	//DRAW DRAGON
	set_material(material_cow, loc_material_PS);
	for (int i = 0; i < 3; i++) {
		ModelViewMatrix = glm::rotate(ViewMatrix, (120.0f * i * TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		if (i % 2) ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(5.0f, 0.0f, 0.0f));
		else ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(3.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (rotation_angle_cow * TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.06f, 0.06f, 0.06f));

		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_object_shaded(OBJECT_DRAGON);
	}

	//DRAW TANK
	set_material(material_tank, loc_material_PS);
	glUniform1i(loc_texture_PS, TEXTURE_ID_TANK);

	for (int i = 0; i < 3; i++) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-1.0f, 0.0f, -0.5f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (100.0f + tank_dist * i) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

		draw_object_shaded(OBJECT_TANK);
	}

/*
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-6.0f, 0.0f, -6.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(12.0f, 12.0f, 12.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix,
		90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	//glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	glLineWidth(3.0f);
	draw_axes();
	glLineWidth(1.0f);
	if (flag_fill_floor == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	draw_object(OBJECT_SQUARE16, 130 / 255.0f, 201 / 255.0f, 250 / 255.0f); // Light Sky Blue
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(3.0f);
	draw_axes();
	glLineWidth(1.0f);
	*/
	glUseProgram(0);
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

#define FLOOR_MIN_FREQUENCY 6.0f
#define FLOOR_MAX_FREQUENCY 10.0f

void keyboard(unsigned char key, int x, int y) {
	static int flag_polygon_mode = 1;
	static int flag_blind_effect = 0;

	if (key == 27) { // ESC key
		glutLeaveMainLoop(); // incur destuction callback for cleanups.
	}
	if (key == 'w') {/*lights in WC*/
		glUseProgram(h_ShaderProgram_PS);
		light[1].light_on = 1 - light[1].light_on;
		glUniform1i(loc_light_PS[1].light_on, light[1].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		return;
	}
	if (key == 'e') {/*lights in EC*/
		glUseProgram(h_ShaderProgram_PS);
		light[0].light_on = 1 - light[0].light_on;
		glUniform1i(loc_light_PS[0].light_on, light[0].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		return;
	}
	if (key == 'm') {/*lights above the bus*/
		glUseProgram(h_ShaderProgram_GS);
		light[2].light_on = 1 - light[2].light_on;
		glUniform1i(loc_light_GS[2].light_on, light[2].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 'z': /*shader 1*/
		flag_blind_effect = 1 - flag_blind_effect;
		glUseProgram(h_ShaderProgram_PS);
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;

	case 'x': /*shader 2-1*/
		if (flag_draw_floor) {
			flag_floor_effect = 1 - flag_floor_effect;
			glutPostRedisplay();
		}
		break;

	case 'c': /*shaer 2-2*/
		if (flag_draw_floor) {
			floor_frequency += 1.0f;
			if (floor_frequency > FLOOR_MAX_FREQUENCY)
				floor_frequency = FLOOR_MAX_FREQUENCY;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_floor_frequency, floor_frequency);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;

	case 'v': /*shaer 2-3*/
		if (flag_draw_floor) {
			floor_frequency -= 1.0f;
			if (floor_frequency < FLOOR_MIN_FREQUENCY)
				floor_frequency = FLOOR_MIN_FREQUENCY;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_floor_frequency, floor_frequency);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;
	}

		/* CAMERA 1: initialize the camera position and orientation
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
			*/
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

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteVertexArrays(1, &spider_VAO);
	glDeleteBuffers(1, &spider_VBO);

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	//initialise camera
	/*
	camera_type = VIEW_WORLD;

	camera_wv.prp = glm::vec3(25.0f, 25.0f, 25.0f);
	camera_wv.vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_wv.vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);

	camera_wv.move = 0;
	camera_wv.fov_y = 15.0f, camera_wv.aspect_ratio = 1.0f; camera_wv.near_clip = 1.0f; camera_wv.far_clip = 1000.0f;
	camera_wv.zoom_factor = 1.0f;

	ModelMatrix_Spider_to_Eye = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.05f, 0.0f));
	ModelMatrix_Spider_to_Eye = glm::rotate(ModelMatrix_Spider_to_Eye,
		TO_RADIAN * 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	ModelMatrix_Bus_to_Eye = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 3.0f, 12.0f));
	ModelMatrix_Bus_to_Eye = glm::rotate(ModelMatrix_Bus_to_Eye,
		TO_RADIAN * 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	// the transformation that moves the driver's camera frame from car body's MC to driver seat
	ModelMatrix_TIGER_to_EYE = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -90.0f, 60.0f));
	ModelMatrix_TIGER_to_EYE = glm::rotate(ModelMatrix_TIGER_to_EYE,
		TO_RADIAN * 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));


	ModelMatrix_IRONMAN_to_EYE = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.05f, 0.0f));
	ModelMatrix_IRONMAN_to_EYE = glm::rotate(ModelMatrix_IRONMAN_to_EYE,
		TO_RADIAN * 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	*/

	camera_type = VIEW_MOVE;
	camera_wv.prp = glm::vec3(25.0f, 25.0f, 25.0f);
	camera_wv.vrp = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_wv.vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix = glm::lookAt(camera_wv.prp, camera_wv.vrp, camera_wv.vup);

	camera_wv.move = 0;
	camera_wv.fov_y = 15.0f, camera_wv.aspect_ratio = 1.0f; camera_wv.near_clip = 1.0f; camera_wv.far_clip = 1000.0f;
	camera_wv.zoom_factor = 1.0f;

	glutMotionFunc(set_ViewMatrix_for_user_camera);
	glutSpecialFunc(special);
	initialize_floor();

	glClearColor(100 / 255.0f, 100 / 255.0f, 100 / 255.0f, 1.0f); // Gray
	initialize_lights_and_material(h_ShaderProgram_PS, loc_global_ambient_color_PS, loc_light_PS, loc_material_PS);
	initialize_lights_and_material(h_ShaderProgram_GS, loc_global_ambient_color_GS, loc_light_GS, loc_material_GS);
	glGenTextures(N_TEXTURES_USED, texture_names);
	glUseProgram(h_ShaderProgram_PS);
	glUniform1i(loc_flag_texture_mapping_PS, flag_texture_mapping);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_floor();
	prepare_dragon();
	//prepare_cow();
	prepare_tank();
	prepare_bus();
	//prepare_ironman();
	prepare_tiger();
	//prepare_ben();
	//prepare_wolf();
	prepare_spider();
	set_up_lights(h_ShaderProgram_GS, loc_light_GS);
	set_up_lights(h_ShaderProgram_PS, loc_light_PS);

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
	fprintf(stdout, "  HW4: Understanding Shaders\n  NAME: 20150555 Minhyuk Nam\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char* argv[]) {
	char program_name[64] = "CSE4170 HW4 20150555 MINHYUK NAM";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'w', 'e', 'm', 'z', 'x', 'c', 'v', KEY-UP, KEY-DOWN, KEY-RIGHT, KEY-LEFT",
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
