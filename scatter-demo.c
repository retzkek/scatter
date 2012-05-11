#include <GLUT/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "neutrons.h"
#include "explosions.h"
#include "frames.h"

// control flags
int frameFlag=0;
int objectFlag=0;
int explosionFlag=1;
int rotateFlag=0;
int waterFlag=1;
int pauseFlag=0;
int tailFlag=1;
int statusFlag=0;

// options
int optMaxNeutrons=500;
int optInitNeutrons=50;
float optInitEnergy=2.0;
int optNumTrails=20;
float optTimeStep=0.5;
int optNumGroups=0;
struct xs *optCrossSections;  // cross-sections must be defined in the conf file
int optNumBins=10;
 
// status vars
static const int nbins=10;

// window info
int ww, wh;

int loadOpts(char* filename)
{
  char line[80];
  char card[80];
  int i, g;
  char* val;
  int found;
    
  FILE* optFile;
  optFile = fopen( filename, "r" );
  if ( optFile == NULL ) return 0;
  
  while (fgets(line,sizeof(line),optFile) != NULL) {
    // check for blank line or comment
    if (line[0]=='\n' || line[0]=='!')continue;
    // replace ":" character with string terminator; basically, split the line at the first ":"
    found=0;
    for (i=0;i<sizeof(line);i++) {
      if (line[i]==':') {
        line[i]='\0';
        found=1;
        break;
      }
    }
    if (!found) continue; // no colon, so it must not be a valid option assignment
    sscanf(line,"%s",&card);
    val=&line[0]+i+1; // address of first character after ":"
    if (!strcmp(card,"max-neutrons")) {
      sscanf(val,"%i",&optMaxNeutrons);
    } else if (!strcmp(card,"init-neutrons")) {
      sscanf(val,"%i",&optInitNeutrons);
    } else if (!strcmp(card,"init-energy")) {
      sscanf(val,"%f",&optInitEnergy);
    } else if (!strcmp(card,"trail-length")) {
      sscanf(val,"%i",&optNumTrails);
    } else if (!strcmp(card,"time-step")) {
      sscanf(val,"%f",&optTimeStep);
    } else if (!strcmp(card,"cross-sections")) {
      sscanf(val,"%i",&optNumGroups);
      optCrossSections = malloc(4*optNumGroups*sizeof(*optCrossSections));
      for (g=0;g<optNumGroups;g++) {
        if (fgets(line,sizeof(line),optFile)==NULL) {
          return 1;
        } else {
          sscanf(line,"%f,%f,%f,%f",&(optCrossSections[g].emax),&(optCrossSections[g].sigf),
                 &(optCrossSections[g].sigc),&(optCrossSections[g].sigs));
        }
      }
    } else if (!strcmp(card,"num-bins")) {
      sscanf(val,"%i",&optNumBins);
    }
  }
  
  return 1;
}

void init()
{
  //static GLfloat pointParam[3] = {0.0, 1.0, 0.0};
  static GLfloat pointParam[3] = {0.0, 0.0, 0.001};
  
  glEnable(GL_FOG);
  {
    GLfloat fogColor[4] = {0.0, 0.2, 0.5,1.0};
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogf(GL_FOG_DENSITY, 0.008);
    glHint(GL_FOG_HINT,GL_DONT_CARE);
    glFogf(GL_FOG_START,1.0);
    glFogf(GL_FOG_END,100.0);
    glFogfv(GL_FOG_COLOR,fogColor);
  }
  
  glEnable(GL_POINT_SMOOTH);
  glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,pointParam);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0, 0.2, 0.5, 1.0);

  initNeutrons(optInitNeutrons,optMaxNeutrons,optInitEnergy,optNumTrails,optNumGroups,optCrossSections,optNumBins);
  frameInit(30.0);
  
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  drawNeutrons(tailFlag);
  if (explosionFlag) drawExplosions();
  
  // measure, limit, and display framerate
  frameMark(FRAME_LIMIT);
  //if (frameFlag) frameDraw(GLUT_BITMAP_HELVETICA_10, 1.0, 1.0, 1.0, 0.05, 0.95);
  
  glutSwapBuffers();

}

void reshape(int w, int h)
{
  ww=w;
  wh=h;
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat)w/(GLfloat) h, 1.0, 500.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(120.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0);
}
  
void animate()
{
  if (rotateFlag) glRotatef(1.0,0.0,1.0,0.0);
    
  if (!pauseFlag) {
    updateNeutrons(optTimeStep);
    updateExplosions();
  }
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
  // "normal" keys; see skeyboard() for "special" keys
  switch (key) {
    case 'f':
      frameFlag=!frameFlag;
      break;
    case 'o':
      objectFlag=!objectFlag;
      break;
    case 'e':
      explosionFlag=!explosionFlag;
      break;
    case 'r':
      rotateFlag=!rotateFlag;
      break;
    case 'w':
      if (waterFlag) {
        glDisable(GL_FOG);
      } else {
        glEnable(GL_FOG);
      }
      waterFlag=!waterFlag;
      break;
    case 'p':
      pauseFlag=!pauseFlag;
      break;
    case 's':
      statusFlag=!statusFlag;
      break;
    case 't':
      tailFlag=!tailFlag;
      break;
    case ',':
      if (!rotateFlag) glRotatef(5.0,0.0,1.0,0.0);
      break;
    case '.':
      if (!rotateFlag) glRotatef(-5.0,0.0,1.0,0.0);
      break;
    case '=':
      optTimeStep+=0.1;
      break;
    case '-':
      if (optTimeStep > 0.1) optTimeStep-=0.1;
      break;
    case 'q':
    case 0x1B: //esc
      exit(0);
      break;
  }
}

void skeyboard(int key, int x, int y)
{
  // special keys; see keyboard() for "normal" keys
  switch (key) {
    case GLUT_KEY_LEFT:
      if (!rotateFlag) glRotatef(1.0,0.0,1.0,0.0);
      break;
    case GLUT_KEY_RIGHT:
      if (!rotateFlag) glRotatef(-1.0,0.0,1.0,0.0);
      break;
  }
}

int main(int argc, char** argv)
{
  int g;
  
  srandom(time(0));
  
  // load config file
  if (!loadOpts("scatter.conf")) {
    printf("%s: ERROR:  Problem when reading config file scatter.conf\n",argv[0]);
    exit(1);
  }
  
  // consistency checks
  if (optInitNeutrons > optMaxNeutrons) {
    printf("%s: ERROR:  init-neutrons must be less than max-neutrons\n",argv[0]);
    exit(1);
  }
  if (optNumGroups <= 0) {
    printf("%s: ERROR:  there must be at least one group of cross sections\n",argv[0]);
    exit(1);
  }
    
  
  printf("%s: Using parameters:\n",argv[0]);
  printf("     Maximum number of neutrons: %i\n",optMaxNeutrons);
  printf("     Initial number of neutrons: %i\n",optInitNeutrons);
  printf("     Initial neutron energy:     %f\n",optInitEnergy);
  printf("     Trail length:               %i\n",optNumTrails);
  printf("     Time step:                  %f\n",optTimeStep);
  printf("     Cross sections:             %i groups\n",optNumGroups);
  printf("          g  emax     fission  capture  scatter\n");
  for (g=0;g<optNumGroups;g++) {
    printf("          %i  %f %f %f %f\n",g+1,optCrossSections[g].emax,optCrossSections[g].sigf,
           optCrossSections[g].sigc,optCrossSections[g].sigs);
  }
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(800,800);
  glutInitWindowPosition(100,100);
  glutCreateWindow("Scatter");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(animate);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(skeyboard); 
  glutMainLoop();
  return 0;
}
