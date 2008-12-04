#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include "detector.h"
#include "track.h"
#include "millepede.h"
#include <vector>
#include <TH1D.h>
#include <TH2D.h>
#include "defs.h"
#include <string>
#include <TProfile.h>
#include "../../src/TCtrack.h"

class Alignment{
    std::vector<Detector*> detectors;
    std::vector<Track> tracks_sim;
    std::vector<TCtrack> tracks_real;
    std::vector<TH1D*> hists_det;
    std::vector<TProfile*> profiles_det;
    std::vector<int> lockedU;
    std::vector<int> lockedZ;
    std::vector<int> lockedT;
    std::vector<int> lockedP;
    bool alignU_;
    bool alignZ_;
    bool alignT_;
    bool alignP_;
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
    std::string infile;
    std::string outfile;
    bool simulation;
    float zero;
    
    void generateTracks(int amount,
			double length=150,
			double startwidht=2,
			double startheight=2,
			double endheight=2,
			double endwidth=2);
      
    void readTracks(std::string tracks);
    void readDetectors(bool simulation_);
    Alignment(){}
        
   
public:
    Alignment(const Alignment& al){};
    Alignment(std::string conffile);
    ~Alignment(){};
     void doFit();
 
    
    
};
#endif
