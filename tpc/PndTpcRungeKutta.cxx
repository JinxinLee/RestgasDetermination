

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
			     const char* eFieldFile,
			     const char* bFieldFile,
			     const double friction)
{
  _dt =timeStep;
  _epsilon = maxRelError;
  _epsilonDeriv = maxRelToSpeedError;
  _epsilonAbs = maxAbsoluteError;
  _sc = specificCharge;
  _eField = new PndTpcEFieldCyl(eFieldFile);
  _bField = new PndTpcEFieldCyl(bFieldFile);
  
  //_bField->Print();
  double scpos = _sc;
  if (scpos < 0)
    scpos = -_sc;

  _dgladd[0]=0; _dgladd[1]=0;
  _dgladd[2]=0; _dgladd[3]=0;
  _dgladd[4]=0; _dgladd[5]=0;
  
  _dglmat[0][0]=-friction*scpos; _dglmat[0][1]=0; _dglmat[0][2]=0;
  _dglmat[0][3]=0; _dglmat[0][4]=0; _dglmat[0][5]=0;
  _dglmat[1][0]=0; _dglmat[1][1]=-friction*scpos; _dglmat[1][2]=0;
  _dglmat[1][3]=0; _dglmat[1][4]=0; _dglmat[1][5]=0;
  _dglmat[2][0]=0; _dglmat[2][1]=0; _dglmat[2][2]=-friction*scpos;
  _dglmat[2][3]=0; _dglmat[2][4]=0; _dglmat[2][5]=0;
  _dglmat[3][0]=1; _dglmat[3][1]=0; _dglmat[3][2]=0;
  _dglmat[3][3]=0; _dglmat[3][4]=0; _dglmat[3][5]=0;
  _dglmat[4][0]=0; _dglmat[4][1]=1; _dglmat[4][2]=0;
  _dglmat[4][3]=0; _dglmat[4][4]=0; _dglmat[4][5]=0;
  _dglmat[5][0]=0; _dglmat[5][1]=0; _dglmat[5][2]=1;
  _dglmat[5][3]=0; _dglmat[5][4]=0; _dglmat[5][5]=0;

  _rk5b[0][0]=0; _rk5b[0][1]=0; _rk5b[0][2]=0; _rk5b[0][3]=0;
  _rk5b[0][4]=0;
  _rk5b[1][0]=1./5.; _rk5b[1][1]=0; _rk5b[1][2]=0; _rk5b[1][3]=0;
  _rk5b[1][4]=0;
  _rk5b[2][0]=3./40.; _rk5b[2][1]=9./40.; _rk5b[2][2]=0; _rk5b[2][3]=0;
  _rk5b[2][4]=0;
  _rk5b[3][0]=3./10.; _rk5b[3][1]=-9./10.; _rk5b[3][2]=6./5.; _rk5b[3][3]=0;
  _rk5b[3][4]=0;
  _rk5b[4][0]=-11./54.; _rk5b[4][1]=5./2.; _rk5b[4][2]=-70./27.; _rk5b[4][3]=35./27.;
  _rk5b[4][4]=0;
  _rk5b[5][0]=1631./55296.; _rk5b[5][1]=175./512.; _rk5b[5][2]=575./13824.;
  _rk5b[5][3]=44275./110592.; _rk5b[5][4]=253./4096.;

  _rk5c1[0]=37./378.; _rk5c1[1]=0; _rk5c1[2]=250./621.;
  _rk5c1[3]=125./594.; _rk5c1[4]=0; _rk5c1[5]=512./1771.;
  
  _rk5c2[0]=2825./27648.; _rk5c2[1]=0; _rk5c2[2]=18575./48384.;
  _rk5c2[3]=13525./55296.; _rk5c2[4]=277./14336.; _rk5c2[5]=1./4.;
  
}

bool
PndTpcRungeKutta::stepForwards(double* aP)
{
  double aPcopy[6];
  for (int j=0; j<6; j++)
  {
    aPcopy[j]=aP[j];
  }
  //steping forwards by _dt
  double error[6];
  getNextPoint(aP,error);

  //defining the maximal allowed error
  double allowedError[6];
  double deriv[6];
  derivates(aPcopy, deriv);
  for (int j=0; j<6; j++)
  {
    allowedError[j] = _epsilon*std::fabs(aPcopy[j])+
      _epsilonDeriv*_dt* std::fabs(deriv[j]) +
      _epsilonAbs;
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
      _dt = 0.95*_dt*1.5;
    else
      _dt = 0.95*_dt*std::pow((worstDim),0.2);
    return(true);//the step was succesfull
  }
  //case the error was to big
  else
  {
    _dt=0.95*_dt*std::pow(worstDim,0.25);
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
    
  //_bfield at the right point
  //FairFieldMap takes coordinates in [cm] and returns B in [kGauss] -> *0.1 !

  double bFieldX, bFieldY, bFieldZ;
  //TVector3 bFieldValue = TVector3(0.,0.,0.);  
  TVector3 bFieldValue = _bField->value(TVector3(aP[3]*100., aP[4]*100., aP[5]*100.));

  bFieldX = bFieldValue.X();
  bFieldY = bFieldValue.Y();
  bFieldZ = bFieldValue.Z();


  _dglmat[0][1] = bFieldZ * _sc;
  _dglmat[0][2] = -bFieldY * _sc;
  _dglmat[1][0] = -bFieldZ * _sc;
  _dglmat[1][2] = bFieldX * _sc;
  _dglmat[2][0] = bFieldY * _sc;
  _dglmat[2][3] = -bFieldX * _sc;

  // //_efield at the right point [V/m]
  TVector3 fieldValue;
  fieldValue = _eField->value(TVector3(aP[3]*100.,aP[4]*100.,aP[5]*100.))*100.;

  // fieldValue = TVector3(0,0,40000); 
  
  _dgladd[0] = fieldValue.X() *_sc;
  _dgladd[1] = fieldValue.Y() *_sc;
  _dgladd[2] = fieldValue.Z() *_sc;

  //fifth order Runge Kutta
  for (int i=0; i<6; i++)
  {
    double midP[6];
    for (int j=0; j<6; j++)
    {
      double bdotk =0;
      for (int l=0; l<i;l++)
	bdotk += _rk5b[i][l]*k[l][j];
      midP[j] = aP[j] + bdotk; 
    }
    derivates(midP,k[i]);
  }
  
  //the advanced solution
  for (int i=0; i<6; i++)
  {
    //the advanced solution
    for (int j=0; j<6;j++)
      aP[i] += _rk5c1[j]*k[j][i];
  }
  //the error estimate
  for (int i=0; i<6; i++)
  {
    errorEstimate[i]=0;
    for (int j=0; j<6; j++)
      errorEstimate[i] += std::fabs( (_rk5c1[j]-_rk5c2[j])*k[j][i]);
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
      der[i] += _dglmat[i][j] * _dt * aP[j];
    }
    der[i] += _dgladd[i] * _dt;
  }
}

PndTpcRungeKutta::~PndTpcRungeKutta()
{
delete _eField;
delete _bField;
}
