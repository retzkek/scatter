#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>

#include "neutrons.h"
#include "explosions.h"
#include "frames.h"
#include "texture.h"

// control flags
int frameFlag=0;
int objectFlag=1;
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
 
// textures
GLuint texCyl, texBot, texTop;

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
    
  FILE* optFile;
  optFile = fopen( filename, "r" );
  if ( optFile == NULL ) return 0;
  
  while (fgets(line,sizeof(line),optFile) != NULL) {
    sscanf(line,"%s %f",&card,&val);
    // replace ":" character with string terminator; basically, split the line at the first ":"
    for (i=0;i<sizeof(line);i++) {
      if (line[i]==':') {
        line[i]='\0';
        break;
      }
    }
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

void drawEnvironment()
{
  //char twe[] = "TWE";
  //char *ch;
  float rad, height;
  
  /*
  // draw central text  
  glColor4f(1.0,1.0,1.0,1.0);
  glLineWidth(20.0);
  glPushMatrix();
    glScalef(0.2,0.3,0.3);
    glTranslatef(-(glutStrokeWidth(GLUT_STROKE_ROMAN,'T')+glutStrokeWidth(GLUT_STROKE_ROMAN,'W')+glutStrokeWidth(GLUT_STROKE_ROMAN,'E'))/2.0,-70.0,0.0);
    for (ch = twe; *ch; ch++)
      glutStrokeCharacter(GLUT_STROKE_ROMAN, *ch);
  glPopMatrix();
  */
  height=150;
  rad=150;
  glColor4f(1.0,1.0,1.0,1.0);
  glEnable(GL_TEXTURE_2D);
    
  glPushMatrix();
  glRotatef(-90,1.0,0.0,0.0);    
    
  //draw bottom
  glBindTexture(GL_TEXTURE_2D,texTop);
  glBegin( GL_QUADS );
  glTexCoord2f(0.0,0.0); glVertex3f(-rad,-rad,-0.5*height);
  glTexCoord2f(8.0,0.0); glVertex3f(rad,-rad,-0.5*height);
  glTexCoord2f(8.0,8.0); glVertex3f(rad,rad,-0.5*height);
  glTexCoord2f(0.0,8.0); glVertex3f(-rad,rad,-0.5*height);
  glEnd();

  //draw top
  glBindTexture(GL_TEXTURE_2D,texTop);
  glBegin( GL_QUADS );
  glTexCoord2f(0.0,0.0); glVertex3f(-rad,-rad,0.5*height);
  glTexCoord2f(8.0,0.0); glVertex3f(rad,-rad,0.5*height);
  glTexCoord2f(8.0,8.0); glVertex3f(rad,rad,0.5*height);
  glTexCoord2f(0.0,8.0); glVertex3f(-rad,rad,0.5*height);
  glEnd();
    
  // draw cylinder
  glBindTexture(GL_TEXTURE_2D,texCyl);
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );    glVertex3f( -0.4*rad, -1.0*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );    glVertex3f( -0.4*rad, -1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );   glVertex3f( 0.4*rad, -1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );   glVertex3f( 0.4*rad, -1.0*rad, -0.5*height );
  glEnd();
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );   glVertex3f( 0.4*rad, -1.0*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );   glVertex3f( 0.4*rad, -1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );    glVertex3f( 1.0*rad, -0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );    glVertex3f( 1.0*rad, -0.4*rad, -0.5*height );
  glEnd();    
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );    glVertex3f( 1.0*rad, -0.4*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );    glVertex3f( 1.0*rad, -0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );   glVertex3f( 1.0*rad, 0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );   glVertex3f( 1.0*rad, 0.4*rad, -0.5*height );
  glEnd();    
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );   glVertex3f( 1.0*rad, 0.4*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );   glVertex3f( 1.0*rad, 0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );    glVertex3f( 0.4*rad, 1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );    glVertex3f( 0.4*rad, 1.0*rad, -0.5*height );
  glEnd();
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );    glVertex3f( 0.4*rad, 1.0*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );    glVertex3f( 0.4*rad, 1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );   glVertex3f( -0.4*rad, 1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );   glVertex3f( -0.4*rad, 1.0*rad, -0.5*height );
  glEnd();
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );   glVertex3f( -0.4*rad, 1.0*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );   glVertex3f( -0.4*rad, 1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );    glVertex3f( -1.0*rad, 0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );    glVertex3f( -1.0*rad, 0.4*rad, -0.5*height );
  glEnd();
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );    glVertex3f( -1.0*rad, 0.4*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );    glVertex3f( -1.0*rad, 0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );   glVertex3f( -1.0*rad, -0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );   glVertex3f( -1.0*rad, -0.4*rad, -0.5*height );
  glEnd();
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0, 0.0 );   glVertex3f( -1.0*rad, -0.4*rad, -0.5*height );
  glTexCoord2f( 0.0, 1.0 );   glVertex3f( -1.0*rad, -0.4*rad, 0.5*height );
  glTexCoord2f( 2.0, 1.0 );    glVertex3f( -0.4*rad, -1.0*rad, 0.5*height );
  glTexCoord2f( 2.0, 0.0 );    glVertex3f( -0.4*rad, -1.0*rad, -0.5*height );
  glEnd();
  /*glPushMatrix();
  glTranslatef(0.0,0.0,20.0);
  glutSolidTorus(20.0,rad-10.0,8,20);
  glPopMatrix();
  */
  glPopMatrix();
    
  glDisable(GL_TEXTURE_2D);
}

void drawStatus()
{
  char str[80]; 
  char *ch; 
  static int btop=220, bbot=20, bleft=20, bright=420;
  int i;
  int bw, bh;
  float r, g, b;

  GLint matrixMode;
  GLboolean lightingOn;

  lightingOn= glIsEnabled(GL_LIGHTING);        /* lighting on? */
  if (lightingOn) glDisable(GL_LIGHTING);

  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, ww, 0, wh); // set scale to window dimensions, so that coords=pixels
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */
  
  if (statusFlag) {    
      // draw left button
    glColor4f(0.0, 0.0, 0.0,1.0);
    glBegin( GL_QUADS );
    glVertex2i(0, bbot);
    glVertex2i(20, bbot);
    glVertex2i(20, btop);
    glVertex2i(0, btop);
    glEnd();
    // draw text
    sprintf(str,"<");
    glColor3f(0.8, 0.8, 0.8);
    glRasterPos2i(2,bbot+(btop-bbot)/2);
    for(ch= str; *ch; ch++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*ch);
    }

    // draw box
    glColor4f(0.0, 0.0, 0.0,0.5);
    glBegin( GL_QUADS );
    glVertex2i(bleft, bbot);
    glVertex2i(bright, bbot);
    glVertex2i(bright, btop);
    glVertex2i(bleft, btop);
    glEnd();
  
    // draw frame rate      
    sprintf(str,"Frame rate: %3.2f frame/s",frameFps());
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(bleft+10,btop-20);
    for(ch= str; *ch; ch++) {
     glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
    }
  
    // draw simulation rate
    sprintf(str,"Simulation rate: %1.2f s/frame",optTimeStep);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(bleft+10,btop-35);
    for(ch= str; *ch; ch++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
    }
  
    // draw k-effective
    sprintf(str,"k-effective: %1.5f",getKeff());
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(bleft+10,btop-50);
    for(ch= str; *ch; ch++) {
     glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
    }
  
    // draw number of neutrons
    sprintf(str,"Number of neutrons: %i",getNumNeutrons());
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(bleft+10,btop-65);
    for(ch= str; *ch; ch++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
    }
  
  
    // draw energy spectrum
    sprintf(str,"Neutron Energy Spectrum");
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(bleft+140,btop-80);
    for(ch= str; *ch; ch++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (int)*ch);
    }
    bw=(bright-bleft-20)/optNumBins;
    for (i=0;i<optNumBins;i++) {
      // draw background   
      glColor4f(0.2, 0.2, 0.2,0.5);
      glBegin( GL_QUADS );
      glVertex2i(bleft+10+bw*i, bbot+10);
      glVertex2i(bleft+10+bw*(i+1), bbot+10);
      glVertex2i(bleft+10+bw*(i+1), bbot+110);
      glVertex2i(bleft+10+bw*i, bbot+110);
      glEnd();
    
      //draw colorbar
      bh=50*getBinValue(i);
      if (bh > 100) bh=100;
      hsv2rgb(5.0-getBinEnergy(i)/optInitEnergy*5.0,1.0,1.0,&r,&g,&b);
      glColor3f(r, g, b);
      glBegin( GL_QUADS );
      glVertex2i(bleft+10+bw*i,bbot+10);
      glVertex2i(bleft+10+bw*(i+1),bbot+10);
      glVertex2i(bleft+10+bw*(i+1),bbot+10+bh);
      glVertex2i(bleft+10+bw*i,bbot+10+bh);
      glEnd();
    }
  } else {
    // draw right button
    glColor4f(0.0, 0.0, 0.0,1.0);
    glBegin( GL_QUADS );
    glVertex2i(0, bbot);
    glVertex2i(20, bbot);
    glVertex2i(20, btop);
    glVertex2i(0, btop);
    glEnd();
    // draw text
    sprintf(str,">");
    glColor3f(0.8, 0.8, 0.8);
    glRasterPos2i(2,bbot+(btop-bbot)/2);
    for(ch= str; *ch; ch++) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)*ch);
    }
  }    
  
  glPopAttrib();
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(matrixMode);
  if (lightingOn) glEnable(GL_LIGHTING);
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

  texCyl=LoadTextureRAW("plate.raw",256,512,1);
  texBot=LoadTextureRAW("plate2.raw",256,256,1);
  texTop=LoadTextureRAW("grate.raw",256,512,1);
  
  initNeutrons(optInitNeutrons,optMaxNeutrons,optInitEnergy,optNumTrails,optNumGroups,optCrossSections,optNumBins);
  frameInit(30.0);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  if (objectFlag) drawEnvironment();
  drawNeutrons(tailFlag);
  if (explosionFlag) drawExplosions();
  drawStatus();
  
  // measure, limit, and display framerate
  frameMark();
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
      optTimeStep-=0.1;
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

void mouse(int button, int state, int x, int y)
{
  y=wh-y; // change to gl coordinates
  
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        if (x < 20 && y > 20 && y <220) statusFlag=!statusFlag;
      }
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
  glutMouseFunc(mouse);
  glutMainLoop();
  return 0;
}
