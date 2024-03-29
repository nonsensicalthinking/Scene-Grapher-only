/*
 * Font.cpp
 *
 *  Created on: Feb 4, 2012
 *      Author: forb
 */
#include <unistd.h>
#include "Font.h"
extern MaterialManager* getMaterialManager();


// Will load a default font
Font::Font(float width, float height)	{
	this->windowDimention = new dimention_t;
	this->windowDimention->height = height;
	this->windowDimention->width = width;
	this->fontColor[0] = 1.0;
	this->fontColor[1] = 1.0;
	this->fontColor[2] = 1.0;


	textures = getMaterialManager();

	fontPath = "font.bmp";

	loadFont();
}

// Will load specified font
Font::Font(float width, float height, string font)	{
	windowDimention = new dimention_t;
	windowDimention->height = height;
	windowDimention->width = width;

	textures = getMaterialManager();

	fontPath = font;

	loadFont();
}

Font::~Font()	{
	killFont();
}

void Font::loadFont()	{
	if( textures->loadBitmap(fontPath) )	{
		buildFont();
//			cout << "Font Build Complete." << endl;
	}
	else
		cout << "Font build failed because of texture!" << endl;
}

void Font::buildFont(void) {
	GLfloat cx, cy;         /* the character coordinates in our texture */
	base = glGenLists(256);

	if( !textures->bindTexture(fontPath) )
		cout << "Failed to find font texture in texture manager." << endl;

	for (loop = 0; loop < 256; loop++)
	{
		cx = (float) (loop % 16) / 16.0f;
		cy = (float) (loop / 16) / 16.0f;
		glNewList(base + loop, GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f(cx, 1 - cy - 0.0625f);
				glVertex2i(0, 0);
				glTexCoord2f(cx + 0.0625f, 1 - cy - 0.0625f);
				glVertex2i(16, 0);
				glTexCoord2f(cx + 0.0625f, 1 - cy);
				glVertex2i(16, 16);
				glTexCoord2f(cx, 1 - cy);
				glVertex2i(0, 16);
			glEnd();
			glTranslated(LETTER_SPACING, 0, 0);
		glEndList();
	}
}

void Font::killFont(void) {
	glDeleteLists(base, 256);
}

void Font::glPrint(GLint x, GLint y, const char *string, int set) {

	// override setting passed in
	set = 0;

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	textures->bindTexture(fontPath);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glOrtho(0, windowDimention->width, 0, windowDimention->height, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			glColor3f(fontColor[0], fontColor[1], fontColor[2]);
			GLfloat emis[] = {1.0, 1.0, 1.0};
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
			glTranslated(x, y, 0);
			glListBase(base - 32 + (128 * set));
			glCallLists(strlen(string), GL_BYTE, string);
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void Font::setFontColor(float color[3])	{
	fontColor[0] = color[0];
	fontColor[1] = color[1];
	fontColor[2] = color[2];
}


