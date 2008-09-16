// $Id: millepede.h,v 1.3 2006/06/16 15:21:49 conrad Exp $
#ifndef millepede_h
#define millepede_h

extern "C" {
  extern int initgl_(int*,int*,int*,int*);
  extern int parsig_(int*,float*);
  extern int constf_(float*,float*);
  extern int initun_(int*,float*);
  extern int zerloc_(float*,float*);
  extern int gener_(float*,float*,float*,float*,float*,float*,float*);
  extern int equloc_(float*,float*,float*,float*);
  extern int fitloc_();
  extern int fitglo_(float*);
  extern int parglo_(float*);
  extern int prtglo_(int*);
  extern float errpar_(int*);
}

static int iPar;
static float sig;
static float fRot[10000];
static int nGlobal, nLocal, nStdDev, printFlag;
static int lun;
static float dergb[10000],derlc[10000];

#define C_PARSIG(A,B) {iPar=A; sig=B; parsig_(&iPar,&sig);}
#define C_CONSTF(A,B) {fRot=A; sig=B; constf_(fRot,&sig);}
#define C_INITUN(A,B) {lun=A; sig=B; initun_(&lun,&sig);}
#define C_ZERLOC(A,B) {dergb=a; derlc=B; zerloc_(dergb,derlc);}
#define C_PRTGLO(A)   {lun=A; prtglo_(&lun);}

#define C_INITGL(A,B,C,D) \
{nGlobal=A; nLocal=B; nStdDev=C; printFlag=D; \
  initgl_(&nGlobal,&nLocal,&nStdDev,&printFlag);}        

#endif
