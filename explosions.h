#ifndef explosions_h
#define explosions_h

#define MAX_EXPLOSIONS 100
#define MAX_SIZE 5.0

struct explosion {
  float x, y, z;
  float size;
};

void addExplosion(float, float, float);
void drawExplosions(void);
void updateExplosions(void); 

#endif 
