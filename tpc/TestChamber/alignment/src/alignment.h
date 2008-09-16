#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include "detector.h"
#include "track.h"
#include "millepede.h"
#include <vector>
#include <TH1D.h>
#include <TH2D.h>
#include "defs.h"

class Alignment{
    std::vector<Detector*> detectors;
    std::vector<Track> tracks;
    std::vector<TH1D*> hists_det;
    std::vector<TH2D*> hists_det2;
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

    bool simulation;
    
    void generateTracks(int amount,
			double length=500,
			double startwidht=2,
			double startheight=2,
			double endheight=2,
			double endwidth=2);
      
    void readTracks();
    void readDetectors(bool simulation_);

   
public:
    Alignment(const Alignment& al){};
    Alignment(bool simulation_);
    Alignment(){
	Alignment(true);
    }
    ~Alignment(){};
     void doFit();

    
    
};
#endif
