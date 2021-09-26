#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <stdbool.h>

#define MAX_PARTICLES 1000
#define WCX		640
#define WCY		480
#define RAIN	0
#define SNOW	1
#define	HAIL	2
#define TEXTID  3
#define ESCAPE  27


float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;

bool buttonPressed = false;

int loop;
int fall;

//floor colors
float r = 1.0;
float g = 1.0;
float b = 0.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -10.0;

typedef struct {
  
  bool alive;	
  float life;	
  float fade; 
  float red;
  float green;
  float blue;
  float xpos;
  float ypos;
  float zpos;
  float vel;
  float gravity;
}particles;

particles par_sys[MAX_PARTICLES];

void normal_keys(unsigned char key, int x, int y) {
  
  if (key == 'c')
    buttonPressed = true;

  if (key == 'r') { // Rain
    fall = RAIN;
    glutPostRedisplay();
  }
  if (key == 'h') { // Hail
    fall = HAIL;
    glutPostRedisplay();
  }
  if (key == 's') { // Snow
    fall = SNOW;Press c
    glutPostRedisplay();
  }
  if (key == '=') { //really '+' - make hail bigger
    hailsize += 0.01;
  }
  if (key == '-') { // make hail smaller
    if (hailsize > 0.1) hailsize -= 0.01;
  }
  if (key == ',') { // really '<' slow down
    if (slowdown > 4.0) slowdown += 0.1;
  }
  if (key == '.') { // really '>' speed up
    if (slowdown > 1.0) slowdown -= 0.1;
  }
  if (key == 'q') { // QUIT
    exit(0);
  }
}

void special_keys(int key, int x, int y) {
  if (key == GLUT_KEY_UP) {
    zoom += 1.0;
  }
  if (key == GLUT_KEY_DOWN) {
    zoom -= 1.0;
  }
  if (key == GLUT_KEY_RIGHT) {
    pan += 1.0;
  }
  if (key == GLUT_KEY_LEFT) {
    pan -= 1.0;
  }
  if (key == GLUT_KEY_PAGE_UP) {
    tilt -= 1.0;
  }
  if (key == GLUT_KEY_PAGE_DOWN) {
    tilt += 1.0;
  }
}


void initParticles(int i) {
    par_sys[i].alive = true;
    par_sys[i].life = 1.0;
    par_sys[i].fade = float(rand()%100)/1000.0f+0.003f;

    par_sys[i].xpos = (float) (rand() % 21) - 10;
    par_sys[i].ypos = 10.0;
    par_sys[i].zpos = (float) (rand() % 21) - 10;

    par_sys[i].red = 0.5;
    par_sys[i].green = 0.5;
    par_sys[i].blue = 1.0;

    par_sys[i].vel = velocity;
    par_sys[i].gravity = -0.8;

}

void init( ) {
  int x, z;

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    for (z = 0; z < 21; z++) {
      for (x = 0; x < 21; x++) {
        ground_points[x][z][0] = x - 10.0;
        ground_points[x][z][1] = accum;
        ground_points[x][z][2] = z - 10.0;

        ground_colors[z][x][0] = r; 
        ground_colors[z][x][1] = g; 
        ground_colors[z][x][2] = b; 
        ground_colors[z][x][3] = 0.0; 
      }
    }

    for (loop = 0; loop < MAX_PARTICLES; loop++) {
        initParticles(loop);
    }
}

// For Rain
void drawRain() {
  float x, y, z;
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(0.5, 0.5, 1.0);
      glBegin(GL_LINES);
        glVertex3f(x, y, z);
        glVertex3f(x, y+0.5, z);
      glEnd();

      // Update values
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= -10) {
        par_sys[loop].life = -1.0;
      }
      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// For Hail
void drawHail() {
  float x, y, z;

  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      glColor3f(0.8, 0.8, 0.9);
      glBegin(GL_QUADS);
       // Front
        glVertex3f(x-hailsize, y-hailsize, z+hailsize); // lower left
        glVertex3f(x-hailsize, y+hailsize, z+hailsize); // upper left
        glVertex3f(x+hailsize, y+hailsize, z+hailsize); // upper right
        glVertex3f(x+hailsize, y-hailsize, z+hailsize); // lower left
        //Left
        glVertex3f(x-hailsize, y-hailsize, z+hailsize);
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z+hailsize);
        // Back
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        //Right
        glVertex3f(x+hailsize, y+hailsize, z+hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z+hailsize);
        //Top
        glVertex3f(x-hailsize, y+hailsize, z+hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z+hailsize);
        //Bottom
        glVertex3f(x-hailsize, y-hailsize, z+hailsize);
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z+hailsize);
      glEnd();

      // Update values
        if (par_sys[loop].ypos <= -10) {
        par_sys[loop].vel = par_sys[loop].vel * -1.0;
      }
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000); 
      par_sys[loop].vel += par_sys[loop].gravity;

      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// For Snow
void drawSnow() {
  float x, y, z;
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(1.0, 1.0, 1.0);
      glPushMatrix(); //its used for making changes temporarily in pushmatrix, matrix consists the info of transfrms
      glTranslatef(x, y, z);
      glutSolidSphere(0.2, 6, 6);
      glPopMatrix();

      // Update values
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= -10) {
        int zi = z - zoom + 10;
        int xi = x + 10;
        ground_colors[zi][xi][0] = 1.0;
        ground_colors[zi][xi][2] = 1.0;
        ground_colors[zi][xi][3] += 1.0;
        if (ground_colors[zi][xi][3] > 1.0) {
          ground_points[xi][zi][1] += 0.1;
        }
        par_sys[loop].life = -1.0;
      }

      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}
void DrawTextXY(double x,double y,double z,double scale,char *s)
{
int i;

glPushMatrix();
glTranslatef(x,y,z);
glScalef(scale,scale,scale);
for (i=0;i < strlen(s);i++)
glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,s[i]);
glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,s[i]);
glPopMatrix();
}
void cover()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glLoadIdentity();
glClearColor(0,0,0,0.0);

glTranslatef(0.0,0.0,-6.0);
glTranslatef(0.0,-1.3,0.0);

	glColor3f(1.00,0.20,0.10);
glLoadName(TEXTID);

DrawTextXY(-1.7,3.5,0.0,0.001,"   GRAPHICAL IMPLEMENTATION OF  ");

glColor3f(0.6,0.8,0.7);
DrawTextXY(-1.75,3,0.0,0.0014,"    SNOW RA-IL PREVAIL ");

glColor3f(0.7,0.6,0.1);
DrawTextXY(-3.25,1.5,0.0,0.0007,"     Submitted by :- ");

glColor3f(1.0,0.5,0.0);
DrawTextXY(-2.5,1.2,0.0,0.001,"       MUKUND B ");
DrawTextXY(1.01,1.2,0.0,0.001,"        SYED SHOAIB ");
glColor3f(0.7,0.8,0.6);
DrawTextXY(-2.5,0.95,0.0,0.001,"     (1CD18CS078) ");
DrawTextXY(1,0.95,0.0,0.001,"        (1CD18CS183) ");

glColor3f(0.7,0.6,0.1);
DrawTextXY(-1.25,0.2,0.0,0.0007,"     Under the guidance of : ");
glColor3f(1.0,0.8,0.4);
DrawTextXY(-3.25,-.2,0.0,0.001,"     Prof. PRIYADARSHINI");
DrawTextXY(.25,-.2,0.0,0.001,"     Prof. JOSEPHINE PREMKUMAR");
DrawTextXY(-2.8,-.5,0.0,0.0007,"     Associate Professor ");
DrawTextXY(1,-.5,0.0,0.0007,"     Associate Professor ");
DrawTextXY(-2,-.75,0.0,0.001,"Cambridge Institute of Technology, Bangalore");
glColor3f(0.8,0.8,0.8);
DrawTextXY(-1,-1,0.0,0.0008,"     Press c to continue... ");

glFlush();			//Finish rendering
glutSwapBuffers();
}
void drawScene( ) {
  int i, j;
  float x, y, z;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();


  glRotatef(pan, 0.0, 1.0, 0.0);
  glRotatef(tilt, 1.0, 0.0, 0.0);

  
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
    // along z - y const
    for (i = -10; i+1 < 11; i++) {
      // along x - y const
      for (j = -10; j+1 < 11; j++) {
        glColor3fv(ground_colors[i+10][j+10]);
        glVertex3f(ground_points[j+10][i+10][0],
              ground_points[j+10][i+10][1],
              ground_points[j+10][i+10][2] + zoom);
        glColor3fv(ground_colors[i+10][j+1+10]);
        glVertex3f(ground_points[j+1+10][i+10][0],
              ground_points[j+1+10][i+10][1],
              ground_points[j+1+10][i+10][2] + zoom);
        glColor3fv(ground_colors[i+1+10][j+1+10]);
        glVertex3f(ground_points[j+1+10][i+1+10][0],
              ground_points[j+1+10][i+1+10][1],
              ground_points[j+1+10][i+1+10][2] + zoom);
        glColor3fv(ground_colors[i+1+10][j+10]);
        glVertex3f(ground_points[j+10][i+1+10][0],
              ground_points[j+10][i+1+10][1],
              ground_points[j+10][i+1+10][2] + zoom);
      }

    }
  glEnd();
  // Which Particles
  if (fall == RAIN) {
    drawRain();
  }else if (fall == HAIL) {
    drawHail();
  }else if (fall == SNOW) {
    drawSnow();
  }
if (!buttonPressed)
cover();

  glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, (float) w / (float) h, .1, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void idle ( ) {
  glutPostRedisplay();
}

int main (int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(WCX, WCY);
  glutCreateWindow("SNOW RA-IL PREVAIL --> MUKUND B(1CD18CS078) & SYED SHOAIB(1CD18CS183)");
  init();
  glutDisplayFunc(drawScene);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(normal_keys);
  glutSpecialFunc(special_keys);
  glutIdleFunc(idle);
  glutMainLoop();
  return 0;
}
