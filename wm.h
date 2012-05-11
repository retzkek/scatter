#ifndef wm_h
#define wm_h

#define WM_WINDOW_LEFT 0
#define WM_WINDOW_TOP 1
#define WM_WINDOW_RIGHT 2
#define WM_WINDOW_BOTTOM 3
#define WM_WINDOW_FLOAT 4

#define WM_WINDOW_COLLAPSED 0
#define WM_WINDOW_EXPANDED 1

#define WM_WINDOW_UNMOVEABLE 0
#define WM_WINDOW_MOVEABLE 1

#define WM_WINDOW_UNCOLLAPSABLE 0
#define WM_WINDOW_COLLAPSABLE 1


void mwInit(int mainWindowWidth, int mainWindowHeight);
void wmAddWindow(const char* label, void (*drawFunc)(int w, int h), int width, int height, int side, int state, int moveable, int collapsable);
void wmSetWindowMouseFunc(void (*mouseFunc)(int x, int y));

void wmMouse(int button, int state, int x, int y);
void wmPassiveMouse(int x, int y);
void wmActiveMouse(int x, int y);

void wmDrawWindows(int w, int h);
void wmArrangeWindows(int w, int h);


#endif
