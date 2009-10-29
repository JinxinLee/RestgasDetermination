#include "PndTpcGas.h"
#include <ostream>
#include <iostream>
#include <fstream>

#include "TError.h"

//#include "ErrLogger/ErrLog.hh"

PndTpcGas::PndTpcGas():fE(0),
		 fB(0),
		 fT(293.),
		 fp(1024.),
		 fVDrift(0),
		 fDl(0),
		 fDt(0),
		 fk(0),
		 fW(0),
		 fCSD(0),		
		 fCSDEpol(0)
{}

PndTpcGas::PndTpcGas(double const E,
	       double const B,
	       double const T,
	       double const p,
	       double const xVDrift,
	       double const xDl,
	       double const xDt,
	       double const xk,
	       double const xW,
	       const std::vector<double>& xCSD,	   
	       double const xCSDEpol):fE(E),
				     fB(B),
				     fT(T),
				     fp(p),
				     fVDrift(xVDrift),
				     fDl(xDl),
				     fDt(xDt),
				     fk(xk),
				     fW(xW),
				     fCSD(xCSD),				   
				     fCSDEpol(xCSDEpol)
{}

PndTpcGas::PndTpcGas(const std::string& Filename,
	       double const E):fE(E)
{
  //Reading the GasFile
	std::cout<<"PndTpcGas: Reading data file: "<<Filename.c_str()<<std::endl;
  std::ifstream infile(Filename.c_str(), std::fstream::in);
  if (!infile.good())
    Fatal("PndTpcGas::PndTpcGas","Input File is not found");
  //ReadGasBegin returns the number of electric fields (and sets fT, fB, fp)
  int noent = ReadGasBegin(&infile);
  if (noent <= 0)
    Fatal("PndTpcGas::PndTpcGas","Number of electric fields nonpositive?\nCould not read File.\nExecution aborted.");
  double e[noent];                          //E field in V/cm
  double vdrift[noent];                     //e-drift velocity in cm/ns
  double dt[noent];                         //transverse diffusion in sqrt(cm)
  double dl[noent];                         //longitudinal diff. in sqrt(cm)
  double kk[noent];                          //attachment coefficient in 1/cm

  //ReadGasArrays returns the number of entries in cluster size distribution
  //and sets the values in the assigned arrays (and fW)
  int nCSDFile = ReadGasArrays(&infile, noent,
				e, vdrift, dt, dl, kk);
  if (nCSDFile < 2)
    Fatal("PndTpcGas::PndTpcGas","Number of cluster sizes too small in File.\nExecution aborted.");
 
  //cluster size distribution: first entry is the rel. number of clusters with
  //1 electron,the last entry the rel. nr. of clusters with more than nCSDFile
  //so the last entry may not be transferred
  int fnCSD = nCSDFile-1;            //omit the last
  fCSD.resize(fnCSD);
  for (int i=0; i < fnCSD;i++)infile >> fCSD[i];
  if (!infile.good())
    Fatal("PndTpcGas::PndTpcGas","Could not read cluster size distribution in File.\nExecution aborted.");
  infile.close();
  //file is read

  //Linear extrapolation to get the value at the right e field

  double inTable=GetPositionOfE(noent, e);
  /*The down rounded value of inTable is the index of the e field (in e)
    that is smaller than fE.
    (Exception: neg. Value , if fE is smaller than the smallest value in e)
    The decimal places render about the position between two entries in e*/

  if (inTable < 0 || inTable > noent -1)
    Warning("PndTpcGas::PndTpcGas","E field out of the range defined in input file");
  fVDrift = LinExpolation(inTable, vdrift, noent);
  fDl = LinExpolation(inTable, dl, noent);
  fDt = LinExpolation(inTable, dt, noent);
  fk = LinExpolation(inTable, kk, noent);
  
  //Define the constant for the quadratic CSD extrapolation in such a way
  //that the csd is continous (the inverse quadratic approximation
  //is rather improper anyhow)
  fCSDEpol = fCSD[fnCSD-1]*(fnCSD-1)*(fnCSD-1);
}

PndTpcGas::~PndTpcGas(){}

void
PndTpcGas::operator=(const PndTpcGas& GasToCopy)
{
  fVDrift = GasToCopy.fVDrift;
  fDl = GasToCopy.fDl;
  fDt = GasToCopy.fDt;
  fk = GasToCopy.fk;
  fW = GasToCopy.fW;
  fCSDEpol= GasToCopy.fCSDEpol;

  fE = GasToCopy.fE;
  fB = GasToCopy.fB;
  fT = GasToCopy.fT;
  fp = GasToCopy.fp;
  
  fCSD=GasToCopy.fCSD;
}

int
PndTpcGas::GetRandomCS(double const r) const {
  int i=0;
  double sum=fCSD[0];
  while(r>sum && ++i<fCSD.size()){
    sum+=fCSD[i];
  }
  if (fCSDEpol > 0)
    while(r>sum && i < 100)//sum could converge < 1
    {
      sum += fCSDEpol/i/i++;
    }
  return i+1;	//if r<=sum then 1 electron is in the cluster
}

void
PndTpcGas::SetCSD(const std::vector<double>& csd)
{
  fCSD=csd;
}

std::ostream& operator<< (std::ostream& stream, const PndTpcGas& g)
{
  stream << "--------------------------------------------------------\n"
	 << "PndTpc gas parameters: \n"
	 << "       drift velocity  VDrift="<<g.fVDrift<<"cm/ns \n"
	 << "       long.diffusion  Dl    ="<<g.fDl<<"sqrt(cm) \n"
	 << "       trans.diffusion Dt    ="<<g.fDt<<"sqrt(cm) \n"
	 << "       attachment      k     ="<<g.fk<<"1/cm\n"
	 << "       eff. ionisation W     ="<<g.fW<<"eV \n"
         << "       ClusterSizeDistribution CSD:";
  for (int i=0; i<g.nCSD();i++)
  {
    if (i%3 == 0)
      stream<<"\n       ";
    stream<<i+1<<": "<<g.fCSD[i]<<"  ";
  }
  stream << "\n"
         << "       extrapol. const CSDEpol="<<g.fCSDEpol 
	 << "\n"
	 << "Gas parameters given for this environment: \n"
	 << "       drift field     E="<<g.fE<<"V/cm \n"
	 << "       magnetic field  B="<<g.fB<<"T \n"
	 << "       pressure        p="<<g.fp<<"mbar \n"
	 << "       temperature     T="<<g.fT<<"K \n"
	 << "--------------------------------------------------------\n";
  return stream;
}

int
PndTpcGas::ReadGasBegin(std::ifstream* const pinfile)
{
  int noent = 0;                       //Nr. of E fields
  (*pinfile).ignore(256, '\n' );
  (*pinfile).ignore(256, '\n' );
  (*pinfile).ignore(256, '\n' );
  (*pinfile).ignore(256, ':');
  (*pinfile) >> fT;                        //temperature
  (*pinfile).ignore(256, ':');
  (*pinfile) >> fp;                        //pressure
  (*pinfile).ignore(256, ':');
  (*pinfile) >> fB;                        //b field
  (*pinfile).ignore(256, ':' );
  (*pinfile).ignore(256, ':');
  (*pinfile) >> noent;
  return(noent);
}

int
PndTpcGas::ReadGasArrays(std::ifstream* const pinfile, int const noent,
		      double* const e, double* const vdrift, double* const dt,
		      double* const dl, double* const kk)
{
  int nCSDFile = 0;                            //Nr. of ClusterSizes
  for (int i=0; i < noent;i++)
    (*pinfile) >> e[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> vdrift[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> dt[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> dl[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> kk[i];
  //ignore ion-mobility (in this version)
  (*pinfile).ignore(256, ':' );
  (*pinfile).ignore(256, '\n' );
  for (int i=0; i < noent;i++)
    (*pinfile).ignore(256, '\n' );
  //Cluster size distribution
  (*pinfile).ignore(512, ':' );
  (*pinfile) >>  fW;                                  //effective ionisation
  (*pinfile).ignore(512, ':');
  (*pinfile).ignore(256, ':');
  (*pinfile) >> nCSDFile;
  return(nCSDFile);
 }

double
PndTpcGas::GetPositionOfE(int const noent, const double* const e)
{
  double inTable=0;
  for (int i=0; i<noent; i++)
   {
     if (e[i] >= fE && i == 0)
     {
       inTable = i - (e[i]-fE)/(e[1]-e[0]);
       break;
     }
     if (i == noent-1 || 
 	(e[i] >= fE && i != 0))
     {
       inTable = i - (e[i]-fE)/(e[i]-e[i-1]);
       break;
     }
  }
  return(inTable);
}
double
PndTpcGas::LinExpolation(double const inTable, const double* const table,
			   int const nTable)
{
  if (nTable == 1)
    return(table[0]);
  else if (inTable <= 0)
    return(table[0] + inTable * (table[1]-table[0]));
  else if (inTable > nTable-1)
    return(table[nTable-1] + 
	   (inTable-nTable+1) * (table[nTable-1]-table[nTable-2]));
  else
    for (int i=1; i<nTable; i++)
    {
      if (i >= inTable)
	return(table[i] - (i-inTable) * (table[i]-table[i-1]));
    }
  return(0);//never reached; compiling without this I get an annoying warning 
}
