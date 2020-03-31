#include<windows.h>
#include<GL/glut.h>
#include<stdio.h>
#include<stdlib.h>
GLdouble radius=1;
static GLfloat theta[]={0.0,0.0,0.0};
static GLint axis=2;
static GLdouble viewer[]={0.0,0.0,5.0};
static GLfloat translate[]={0.0,0.0,0.0};
static GLfloat width=1.0;
int rFlag;
void display(void)
{


    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(translate[0],translate[1],translate[2]);
    gluLookAt(viewer[0],viewer[1],viewer[2],0.0,0.0,0.0,0.0,1.0,0.0);
    glRotatef(theta[0],1.0,0.0,0.0);
    glRotatef(theta[1],0.0,1.0,0.0);
    glRotatef(theta[2],0.0,0.0,1.0);
    if(rFlag==1)
    {
        axis=0;
        theta[axis]+=0.1;
        if(theta[axis]>360.0)
            theta[axis]-=360.0;
        glutPostRedisplay();
    }
    if(rFlag==2)
    {
        axis=1;
        theta[axis]+=0.1;
        if(theta[axis]>360.0)
            theta[axis]-=360.0;
        glutPostRedisplay();
    }
    if(rFlag==3)
    {
        axis=2;
        theta[axis]+=0.1;
        if(theta[axis]>360.0)
            theta[axis]-=360.0;
        glutPostRedisplay();
    }
    if(rFlag==4)
        axis=10;


    glLineWidth(width);
    glutSolidSphere(radius,20,20);




    glFlush();
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
void rotateMenu(int option)
{
    if(option==1)
        rFlag=1;
    if(option==2)
        rFlag=2;
    if(option==3)
        rFlag=3;
    if(option==4)
        rFlag=4;
}
void keys(unsigned char key, int x, int y)
{
    if(key=='x') viewer[0]-=1.0;
    if(key=='X') viewer[0]+=1.0;
    if(key=='y') viewer[1]-=1.0;
    if(key=='Y') viewer[1]+=1.0;
    if(key=='z') viewer[2]-=1.0;
    if(key=='Z') viewer[2]+=1.0;
    if(key=='b') glColor3f(0.0,0.0,1.0);
    if(key=='c') glColor3f(0.5, 1.0, 1.0);
    if(key=='p') glColor4f(1.0, 0.0, 1.0, 0.0);
    if(key=='o') glColor3f(1.0, 0.5, 0.0);
    if(key=='g') glColor3f(0.0,1.0,0.0);
    if(key=='v') glColor3f(1.0,1.0,0.0);
    if(key=='W') width+=0.1;
    if(key=='w') width-=0.1;
    glutPostRedisplay();

}
void spincube()
{
    theta[axis]+=0.1;
    if(theta[axis]>360.0)
        theta[axis]-=360.0;
    glutPostRedisplay();
}
void reshapeFunc(int x, int y)
{
    if (y == 0 || x == 0) return;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0

    gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,20.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  //Use the whole window for rendering
}
void SpecialInput(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_UP:
        translate[1]+=0.1;
        glTranslatef(translate[0],translate[1],translate[2]);
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        translate[1]-=0.1;
        glTranslatef(translate[0],translate[1],translate[2]);
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        translate[0]-=0.1;
        glTranslatef(translate[0],translate[1],translate[2]);
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        translate[0]+=0.1;
        glTranslatef(translate[0],translate[1],translate[2]);
        glutPostRedisplay();
        break;
    }
}


int main (int argc, char **argv)
{

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH);

    glutInitWindowSize(1024,768);
    glutInitWindowPosition(100,50);

    glutCreateWindow("Rotating Sphere Animation");
     glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    myInit();
    glutReshapeFunc(reshapeFunc);
    glutDisplayFunc(display);
    glutKeyboardFunc(keys);
    glutSpecialFunc(SpecialInput);
    glutIdleFunc(spincube);
    glutCreateMenu(rotateMenu);
    glutAddMenuEntry("Rotate around X-axis",1);
    glutAddMenuEntry("Rotate around Y-axis",2);
    glutAddMenuEntry("Rotate around Z-axis",3);
    glutAddMenuEntry("Stop Rotation",4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glClearColor(1.0,1.0,1.0,0.0);
    glColor3f(1.0,0.0,0.0);
    printf("\nA Mini Project by:\n");
    printf("\nPAVAN KUMAR S and VIGNESH R\n");
    glutMainLoop();
    return 0;
}
