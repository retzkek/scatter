#ifndef frames_h
#define frames_h

#include <sys/time.h>
#include <errno.h>
#include <stdio.h>

#define FRAME_NOLIMIT 0
#define FRAME_LIMIT 1

struct timeval frameStartTime, frameEndTime;
struct timespec sleepTime;
double ftime; // this MUST be a double!
float fps, maxFps; 


float frameInit(float maxFramesPerSecond) 
{
  maxFps=maxFramesPerSecond;
  gettimeofday(&frameStartTime, NULL);
  ftime=frameStartTime.tv_sec+frameStartTime.tv_usec*1.0e6;
}

float frameMark(int limit) 
{             
  double ftime1; // this MUST be a double!
  int i;
  //int esv;
    
  /*
  i=0;
  do {
    gettimeofday(&frameEndTime, NULL);
    ftime1=frameEndTime.tv_sec+frameEndTime.tv_usec*1.0E-6;
    fps= 1.0/(ftime1-ftime);
    i++;
  } while (fps > maxFps); 
  printf("%i\n",i);
  ftime=ftime1;
  return fps;
  */
  gettimeofday(&frameEndTime, NULL);
  ftime1=frameEndTime.tv_sec*1e9+frameEndTime.tv_usec*1e3;
  if (limit == FRAME_LIMIT) {
    if (1.0e9/(ftime1-ftime) > maxFps) {
      // we need to sleep a bit to maintain the frame rate
      sleepTime.tv_sec=0;
      sleepTime.tv_nsec=(1.0e9/maxFps-(ftime1-ftime));
      nanosleep(&sleepTime,NULL);
      gettimeofday(&frameEndTime, NULL);
      ftime1=frameEndTime.tv_sec*1e9+frameEndTime.tv_usec*1e3;
    }
  }
  fps= 1.0e9/(ftime1-ftime);  // calculate the actual FPS (or an approximation thereof)
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
