//
// C++ Interface: PndLmdAlignPar
//
#ifndef PNDLMDALIGNPAR_H
#define PNDLMDALIGNPAR_H

#include <vector>
#include <TVector2.h>
#include <TString.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

using namespace std;
//! Alignment Parameter Class for LMD
class PndLmdAlignPar : public FairParGenericSet
  {
    public :
		PndLmdAlignPar (const char* name="PndLmdAlignPar",
                        const char* title="PndLmd alignment parameter",
                        const char* context="TestDefaultContext");
		~PndLmdAlignPar(void){};
		void clear(void){};
		void putParams(FairParamList* list);
		Bool_t getParams(FairParamList* list);
		
		void Print();
		/** Accessor functions **/
		Double_t GetShiftX(int j) const {return fshift_x[j];}
		Double_t GetShiftY(int j) const {return fshift_y[j];}
		Double_t GetShiftZ(int j) const {return fshift_z[j];}
		Double_t GetRotateX(int j) const {return frotate_x[j];}
		Double_t GetRotateY(int j) const {return frotate_y[j];}
		Double_t GetRotateZ(int j) const {return frotate_z[j];}
   
  private:
    //Planes parameters
    Int_t fsensors;//Number of sensors no one plane [one side]
    Int_t fsides;//Number of sides [=1 for double-sided sensors]
    Int_t fplanes;//Number of planes

    //Sensor parameters
    vector<double> fshift_x;
    vector<double> fshift_y;
    vector<double> fshift_z;
    vector<double> frotate_x;
    vector<double> frotate_y;
    vector<double> frotate_z;

    /* Double_t fshift_x_0_0,fshift_y_0_0,fshift_z_0_0; */
    /* Double_t fshift_x_0_1,fshift_y_0_1,fshift_z_0_1; */
    /* Double_t fshift_x_0_2,fshift_y_0_2,fshift_z_0_2; */
    /* Double_t fshift_x_0_3,fshift_y_0_3,fshift_z_0_3; */
    /* Double_t fshift_x_0_4,fshift_y_0_4,fshift_z_0_4; */
    /* Double_t fshift_x_0_5,fshift_y_0_5,fshift_z_0_5; */
    /* Double_t fshift_x_0_6,fshift_y_0_6,fshift_z_0_6; */
    /* Double_t fshift_x_0_7,fshift_y_0_7,fshift_z_0_7; */
    /* Double_t fshift_x_1_0,fshift_y_1_0,fshift_z_1_0; */
    /* Double_t fshift_x_1_1,fshift_y_1_1,fshift_z_1_1; */
    /* Double_t fshift_x_1_2,fshift_y_1_2,fshift_z_1_2; */
    /* Double_t fshift_x_1_3,fshift_y_1_3,fshift_z_1_3; */
    /* Double_t fshift_x_1_4,fshift_y_1_4,fshift_z_1_4; */
    /* Double_t fshift_x_1_5,fshift_y_1_5,fshift_z_1_5; */
    /* Double_t fshift_x_1_6,fshift_y_1_6,fshift_z_1_6; */
    /* Double_t fshift_x_1_7,fshift_y_1_7,fshift_z_1_7; */
    /* Double_t fshift_x_2_0,fshift_y_2_0,fshift_z_2_0; */
    /* Double_t fshift_x_2_1,fshift_y_2_1,fshift_z_2_1; */
    /* Double_t fshift_x_2_2,fshift_y_2_2,fshift_z_2_2; */
    /* Double_t fshift_x_2_3,fshift_y_2_3,fshift_z_2_3; */
    /* Double_t fshift_x_2_4,fshift_y_2_4,fshift_z_2_4; */
    /* Double_t fshift_x_2_5,fshift_y_2_5,fshift_z_2_5; */
    /* Double_t fshift_x_2_6,fshift_y_2_6,fshift_z_2_6; */
    /* Double_t fshift_x_2_7,fshift_y_2_7,fshift_z_2_7; */
    /* Double_t fshift_x_3_0,fshift_y_3_0,fshift_z_3_0; */
    /* Double_t fshift_x_3_1,fshift_y_3_1,fshift_z_3_1; */
    /* Double_t fshift_x_3_2,fshift_y_3_2,fshift_z_3_2; */
    /* Double_t fshift_x_3_3,fshift_y_3_3,fshift_z_3_3; */
    /* Double_t fshift_x_3_4,fshift_y_3_4,fshift_z_3_4; */
    /* Double_t fshift_x_3_5,fshift_y_3_5,fshift_z_3_5; */
    /* Double_t fshift_x_3_6,fshift_y_3_6,fshift_z_3_6; */
    /* Double_t fshift_x_3_7,fshift_y_3_7,fshift_z_3_7; */

    /* Double_t frotate_x_0_0,frotate_y_0_0,frotate_z_0_0; */
    /* Double_t frotate_x_0_1,frotate_y_0_1,frotate_z_0_1; */
    /* Double_t frotate_x_0_2,frotate_y_0_2,frotate_z_0_2; */
    /* Double_t frotate_x_0_3,frotate_y_0_3,frotate_z_0_3; */
    /* Double_t frotate_x_0_4,frotate_y_0_4,frotate_z_0_4; */
    /* Double_t frotate_x_0_5,frotate_y_0_5,frotate_z_0_5; */
    /* Double_t frotate_x_0_6,frotate_y_0_6,frotate_z_0_6; */
    /* Double_t frotate_x_0_7,frotate_y_0_7,frotate_z_0_7; */
    /* Double_t frotate_x_1_0,frotate_y_1_0,frotate_z_1_0; */
    /* Double_t frotate_x_1_1,frotate_y_1_1,frotate_z_1_1; */
    /* Double_t frotate_x_1_2,frotate_y_1_2,frotate_z_1_2; */
    /* Double_t frotate_x_1_3,frotate_y_1_3,frotate_z_1_3; */
    /* Double_t frotate_x_1_4,frotate_y_1_4,frotate_z_1_4; */
    /* Double_t frotate_x_1_5,frotate_y_1_5,frotate_z_1_5; */
    /* Double_t frotate_x_1_6,frotate_y_1_6,frotate_z_1_6; */
    /* Double_t frotate_x_1_7,frotate_y_1_7,frotate_z_1_7; */
    /* Double_t frotate_x_2_0,frotate_y_2_0,frotate_z_2_0; */
    /* Double_t frotate_x_2_1,frotate_y_2_1,frotate_z_2_1; */
    /* Double_t frotate_x_2_2,frotate_y_2_2,frotate_z_2_2; */
    /* Double_t frotate_x_2_3,frotate_y_2_3,frotate_z_2_3; */
    /* Double_t frotate_x_2_4,frotate_y_2_4,frotate_z_2_4; */
    /* Double_t frotate_x_2_5,frotate_y_2_5,frotate_z_2_5; */
    /* Double_t frotate_x_2_6,frotate_y_2_6,frotate_z_2_6; */
    /* Double_t frotate_x_2_7,frotate_y_2_7,frotate_z_2_7; */
    /* Double_t frotate_x_3_0,frotate_y_3_0,frotate_z_3_0; */
    /* Double_t frotate_x_3_1,frotate_y_3_1,frotate_z_3_1; */
    /* Double_t frotate_x_3_2,frotate_y_3_2,frotate_z_3_2; */
    /* Double_t frotate_x_3_3,frotate_y_3_3,frotate_z_3_3; */
    /* Double_t frotate_x_3_4,frotate_y_3_4,frotate_z_3_4; */
    /* Double_t frotate_x_3_5,frotate_y_3_5,frotate_z_3_5; */
    /* Double_t frotate_x_3_6,frotate_y_3_6,frotate_z_3_6; */
    /* Double_t frotate_x_3_7,frotate_y_3_7,frotate_z_3_7; */
  
    ClassDef(PndLmdAlignPar,5);
  };

#endif /* !PNDSDSSTRIPDIGIPAR_H*/

