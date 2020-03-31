#include <GL/glut.h>
#include <math.h>

#include "models.h"
#include "draw.h"

#define C_F_DR (3.14/180.0)
#define C_F_RD (180.0/3.14);

float cycle_progress = 0.0;
float coordinate=0.0;
float red_component;
float speed = 0.2;

float draw_engine(int state)
{
    float fly_wheel_angle = (cycle_progress/100.0)*360.0-90;
    float stick_angle = asin((4.5*cos(fly_wheel_angle*C_F_DR))/15.0)*C_F_RD;
    float stick_2_positionY = 15.0*cos(stick_angle*C_F_DR) - 4.5*sin(fly_wheel_angle*C_F_DR);


    glColor3f(1.0,1.0,0.0);

    glPushMatrix();
    glRotatef(fly_wheel_angle,0.0,0.0,1.0);
    fly_wheel(5,2);
    glPopMatrix();

    glPushMatrix();

    glTranslatef(0.0,0.0,1.30);

    glPushMatrix();

    glColor3f(1.0,0.0,0.0);

    glTranslatef(cos(fly_wheel_angle*C_F_DR)*4.5,(sin(fly_wheel_angle*C_F_DR)*4.5),0.0);
    glRotatef(-stick_angle,0.0,0.0,1.0);
    fly_wheel(0.5,0.5);
    glTranslatef(0.0,-7.5,0.0);

    stick(15.0,1,0.5);
    glTranslatef(0.0,-7.5,0.0);
    fly_wheel(0.5,0.5);

    glPopMatrix();

    glPushMatrix();

    glTranslatef(0.0,-stick_2_positionY-2.5,0.0);

    glColor3f(1.0,0.4,0.0);
    stick(5.0,2,1.0);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0,-stick_2_positionY-5.0,0.0);
    glColor3f(0.0,1.0,0.0);
    lid(9.9,7.9);
    glPopMatrix();

    glPushMatrix();

    glTranslatef(0.0,-19.5,0.0);
    {
        glPushMatrix();
        glTranslatef(0.0,-7.5,0.0);

        if(cycle_progress<=50)
        {
            if(cycle_progress<25)
                red_component = 1.0;
            else
                red_component = 1.0 - ((cycle_progress-25.0)/25.0);
        }
        else
        {
            if(cycle_progress<75)
                red_component = 0.0;
            else
                red_component = (cycle_progress-75.0)/25.0;
        }

        gas(7.9,9.9,19.5+7.5-stick_2_positionY-5.1,red_component);
        glPopMatrix();
    }

    chamber(10,8,15);


    glPopMatrix();

    glPushMatrix();

    switch(state)
    {
    case S_ISO_EXP:
        if(coordinate<11.0)
            coordinate+=1.0;
        break;
    case S_ISO_CMP:
        if(coordinate>-11.0)
            coordinate-=1.0;
        break;
    case S_ADI_CMP:
    case S_ADI_EXP:
        if(coordinate<0.0)
            coordinate+=1.0;
        else if(coordinate>0.0)
            coordinate-=1.0;
        break;
    }


    glTranslatef(coordinate,-27.0-2.5,0.0);
    reservoirs(9,32,5);

    glPopMatrix();

    glPopMatrix();

    cycle_progress+=speed*(100.0/60.0);
    if(cycle_progress>100.0)
        cycle_progress = 0.0;

    return cycle_progress;
}

void HUD(int state)
{
    int s[4];

    switch(state)
    {
    case S_ISO_EXP:
        s[0] = 1;
        s[1] = 0;
        s[2] = 0;
        s[3] = 0;
        break;
    case S_ADI_EXP:
        s[0] = 0;
        s[1] = 1;
        s[2] = 0;
        s[3] = 0;
        break;
    case S_ISO_CMP:
        s[0] = 0;
        s[1] = 0;
        s[2] = 1;
        s[3] = 0;
        break;
    case S_ADI_CMP:
        s[0] = 0;
        s[1] = 0;
        s[2] = 0;
        s[3] = 1;
        break;
    }


    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0,1600.0,900.0,0.0);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);


    glPushMatrix();
    glScalef(0.6,0.6,0.6);

    indicator100x100(50,100,s[0]);
    process(150,100,0);
    indicator100x100(50,200,s[1]);
    process(150,200,1);
    indicator100x100(50,300,s[2]);
    process(150,300,2);
    indicator100x100(50,400,s[3]);
    process(150,400,3);

    index(270,600);

    glPopMatrix();

    {
        float height = 200+ red_component*250;

        glPushMatrix();
        glScalef(0.8,0.8,0.8);
        thermometer140x600(40,470,height,red_component);
        glPopMatrix();
    }

    graph(1000,10);
    speed_indicator(1200,580,speed);

    glEnable(GL_LIGHTING);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
