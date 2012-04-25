#include <iostream>
#include "PndLmdAlignPar.h"
#include "FairParamList.h"
#include "TMath.h"
#include "TString.h"
PndLmdAlignPar::PndLmdAlignPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context),fshift_x(0),fshift_y(0),fshift_z(0),frotate_x(0),frotate_y(0),frotate_z(0)
{
  clear();
}

void PndLmdAlignPar::putParams(FairParamList* list)
{
  if(!list) return;
  list->add("num_sensors", (Int_t)fsensors);
  list->add("num_sides", (Int_t)fsides);
  list->add("num_planes", (Int_t)fplanes);
  for(int j=0;j<fplanes;j++){
    for(int i=0;i<fsensors;i++){
      int curpos = j*fsensors*fsides+i;
      TString name_shx = "shift_x_";
      name_shx +=j;
      name_shx +="_";
      name_shx +=i;
      list->add(name_shx, (Double_t)fshift_x[curpos]);
      TString name_shy = "shift_y_";
      name_shy +=j;
      name_shy +="_";
      name_shy +=i;
      list->add(name_shy, (Double_t)fshift_y[curpos]);
      TString name_shz = "shift_z_";
      name_shz +=j;
      name_shz +="_";
      name_shz +=i;
      list->add(name_shz, (Double_t)fshift_z[curpos]);
      TString name_rotx = "rotate_x_";
      name_rotx +=j;
      name_rotx +="_";
      name_rotx +=i;
      list->add(name_rotx, (Double_t)frotate_x[curpos]);
      TString name_roty = "rotate_y_";
      name_roty +=j;
      name_roty +="_";
      name_roty +=i;
      list->add(name_roty, (Double_t)frotate_y[curpos]);
      TString  name_rotz = "rotate_z_";
      name_rotz +=j;
      name_rotz +="_";
      name_rotz +=i;
      list->add(name_rotz, (Double_t)frotate_z[curpos]);
    }
  }
}

Bool_t PndLmdAlignPar::getParams(FairParamList* list)
{
  if (!list) return kFALSE;
 

  if (!list->fill("num_sensors", &fsensors)) return kFALSE;
  if (!list->fill("num_sides", &fsides)) return kFALSE;
  if (!list->fill("num_planes", &fplanes)) return kFALSE;
  fshift_x.resize(fsensors*fsides*fplanes,0);
  fshift_y.resize(fsensors*fsides*fplanes,0);
  fshift_z.resize(fsensors*fsides*fplanes,0);
  frotate_x.resize(fsensors*fsides*fplanes,0);
  frotate_y.resize(fsensors*fsides*fplanes,0);
  frotate_z.resize(fsensors*fsides*fplanes,0);


 for(int j=0;j<fplanes;j++){
    for(int i=0;i<fsensors;i++){
      int curpos = j*fsensors*fsides+i;
      TString name_shx = "shift_x_";
      name_shx +=j;
      name_shx +="_";
      name_shx +=i;
      if (!list->fill(name_shx, &fshift_x[curpos])) return kFALSE;
      TString name_shy = "shift_y_";
      name_shy +=j;
      name_shy +="_";
      name_shy +=i;
      if (!list->fill(name_shy, &fshift_y[curpos])) return kFALSE;
      TString name_shz = "shift_z_";
      name_shz +=j;
      name_shz +="_";
      name_shz +=i;
      if (!list->fill(name_shz, &fshift_z[curpos])) return kFALSE;
      TString name_rotx = "rotate_x_";
      name_rotx +=j;
      name_rotx +="_";
      name_rotx +=i;
      if (!list->fill(name_rotx, &frotate_x[curpos])) return kFALSE;
      TString name_roty = "rotate_y_";
      name_roty +=j;
      name_roty +="_";
      name_roty +=i;
      if (!list->fill(name_roty, &frotate_y[curpos])) return kFALSE;
      TString  name_rotz = "rotate_z_";
      name_rotz +=j;
      name_rotz +="_";
      name_rotz +=i;
      if (!list->fill(name_rotz, &frotate_z[curpos])) return kFALSE;
    }
  }
  return kTRUE;
}

ClassImp(PndLmdAlignPar);

void PndLmdAlignPar::Print()
{
  std::cout<<"LUMI Alignment Parameters for geometry with :"<<std::endl;
  std::cout<<"   Sensor per plane         = "<<fsensors<<std::endl;
  std::cout<<"   Sides with sensors (1=double-sided sensors) = "<<fsides<<std::endl;
  std::cout<<"   Planes        = "<<fplanes<<std::endl;
  std::cout<<"   fshift_x.size() = "<< fshift_x.size()<<std::endl;
  std::cout<<"   fshift_y.size() = "<< fshift_z.size()<<std::endl;
  std::cout<<"   fshift_z.size() = "<< fshift_z.size()<<std::endl;
  std::cout<<"   frotate_x.size() = "<< frotate_x.size()<<std::endl;
  std::cout<<"   frotate_y.size() = "<< frotate_y.size()<<std::endl;
  std::cout<<"   frotate_z.size() = "<< frotate_z.size()<<std::endl;
  return;
}
