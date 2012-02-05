/*
 * font.h
 *
 *  Created on: Jun 19, 2010
 *      Author: Derek Brooks
 */



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <math.h>
#include "MaterialManager.h"

#ifndef FONT_H_
#define FONT_H_

// distance between leading edges of letters in pixels
#define LETTER_SPACING 13

using namespace std;


// NOTE: because of this extern declaration, remember that
// things aren't the way they may seem, wasted 45m trying
// to track down a segfault that was due to the object never
// having been instantiated, but for some reason we were able to
// call one of its member functions without instantiation of
// said object...wtf?  JUST REMEMBER! DANGER!
extern MaterialManager* getMaterialManager();



typedef struct dimension_s	{
	float width;
	float height;
}dimention_t;



class Font	{
private:
	GLuint base;            /* base display list for the font */
//	GLuint texture;      /* storage for our two textures */
	GLuint loop;            /* generic loop variable */
	GLfloat cnt1, cnt2;     /* counters for movement and coloring */
	dimention_t *windowDimention;
	MaterialManager* textures;

	string fontPath;

public:
	Font(float width, float height);	// Will load a default font
	Font(float width, float height, string font);	// Will load specified font
	~Font();
	void loadFont();
	void buildFont(void);
	void killFont(void);
	void glPrint(GLint x, GLint y, const char *string, int set);

};



#endif /* FONT_H_ */
