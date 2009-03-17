#include "../../src/TChough2.h"
#include "../../src/TCcluster.h"
#include <vector>
int main(){
  using namespace std;
  std::vector<TCcluster*> clusters;
  TCcluster* cl;
  TVector3 X(1.,0.,0.);
  TVector3 Y(0.,1.,0.);
  TVector3 Z(0.,0.,1.);
  cl = new TCcluster(X,X,2,1);
  clusters.push_back(cl);
  TChough2* a = new TChough2(Z,Y);
  
  delete a;

}
