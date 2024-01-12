#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "fileReaders.h"

int SCREEN_HEIGHT;
int SCREEN_WIDTH;

void drawItems(int screenWidth, int screenHeight)
{
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(0.0f, screenHeight * 0.8); // Initial position for the first item

    // pthread_mutex_lock(&glCopyMutex);

    for (int i = 0; i < PRODUCT_COUNT; i++)
    {
        char itemString[60];

        // TODO: ADD THE MUTEX INITALIZATION

        // TODO: ADD THE PRODUCT NAME & PRICE;
        sprintf(itemString, "%s : %d", products[i].name, products[i].amountInStock);

        // Scale the text to achieve a larger font size
        glPushMatrix();
        // glScalef(1.5, 1.5, 1.0); // Adjust the scaling factor as needed

        // Draw the text with scaled size
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)itemString);

        glPopMatrix();

        // Move to the next position
        glRasterPos2f(screenWidth * 0.05 + ((i)*screenWidth * 0.1), screenHeight * 0.8);
    }

    // pthread_mutex_unlock(&glCopyMutex);
}
void drawShelfsItems(int screenWidth, int screenHeight, float spacing)
{
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(0.05 * screenWidth, screenHeight * 0.7); // Initial position for the first item

    for (int i = 0; i < 7; i++)
    {
        float xPos = (0.2 * screenWidth) + (i * (screenWidth * 0.1 + spacing));
        char itemString[60];

        // TODO: ADD THE MUTEX INITALIZATION

        // TODO: ADD THE PRODUCT NAME & PRICE;
        sprintf(itemString, "%s : %d", products[i].name, products[i].currentAmountOnShelves);

        // Scale the text to achieve a larger font size
        glPushMatrix();
        // glScalef(1.5, 1.5, 1.0); // Adjust the scaling factor as needed

        // Draw the text with scaled size
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)itemString);

        glPopMatrix();

        // Move to the next position
        glRasterPos2f(xPos, screenHeight * 0.7);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    // Get the screen width and height
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Set the rectangle dimensions
    float rectangleWidth = screenWidth;
    float rectangleHeight = screenHeight * 0.25;

    // Draw the red rectangle
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(0.0, screenHeight - rectangleHeight);            // Top-left corner
    glVertex2f(rectangleWidth, screenHeight - rectangleHeight); // Top-right corner
    glVertex2f(rectangleWidth, screenHeight);                   // Bottom-right corner
    glVertex2f(0.0, screenHeight);                              // Bottom-left corner
    glEnd();

    // Set circle properties
    float circleRadius = screenWidth * 0.03;
    float circlePosY = screenHeight * 0.08;

    for (int i = 0; i < 5; ++i)
    {
        float circlePosX = (i + 0.5) * (screenWidth / 5.0);

        glBegin(GL_POLYGON);
        glColor3f(1.0, 1.0, 1.0);
        for (int j = 0; j < 360; ++j)
        {
            float angle = j * M_PI / 180.0;
            float x = circlePosX + circleRadius * cos(angle);
            float y = screenHeight - (circlePosY + circleRadius * sin(angle));
            glVertex2f(x, y);
        }
        glEnd();
    }
    float spacing = 0.0001 * screenWidth;
    drawItems(screenWidth, screenHeight);
    // drawShelfsItems(screenWidth,  screenHeight,spacing)

    for (int i = 0; i < 7; i++)
    {
        // Toggle the color for each rectangle

        // Calculate the x-position for each rectangle
        float xPos = 0.002 * screenWidth + i * (rectangleWidth * 0.13 + spacing);

        // Draw the rectangle
        glBegin(GL_POLYGON);
        if (products[i].currentAmountOnShelves < SHELF_THRESHOLD)
        {
            glColor3f(1.0, 1.0, 1.0);
        }
        else
        {
            glColor3f(0.5, 0.5, 0.5);
        }
        glVertex2f(xPos, screenHeight * 0.7);                        // Top-left corner
        glVertex2f(xPos + rectangleWidth * 0.1, screenHeight * 0.7); // Top-right corner
        glVertex2f(xPos + rectangleWidth * 0.1, screenHeight * 0.3); // Bottom-right corner
        glVertex2f(xPos, screenHeight * 0.3);                        // Bottom-left corner
        glEnd();

        char itemString[50];
        glColor3f(0.0, 0.0, 0.0);
        sprintf(itemString, "#Product on Shelf : %d", products[i].currentAmountOnShelves);
        glRasterPos2f(xPos + (rectangleWidth * 0.1) / 6.3, screenHeight * 0.51);
        drawString(itemString);
        sprintf(itemString, "Product Name : %s", products[i].name);
        glRasterPos2f(xPos + (rectangleWidth * 0.1) / 6.3, screenHeight * 0.55);
        drawString(itemString);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void drawString(const char *str)
{
    for (int i = 0; str[i] != '\0'; ++i)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
    }
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glMatrixMode(GL_MODELVIEW);
}