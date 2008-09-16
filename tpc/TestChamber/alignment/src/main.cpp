#include "alignment.h"
#include "millepede.h"
#include "track.h"
#include "detector.h"
#include <iostream>
#include <vector>
#include <TMatrixT.h>
#include <TRandom3.h>
#include <TH1D.h>
#include <TH2D.h>
#include <string>
#include <TFile.h>
#include "defs.h"


using namespace std;

vector<Detector*> detectors;
vector<Track> tracks;
vector<TH1D*> hists_det;
vector<TH2D*> hists_det2;

 

int main(int argc, const char* argv[]){
    /*
     these 3 arrays are for sending information to millepede and for
     getting them back. It is to be said that the array par isnt used
     because you can get millepede to write the results to a file
     This is not yet implemented, but should be only adding another
     line calling the write part of millepede

    */
    float dergb[NGLB];     //!< vector of global derivatives
    float derlc[NPARTRCK]; //!< vector of local derivatives
    float par[NGLB];       //!< vector of parameters

    cout<<"alginment creation main"<<endl;
    Alignment* al = new Alignment(true);
    cout<<"alginment creation main after "<<endl;

    al->doFit();
    return 0;
}
