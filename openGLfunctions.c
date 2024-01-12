#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "fileReaders.h"

void drawStock(float topSpacing, float color[], int screenWidth, int screenHeight) {
    glColor3fv(color);

    // Set rectangle width to be the same as screen width
    float rectangleWidth = (float)screenWidth / 2.0f;

    // Increase the height of the grey rectangle (adjust the factor as needed)
    float greyRectangleHeight = -0.9f * screenHeight;

    glBegin(GL_QUADS);
    glVertex2f(-rectangleWidth / 2.0f, 1.0f - topSpacing);               // Top-left
    glVertex2f(-rectangleWidth / 2.0f, 1.0f - topSpacing - greyRectangleHeight); // Bottom-left
    glVertex2f(rectangleWidth / 2.0f, 1.0f - topSpacing - greyRectangleHeight);  // Bottom-right
    glVertex2f(rectangleWidth / 2.0f, 1.0f - topSpacing);               // Top-right
    glEnd();
}

void drawShelf(float shelfCenter, float shelfHeight) {
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color for shelves
    // Calculate the shelf dimensions
    float shelfWidth = 0.36f;
    float shelfTop = shelfHeight / 0.5f;
    float shelfBottom = -shelfHeight / 0.5f;
    // Draw the shelf
    glBegin(GL_QUADS);
    glVertex2f(shelfCenter - shelfWidth / 2.0f, shelfTop);
    glVertex2f(shelfCenter - shelfWidth / 2.0f, shelfBottom);
    glVertex2f(shelfCenter + shelfWidth / 7.0f, shelfBottom);
    glVertex2f(shelfCenter + shelfWidth / 7.0f, shelfTop);
    glEnd();
}

void drawManager(float centerX, float centerY, float radius, int segments) {
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY); // Center of the circle
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        glVertex2f(centerX + x, centerY + y);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    float greyColor[] = {0.5f, 0.5f, 0.5f}; // Grey color
    float redColor[] = {1.0f, 0.0f, 0.0f};   // Red color

    // Get the screen size
     int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int  screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    drawStock(0.5f, redColor, screenWidth, screenHeight);  // Bottom rectangle in red

    float shelfHeight = 0.15f; // Adjust the shelf height as needed
    for (int i = 0; i < 7; i++) {
        float shelfCenter = -1.0f + 0.3f + i * 0.25f; // Calculate the center of the shelf
        drawShelf(shelfCenter, shelfHeight);
    }

    // Draw 10 white circles at the top of the screen with increased size and spacing
    for (int i = 0; i < 3; i++) {
        float circleSpacing = 0.05f;
        float circleRadius = 0.08f;
        float circleX = -0.6f + i * (circleSpacing + 2 * circleRadius);
        float circleY = 0.85f;
        drawManager(circleX, circleY, circleRadius, 30);
    }

    glFlush();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glMatrixMode(GL_MODELVIEW);
}
