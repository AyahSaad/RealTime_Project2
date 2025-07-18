#ifndef openGL_H
#define openGL_H


#include <math.h>



void drawStock(float topSpacing, float color[], int screenWidth, int screenHeight);
void drawWhiteRectangle(void);
void drawShelf(float shelfCenter, float shelfHeight);
void drawManager(float centerX, float centerY, float radius, int segments);
void display(void);
void reshape(int w, int h);
void drawItems(int screenWidth, int screenHeight);
void drawShelfsItems(int screenWidth, int screenHeight);
void drawString(const char *str);
#endif