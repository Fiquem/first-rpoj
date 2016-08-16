#pragma once

#include "gl_utils.h"
#include "shader.h"

#define DEFAULT_FONT_SIZE 24

typedef struct Character{
	int xpos, ypos;
	int width, height;
	int buffer;
}Character;

typedef struct Font{
	// uhhhhhhhhhhhhhhhhhjjhhhhhhhhhhhhh
	GLuint texture;
	Shader_Meta shader;
	Shader_Meta selected_shader;
	int size;
	Character* chars;
}Font;

typedef struct Text{
	Font font;
	const char* text;
	bool selected;
	float width, height, xpos, ypos;
}Text;

Font load_font(const char* font_img, const char* font_meta);
Text set_text(Font f, const char* s, int w, int h, int x, int y);
void draw_text(const char* text, Font f, float x, float y);
void draw_text(Text t);