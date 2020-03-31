#include <GL/glut.h>
#include<math.h>

#include "BmpLoader.h"

#define PI 3.14

unsigned int tex_processes,tex_on,tex_off,tex_index,tex_graph,tex_thermometer,tex_speed,tex_speeds;

GLuint loadTexture(const char * filename)
{
    BmpLoader img(filename);
    GLuint texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img.width,img.height,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,img.textureData);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,img.width,img.height,GL_BGR_EXT,GL_UNSIGNED_BYTE,img.textureData);
    return texture;
}

GLuint loadTextureAlpha(const char * filename)
{
    GLuint texture;
    glGenTextures(1,&texture);
    BmpLoader img(filename);

    unsigned char * textureData_alpha;
    textureData_alpha = new unsigned char[img.height*img.width*4];

    for(int i = 0 ; i < img.height*img.width ; i++)
    {
        textureData_alpha[i*4 + 0] = img.textureData[i*3 +0];
        textureData_alpha[i*4 + 1] = img.textureData[i*3 +1];
        textureData_alpha[i*4 + 2] = img.textureData[i*3 +2];
        if(textureData_alpha[i*4 + 0] == 0 && textureData_alpha[i*4 + 2]==0
           && textureData_alpha[i*4 + 1] ==255)
            textureData_alpha[i*4 + 3] = 0;
        else
            textureData_alpha[i*4 + 3] = 255;
    }


    glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img.width,img.height,0,GL_BGRA_EXT,GL_UNSIGNED_BYTE,textureData_alpha);
    return texture;
}

float* normalize(float*v)
{
    float length = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

    for(int i = 0 ; i < 3 ; i++)
    {
        v[i] = v[i]/ length;
    }

    return v;
}

float *cross_product(float*a,float*b)
{
    float result[] = {a[1]*b[2]-a[2]*b[1], - ( a[0]*b[2]-a[2]*b[0]),
                        a[0]*b[1]-a[1]*b[0]};

    normalize(result);

    return result;
}

float *calculate_normal(float *a,float *b,float*c)
{
    float x[] = {b[0]-a[0],b[1]-a[1],b[2]-a[2]};
    float y[] = {c[0]-a[0],c[1]-a[1],c[2]-a[2]};

    float *result = cross_product(x,y);

    return result;
}


void fly_wheel(float radius,float height)
{
    float x,y;

    glPushMatrix();

    glPushMatrix();
    glTranslatef(0.0,0.0,height/2);

    glNormal3f(0.0,0.0,1.0);

    glBegin(GL_POLYGON);

    for(float theta = 0; theta<=360 ; theta+=20)
    {
        x= radius * cos(theta*(PI/180.0));
        y= radius * sin(theta*(PI/180.0));

        glVertex3f(x,y,0);
    }

    glEnd();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0,0.0,-height/2);

    glNormal3f(0.0,0.0,-1.0);

    glBegin(GL_POLYGON);

    for(float theta = 360; theta>=0 ; theta-=20)
    {
        x= radius * cos(theta*(PI/180.0));
        y= radius * sin(theta*(PI/180.0));

        glVertex3f(x,y,0);
    }

    glEnd();

    glPopMatrix();

    float vPrev[2];
    float v[2];


    glBegin(GL_QUADS);


    for(float theta = 0 ; theta <= 360 ; theta+=20)
    {
        vPrev[0] = v[0];
        vPrev[1] = v[1];

        v[0]= radius * cos(theta*(PI/180.0));
        v[1]= radius * sin(theta*(PI/180.0));

        if(theta==0)
            continue;

        float a[] = {vPrev[0],vPrev[1],-height/2};
        float b[] = {vPrev[0],vPrev[1],height/2};
        float c[] = {v[0],v[1],height/2};



        glNormal3fv(calculate_normal(c,b,a));

        glVertex3f(vPrev[0],vPrev[1],height/2);
        glVertex3f(vPrev[0],vPrev[1],-height/2);
        glVertex3f(v[0],v[1],-height/2);
        glVertex3f(v[0],v[1],height/2);
    }

    glEnd();

    glPopMatrix();
}

void stick(float length,float width,float depth)
{
    glPushMatrix();

    glBegin(GL_QUADS);

    //front
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(width/2,-length/2,depth/2);
    glVertex3f(-width/2,-length/2,depth/2);
    glVertex3f(-width/2,length/2,depth/2);
    glVertex3f(width/2,length/2,depth/2);

    //back
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(-width/2,-length/2,-depth/2);
    glVertex3f(width/2,-length/2,-depth/2);
    glVertex3f(width/2,length/2,-depth/2);
    glVertex3f(-width/2,length/2,-depth/2);

    //left
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(-width/2,-length/2,-depth/2);
    glVertex3f(-width/2,-length/2,depth/2);
    glVertex3f(-width/2,length/2,depth/2);
    glVertex3f(-width/2,length/2,-depth/2);

    //right
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(width/2,-length/2,-depth/2);
    glVertex3f(width/2,-length/2,depth/2);
    glVertex3f(width/2,length/2,depth/2);
    glVertex3f(width/2,length/2,-depth/2);

    glEnd();

    glPopMatrix();
}

void lid(float length,float breadth)
{
    glPushMatrix();

    glBegin(GL_QUADS);

    //bottom
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(-length/2,0.0,breadth/2);
    glVertex3f(length/2,0.0,breadth/2);
    glVertex3f(length/2,0.0,-breadth/2);
    glVertex3f(-length/2,0.0,-breadth/2);

    //top
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(length/2,1.0,breadth/2);
    glVertex3f(-length/2,1.0,breadth/2);
    glVertex3f(-length/2,1.0,-breadth/2);
    glVertex3f(length/2,1.0,-breadth/2);

    //left
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(-length/2,0.0,-breadth/2);
    glVertex3f(-length/2,0.0,breadth/2);
    glVertex3f(-length/2,1.0,breadth/2);
    glVertex3f(-length/2,1.0,-breadth/2);

    //right
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(length/2,0.0,breadth/2);
    glVertex3f(length/2,0.0,-breadth/2);
    glVertex3f(length/2,1.0,-breadth/2);
    glVertex3f(length/2,1.0,breadth/2);

    //front
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(-length/2,0.0,breadth/2);
    glVertex3f(length/2,0.0,breadth/2);
    glVertex3f(length/2,1.0,breadth/2);
    glVertex3f(-length/2,1.0,breadth/2);

    //back
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(length/2,0.0,-breadth/2);
    glVertex3f(-length/2,0.0,-breadth/2);
    glVertex3f(-length/2,1.0,-breadth/2);
    glVertex3f(length/2,1.0,-breadth/2);

    glEnd();

    glPopMatrix();
}

void chamber(float length,float breadth,float height)
{

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();

    glBegin(GL_QUADS);

    glColor4f(1.0,1.0,1.0,0.3);

    /*//bottom
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(-length/2,-height/2,breadth/2);
    glVertex3f(length/2,-height/2,breadth/2);
    glVertex3f(length/2,-height/2,-breadth/2);
    glVertex3f(-length/2,-height/2,-breadth/2);*/

    //front
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(-length/2,-height/2,breadth/2);
    glVertex3f(length/2,-height/2,breadth/2);
    glVertex3f(length/2,height/2,breadth/2);
    glVertex3f(-length/2,height/2,breadth/2);

    //back
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(length/2,-height/2,-breadth/2);
    glVertex3f(-length/2,-height/2,-breadth/2);
    glVertex3f(-length/2,height/2,-breadth/2);
    glVertex3f(length/2,height/2,-breadth/2);

    //right
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(length/2,-height/2,-breadth/2);
    glVertex3f(length/2,-height/2,breadth/2);
    glVertex3f(length/2,height/2,breadth/2);
    glVertex3f(length/2,height/2,-breadth/2);

    //left
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(-length/2,-height/2,breadth/2);
    glVertex3f(-length/2,-height/2,-breadth/2);
    glVertex3f(-length/2,height/2,-breadth/2);
    glVertex3f(-length/2,height/2,breadth/2);

    glEnd();

    glBegin(GL_QUADS);

    /*//bottom
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(-(length+1)/2,-(height+0.5)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,-(height+0.5)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,-(height+0.5)/2,-(breadth+1)/2);
    glVertex3f(-(length+1)/2,-(height+0.5)/2,-(breadth+1)/2);*/

    //front
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(-(length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,(height)/2,(breadth+1)/2);
    glVertex3f(-(length+1)/2,(height)/2,(breadth+1)/2);

    //back
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f((length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f(-(length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f(-(length+1)/2,(height)/2,-(breadth+1)/2);
    glVertex3f((length+1)/2,(height)/2,-(breadth+1)/2);

    //right
    glNormal3f(1.0,0.0,0.0);
    glVertex3f((length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f((length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,(height)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,(height)/2,-(breadth+1)/2);

    //left
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(-(length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f(-(length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f(-(length+1)/2,(height)/2,-(breadth+1)/2);
    glVertex3f(-(length+1)/2,(height)/2,(breadth+1)/2);

    //coverings
    //front
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(-(length+1)/2,(height)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,(height)/2,(breadth+1)/2);
    glVertex3f((length)/2,(height)/2,(breadth)/2);
    glVertex3f(-(length)/2,(height)/2,(breadth)/2);

    //back
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(-(length+1)/2,(height)/2,-(breadth+1)/2);
    glVertex3f((length+1)/2,(height)/2,-(breadth+1)/2);
    glVertex3f((length)/2,(height)/2,-(breadth)/2);
    glVertex3f(-(length)/2,(height)/2,-(breadth)/2);

    //right
    glNormal3f(0.0,1.0,0.0);
    glVertex3f((length+1)/2,(height)/2,(breadth+1)/2);
    glVertex3f((length+1)/2,(height)/2,-(breadth+1)/2);
    glVertex3f((length)/2,(height)/2,-(breadth)/2);
    glVertex3f((length)/2,(height)/2,(breadth)/2);

    //left
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(-(length+1)/2,(height)/2,-(breadth+1)/2);
    glVertex3f(-(length+1)/2,(height)/2,(breadth+1)/2);
    glVertex3f(-(length)/2,(height)/2,(breadth)/2);
    glVertex3f(-(length)/2,(height)/2,-(breadth)/2);

    //front
    glNormal3f(0.0,1.0,0.0);
    glVertex3f((length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f(-(length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f(-(length)/2,-(height)/2,(breadth)/2);
    glVertex3f((length)/2,-(height)/2,(breadth)/2);

    //back
    glNormal3f(0.0,1.0,0.0);
    glVertex3f((length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f(-(length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f(-(length)/2,-(height)/2,-(breadth)/2);
    glVertex3f((length)/2,-(height)/2,-(breadth)/2);

    //right
    glNormal3f(0.0,1.0,0.0);
    glVertex3f((length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f((length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f((length)/2,-(height)/2,(breadth)/2);
    glVertex3f((length)/2,-(height)/2,-(breadth)/2);

    //left
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(-(length+1)/2,-(height)/2,(breadth+1)/2);
    glVertex3f(-(length+1)/2,-(height)/2,-(breadth+1)/2);
    glVertex3f(-(length)/2,-(height)/2,-(breadth)/2);
    glVertex3f(-(length)/2,-(height)/2,(breadth)/2);

    glEnd();

    glPopMatrix();

    glDisable(GL_BLEND);


}

void reservoirs(float length,float total_breadth,float height)
{
    glPushMatrix();

    glTranslatef(-total_breadth/2,0.0,0.0);

    glBegin(GL_QUADS);

    glColor3f(1.0,0.07,0.0);

    //top
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(0.0,height/2,length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(0.0,height/2,-length/2);

    //bottom
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(0.0,-height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(0.0,-height/2,length/2);

    //front
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(0.0,-height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(0.0,height/2,length/2);

    //back
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,height/2,-length/2);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);
    glVertex3f(0.0,-height/2,-length/2);

    //right
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);

    //left
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(0.0,-height/2,-length/2);
    glVertex3f(0.0,-height/2,length/2);
    glVertex3f(0.0,height/2,length/2);
    glVertex3f(0.0,height/2,-length/2);


    glEnd();

    glTranslatef(total_breadth/3,0.0,0.0);

    glBegin(GL_QUADS);

    glColor3f(1.0,1.0,1.0);

    //top
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(0.0,height/2,length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(0.0,height/2,-length/2);

    //bottom
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(0.0,-height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(0.0,-height/2,length/2);

    //front
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(0.0,-height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(0.0,height/2,length/2);

    //back
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,height/2,-length/2);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);
    glVertex3f(0.0,-height/2,-length/2);

    //right
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);

    //left
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(0.0,-height/2,-length/2);
    glVertex3f(0.0,-height/2,length/2);
    glVertex3f(0.0,height/2,length/2);
    glVertex3f(0.0,height/2,-length/2);

    glEnd();

    glTranslatef(total_breadth/3,0.0,0.0);

    glBegin(GL_QUADS);

    glColor3f(0.0,0.4,1.0);

    //top
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(0.0,height/2,length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(0.0,height/2,-length/2);

    //bottom
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(0.0,-height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(0.0,-height/2,length/2);

    //front
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(0.0,-height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(0.0,height/2,length/2);

    //back
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(0.0,height/2,-length/2);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);
    glVertex3f(0.0,-height/2,-length/2);

    //right
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(total_breadth/3,height/2,-length/2);
    glVertex3f(total_breadth/3,height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,length/2);
    glVertex3f(total_breadth/3,-height/2,-length/2);

    //left
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(0.0,-height/2,-length/2);
    glVertex3f(0.0,-height/2,length/2);
    glVertex3f(0.0,height/2,length/2);
    glVertex3f(0.0,height/2,-length/2);

    glEnd();

    glPopMatrix();
}

void process(float x,float y, int process)
{
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    if(tex_processes == 0 )
        tex_processes = loadTexture("processes.bmp");

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D,tex_processes);

    glTranslatef(x,y,0);
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glTexCoord2d(0.0,(1.0/4.0)*4-(1.0/4.0)*process);  glVertex2d(0,0);
    glTexCoord2d(0.0,(1.0/4.0)*3-(1.0/4.0)*process);  glVertex2d(0,100);
    glTexCoord2d(1.0,(1.0/4.0)*3-(1.0/4.0)*process);  glVertex2d(1000,100);
    glTexCoord2d(1.0,(1.0/4.0)*4-(1.0/4.0)*process);  glVertex2d(1000,0);
    glEnd();

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
}

void indicator100x100(float x,float y, int state)
{
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    if(tex_on == 0 )
        tex_on = loadTexture("on.bmp");

    if(tex_off == 0 )
        tex_off = loadTexture("off.bmp");

    glPushMatrix();

    if(state==1)
        glBindTexture(GL_TEXTURE_2D,tex_on);
    else
        glBindTexture(GL_TEXTURE_2D,tex_off);

    glTranslatef(x,y,0);
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glTexCoord2d(0.0,1.0);  glVertex2d(0,0);
    glTexCoord2d(0.0,0.0);  glVertex2d(0,100);
    glTexCoord2d(1.0,0.0);  glVertex2d(100,100);
    glTexCoord2d(1.0,1.0);  glVertex2d(100,0);
    glEnd();

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
}

float gas(float length,float breadth,float height,float red_component)
{
    glPushMatrix();

    glEnable(GL_BLEND);

    glBegin(GL_QUADS);

    glColor4f(red_component,1.0-red_component,0.0,0.6);

    //top
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(breadth/2,height,length/2);
    glVertex3f(-breadth/2,height,length/2);
    glVertex3f(-breadth/2,height,-length/2);
    glVertex3f(breadth/2,height,-length/2);

    //bottom
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(-breadth/2,0.0,length/2);
    glVertex3f(breadth/2,0.0,length/2);
    glVertex3f(breadth/2,0.0,-length/2);
    glVertex3f(-breadth/2,0.0,-length/2);

    //front
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(breadth/2,height,length/2);
    glVertex3f(-breadth/2,height,length/2);
    glVertex3f(-breadth/2,0.0,length/2);
    glVertex3f(breadth/2,0.0,length/2);

    //back
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(-breadth/2,height,-length/2);
    glVertex3f(breadth/2,height,-length/2);
    glVertex3f(breadth/2,0.0,-length/2);
    glVertex3f(-breadth/2,0.0,-length/2);

    //right
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(breadth/2,height,-length/2);
    glVertex3f(breadth/2,height,length/2);
    glVertex3f(breadth/2,0.0,length/2);
    glVertex3f(breadth/2,0.0,-length/2);

    //left
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(-breadth/2,height,length/2);
    glVertex3f(-breadth/2,height,-length/2);
    glVertex3f(-breadth/2,0.0,-length/2);
    glVertex3f(-breadth/2,0.0,length/2);

    glEnd();

    glDisable(GL_BLEND);

    glPopMatrix();
}

void index(float x,float y)
{
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    if(tex_index == 0 )
        tex_index = loadTexture("index.bmp");

    glBindTexture(GL_TEXTURE_2D,tex_index);

    glPushMatrix();

    glTranslatef(x,y,0);
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glTexCoord2d(0.0,1.0);  glVertex2d(0,0);
    glTexCoord2d(0.0,0.0);  glVertex2d(0,300);
    glTexCoord2d(1.0,0.0);  glVertex2d(800,300);
    glTexCoord2d(1.0,1.0);  glVertex2d(800,0);
    glEnd();

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
}

void graph(float x,float y)
{
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    if(tex_graph == 0 )
        tex_graph = loadTexture("graph.bmp");

    glBindTexture(GL_TEXTURE_2D,tex_graph);

    glPushMatrix();

    glTranslatef(x,y,0);
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glTexCoord2d(0.0,1.0);  glVertex2d(0,0);
    glTexCoord2d(0.0,0.0);  glVertex2d(0,500);
    glTexCoord2d(1.0,0.0);  glVertex2d(500,500);
    glTexCoord2d(1.0,1.0);  glVertex2d(500,0);
    glEnd();

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
}

void thermometer140x600(float x,float y,float height,float red_component)
{
    glPushMatrix();
    glTranslated(x,y,0);
    glBegin(GL_QUADS);
    glColor3f(red_component,1.0-red_component,0.0);
    glVertex2d(0,600-height);
    glVertex2d(0,600);
    glVertex2d(140,600);
    glVertex2d(140,600-height);
    glEnd();
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);

    if(tex_thermometer == 0 )
        tex_thermometer = loadTextureAlpha("thermometer.bmp");

    glBindTexture(GL_TEXTURE_2D,tex_thermometer);

    glPushMatrix();

    glTranslatef(x,y,0);
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glTexCoord2d(0.0,1.0);  glVertex2d(0,0);
    glTexCoord2d(0.0,0.0);  glVertex2d(0,600);
    glTexCoord2d(1.0,0.0);  glVertex2d(140,600);
    glTexCoord2d(1.0,1.0);  glVertex2d(140,0);
    glEnd();

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void speed_indicator(float x,float y,float speed)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);

    if(tex_speeds == 0 )
        tex_speeds = loadTexture("speeds.bmp");
    if(tex_speed == 0 )
        tex_speed = loadTexture("speed.bmp");

    glBindTexture(GL_TEXTURE_2D,tex_speed);

    glPushMatrix();

    glTranslatef(x,y,0);
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glTexCoord2d(0.0,1.0);  glVertex2d(0,0);
    glTexCoord2d(0.0,0.0);  glVertex2d(0,250);
    glTexCoord2d(1.0,0.0);  glVertex2d(250,250);
    glTexCoord2d(1.0,1.0);  glVertex2d(250,0);
    glEnd();

   glBindTexture(GL_TEXTURE_2D,tex_speeds);

    glTranslatef(255,35,0);
    glBegin(GL_QUADS);
    glColor3f(1.0,1.0,1.0);
    glTexCoord2d(speed/1.1,1.0);                        glVertex2d(0,0);
    glTexCoord2d(speed/1.1,1.0-(1.0/11));              glVertex2d(0,100);
    glTexCoord2d(speed/1.1+(1.0/11),1.0-(1.0/11));    glVertex2d(100,100);
    glTexCoord2d(speed/1.1+(1.0/11),1.0);              glVertex2d(100,0);
    glEnd();

    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

