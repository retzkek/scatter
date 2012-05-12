#include <GL/glut.h>

#include "explosions.h"

struct explosion es[MAX_EXPLOSIONS];

void addExplosion(float x, float y, float z)
{
  int i, n;
  
  n=-1;
  for (i=0;i<MAX_EXPLOSIONS;i++) {
    if (es[i].size < 0.0) {
      n=i;
      break;
    }
  }
  if (n >= 0) {
    es[n].size=0.5;
    es[n].x=x;
    es[n].y=y;
    es[n].z=z;
  }
}

void drawExplosions(void)
{
  int i;
  
  for (i=0;i<MAX_EXPLOSIONS;i++) {
    if (es[i].size >= 0.0) {
      glColor4f(0.2, 0.5, 0.6, (1.0-es[i].size/MAX_SIZE));
      glPushMatrix();
        glTranslated(es[i].x, es[i].y, es[i].z);
        glutSolidSphere(es[i].size, 10, 10);
      glPopMatrix();
    }
  }
}

void updateExplosions(void)
{
  int i;
  
  for (i=0;i<MAX_EXPLOSIONS;i++) {
    if (es[i].size >= 0.0) {
      es[i].size+=0.5;
      if (es[i].size > MAX_SIZE) es[i].size=-1.0;
    }        
  }
}

