#ifndef neutrons_h
#define neutrons_h

#define NEUTRON_ALIVE       0
#define NEUTRON_TRAIL_FADE  1
#define NEUTRON_DEAD        2

struct trail {
  double x, y, z;
  double rad;
};

struct neutron {
  int state;
  double energy;
  double x,y,z;           // current location
  double i, j, k;         // direction vector
  struct trail *trail;    // trails
};

struct xs {
  float emax;
  float sigf;
  float sigc;
  float sigs;
};

void initNeutrons(int,int,float,int,int,struct xs*);
void drawNeutrons(int);
void updateNeutrons(double);

void initNeutron(int,double);
void scatterNeutron(int);
void addNeutron(double, double, double);

void hsv2rgb(double, double, double, double*, double*, double*);


/*static const double sigma[3][4] =
  // speed         fiss   capture    scatter
{{0.5,  0.5,   0.02,     0.01},
{1.0,   0.04,  0.04,     0.07},
{2.0,       0.01,  0.01,     0.12}};
 
static const int nGroups = 3;
*/
#endif
