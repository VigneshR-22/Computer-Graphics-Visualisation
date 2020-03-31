#include<windows.h>
#include<stdio.h>
#include<GL/glut.h>
#include<stdlib.h>
GLfloat vertex[4][3]={{0.0,0.0,0.0},{250.0,500.0,100.0},{500.0,250.0,250.0},{250.0,100.0,250.0}};
GLfloat colors[4][3]={{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,1.0,1.0},{0.0,0.0,0.0}};
int n;
void triangle(GLfloat *a,GLfloat *b,GLfloat *c)
{
    glVertex3fv(a);
    glVertex3fv(b);
    glVertex3fv(c);
}
void tetra(GLfloat *a,GLfloat *b,GLfloat *c,GLfloat *d)
{
    glColor3fv(colors[0]);
    triangle(a,b,c);
    glColor3fv(colors[1]);
    triangle(a,c,d);
    glColor3fv(colors[2]);
    triangle(a,d,b);
    glColor3fv(colors[3]);
    triangle(b,d,c);
}
void divide_tetra(GLfloat *a,GLfloat *b,GLfloat *c,GLfloat *d,int k)
{
    GLfloat mid[6][3];
    int j;
    if(k>0)
    {
        for(j=0;j<3;j++) mid[0][j]=(a[j]+b[j])/2;
        for(j=0;j<3;j++) mid[1][j]=(a[j]+c[j])/2;
        for(j=0;j<3;j++) mid[2][j]=(a[j]+d[j])/2;
        for(j=0;j<3;j++) mid[3][j]=(b[j]+c[j])/2;
        for(j=0;j<3;j++) mid[4][j]=(c[j]+d[j])/2;
        for(j=0;j<3;j++) mid[5][j]=(b[j]+d[j])/2;
        divide_tetra(a,mid[0],mid[1],mid[2],k-1);
        divide_tetra(mid[0],b,mid[3],mid[5],k-1);
        divide_tetra(mid[1],mid[3],c,mid[4],k-1);
        divide_tetra(mid[2],mid[4],d,mid[5],k-1);
    }
    else tetra(a,b,c,d);
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    divide_tetra(vertex[0],vertex[1],vertex[2],vertex[3],n);
    glEnd();
    glutPostRedisplay();
    glutSwapBuffers();
}
void init()
{
    glClearColor(1.0,1.0,1.0,0.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0,500.0,0.0,500.0,-500.0,500.0);
    glMatrixMode(GL_MODELVIEW);
}
int main(int argc,char **argv)
{
    printf("Enter the no of subdivisions:");
    scanf("%d",&n);
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Tetrahedron");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
}
