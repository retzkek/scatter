#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "neutrons.h"
#include "explosions.h"

        
struct neutron *ns;

// global parameters
int mxn;    // max number of neutrons
double e0;  // initial neutron energy
int ntr;    // number of trails
int ng;     // number of energy groups for cross-sections
struct xs *xs;

void initNeutrons(int initNeutrons, int maxNeutrons, float initEnergy, int numTrails, int numGroups, struct xs *crossSections)
{
  int i;
  
  // assign global parameters
  mxn=maxNeutrons;
  e0=initEnergy;
  ntr=numTrails;
  ng=numGroups;
  xs=crossSections;
  
  ns=(struct neutron *) malloc(mxn*sizeof(*ns));  // allocate memory for neutron array
  for (i=0;i<mxn;i++) {
    ns[i].trail=(struct trail *) malloc(ntr*sizeof(*(ns[i].trail))); // allocate memory for trail array
    initNeutron(i,50.0);
  }
  for (i=initNeutrons;i<mxn;i++) ns[i].state=NEUTRON_DEAD;
}

void initNeutron(int n, double boundRad)
{
  int i;
  
  ns[n].state=NEUTRON_ALIVE;
  ns[n].x=(double)random()/RAND_MAX*boundRad*2-boundRad;
  for (i=0;i<ntr;i++) ns[n].trail[i].x=ns[n].x;
  ns[n].y=(double)random()/RAND_MAX*boundRad*2-boundRad;
  for (i=0;i<ntr;i++) ns[n].trail[i].y=ns[n].y;
  ns[n].z=(double)random()/RAND_MAX*boundRad*2-boundRad;
  for (i=0;i<ntr;i++) ns[n].trail[i].z=ns[n].z;
  scatterNeutron(n);
  ns[n].energy=e0;
  for (i=0;i<ntr;i++) ns[n].trail[i].rad=-1.0;
}

void drawNeutrons(int trails)
{
  int i;
  double r, g, b;
  int nt;
  double xt,yt,zt;

  // draw neutrons
  for (i=0;i<mxn;i++) {
    if (ns[i].state==NEUTRON_ALIVE) {
      hsv2rgb(4.0-ns[i].energy/e0*4.0,1.0,1.0,&r,&g,&b);
      glColor3d(r, g, b);
      glPointSize(40);
      glBegin( GL_POINTS );
        glVertex3f(ns[i].x, ns[i].y, ns[i].z);
      glEnd();
      /*glPushMatrix();
        glTranslated(ns[i].x, ns[i].y, ns[i].z);
        glutSolidSphere(0.5, 10, 10);
      glPopMatrix();*/
    }
  }
  
  // draw trails
  if (trails) {
    for (i=0;i<mxn;i++) {
      if (ns[i].state==NEUTRON_ALIVE || ns[i].state==NEUTRON_TRAIL_FADE) {
        // draw trails
        for (nt=0;nt<ntr;nt++) {
          if (ns[i].trail[nt].rad > 0.0) {
            glColor4f(0.2,0.5,0.6,0.5*(1-(float)nt/ntr)+0.05);
            glPointSize(ns[i].trail[nt].rad);
            glBegin( GL_POINTS );
              glVertex3f(ns[i].trail[nt].x, ns[i].trail[nt].y, ns[i].trail[nt].z);
            glEnd();
            /*glPushMatrix();
            {
              glTranslated(ns[i].hist[nt].x, ns[i].hist[nt].y, ns[i].hist[nt].z);
              glutSolidSphere(ns[i].histr[nt],10,10);
              //glutSolidSphere(1.0,10,10);
            }
            glPopMatrix();*/
          }
        }
      }
    }
  }
}

void updateNeutrons(double dt)
{
  int i, j, g;
  double p;
  double sigf, sigc, sigs, sigt;
  static int count=0;
  
  // update trails
  count++;
  if (count==1) {
    for (i=0;i<mxn;i++) {
      if (ns[i].state==NEUTRON_ALIVE) {
        for (j=ntr-1;j>0;j--) {
          ns[i].trail[j].x=ns[i].trail[j-1].x;
          ns[i].trail[j].y=ns[i].trail[j-1].y;
          ns[i].trail[j].z=ns[i].trail[j-1].z;
          ns[i].trail[j].rad=ns[i].trail[j-1].rad;
        }
        ns[i].trail[0].x=ns[i].x;
        ns[i].trail[0].y=ns[i].y;
        ns[i].trail[0].z=ns[i].z;
        ns[i].trail[0].rad=(double)random()/RAND_MAX*30+20;
      } else if (ns[i].state==NEUTRON_TRAIL_FADE) {
        for (j=ntr-1;j>0;j--) {
          ns[i].trail[j].x=ns[i].trail[j-1].x;
          ns[i].trail[j].y=ns[i].trail[j-1].y;
          ns[i].trail[j].z=ns[i].trail[j-1].z;
          ns[i].trail[j].rad=ns[i].trail[j-1].rad;
        }
        ns[i].trail[0].rad=-1;
        //for (j=0;j<ntr-1;j++) ns[i].trail[j].rad=ns[i].trail[j+1].rad;
        
        if (ns[i].trail[ntr-1].rad==-1.0) ns[i].state=NEUTRON_DEAD; //once all the trails are gone, we make the slot available
      }
    }
    count=0;
  }
    
  // update neutrons
  for (i=0;i<mxn;i++) {
    if (ns[i].state==NEUTRON_ALIVE) {
      // determine new location
      ns[i].x=ns[i].x+dt*ns[i].energy*ns[i].i;
      ns[i].y=ns[i].y+dt*ns[i].energy*ns[i].j;
      ns[i].z=ns[i].z+dt*ns[i].energy*ns[i].k;
    
      for (g=0;g<ng;g++) {
        if (ns[i].energy <= xs[g].emax) {
          sigf=xs[g].sigf;
          sigc=xs[g].sigc;
          sigs=xs[g].sigs;
          sigt=sigf+sigc+sigs;
          break;
        }
      }
    
      if ((double)random()/RAND_MAX <= dt*ns[i].energy*sigt) {
        // interaction!
        p=(double)random()/RAND_MAX*sigt;
        if (p <= sigf) {
          // fission
          ns[i].state=NEUTRON_TRAIL_FADE;
          addNeutron(ns[i].x,ns[i].y,ns[i].z);
          addNeutron(ns[i].x,ns[i].y,ns[i].z);
          addNeutron(ns[i].x,ns[i].y,ns[i].z);
          addExplosion(ns[i].x,ns[i].y,ns[i].z);

        } else if (p <= sigf+sigc) {
          // capture
          ns[i].state=NEUTRON_TRAIL_FADE;
        } else {
          // scatter
          scatterNeutron(i);
        }      
      }  
    }
  }
}

void scatterNeutron(int n)
{
  double ii, jj, kk;
  double norm;
 
  ii=(double)random()/RAND_MAX*100.0-50.0;
  jj=(double)random()/RAND_MAX*100.0-50.0;
  kk=(double)random()/RAND_MAX*100.0-50.0; 

  norm=sqrt(ii*ii+jj*jj+kk*kk);

  ns[n].i=ii/norm;
  ns[n].j=jj/norm;
  ns[n].k=kk/norm;
  ns[n].energy=ns[n].energy*((float)random()/RAND_MAX*0.5+0.5);
}

void addNeutron(double x, double y, double z)
{
  int nn;
  int found;
  
  // find "dead" neutron to use
  found=0;
  for (nn=0;nn<mxn;nn++)  {
    if (ns[nn].state==NEUTRON_DEAD) {
      found=1;
      break;
    }
  }
  if (found==0) nn=-1;
    
  if (nn>=0) {
    initNeutron(nn,1.0); // the bounds don't matter - we set the location below
    ns[nn].x=x;
    ns[nn].y=y;
    ns[nn].z=z;
  }
    
}

void hsv2rgb(double h, double s, double v, double* r, double* g, double* b)
{

  // H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
  // RGB are each returned on [0, 1].
  double m, n, f;
  int i;

  if(h == -1) {
    *r=v;
    *g=v;
    *b=v;
  }
  i = floor(h);
  f = h - (double)i;
  if(!(i & 1)) f = 1 - f; // if i is even
  m = v * (1 - s);
  n = v * (1 - s * f);
  switch (i) {
    case 6:
    case 0: 
      *r=v;
      *g=n;
      *b=m;
      break;
    case 1: 
      *r=n;
      *g=v;
      *b=m;
      break;
    case 2: 
      *r=m;
      *g=v;
      *b=n;
      break;
    case 3: 
      *r=m;
      *g=n;
      *b=v;    
      break;
    case 4: 
      *r=n;
      *g=m;
      *b=v;
      break;
    case 5: 
      *r=v;
      *g=m;
      *b=n;
  }

} 
  
