#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>

#include "neutrons.h"
#include "explosions.h"

#define K_HIST 10
        
struct neutron *ns;

// global parameters
int mxn;            // max number of neutrons
float e0;          // initial neutron energy
int ntr;            // number of trails
int ng;             // number of energy groups for cross-sections
struct xs *xs;      // cross sections
int nbins;          // number of energy bins

// statistics
float khist[K_HIST]; // number of neutrons 
int ncount;
float *binmax;      // max energy for each bin
int *bins;          // energy bins

float getKeff()
{
  int i;
  float keff;
  
  keff=0.0;
  for (i=0;i<K_HIST;i++) {
    keff+=khist[i];
  }
  keff=keff/K_HIST;
  return keff;
}

float getBinValue(int b)
{
  float frac;
  if (ncount > 0) {
    if (b==0) {
      frac=((float)bins[0]/ncount)/binmax[0];
    } else {
      frac=((float)bins[b]/ncount)/(binmax[b]-binmax[b-1]);
    }      
  } else {
    frac=0.0;
  }
  return frac;
}

float getBinEnergy(int b)
{
  float Eavg;
  if (b==0) {
    Eavg=binmax[b]/2.0;
  } else {
    Eavg=(binmax[b]+binmax[b-1])/2.0;
  }
  return Eavg;
}

int getNumNeutrons()
{
  int n;
  n=ncount;
  return n;
}

void initNeutrons(int initNeutrons, int maxNeutrons, float initEnergy, int numTrails, int numGroups, struct xs *crossSections, int numBins)
{
  int i;
  
  // assign global parameters
  mxn=maxNeutrons;
  e0=initEnergy;
  ntr=numTrails;
  ng=numGroups;
  xs=crossSections;
  nbins=numBins;
  
  ns=(struct neutron *) malloc(mxn*sizeof(*ns));  // allocate memory for neutron array
  for (i=0;i<mxn;i++) {
    ns[i].trail=(struct trail *) malloc(ntr*sizeof(*(ns[i].trail))); // allocate memory for trail array
    initNeutron(i,50.0);
  }
  for (i=initNeutrons;i<mxn;i++) ns[i].state=NEUTRON_DEAD; // kill non-initial neutrons
  
  // initialize statistics
  for (i=0;i<K_HIST;i++) khist[i]=1.0; // initialize neutron stats
  
  binmax=malloc(nbins*sizeof(*binmax));
  for (i=0;i<nbins;i++) {
    binmax[i]=e0*(i+1)/nbins; // set bin ranges
  }
  bins=malloc(nbins*sizeof(*bins));
  
}

void initNeutron(int n, float boundRad)
{
  int i;
  
  ns[n].state=NEUTRON_ALIVE;
  ns[n].x=(float)random()/RAND_MAX*boundRad*2-boundRad;
  for (i=0;i<ntr;i++) ns[n].trail[i].x=ns[n].x;
  ns[n].y=(float)random()/RAND_MAX*boundRad*2-boundRad;
  for (i=0;i<ntr;i++) ns[n].trail[i].y=ns[n].y;
  ns[n].z=(float)random()/RAND_MAX*boundRad*2-boundRad;
  for (i=0;i<ntr;i++) ns[n].trail[i].z=ns[n].z;
  ns[n].energy=0.0;
  scatterNeutron(n);
  ns[n].energy=e0*sin((float)random()/RAND_MAX*3.14159);
  for (i=0;i<ntr;i++) ns[n].trail[i].rad=-1.0;
}

void drawNeutrons(int trails)
{
  int i;
  float r, g, b;
  int nt;
  float xt,yt,zt;

  // draw neutrons
  for (i=0;i<mxn;i++) {
    if (ns[i].state==NEUTRON_ALIVE) {
      hsv2rgb(5.0-ns[i].energy/e0*5.0,1.0,1.0,&r,&g,&b);
      glColor3f(r, g, b);
      glPointSize(10);
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

void updateNeutrons(float dt)
{
  int i, j, g, b;
  float p;
  float sigf, sigc, sigs, sigt;
  static int count=0;
  float traildt=0.5;
  int n0, n1;
    
  // update trails
  count++;
  if (dt*count >= traildt) {
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
        ns[i].trail[0].rad=(float)random()/RAND_MAX*7+3;
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
  
  // initialize bins
  for (i=0;i<nbins;i++) bins[i]=0;
  
  n0=0;
  n1=0;
  // update neutrons
  for (i=0;i<mxn;i++) {
    if (ns[i].state==NEUTRON_ALIVE) {
      n0++;
      n1++;
      
      // add neutron to energy bin
      for (b=0;b<nbins;b++) {
        if (ns[i].energy <= binmax[b]) {
          bins[b]++;
          break;
        }
      }
      
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
    
      if ((float)random()/RAND_MAX <= dt*ns[i].energy*sigt) {
        // interaction!
        p=(float)random()/RAND_MAX*sigt;
        if (p <= sigf) {
          // fission
          n1+=2;
          ns[i].state=NEUTRON_TRAIL_FADE;
          addNeutron(ns[i].x,ns[i].y,ns[i].z);
          addNeutron(ns[i].x,ns[i].y,ns[i].z);
          addNeutron(ns[i].x,ns[i].y,ns[i].z);
          addExplosion(ns[i].x,ns[i].y,ns[i].z);

        } else if (p <= sigf+sigc) {
          // capture
          n1--;
          ns[i].state=NEUTRON_TRAIL_FADE;
        } else {
          // scatter
          scatterNeutron(i);
        }      
      }  
    }
  }
  for (i=K_HIST-1;i>0;i--) khist[i]=khist[i-1];
  if (n0 > 0) {
    khist[0]=(float)n1/n0;
  } else {
    khist[0]=0;
  }
  ncount=n0;
  
}

void scatterNeutron(int n)
{
  float ii, jj, kk;
  float norm;
 
  ii=(float)random()/RAND_MAX*100.0-50.0;
  jj=(float)random()/RAND_MAX*100.0-50.0;
  kk=(float)random()/RAND_MAX*100.0-50.0; 

  norm=sqrt(ii*ii+jj*jj+kk*kk);

  ns[n].i=ii/norm;
  ns[n].j=jj/norm;
  ns[n].k=kk/norm;
  ns[n].energy=ns[n].energy*((float)random()/RAND_MAX*0.5+0.5);
}

void addNeutron(float x, float y, float z)
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

void hsv2rgb(float h, float s, float v, float* r, float* g, float* b)
{

  // H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
  // RGB are each returned on [0, 1].
  float m, n, f;
  int i;

  if(h == -1) {
    *r=v;
    *g=v;
    *b=v;
  }
  i = floor(h);
  f = h - (float)i;
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

