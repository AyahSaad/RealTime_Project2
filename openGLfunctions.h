#ifndef openGL_H
#define openGL_H

#include <GL/glut.h>
#include <math.h>



void drawStock(float topSpacing, float color[], int screenWidth, int screenHeight);
void drawShelf(float shelfCenter, float shelfHeight);
void drawManager(float centerX, float centerY, float radius, int segments);
void display(void);
void reshape(int width, int height);

#endif