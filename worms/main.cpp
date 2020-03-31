#include<windows.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <GL/glut.h>

#ifdef WIN32
#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) (srand((x)))
#endif

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* operational constants */
#define RADIAN .0174532
#define CIRCLE_POINTS 25
#define SIDETOLERANCE .01
#define INITH 500
#define INITW 500

/* worm options */
#define SEGMENTS 20
#define SEG_RADIUS 0.01
#define STEPSIZE 0.01
#define MAXTURN (20 * RADIAN)        /* in radians */
#define MAXWORMS 400
#define INITWORMS 40
#define MARKTICKS 100

typedef struct worm_s {
  float dir;                   /* direction in radians */
  float segx[SEGMENTS];        /* location of segments. */
  float segy[SEGMENTS];
  GLfloat *color;              /* pointer to the RGB color of the worm */
  int head;                    /* which elt of seg[xy] is currently head */
                               /* the tail is always (head+1 % SEGMENTS) */
} worm_t;

/* colors available for worms... this is a huge mess because I
   originally brought these colors in from rgb.txt as integers,
   but they have to be normalized into floats. And C is stupid
   and truncates them unless I add the annoying .0's
 */

const GLfloat colors[][3] = {
  { 255.0/255.0,   0.0/255.0,   0.0/255.0},
  { 238.0/255.0,   0.0/255.0,   0.0/255.0},
  { 205.0/255.0,   0.0/255.0,   0.0/255.0},
  {   0.0/255.0, 255.0/255.0,   0.0/255.0},
  {   0.0/255.0, 238.0/255.0,   0.0/255.0},
  {   0.0/255.0, 205.0/255.0,   0.0/255.0},
  {   0.0/255.0,   0.0/255.0, 255.0/255.0},
  {   0.0/255.0,   0.0/255.0, 238.0/255.0},
  {   0.0/255.0,   0.0/255.0, 205.0/255.0},
  { 255.0/255.0, 255.0/255.0,   0.0/255.0},
  { 238.0/255.0, 238.0/255.0,   0.0/255.0},
  { 205.0/255.0, 205.0/255.0,   0.0/255.0},
  {   0.0/255.0, 255.0/255.0, 255.0/255.0},
  {   0.0/255.0, 238.0/255.0, 238.0/255.0},
  {   0.0/255.0, 205.0/255.0, 205.0/255.0},
  { 255.0/255.0,   0.0/255.0, 255.0/255.0},
  { 238.0/255.0,   0.0/255.0, 238.0/255.0},
  { 205.0/255.0,   0.0/255.0, 205.0/255.0},
};

#define COLORS 18

/* define's for the menu item numbers */
#define MENU_NULL          0
#define MENU_FILLED        1
#define MENU_UNFILLED      2
#define MENU_QUIT          3

/* flag to determine how to draw worms; set by popup menu -- starts out
   filled in
 */
int filled = 1;

/* the global worm array */
worm_t worms[MAXWORMS];
int curworms = 0;

/* global window extent variables */
GLfloat gleft = -1.0, gright = 1.0, gtop = 1.0, gbottom = -1.0;
GLint wsize, hsize;

/* globals for marking */
float markx, marky;
int marktime;

/* prototypes */
void mydisplay(void);

void drawCircle(float x0, float y0, float radius)
{
  int i;
  float angle;

  /* a table of offsets for a circle (used in drawCircle) */
  static float circlex[CIRCLE_POINTS];
  static float circley[CIRCLE_POINTS];
  static int   inited = 0;

  if (! inited) {
    for (i = 0; i < CIRCLE_POINTS; i++) {
      angle = 2.0 * M_PI * i / CIRCLE_POINTS;
      circlex[i] = cos(angle);
      circley[i] = sin(angle);
    }
    inited++;
  };

  if (filled)
    glBegin(GL_POLYGON);
  else
    glBegin(GL_LINE_LOOP);
  for(i = 0; i < CIRCLE_POINTS; i++)
    glVertex2f((radius * circlex[i]) + x0, (radius * circley[i]) + y0);
  glEnd();

  return;
}

void drawWorm(worm_t *theworm)
{
  int i;

  glColor3fv(theworm->color);
  for (i = 0; i < SEGMENTS; i++)
    drawCircle(theworm->segx[i], theworm->segy[i], SEG_RADIUS);

  return;
}

void myinit(void)
{
  int i, j, thecolor;
  float thedir;

  srand48(time(NULL));

  curworms = INITWORMS;

  for (j = 0; j < curworms; j++) {
    /* divide the circle up into a number of pieces, and send one worm
       each direction.
     */
    worms[j].dir = ((2.0 * M_PI) / curworms) * j;
    thedir = worms[j].dir;

    worms[j].segx[0] = 0.0;
    worms[j].segy[0] = 0.0;

    for (i = 1; i < SEGMENTS; i++) {
      worms[j].segx[i] = worms[j].segx[i-1] + (STEPSIZE * cos(thedir));
      worms[j].segy[i] = worms[j].segx[i-1] + (STEPSIZE * sin(thedir));
    };
    worms[j].head = (SEGMENTS - 1);

    /* make this worm one of the predefined colors */
    thecolor = (int) COLORS * drand48();
    worms[j].color = (GLfloat *) colors[thecolor];
  };

  /* now that they are all set, draw them as though they have just been
     uncovered
   */
  mydisplay();
}



/* this routine is called after the coordinates are changed to make sure
   worms outside the window come back into view right away. (This behavior
   is arbitrary, but they are my worms, and they'll do what I please!)
 */

void warpWorms(void)
{
  register int j, head;

  for (j = 0; j < curworms; j++) {
    head = worms[j].head;

    if (worms[j].segx[head] < gleft)
      worms[j].segx[head] = gleft;
    if (worms[j].segx[head] > gright)
      worms[j].segx[head] = gright;
    if (worms[j].segx[head] > gtop)
      worms[j].segx[head] = gtop;
    if (worms[j].segx[head] < gbottom)
      worms[j].segx[head] = gbottom;
  }
}

/* a bunch of extra hoopla goes on here to change the Global coordinate
   space at teh same rate that the window itself changes. This give the
   worms more space to play in when the window gets bigger, and vice versa.
   The alternative would be to end up with big worms when the window gets
   big, and that looks silly.
 */

void myreshape (GLsizei w, GLsizei h)
{
  float ratiow = (float) w/INITW;
  float ratioh = (float) h/INITH;

  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gleft = -1 * ratiow;
  gright = 1 * ratiow;
  gbottom = -1 * ratioh;
  gtop = 1 * ratioh;

  gluOrtho2D(gleft, gright, gbottom, gtop);
  warpWorms();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  wsize = w; hsize = h;

  return;
}


/* given a pointer to a worm, this routine will decide on the next
   place to put a head and will advance the head pointer
 */

void updateWorm(worm_t *theworm)
{
  int newhead;
  float prevx, prevy;
  float newh = -1, newv = -1;
  float num, denom;

  /* make an easy to reference local copy of head, and update it in
     the worm structure. The new head replaces the old tail.
   */
  newhead = (theworm->head + 1) % SEGMENTS;

  prevx = theworm->segx[theworm->head];
  prevy = theworm->segy[theworm->head];

  /* if there is a mark, home in on it. After this, we still allow
     the random adjustment so that the worms play around a bit on the
     way to the mark.
   */
  if (marktime) {
    num = marky - prevy;
    denom = markx - prevx;
    theworm->dir = atan2(num,denom);
  };

  /* make a bit of a turn: between -MAXTURN and MAXTURN degrees change
     to dir (actualy theworm->dir is in radians for later use with
     cosf().
   */
  theworm->dir += (MAXTURN - (2 * MAXTURN * (float) drand48()));

  theworm->segx[newhead] = prevx + (STEPSIZE * cos(theworm->dir));
  theworm->segy[newhead] = prevy + (STEPSIZE * sin(theworm->dir));

  /* if we are at an edge, change direction so that we are heading away
     from the edge in question. There might be a problem here handling
     corner cases, but I have never seen a worm get stuck, so what the
     heck...
   */
  if (theworm->segx[newhead] <= gleft)
    theworm->dir = 0;
  if (theworm->segx[newhead] >= gright)
    theworm->dir = (180 * RADIAN);
  if (theworm->segy[newhead] >= gtop)
    theworm->dir = (270 * RADIAN);
  if (theworm->segy[newhead] <= gbottom)
    theworm->dir = (90 * RADIAN);

  if ((newv >= 0) || (newh >= 0)) {
    newh = (newh<0) ? 0 : newh;
    newv = (newv<0) ? 0 : newv;
  };

  /* update the permanent copy of the new head index */
  theworm->head = newhead;
}

/* updates the worms -- drawing takes place here, which may actually
   be a bad idea. It will probably be better to update the internal
   state only here, then post a redisplay using GLUT.
*/

void myidle (void)
{
  register int i, tail;

  if (marktime)
    marktime--;

  for (i = 0; i < curworms; i++) {
    /* first find tail */
    tail = (worms[i].head + 1) % SEGMENTS;

    /* erase tail */
    glColor3f(0.0, 0.0, 0.0);
    drawCircle(worms[i].segx[tail], worms[i].segy[tail], SEG_RADIUS);

    /* update head segment position and head pointer */
    updateWorm(&worms[i]);

    /* draw head */
    glColor3fv(worms[i].color);
    drawCircle(worms[i].segx[worms[i].head], worms[i].segy[worms[i].head],
	       SEG_RADIUS);
  };

  glFlush();
  return;
}

/* redraws the worms from scratch -- called after a window gets obscured */

void mydisplay(void)
{
  int i;

#ifndef WORMS_EAT_BACKGROUND
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
#endif

  for (i = 0; i < curworms; i++)
    drawWorm(&worms[i]);

  glFlush();
  return;
}

/* this routine gets called when the mouse is clicked. The incoming
   coordinates are in screen coordinates relative to the upper-left corner
   of the window, and oriented according to X, not to GL. So, here we
   convert the given coordinates into worm-world coordinates, and set the
   mark.
 */

void markSpot(int x, int y)
{
  /* map into the corridinate space I am using */
  markx = (float)((x - wsize/2)*(gright - gleft)/wsize);
  marky = -(float)((y - hsize/2)*(gtop - gbottom)/hsize);

  marktime = MARKTICKS;
}

void handleMouse(int btn, int state, int x, int y)
{
  switch (btn) {

  case (GLUT_LEFT_BUTTON):
    if (state == GLUT_UP)
      markSpot(x,y);
    break;

  default:
    /* do nothing */
    break;
  }

  return;
}

void menuSelect(int value)
{
  switch (value) {
    case MENU_FILLED:
      filled = 1;
      break;

    case MENU_UNFILLED:
      filled = 0;
      break;

    case MENU_QUIT:
      exit(0);
      break;

    case MENU_NULL:
      return;

    default:
      break;
    };

  glutPostRedisplay();
  return;
}

void visibility(int status)
{
  if (status == GLUT_VISIBLE)
    glutIdleFunc(myidle);
  else
    glutIdleFunc(NULL);
}

/* this is where GLUT is initialized, and the whole thing starts up.
   All animation and redisplay happens via the callbacks registered below.
 */

int main(int argc, char **argv)
{
  int fillmenu = 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(INITW, INITH);
  glutCreateWindow("Worms");

  myinit();

  glutDisplayFunc(mydisplay);
  glutVisibilityFunc(visibility);
  glutReshapeFunc(myreshape);
  glutMouseFunc(handleMouse);

  /* popup menu, courtsey of GLUT */
  fillmenu = glutCreateMenu(menuSelect);
  glutAddMenuEntry("Filled", MENU_FILLED);
  glutAddMenuEntry("Unfilled", MENU_UNFILLED);

  glutCreateMenu(menuSelect);
  glutAddMenuEntry("     WORMS", MENU_NULL);
  glutAddSubMenu("Drawing Mode", fillmenu);
  glutAddMenuEntry("Quit", MENU_QUIT);

  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
  return 0;
}


