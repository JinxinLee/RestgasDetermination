// -------------------------------------------------------------------------
// -----                PndRestGas source file                         -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include <stdlib.h>
#include "PndRestGas.h"

using namespace std;

// -----   Default constructor   ------------------------------------------
PndRestGas::PndRestGas()
{

}

// -----   Standard constructor   -----------------------------------------
PndRestGas::PndRestGas(char DDfname[80])
{

  // create DD function from data in DDfname
	LoadDD(DDfname);
	
}

// -----   Standard constructor   -----------------------------------------
PndRestGas::PndRestGas(char DDfname[80], Double_t zmin, Double_t zmax)
{

  // create DD function from data in DDfname
  LoadDD(DDfname);

	// set min/max considered extension of beam tube along z-axis
	SetzRange(zmin,zmax);
	
}

// -----   Destructor   ---------------------------------------------------
PndRestGas::~PndRestGas() {

  free(fDDvals);
	fDDvals=NULL;

}
// ------------------------------------------------------------------------
// return rest gas density at zval
TGraph*  fDensityGraph;
Double_t GetDDValue(Double_t *zval, Double_t *p) {
  
	Double_t y = fDensityGraph->Eval(zval[0]);
  return y;
	
} 

// .........................................................................
void PndRestGas::LoadDD(char DDfname[80])
{
  
	// read gas density values from the file
	Int_t npoints;
	Double_t zmin, zmax;
	fDDvals = ReadDD(DDfname, &npoints, &zmin, &zmax);
	SetzRange(zmin,zmax);

  // create function fDensityFun with DD
	fDensityGraph = new TGraph(npoints,fDDvals[0],fDDvals[1]);
	fDensityFun = new TF1("Rest Gas DD",GetDDValue,fzmin,fzmax,0);

}

// ------------------------------------------------------------------------
// read DD from file DDfname
// get number of points npoints, zmin, and zmax
Float_t** PndRestGas::ReadDD(char DDfname[80], Int_t* npoints,
  Double_t* zmin, Double_t* zmax)
{
  
  Int_t i;
	Float_t *x, *y;
  Float_t a, b;
  Float_t **values;
            
  /* BEGIN Ugly piece of code to count the lines ... */
  Int_t lines=0;
  
  FILE *file = fopen(DDfname, "r"); /* Get filename */
  int ch, prev = '\n' /* so empty files have no lines */;
  while ( (ch = fgetc(file)) != EOF ) /* Read all chars in the file. */ {
    if ( ch == '\n' ) {
      ++lines; /* Bump the counter for every newline. */
    }
    prev = ch; /* Keep a copy to later test whether... */
  }
  fclose(file);
  if ( prev != '\n' ) /* ...the last line did not end in a newline. */ {
    ++lines; /* If so, add one more to the total. */
  }
  
  *npoints = lines;
    
  /* END Ugly piece of code to count the lines ... */
  
  // allocate memory for density data ...
	x = (Float_t*) malloc(lines*sizeof(Float_t));
  y = (Float_t*) malloc(lines*sizeof(Float_t));
      
  // and fill with data from file
	FILE * fp = fopen(DDfname,"r");
  for(i=0;i<lines;i++) {
    fscanf(fp,"%f %f",&a,&b);
    x[i]=a;
    y[i]=b;
  }
  fclose(fp);
      
  // copy values to output
	values = (Float_t**) malloc(sizeof(Float_t*) * 2);
  values[0] = x;
  values[1] = y;
	
	// set zmin/zmax
	*zmin = x[0];
	*zmax = x[*npoints-1];
      
  return values;
     
}
  
// ------------------------------------------------------------------------
