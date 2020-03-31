#include<windows.h>
#include<stdio.h>
#include<GL/glut.h>
#include<stdlib.h>
int x,y,rflag=0;
void draw_pixel(float x1,float y1)
{
    glColor3f(0.0,0.0,1.0);
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glVertex2f(x1,y1);
    glEnd();
}
void triangle()
{
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_POLYGON);
    glVertex2f(100,100);
    glVertex2f(250,400);
    glVertex2f(400,100);
    glEnd();
}
float trX=0.0,trY=0.0,th=0.0;
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if(rflag==1)
    {
        trX=0.0;
        trY=0.0;
        th+=0.1;
        draw_pixel(0.0,0.0);
    }
    if(rflag==2)
    {
        trX=x;
        trY=y;
        th+=0.1;
        draw_pixel(x,y);
    }
    glTranslatef(trX,trY,0.0);
    glRotatef(th,0.0,0.0,1.0);
    glTranslatef(-trX,-trY,0.0);
    triangle();
    glutPostRedisplay();
    glutSwapBuffers();
}
void myInit()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-500.0,500.0,-500.0,500.0);
    glMatrixMode(GL_MODELVIEW);
}
void rotatemenu(int option)
{
    if(option==1)
        rflag=1;
    if(option==2)
        rflag=2;
    if(option==3)
        rflag=3;
}
int main(int argc,char **argv)
{
    printf("Enter the fixed points");
    scanf("%d %d",&x,&y);
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(0,0);
    glutCreateWindow("triangle");
    myInit();
    glutDisplayFunc(display);
    glutCreateMenu(rotatemenu);
    glutAddMenuEntry("ORIGIN",1);
    glutAddMenuEntry("FIXED POINT",2);
    glutAddMenuEntry("STOP",3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
}
