

#include "PndTpcRungeKutta.h"

#include <iostream>
#include <cmath>
#include "TVector3.h"
#include "PndTpcEFieldCyl.h" 
#include "PndMultiField.h"
#include "PndFieldMap.h"





PndTpcRungeKutta::PndTpcRungeKutta(const double timeStep,
				   const double maxRelError,
				   const double maxRelToSpeedError,
				   const double maxAbsoluteError,
				   const double specificCharge,
				   PndTpcEFieldCyl* eField,
				   PndMultiField* bField,
				   const double friction)
{
  fdt =timeStep;
  fepsilon = maxRelError;
  fepsilonDeriv = maxRelToSpeedError;
  fepsilonAbs = maxAbsoluteError;
  fsc = specificCharge;
  feField = eField;
  fbField = bField;
  fbFieldCyl = NULL;
  ffriction = friction;
  fconstE_flag = false;
  fconstB_flag = false;

  init();
  
}

PndTpcRungeKutta::PndTpcRungeKutta(const double timeStep,
				   const double maxRelError,
				   const double maxRelToSpeedError,
				   const double maxAbsoluteError,
				   const double specificCharge,
				   const char* eFieldFile,
				   const char* bFieldFile,
				   const double friction)
{
  fdt =timeStep;
  fepsilon = maxRelError;
  fepsilonDeriv = maxRelToSpeedError;
  fepsilonAbs = maxAbsoluteError;
  fsc = specificCharge;
  feField = new PndTpcEFieldCyl(eFieldFile);
  fbFieldCyl = new PndTpcEFieldCyl(bFieldFile);
  fbField=NULL;
  ffriction = friction;

  fconstE_flag = false;
  fconstB_flag = false;
  
  init();
  
}


PndTpcRungeKutta::~PndTpcRungeKutta()
{
  delete feField;
  if(fbField!=NULL) 
    delete fbField;
  if(fbFieldCyl!=NULL) 
    delete fbFieldCyl;
}

bool
PndTpcRungeKutta::stepForwards(double* aP)
{
  double aPcopy[6];
  for (int j=0; j<6; j++)
  {
    aPcopy[j]=aP[j];
  }
  //steping forwards by fdt
  double error[6];
  getNextPoint(aP,error);

  //defining the maximal allowed error
  double allowedError[6];
  double deriv[6];
  derivates(aPcopy, deriv);
  for (int j=0; j<6; j++)
  {
    allowedError[j] = fepsilon*std::fabs(aPcopy[j])+
      fepsilonDeriv*fdt* std::fabs(deriv[j]) +
      fepsilonAbs;
  }
  //is the estimated error to big?
  double worstDim =-1;
  for (int j=0; j<6; j++)
  {
    //avoid division through 0
    if (error[j] <= allowedError[j]/8. )
      continue;
    else if (worstDim > allowedError[j]/error[j] || worstDim == -1)
      worstDim = allowedError[j]/error[j];
  }
  if (worstDim == -1) //error was always small enough
    worstDim = 8;
  else if (worstDim <= 0) //allowedError was one time 0 (every error is to big)
  {
    worstDim = 1;// the step will be done because repeating is useless
    std::cout<<"Allowed Error was 0. Could not decrease step size appropriate!"<<std::endl;
  }
  //redifining the time step
  //case the error was ok
  if (worstDim >= 1)
  {
    //never make the stepsize to big
    if (worstDim > 7.6)
      fdt = 0.95*fdt*1.5;
    else
      fdt = 0.95*fdt*std::pow((worstDim),0.2);
    return(true);//the step was succesfull
  }
  //case the error was to big
  else
  {
    fdt=0.95*fdt*std::pow(worstDim,0.25);
    //reset the old values
    for (int j=0; j<6; j++)
    {
      aP[j] = aPcopy[j];
    }
  }
  return(false); //the stepping failed
}

void
PndTpcRungeKutta::getNextPoint(double* aP, double* errorEstimate)
{
  double k[6][6];
    
  TVector3 bFieldValue;
  double bFieldX, bFieldY, bFieldZ;
  
  if(!fconstB_flag){
    if(fbField!=0) {       //use PndMultiField 
      
      double pos_temp[3];
      double field_val[3];
      for(int i=0; i<3; i++)
	pos_temp[i] = aP[i+3]*100.;

      fbField->GetFieldValue(pos_temp, field_val);
      
      bFieldX = field_val[0]*0.1;
      bFieldY = field_val[1]*0.1;
      bFieldZ = field_val[2]*0.1;
      
      /*FairFieldMap takes coordinates in [cm] and returns B in [kGauss] -> *0.1 ! */
   
      // bFieldX = fbField->GetBx(aP[3]*100., aP[4]*100., aP[5]*100.)*0.1;  
      // bFieldY = fbField->GetBy(aP[3]*100., aP[4]*100., aP[5]*100.)*0.1;
      // bFieldZ = fbField->GetBz(aP[3]*100., aP[4]*100., aP[5]*100.)*0.1;
    }
    
    if(fbFieldCyl!=NULL) { //use PndTpcEFieldCyl for B field representation
      bFieldValue = fbFieldCyl->value(TVector3(aP[3]*100., aP[4]*100., aP[5]*100.));
      
      bFieldX = bFieldValue.X();
      bFieldY = bFieldValue.Y();
      bFieldZ = bFieldValue.Z();
    }
  }  
  else {
    bFieldValue = fconstB;      
    bFieldX = bFieldValue.X();
    bFieldY = bFieldValue.Y();
    bFieldZ = bFieldValue.Z();
  }
  
  fdglmat[0][1] = bFieldZ * fsc;
  fdglmat[0][2] = -bFieldY * fsc;
  fdglmat[1][0] = -bFieldZ * fsc;
  fdglmat[1][2] = bFieldX * fsc;
  fdglmat[2][0] = bFieldY * fsc;
  fdglmat[2][3] = -bFieldX * fsc;


  // read in E field ------------------------------------------------
  TVector3 fieldValue;
  if(! fconstE_flag) {
    // //fefield at the right point [V/m]
    fieldValue = feField->value(TVector3(aP[3]*100.,aP[4]*100.,aP[5]*100.))*100.;
  }
  else 
    fieldValue = fconstE;

  
  fdgladd[0] = fieldValue.X() *fsc;
  fdgladd[1] = fieldValue.Y() *fsc;
  fdgladd[2] = fieldValue.Z() *fsc;

  //fifth order Runge Kutta
  for (int i=0; i<6; i++)
  {
    double midP[6];
    for (int j=0; j<6; j++)
    {
      double bdotk =0;
      for (int l=0; l<i;l++)
	bdotk += frk5b[i][l]*k[l][j];
      midP[j] = aP[j] + bdotk; 
    }
    derivates(midP,k[i]);
  }
  
  //the advanced solution
  for (int i=0; i<6; i++)
  {
    //the advanced solution
    for (int j=0; j<6;j++)
      aP[i] += frk5c1[j]*k[j][i];
  }
  //the error estimate
  for (int i=0; i<6; i++)
  {
    errorEstimate[i]=0;
    for (int j=0; j<6; j++)
      errorEstimate[i] += std::fabs( (frk5c1[j]-frk5c2[j])*k[j][i]);
  }
}

void
PndTpcRungeKutta::derivates(const double* const aP, double* der)
{
  //run through rows
  for (int i = 0; i < 6; i++)
  {
    der[i] = 0;
    //run through columns
    for (int j = 0; j < 6; j++)
    {
      der[i] += fdglmat[i][j] * fdt * aP[j];
    }
    der[i] += fdgladd[i] * fdt;
  }
}


void
PndTpcRungeKutta::init() {
  
  double scpos = fsc;
  if (scpos < 0)
    scpos = -fsc;
  
  fdgladd[0]=0; fdgladd[1]=0;
  fdgladd[2]=0; fdgladd[3]=0;
  fdgladd[4]=0; fdgladd[5]=0;
  
  fdglmat[0][0]=-ffriction*scpos; fdglmat[0][1]=0; fdglmat[0][2]=0;
  fdglmat[0][3]=0; fdglmat[0][4]=0; fdglmat[0][5]=0;
  fdglmat[1][0]=0; fdglmat[1][1]=-ffriction*scpos; fdglmat[1][2]=0;
  fdglmat[1][3]=0; fdglmat[1][4]=0; fdglmat[1][5]=0;
  fdglmat[2][0]=0; fdglmat[2][1]=0; fdglmat[2][2]=-ffriction*scpos;
  fdglmat[2][3]=0; fdglmat[2][4]=0; fdglmat[2][5]=0;
  fdglmat[3][0]=1; fdglmat[3][1]=0; fdglmat[3][2]=0;
  fdglmat[3][3]=0; fdglmat[3][4]=0; fdglmat[3][5]=0;
  fdglmat[4][0]=0; fdglmat[4][1]=1; fdglmat[4][2]=0;
  fdglmat[4][3]=0; fdglmat[4][4]=0; fdglmat[4][5]=0;
  fdglmat[5][0]=0; fdglmat[5][1]=0; fdglmat[5][2]=1;
  fdglmat[5][3]=0; fdglmat[5][4]=0; fdglmat[5][5]=0;

  frk5b[0][0]=0; frk5b[0][1]=0; frk5b[0][2]=0; frk5b[0][3]=0;
  frk5b[0][4]=0;
  frk5b[1][0]=1./5.; frk5b[1][1]=0; frk5b[1][2]=0; frk5b[1][3]=0;
  frk5b[1][4]=0;
  frk5b[2][0]=3./40.; frk5b[2][1]=9./40.; frk5b[2][2]=0; frk5b[2][3]=0;
  frk5b[2][4]=0;
  frk5b[3][0]=3./10.; frk5b[3][1]=-9./10.; frk5b[3][2]=6./5.; frk5b[3][3]=0;
  frk5b[3][4]=0;
  frk5b[4][0]=-11./54.; frk5b[4][1]=5./2.; frk5b[4][2]=-70./27.; frk5b[4][3]=35./27.;
  frk5b[4][4]=0;
  frk5b[5][0]=1631./55296.; frk5b[5][1]=175./512.; frk5b[5][2]=575./13824.;
  frk5b[5][3]=44275./110592.; frk5b[5][4]=253./4096.;

  frk5c1[0]=37./378.; frk5c1[1]=0; frk5c1[2]=250./621.;
  frk5c1[3]=125./594.; frk5c1[4]=0; frk5c1[5]=512./1771.;
  
  frk5c2[0]=2825./27648.; frk5c2[1]=0; frk5c2[2]=18575./48384.;
  frk5c2[3]=13525./55296.; frk5c2[4]=277./14336.; frk5c2[5]=1./4.;
}

void
PndTpcRungeKutta::setConstE(TVector3 cE) {
  fconstE = cE;
  fconstE_flag=true;
}

void
PndTpcRungeKutta::setConstB(TVector3 cB) {
  fconstB = cB;
  fconstB_flag=true;
}
