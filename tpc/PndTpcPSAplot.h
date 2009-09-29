#ifndef TPCPSAPLOT_H
#define TPCPSAPLOT_H

#include "TGraph.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TSystem.h"

#include "PndTpcSample.h"
#include "PndTpcDigi.h"
#include "PndTpcSignal.h"
#include "PndTpcFrontend.h"


#include<vector>
#include<string>

class PndTpcPSAplot {
 public:


  PndTpcPSAplot(std::vector<PndTpcSample*>*,
			 std::vector<PndTpcDigi*>*,
			 std::vector<PndTpcSignal*>*,
			 const PndTpcFrontend* const,
			 std::string);

  ~PndTpcPSAplot();

  void Draw();

 private:
  TGraph* fsampG;
  TGraph* fdigiG;
  TLine** fdigiLines;
  int fnDigiLines;
  TLine** fsigLines;
  int fnSigLines;
  std::string fname;
};



#endif
