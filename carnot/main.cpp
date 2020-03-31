/* Code by Ratul Thakur
    email : edratulthakur0@gmail.com
    youtube channel : youtube.com/thepentamollisproject

    Feel free to use this code in any way you want
    :-)

    Dated : 18th Feb 2018
    */

#include<windows.h>
#include <GL/glut.h>
#include <iostream>

#include "draw.h"

extern float speed;

void display();
void reshape(int,int);
void timer(int);
void keyboard(unsigned char,int,int);

void init()
{

    glutFullScreen();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);

    float pos[] = {2.0,2.0,6.0};
    float amb[] = {0.4,0.4,0.4};

    glLightfv(GL_LIGHT0,GL_POSITION,pos);
    //glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
}

int main(int argc,char**argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(16*60,9*60);
    glutCreateWindow("Carnot's Engine - Simulation");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0,timer,0);
    glutKeyboardFunc(keyboard);
    init();

    glutMainLoop();

    return 0;
}

float theta=30.0;
int state=1;
int theta_state=1;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0,8.2,-45.0);
    glRotatef(30.0,1.0,0.0,0.0);
    glRotatef(theta,0.0,1.0,0.0);

    HUD(state);

    float progress = draw_engine(state);
    if(progress>= 0 && progress < 25)
        state = S_ISO_EXP;
    else if(progress>=25 && progress < 50)
        state = S_ADI_EXP;
    else if(progress>=50 && progress <75)
        state = S_ISO_CMP;
    else
        state = S_ADI_CMP;

    theta+=0.2*theta_state;
    if(theta>=65)
        theta_state=-1;
    else if(theta<=-65)
        theta_state=1;

    glutSwapBuffers();
}

void reshape(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,16.0/9.0,1,100);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int)
{
    glutPostRedisplay();
    glutTimerFunc(1000.0/60.0,timer,0);
}

void keyboard(unsigned char key,int x,int y)
{
    switch(key)
    {
    case 'q':
    case 'Q':
        if(speed>=0.1)
            speed-=0.1;
        break;
    case 'e':
    case 'E':
        if(speed<1)
            speed+=0.1;
        break;
    case 'w':
    case 'W':
        exit(0);
        break;
    }

    std::cout<<speed<<std::endl;
}




