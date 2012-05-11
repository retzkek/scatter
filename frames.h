#ifndef frames_h
#define frames_h

#include <sys/time.h>
#include <stdio.h>

struct timeval frameStartTime, frameEndTime;
double ftime; // this MUST be a double!
float fps, maxFps; 


float frameInit(float maxFramesPerSecond) 
{
  maxFps=maxFramesPerSecond;
  gettimeofday(&frameStartTime, NULL);
  ftime=frameStartTime.tv_sec+frameStartTime.tv_usec*1.0e6;
}

float frameMark() 
{             
  double ftime1; // this MUST be a double!

  do {
    gettimeofday(&frameEndTime, NULL);
    ftime1=frameEndTime.tv_sec+frameEndTime.tv_usec*1.0E-6;
    fps= 1.0/(ftime1-ftime);
  } while (fps > maxFps); 
  ftime=ftime1;
  return fps;
}

float frameFps()
{
  return fps;
}

void frameDraw(void* font, float r, float g, float b, float x, float y)
{
  /* font: font to use, e.g., GLUT_BITMAP_HELVETICA_10
  r, g, b: text colour
  x, y: text position in window: range [0,0] (bottom left of window)
  to [1,1] (top right of window). */

  char str[30]; 
  char *ch; 
  GLint matrixMode;
  GLboolean lightingOn;

  sprintf(str, "Frames per second: %2.0f", fps);

  lightingOn= glIsEnabled(GL_LIGHTING);        /* lighting on? */
  if (lightingOn) glDisable(GL_LIGHTING);

  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, 1.0, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */
  glColor3f(r, g, b);
  glRasterPos3f(x, y, 0.0);
  for(ch= str; *ch; ch++) {
    glutBitmapCharacter(font, (int)*ch);
  }
  glPopAttrib();
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(matrixMode);
  if (lightingOn) glEnable(GL_LIGHTING);
}
  
#endif
