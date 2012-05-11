#include "wm.h"
#include "texture.h"
#include <GLUT/glut.h>

#define WM_BUTTON_UP 0
#define WM_BUTTON_DOWN 1
#define WM_BUTTON_HOVER 2

#define WM_WINDOW_ANIMATE_COLLAPSE -1
#define WM_WINDOW_ANIMATE_EXPAND -2

#define WM_FALSE 0
#define WM_TRUE 1

struct win {
  char *label;
  int x, y, w, h;     // window left x, top y, width, height
  int oldx, oldy;
  int side;           // dock side
  int bstate;         // button state
  int bx, by, bw, bh; // button left x, top y, width, height
  int blx, bly, blr;  // button label x, y, rotation
  int state;          // window state 
  //int ans;            // collapse/expand animation step
  int moveable;
  void(*drawFunc)();
  void(*mouseFunc)();
};

int nWindows;
struct win *ws=0;

// textures
GLuint btnNormalEnd, btnNormalMid, btnHoverEnd, btnHoverMid, btnPressedMid, btnPressedEnd;

static const int bsize=20;
//static const int nans=10;

int mww, mwh;    // main window width and height
int btnx, btny;  // mouse position at button down
int btn;         // button pressed
int wmove;       // flag indicating whether we moved a window or not

void wmInit(int mainWindowWidth, int mainWindowHeight)
{
  mww=mainWindowWidth;
  mwh=mainWindowHeight;
  
  btnNormalEnd=LoadTextureRAW("rc/button_normal_end.raw",24,24,0);
  btnNormalMid=LoadTextureRAW("rc/button_normal_mid.raw",24,24,0);
  btnHoverEnd=LoadTextureRAW("rc/button_hover_end.raw",24,24,0);
  btnHoverMid=LoadTextureRAW("rc/button_hover_mid.raw",24,24,0);
  btnPressedEnd=LoadTextureRAW("rc/button_pressed_end.raw",24,24,0);
  btnPressedMid=LoadTextureRAW("rc/button_pressed_mid.raw",24,24,0);
}

void wmAddWindow(const char* label, void(*drawFunc)(), int width, int height, int side, int state, int moveable, int collapsable)
{
  nWindows++;
  
  if (ws == 0) {
    ws=(struct win *)malloc(sizeof(*ws));
  } else {
    ws=(struct win *)realloc(ws,nWindows*sizeof(*ws));
  }
  
  ws[nWindows-1].label=malloc(strlen(label)*sizeof(label));
  strcpy(ws[nWindows-1].label,label);
  ws[nWindows-1].w=width;
  ws[nWindows-1].h=height;
  ws[nWindows-1].x=-1;
  ws[nWindows-1].y=-1;
  ws[nWindows-1].side=side;
  ws[nWindows-1].state=state;
  ws[nWindows-1].drawFunc=drawFunc;
  ws[nWindows-1].mouseFunc=0;
  ws[nWindows-1].bstate=WM_BUTTON_UP;
  ws[nWindows-1].moveable=moveable;
  
}

void wmArrangeWindows(int w, int h)
{
  int i;
  int lbot, rbot, tleft, bleft;
  int lwidth;
  char *ch;
  
  mww=w;
  mwh=h;
  
  lbot=0;
  rbot=0;
  tleft=0;
  bleft=0;
  
  for (i=0;i<nWindows;i++) {
    lwidth=0;
    for (ch=ws[i].label;*ch;ch++)
      lwidth+=glutStrokeWidth(GLUT_STROKE_MONO_ROMAN,*ch)*0.1;
    
    switch (ws[i].side) {
      case WM_WINDOW_LEFT:
        ws[i].x=bsize;
        if (ws[i].y < 0) {
          ws[i].y=lbot;
        }
        ws[i].bx=0;
        ws[i].by=ws[i].y;
        ws[i].bw=bsize;
        ws[i].bh=ws[i].h;
        ws[i].blx=ws[i].bx+ws[i].bw*0.75;
        ws[i].bly=ws[i].by+(ws[i].bh+lwidth)/2;
        ws[i].blr=90;
        lbot+=ws[i].h;
        break;
      case WM_WINDOW_TOP:
        if (ws[i].x < 0) {
          ws[i].x=tleft;
        }
        ws[i].y=bsize;
        ws[i].bx=ws[i].x;
        ws[i].by=0;
        ws[i].bw=ws[i].w;
        ws[i].bh=bsize;
        ws[i].blx=ws[i].bx+(ws[i].bw-lwidth)/2;
        ws[i].bly=ws[i].by+ws[i].bh*0.75;
        ws[i].blr=0;
        tleft+=ws[i].w;
        break;
      case WM_WINDOW_RIGHT:
        ws[i].x=w-ws[i].w-bsize;
        if (ws[i].x < 0) {
          ws[i].y=rbot;
        }
        ws[i].bx=w-bsize;
        ws[i].by=ws[i].y;
        ws[i].bw=bsize;
        ws[i].bh=ws[i].h;
        ws[i].blx=ws[i].bx+ws[i].bw*0.75;
        ws[i].bly=ws[i].by+(ws[i].bh+lwidth)/2;
        ws[i].blr=90;
        rbot+=ws[i].h;
        break;
      case WM_WINDOW_BOTTOM:
        if (ws[i].x < 0) {
          ws[i].x=bleft;
        }
        ws[i].y=h-ws[i].h-bsize;
        ws[i].bx=ws[i].x;
        ws[i].by=h-bsize;
        ws[i].bw=ws[i].w;
        ws[i].bh=bsize;
        ws[i].blx=ws[i].bx+(ws[i].bw-lwidth)/2;
        ws[i].bly=ws[i].by+ws[i].bh*0.75;
        ws[i].blr=0;
        bleft+=ws[i].w;
        break;
      case WM_WINDOW_FLOAT:
        if (ws[i].x < 0) {
          ws[i].x=0;
        }
        if (ws[i].y < 0) {
          ws[i].y=bsize;
        }
        ws[i].bx=ws[i].x;
        ws[i].by=ws[i].y-bsize;
        ws[i].bw=ws[i].w;
        ws[i].bh=bsize;
        ws[i].blx=ws[i].bx+(ws[i].bw-lwidth)/2;
        ws[i].bly=ws[i].by+ws[i].bh*0.75;
        ws[i].blr=0;
        break;
    }
  }
}    
        

void wmDrawWindows(int w, int h)
{
  GLint matrixMode;
  GLboolean lightingOn;

  int i;
  int bx, by, bw, bh;
  char *ch;
  
  // save current state
  lightingOn= glIsEnabled(GL_LIGHTING);        /* lighting on? */
  if (lightingOn) glDisable(GL_LIGHTING);
  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0); // set scale to window dimensions, so that coords=pixels
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */

  
  for (i=0;i<nWindows;i++) {
    glPushMatrix();
    
    GLuint texmid, texend;
    
    // draw expand/collapse button
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0, 1.0,1.0);
    if (ws[i].bstate == WM_BUTTON_DOWN) {
      texend=btnPressedEnd;
      texmid=btnPressedMid;
    } else if (ws[i].bstate == WM_BUTTON_HOVER) {
      texend=btnHoverEnd;
      texmid=btnHoverMid;
    } else {
      texend=btnNormalEnd;
      texmid=btnNormalMid;
    }
    if (ws[i].side == WM_WINDOW_LEFT || ws[i].side == WM_WINDOW_RIGHT) {
      glBindTexture(GL_TEXTURE_2D,texend);
      glBegin( GL_QUADS );
      glTexCoord2f(0,0); glVertex2i(ws[i].bx,          ws[i].by);
      glTexCoord2f(0,1); glVertex2i(ws[i].bx+ws[i].bw, ws[i].by);
      glTexCoord2f(1,1); glVertex2i(ws[i].bx+ws[i].bw, ws[i].by+24);
      glTexCoord2f(1,0); glVertex2i(ws[i].bx,          ws[i].by+24);
      glTexCoord2f(1,0); glVertex2i(ws[i].bx,          ws[i].by+ws[i].bh-24);
      glTexCoord2f(1,1); glVertex2i(ws[i].bx+ws[i].bw, ws[i].by+ws[i].bh-24);
      glTexCoord2f(0,1); glVertex2i(ws[i].bx+ws[i].bw, ws[i].by+ws[i].bh);
      glTexCoord2f(0,0); glVertex2i(ws[i].bx,          ws[i].by+ws[i].bh);
      glEnd();
      glBindTexture(GL_TEXTURE_2D,texmid);
      glBegin( GL_QUADS );
      glTexCoord2f(0,0); glVertex2i(ws[i].bx,          ws[i].by+24);
      glTexCoord2f(0,1); glVertex2i(ws[i].bx+ws[i].bw, ws[i].by+24);
      glTexCoord2f(1,1); glVertex2i(ws[i].bx+ws[i].bw, ws[i].by+ws[i].bh-24);
      glTexCoord2f(1,0); glVertex2i(ws[i].bx,          ws[i].by+ws[i].bh-24);
      glEnd();
    } else {
      glBindTexture(GL_TEXTURE_2D,texend);
      glBegin( GL_QUADS );
      glTexCoord2f(0,0); glVertex2i(ws[i].bx,          ws[i].by);
      glTexCoord2f(0,1); glVertex2i(ws[i].bx,          ws[i].by+ws[i].bh);
      glTexCoord2f(1,1); glVertex2i(ws[i].bx+24,       ws[i].by+ws[i].bh);
      glTexCoord2f(1,0); glVertex2i(ws[i].bx+24,       ws[i].by);
      
      glTexCoord2f(1,0); glVertex2i(ws[i].bx+ws[i].bw-24, ws[i].by);
      glTexCoord2f(1,1); glVertex2i(ws[i].bx+ws[i].bw-24, ws[i].by+ws[i].bh);
      glTexCoord2f(0,1); glVertex2i(ws[i].bx+ws[i].bw,    ws[i].by+ws[i].bh);
      glTexCoord2f(0,0); glVertex2i(ws[i].bx+ws[i].bw,    ws[i].by);
      glEnd();
      glBindTexture(GL_TEXTURE_2D,texmid);
      glBegin( GL_QUADS );
      glTexCoord2f(0,0); glVertex2i(ws[i].bx+24,          ws[i].by);
      glTexCoord2f(1,0); glVertex2i(ws[i].bx+ws[i].bw-24, ws[i].by);
      glTexCoord2f(1,1); glVertex2i(ws[i].bx+ws[i].bw-24, ws[i].by+ws[i].bh);
      glTexCoord2f(0,1); glVertex2i(ws[i].bx+24,          ws[i].by+ws[i].bh);
      glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    // button text
    glColor4f(0.0,0.0,0.0,1.0);
    glLineWidth(1);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(ws[i].blx,ws[i].bly,0.0);
    glScalef(0.1,-0.1,1.0);
    glRotatef(ws[i].blr,0.0,0.0,1.0);
    for (ch=ws[i].label;*ch;ch++)
      glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *ch);
    glPopMatrix();
    
    // draw window
    if (ws[i].state == WM_WINDOW_EXPANDED) {
      glTranslatef(ws[i].x, ws[i].y, 0.0); // move to window location
      ws[i].drawFunc(ws[i].w, ws[i].h);
    } 
    
    glPopMatrix();
  }

  
  // restore state
  glPopAttrib();
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(matrixMode);
  if (lightingOn) glEnable(GL_LIGHTING);
}

void wmMouse(int button, int state, int x, int y) 
{
  int i;

  btn=button;
  btnx=x;
  btny=y;
  
  if (button == GLUT_LEFT_BUTTON) { 
    for (i=0;i<nWindows;i++) {
      // mouse click over expand/collapse button?
      if (x > ws[i].bx && x < ws[i].bx+ws[i].bw &&
          y > ws[i].by && y < ws[i].by+ws[i].bh) {
        if (state == GLUT_DOWN) {
          ws[i].bstate=WM_BUTTON_DOWN;
          wmove=WM_FALSE;
          ws[i].oldx=ws[i].x;
          ws[i].oldy=ws[i].y;
        } else if (state == GLUT_UP && wmove == WM_FALSE) {
          ws[i].bstate=WM_BUTTON_UP;
          // expand/collapse window
          if (ws[i].state == WM_WINDOW_EXPANDED) {
            ws[i].state=WM_WINDOW_COLLAPSED;
            //ws[i].ans=nans;
          } else {
            ws[i].state=WM_WINDOW_EXPANDED;
            //ws[i].ans=1;
          }
        }
      }
    }
  }
}

void wmPassiveMouse(int x, int y) 
{
  int i;
  
  for (i=0;i<nWindows;i++) {
    // mouse over expand/collapse button?
    if (x > ws[i].bx && x < ws[i].bx+ws[i].bw &&
        y > ws[i].by && y < ws[i].by+ws[i].bh) {
      ws[i].bstate=WM_BUTTON_HOVER;
    } else {
      ws[i].bstate=WM_BUTTON_UP;
    }
  }
}

void wmActiveMouse(int x, int y) 
{
  int i;
  int dx, dy;
  
  for (i=0;i<nWindows;i++) {
    // expand/collapse button pressed?
    if (ws[i].moveable == WM_WINDOW_MOVEABLE && 
        ws[i].bstate == WM_BUTTON_DOWN && 
        btn == GLUT_LEFT_BUTTON) {
      wmove=WM_TRUE;
      
      // determine new location
      switch (ws[i].side) {
        case WM_WINDOW_LEFT:
        case WM_WINDOW_RIGHT:
          dy=ws[i].oldy-ws[i].y+y-btny;
          if (ws[i].y+dy >= 0 && ws[i].y+ws[i].h+dy < mwh) {
            ws[i].y=ws[i].y+dy;
            ws[i].by=ws[i].by+dy;
            ws[i].bly=ws[i].bly+dy;
          }
          break;
        case WM_WINDOW_TOP:
        case WM_WINDOW_BOTTOM:
          dx=ws[i].oldx-ws[i].x+x-btnx;
          if (ws[i].x+dx >= 0 && ws[i].x+ws[i].w+dx < mww) {
            ws[i].x=ws[i].x+dx;
            ws[i].bx=ws[i].bx+dx;
            ws[i].blx=ws[i].blx+dx;
          }
          break;
        case WM_WINDOW_FLOAT:
          dy=ws[i].oldy-ws[i].y+y-btny;
          dx=ws[i].oldx-ws[i].x+x-btnx;
          if (ws[i].y+dy >= 0 && ws[i].y+ws[i].h+dy < mwh) {
            ws[i].y=ws[i].y+dy;
            ws[i].by=ws[i].by+dy;
            ws[i].bly=ws[i].bly+dy;
          }
          if (ws[i].x+dx >= 0 && ws[i].x+ws[i].w+dx < mww) {
            ws[i].x=ws[i].x+dx;
            ws[i].bx=ws[i].bx+dx;
            ws[i].blx=ws[i].blx+dx;
          }
          break;
      }
      
      // determine new side
      if (x < 50) {
        ws[i].side=WM_WINDOW_LEFT;
      } else if (x > mww-50) {
        ws[i].side=WM_WINDOW_RIGHT;
      } else if (y < 50) {
        ws[i].side=WM_WINDOW_TOP;
      } else if (y > mwh-50) {
        ws[i].side=WM_WINDOW_BOTTOM;
      } else {
        ws[i].side=WM_WINDOW_FLOAT;
      }
      wmArrangeWindows(mww, mwh);       
    }
  }
}
