#include "text.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "maths_funcs.h"

GLuint text_vao, text_point_vbo, text_tex_vbo;

Font load_font (const char* font_img, const char* font_meta){
	Font f;
	f.size = DEFAULT_FONT_SIZE;

	// load font tex
	int x, y, n;
    unsigned char* image = stbi_load(font_img, &x, &y, &n, 0);

	glGenTextures (1, &f.texture);
	glActiveTexture (f.texture);
	glBindTexture (GL_TEXTURE_2D, f.texture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// load font shader
	create_program_from_files ("text.vert", "text.frag", &f.shader);
	glUseProgram (f.shader.program);
	glUniformMatrix4fv (f.shader.P_loc, 1, GL_FALSE, ortho(0.0, 800.0, 0.0, 600.0, 0.05, 100.0).m);
	// glUniform3f (f.shader.colour_loc, 0.0,0.0,0.0);

	// jus copied I@ll do better later so tired now
	glGenBuffers(1, &text_point_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, text_point_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, NULL, GL_STATIC_DRAW);
	glGenBuffers(1, &text_tex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, text_tex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, NULL, GL_STATIC_DRAW);

	glGenVertexArrays(1, &text_vao);
	glBindVertexArray(text_vao);
	glEnableVertexAttribArray (POINT);
	glEnableVertexAttribArray (TEX_COORD);
	glBindBuffer (GL_ARRAY_BUFFER, text_point_vbo);
	glVertexAttribPointer (POINT, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer (GL_ARRAY_BUFFER, text_tex_vbo);
	glVertexAttribPointer (TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// glBindVertexArray(0);     

	// // will figure out size later. don't really care rn.

	// // load character info from meta
	// NO META FOR THE MOMENT
	int rows = 16;
	int cols = 16;
	int char_size_px = 1024/16;
	f.chars = (Character*)malloc(sizeof(Character)*256);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++){
			f.chars[(i*cols) + j].xpos = char_size_px * j;
			f.chars[(i*cols) + j].ypos = (char_size_px * i);
			f.chars[(i*cols) + j].width = char_size_px;
			f.chars[(i*cols) + j].height = char_size_px;
			f.chars[(i*cols) + j].buffer = 0;
		}

	return f;
}

void draw_text (const char* text, Font f, float x, float y){
	glUseProgram (f.shader.program);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(text_vao);


    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, f.texture);

    int tex_width = 1024/4;
    int tex_height = 1024/4;

	int i = 0;
	while(text[i] != '\0'){
		Character c = f.chars[text[i] - ' '];

        GLfloat xpos = (c.xpos%16);
        GLfloat ypos = (c.ypos/16);

        GLfloat w = c.width;
        GLfloat h = c.height;

        //printf("%c %i - %f %f %f %f\n", text[i], (int)text[i], xpos, ypos, w, h);

        // Update VBO for each character
	    GLfloat vertices[12] = {
	        x,     y,
	        x + w, y,
	        x,     y + h,

	        x,     y + h,
	        x + w, y,
	        x + w, y + h
	    };
	    // GLfloat tex_coords[12] = {
	    //     xpos/tex_width,       ypos/tex_height,
	    //     (xpos + w)/tex_width, ypos/tex_height,
	    //     xpos/tex_width,       (ypos + h)/tex_height,

	    //     xpos/tex_width,       (ypos + h)/tex_height,
	    //     (xpos + w)/tex_width, ypos/tex_height,
	    //     (xpos + w)/tex_width, (ypos + h)/tex_height
	    // };
	    GLfloat tex_coords[12] = {
	        xpos, ypos - h,
	        xpos + w, ypos - h,
	        xpos, ypos,

	        xpos, ypos,
	        xpos + w, ypos - h,
	        xpos + w, ypos
	    };

	    for (int j = 0; j < 12; j++)
	    	tex_coords[j] /= 1024.0;

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, text_point_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, text_tex_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, tex_coords, GL_DYNAMIC_DRAW);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += 64;

		i++;
	}
	glDisable (GL_BLEND);
}