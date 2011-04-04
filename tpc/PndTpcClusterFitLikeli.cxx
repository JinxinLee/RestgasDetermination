#include "PndTpcClusterFitLikeli.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcPadShapePool.h"
#include "PndTpcPRLookupTable.h"

#include <iostream>
using namespace std;

PndTpcClusterFitLikeli::PndTpcClusterFitLikeli(unsigned int nsigmaSteps,
					       double sigmaMin, double sigmaMax,
					       double irange, double step,
					       double intStep) :
  fdigis(), famps(), fnSigmaSteps(nsigmaSteps), fsigMin(sigmaMin), fsigMax(sigmaMax)
{
  
  cout << "Setting up PndTpcClusterFitLikeli... " << endl;
  const PndTpcPadShapePool* pool=PndTpcDigiMapper::getInstance()->getPadShapePool();
  //copy gem (we need to modify it)
  PndTpcGem gem(*(PndTpcDigiMapper::getInstance()->getGem()));
  double spread=gem.spread();
  double srange=sigmaMax-sigmaMin; assert(srange>0);
  double sstep=srange/(double)nsigmaSteps;
  // loop over PadShapes and 
  // create lookup tables for different charge cloud spreads
  unsigned int n=pool->numPadShapes();
  for(unsigned int i=0;i<n;++i){
    cout << "PadShape id="<<i<<endl;
    vector<PndTpcPRLookupTable*>* myLUTs=new vector<PndTpcPRLookupTable*>(nsigmaSteps);
    flookUp.push_back(myLUTs);
    PndTpcAbsPadShape* myShape=pool->GetPadShape(i);
    for(unsigned int is=0;is<nsigmaSteps;++is){// loop over steps in sigma
      double sigma=sigmaMin+(double)is * sstep;
      fSigmas[is]=sigma;
      cout << "Sigma= " << sigma << endl;
      // create new lookuptable
      (*myLUTs)[is]=new PndTpcPRLookupTable();
      // set convoluted spread
      gem.setspread(spread+sigma);
      // do integration
      (*myLUTs)[is]->BuildTable(gem,*myShape,
			     irange,step,intStep);
    } // end loop over steps in sigma
  } // end loop over padshapes

}
    
  
// Pad response function... interpolated for sigma
double 
PndTpcClusterFitLikeli::PRF(double x, double y, 
			    double sigma, unsigned int padShapeID){
  // get points for interpolation
  int ilow=floor((sigma-fsigMin)/fnSigmaSteps);
  // make sure we are inside boundaries
  if(ilow<0)ilow=0;
  int ihigh=ilow+1;
  if(ihigh>=flookUp[padShapeID]->size())ihigh=flookUp[padShapeID]->size()-1;
  if(ihigh==ilow)--ilow;

  double val1=(*flookUp[padShapeID])[ilow]->GetValue(x,y);
  double val2=(*flookUp[padShapeID])[ihigh]->GetValue(x,y);

  // linear interpolation
  return val1 + (sigma-fSigmas[ilow])/(fSigmas[ihigh]-fSigmas[ilow])*(val2-val1);
}


double 
PndTpcClusterFitLikeli::DoEval(const double* par) const {
  // parameters: par[0..4]
  // x, y, sig_x, sig_y, Amp

  // loop over pads
  double L=0;
  unsigned int n=fdigis->size();
  // loglikelihood to fit a 2D gaussian charge cloud convoluted 
  // with the padresponse function
  for(unsigned int i=0;i<n;++i){
    // Amp * PRF(x-xdigi,y-ydigi) - ampdigi

  }
 
  return L;
}
    
 
