#ifndef neutrons_h
#define neutrons_h

#define NEUTRON_ALIVE       0
#define NEUTRON_TRAIL_FADE  1
#define NEUTRON_DEAD        2

struct trail {
  float x, y, z;
  float rad;
};

struct neutron {
  int state;
  float energy;
  float x,y,z;           // current location
  float i, j, k;         // direction vector
  struct trail *trail;    // trails
};

struct xs {
  float emax;
  float sigf;
  float sigc;
  float sigs;
};

float getKeff(void);
float getBinValue(int);
float getBinEnergy(int);
int getNumNeutrons(void);

void initNeutrons(int,int,float,int,int,struct xs*,int);
void drawNeutrons(int);
void updateNeutrons(float);

void initNeutron(int,float);
void scatterNeutron(int);
void addNeutron(float, float, float);

void hsv2rgb(float, float, float, float*, float*, float*);

  

#endif
