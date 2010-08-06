//======================================================================
// Description:
//       Class PndEmcStructure
//
//======================================================================


//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"		
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcXtal.h"

#include "TGeoMatrix.h"		
#include "TGeoManager.h"			
#include "TGeoArb8.h"
#include "TVector3.h"		
#include "TRotation.h"
#include "TMath.h"		
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TString.h"
#include "TObjString.h"

//---------------
// C++ Headers --
//---------------
#include "stdlib.h"
#include <iostream>
#include <fstream>				
//#include <string>
#include <cassert>		

using std::cout;
using std::endl;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
PndEmcStructure* PndEmcStructure::_instance = 0;

PndEmcStructure::~PndEmcStructure(){}
		
PndEmcStructure* PndEmcStructure::Instance (TGeoManager *geoMan)
{
	if (_instance == 0) {
		if (geoMan==0){
			cout<<"Empty geometry passed to PndEmcStructure"<<endl;
			abort(); 
      }
		else
			_instance = new PndEmcStructure(geoMan);
  }
  return _instance;
}

// If PndEmcStructure is called without input parameters
// it means that it should have been instantiated with TGeoManager before or use gGeoManager

PndEmcStructure* PndEmcStructure::Instance ()
{
	if (_instance == 0) {
		TGeoManager *geoMan = gGeoManager;
		if (geoMan==0){
			cout<<"gGeoManager does not exist"<<endl;
			abort(); 
      }
		else
			_instance = new PndEmcStructure(geoMan);
  }
  return _instance;
}

PndEmcStructure::PndEmcStructure(TGeoManager *geoMan)
{
  
	// Instantiate mapper to convert from detId to TCI 
	// PndEmcMapper should be initiated before
   PndEmcMapper *fEmcMap=PndEmcMapper::Instance();
  
	int module,copy,row,crystal;
	TString node_path;
	const char* crystal_name;
	const TGeoMatrix *crystal_matrix;
	const double *trans;
	TGeoRotation geoRot;
	TRotation rot;
	Double_t phi, theta, psi;
	int detId;
	
	TGeoIterator next(geoMan->GetTopVolume());
   TGeoNode *node;
   while ((node=next())) {
		next.GetPath(node_path);

		bool isEmcModule=crystal_name_analysis(node_path,module,copy,row,crystal);
		if (!isEmcModule) continue;
		
      detId =  module*100000000 + row*1000000 + copy*10000 + crystal; 
 //     cout<<"module == "<< module << ", detID == "<< detId <<endl;
      PndEmcTwoCoordIndex *tci=fEmcMap->GetTCI(detId);
		
		if (tci==0)
		{
			cout<<"Not found tci for index = "<<detId<<" in PndEmcStructure"<<endl;
		   abort();	
		}
		
		// Obtaine TGeoMatrix matrix for the current node, later it is factorised to translation and rotation
		crystal_matrix=next.GetCurrentMatrix();
		
		trans=crystal_matrix->GetTranslation();
		TVector3 pos(trans[0],trans[1],trans[2]);
		emcX[detId] = pos.X(); 
		emcY[detId] = pos.Y();
		emcZ[detId] = pos.Z();
		
//		cout<<"emcX["<<detId<<"]="<< emcX[detId] <<endl;
//		cout<<"emcY["<<detId<<"]="<< emcY[detId] <<endl;
//		cout<<"emcZ["<<detId<<"]="<< emcZ[detId] <<endl;
		geoRot.SetMatrix(crystal_matrix->GetRotationMatrix());
		
		PndEmcXtal *xtal;

        // Check the geometry type and convert to TGeoTrap.

        TString shapeType = node->GetVolume()->GetShape()->ClassName();
        if (shapeType == "TGeoTrap") {
            TGeoTrap *trap = dynamic_cast<TGeoTrap *>(
                node->GetVolume()->GetShape());
            
            xtal = new PndEmcXtal(tci, *trap, pos, geoRot);
        } else if (shapeType == "TGeoArb8") {
            TGeoArb8 *arb8 = dynamic_cast<TGeoArb8 *>(
                node->GetVolume()->GetShape());

            // Approximate the shape with a TGeoTrap.

            Double_t *verts = arb8->GetVertices();
            
            Double_t dz = arb8->GetDz();
            Double_t tx = (verts[4*2+0] + verts[5*2+0] + verts[6*2+0] + verts[7*2+0] - verts[0*2+0] - verts[1*2+0] - verts[2*2+0] - verts[3*2+0]) / (2 * 4 * dz);
            Double_t ty = (verts[4*2+1] + verts[5*2+1] + verts[6*2+1] + verts[7*2+1] - verts[0*2+1] - verts[1*2+1] - verts[2*2+1] - verts[3*2+1]) / (2 * 4 * dz);
            
            Double_t thetac = TMath::ATan(TMath::Sqrt(tx*tx + ty*ty)) * TMath::RadToDeg();
            Double_t phic = TMath::ATan2(ty, tx) * TMath::RadToDeg();
            
            Double_t h1 = (verts[1*2+1] + verts[2*2+1] - verts[0*2+1] - verts[3*2+1])/(2*2);
            Double_t bl1 = (verts[3*2+0] - verts[0*2+0]) / 2;
            Double_t tl1 = (verts[2*2+0] - verts[1*2+0]) / 2;
            Double_t alpha1 = TMath::ATan((verts[1*2+0]+verts[2*2+0]-verts[0*2+0]-verts[3*2+0])/(2*2*h1)) * TMath::RadToDeg();
            
            Double_t h2 = (verts[5*2+1] + verts[6*2+1] - verts[4*2+1] - verts[7*2+1])/(2*2);
            Double_t bl2 = (verts[7*2+0] - verts[4*2+0]) / 2;
            Double_t tl2 = (verts[6*2+0] - verts[5*2+0]) / 2;
            Double_t alpha2 = TMath::ATan((verts[5*2+0]+verts[6*2+0]-verts[4*2+0]-verts[7*2+0])/(2*2*h2)) * TMath::RadToDeg();
            
            TGeoTrap crystal_shape(dz, thetac, phic,
                h1, bl1, tl1, alpha1,
                h2, bl2, tl2, alpha2);
            
            // Check the conversion.
            //Double_t *verts2 = crystal_shape.GetVertices();
            //for (size_t i = 0; i < 8; ++i) {
            //    if (TMath::Abs(verts[i*2+0] - verts2[i*2+0]) > 1e-8
            //        || TMath::Abs(verts[i*2+1] - verts2[i*2+1]) > 1e-8) {
            //        cout << "TGeoArb8 conversion bad in module " << module << "  " << "   x " << verts[i*2+0] << "->" << verts2[i*2+0] << "     y "  << verts[i*2+1] << "->" << verts2[i*2+1] << endl;
            //    }
            //}
            
            xtal = new PndEmcXtal(tci, crystal_shape, pos, geoRot);
        } else if (shapeType == "TGeoBBox" || shapeType == "TGeoScaledShape") {
            TGeoBBox const *box = dynamic_cast<TGeoBBox const*>(
                node->GetVolume()->GetShape());
			
            // Convert to TGeoTrap.
            TGeoTrap crystal_shape(box->GetDZ(), 0, 0,
                box->GetDY(), box->GetDX(), box->GetDX(), 0,
                box->GetDY(), box->GetDX(), box->GetDX(), 0);
            
            xtal = new PndEmcXtal(tci,crystal_shape,pos,geoRot);
         } else {
            cout << "Unknown geometry type " << shapeType << " in module " << module << endl;
            abort();
        }

        fTciXtalMap[tci]=xtal;
   }
}

bool PndEmcStructure::crystal_name_analysis(TString node_path,int &module,int &copy,int &row,int &crystal)
{
        // The following code extract information about module, copy, row and crystal number from the path name of the TGeoNode which corresponds to emc crystal
	// Name convention is according to PndEmc.cxx

	if (!(node_path.Contains("emc") || node_path.Contains("Crystal") || node_path.Contains("FscModuleVolume"))) return false;
//	cout<<"1---mode_path="<<node_path<<endl;
	// Case of old Fsc
	if (node_path.Contains("Fsc_")){
		//at the moment all the layers of module in Fsc are not taken into account, only the whole block is selected
		if (node_path.Contains("FscLayer"))
			return false;
		TObjArray *subStrL = TPRegexp("^cave/Fsc_(\\d+)/emc(\\d+)r(\\d+)c(\\d+)_0$").MatchS(node_path);
		copy  = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
		module = (((TObjString *)subStrL->At(2))->GetString()).Atoi();
		row  = (((TObjString *)subStrL->At(3))->GetString()).Atoi();
		crystal  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();
		return true;
	}
	// Case of new Fsc
	if (node_path.Contains("FscModuleVolume")){
		//at the moment all the layers of module in Fsc are not taken into account, only the whole block is selected
//		cout<<"2---mode_path="<<node_path<<endl;
		if (node_path.Contains("FscLayer") || node_path.Contains("FscTyvek") || node_path.Contains("FscFibHole"))
			return false;
		TObjArray *subStrL = TPRegexp("^cave/Emc(\\d+)_(\\d+)/FscModuleVolume_(\\d+)$").MatchS(node_path);
	//	cout<<"3---mode_path="<<node_path<<endl;
		copy  = (((TObjString *)subStrL->At(2))->GetString()).Atoi()+1;
		module = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
		int ModCopy =  (((TObjString *)subStrL->At(3))->GetString()).Atoi();
		row  = ModCopy%100;
		crystal  = ModCopy/100;
	//	cout<<"copy="<<copy<<" module="<<module<<" row="<<row<<" crystal"<<crystal<<endl;
		return true;
	}
	
	// Case of barrel
	if (node_path.Contains("EmcLayer")){
		TObjArray *subStrL = TPRegexp("^cave/Emc\\d+_(\\d+)/EmcLayer\\d_0/emc(\\d+)r(\\d+)c(\\d+)_0$").MatchS(node_path);
		if(subStrL->GetLast()<4){
			cout<<"crystal name in barrel Emc: "<<node_path<<" missmatch pattern"<<endl;
			return false;
		}

		copy  = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
		module = (((TObjString *)subStrL->At(2))->GetString()).Atoi();
		row  = (((TObjString *)subStrL->At(3))->GetString()).Atoi();
		crystal  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();
	}
	//case of test calorimeter
	else if (node_path.Contains("EmcTest")) {
	  TObjArray *subStrL = TPRegexp("^cave/EmcTest_(\\d+)/emc(\\d+)r(\\d+)c(\\d+)_0$").MatchS(node_path);
	  if(subStrL->GetLast()<4){
	    cout<<"crystal name in test Emc: "<<node_path<<" missmatch pattern"<<endl;
	    return false;
	  }
	  copy  = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
	  module = (((TObjString *)subStrL->At(2))->GetString()).Atoi();
	  row  = (((TObjString *)subStrL->At(3))->GetString()).Atoi();
	  crystal  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();
	}

   	//case new version of forward end-cap (from "emc_module3new.root" file)
   else if (node_path.Contains("QuarterVol")) { 

     TObjArray *subStrL = TPRegexp("^cave/Emc3_0/QuarterVol(\\d+)_(\\d+)/SubunitVol(\\d+)_(\\d+)/BoxVol(\\d+)_(\\d+)/CrystalVol(\\d+)_(\\d+)$").MatchS(node_path);

     if(subStrL->GetLast()<4){
	 cout<<"crystal name in NEW FwEndCap Emc: "<<node_path<<" missmatch pattern"<<endl;
	 return false;
      }

      copy  = (((TObjString *)subStrL->At(2))->GetString()).Atoi();

      Int_t copyNoSub = (((TObjString *)subStrL->At(4))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
      Int_t copyNoBox  = (((TObjString *)subStrL->At(6))->GetString()).Atoi();
      Int_t copyNoCrys  = (((TObjString *)subStrL->At(8))->GetString()).Atoi();

      Int_t col=0, k1=0, nRow=-1, nCrys=-1;
      Int_t subrow=4;   // 4 crystals in each subvolume
      Int_t next=0;     // starts (from the middle) next column 
      
    if((copyNoSub >=  0) && (copyNoSub <=  6)){
       next  = copyNoSub + 2;
       col   = 0;
    }else if((copyNoSub >=  7) && (copyNoSub <= 13)){
       next  = (copyNoSub-7) +2;
      col   = 1;
    }else if((copyNoSub >= 14) && (copyNoSub <= 19)){
      next  = (copyNoSub-14) +2;
      col   = 2;
    }else if((copyNoSub >= 20) && (copyNoSub <= 26)){
      next  = (copyNoSub-20) +1;
      col   = 3;
    }else if((copyNoSub >= 27) && (copyNoSub <= 34)){
      next  = (copyNoSub-27);
      col   = 4;
    }else if((copyNoSub >= 35) && (copyNoSub <= 41)){
      next  = (copyNoSub-35);
      col   = 5;
    }else if((copyNoSub >= 42) && (copyNoSub <= 47)){
      next  = (copyNoSub-42);
      col   = 6;
    }else if((copyNoSub >= 48) && (copyNoSub <= 52)){
      next  = (copyNoSub-48);
      col   = 7;
    }else if((copyNoSub >= 53) && (copyNoSub <= 54)){
      next  = (copyNoSub-53);
      col   = 8;
    }

      Int_t flag=1;
      
      if (next<2  && col <3) flag=0; // 6 copyNoSub in the beam-pipe area
      if (next==0 && col==3) flag=0; // 7th copyNoSub in the beam-pipe area
      if (col>7  && next >1) flag=0; // empty copyNoSub in the residual area
      if (col>6  && next >4) flag=0; //  -||-
      if (col>5  && next >5) flag=0; //  -||-
      if (col>4  && next >6) flag=0; //  -||-
      if (col>1  && next >7) flag=0; //  -||-
      
     //18.02.09
     if (flag){
       if ( (copyNoBox == 0)  || (copyNoBox == 3) ){
       	 if(copyNoCrys == 1 || copyNoCrys == 3){ 
	   nCrys = next*4 + 3;
	 }else if (copyNoCrys == 0 || copyNoCrys == 2){
	   nCrys = next*4 + 4;
      	 }
       }else if ( (copyNoBox == 1)  || (copyNoBox == 2) ){
       	 if(copyNoCrys == 0 || copyNoCrys == 2){ 
	   nCrys = next*4 + 2;
	 }else if (copyNoCrys == 1 || copyNoCrys == 3){
	   nCrys = next*4 + 1; 
	 }
       }
       if ( (copyNoBox == 0)  || (copyNoBox == 2) ){
	 if(copyNoCrys == 0 || copyNoCrys == 3){ 
	   nRow = subrow*col + 4;
	 }else if (copyNoCrys == 1 || copyNoCrys == 2){
	   nRow = subrow*col + 3;
	 }
       }else if ( (copyNoBox == 1)  || (copyNoBox == 3) ){
	 if(copyNoCrys == 0 || copyNoCrys == 3){ 
	   nRow = subrow*col + 2;
	 }else if (copyNoCrys == 1 || copyNoCrys == 2){
	   nRow = subrow*col + 1; 
	 }
       }
     }

      module = 3;
      row = nRow;
      crystal = nCrys;

   }
    	//case new version of backward end-cap (from "emc_module4new.root" file) - 9.10.2008
   else if (node_path.Contains("Quarter4Vol")) { 

     TObjArray *subStrL;
	  
     if (node_path.Contains("SubunitVol_")){
       subStrL = TPRegexp("^cave/Emc4_0/Quarter4Vol_(\\d+)/SubunitVol_(\\d+)/BoxVol_(\\d+)/CrystalVol_(\\d+)$").MatchS(node_path);
     }else{
       if (node_path.Contains("BoxVol_")){
	 subStrL = TPRegexp("^cave/Emc4_0/Quarter4Vol_(\\d+)/SubunitVol(\\d+)_(\\d+)/BoxVol_(\\d+)/CrystalVol_(\\d+)$").MatchS(node_path);
       }else{
	 subStrL = TPRegexp("^cave/Emc4_0/Quarter4Vol_(\\d+)/SubunitVol(\\d+)_(\\d+)/BoxVol(\\d+)_(\\d+)/CrystalVol_(\\d+)$").MatchS(node_path);
       }
     }
     
     copy  = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
     //cout << "4: copy "<<copy<<endl;
     
     Int_t copyNoSub;
     Int_t copyNoBox;
     Int_t copyNoCrys;  
     
     if (node_path.Contains("SubunitVol_")){
       copyNoSub = (((TObjString *)subStrL->At(2))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
       copyNoBox  = (((TObjString *)subStrL->At(3))->GetString()).Atoi();
       copyNoCrys  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();
       
     }else{
       if (node_path.Contains("BoxVol_")){		  
	 copyNoSub = (((TObjString *)subStrL->At(3))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
	 copyNoBox  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();
	 copyNoCrys  = (((TObjString *)subStrL->At(5))->GetString()).Atoi();
	 
       }else{
	 copyNoSub = (((TObjString *)subStrL->At(3))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
	 copyNoBox  = (((TObjString *)subStrL->At(5))->GetString()).Atoi();
	 copyNoCrys  = (((TObjString *)subStrL->At(6))->GetString()).Atoi();
       }
     }
     
     Int_t col=0, k1=0, nRow=-1, nCrys=-1;
     Int_t subrow=4;   // 4 crystals in each subvolume
     Int_t next=0;     // starts (from the middle) next column 
	  
     // Below: for BwEndCap we have 13 subunits
     //  Now, 26.02.2009, 3 crystals in the middle's subunit are added =>
     // => number of Subunits for BwEncCap & straight geometry is the same   
     if((copyNoSub >=  0) && (copyNoSub <=  3)){
       next  = copyNoSub;
       col   = 0;
     }else if((copyNoSub >=  4) && (copyNoSub <= 7)){
       next  = (copyNoSub-4);
       col   = 1;
     }else if((copyNoSub >= 8) && (copyNoSub <= 10)){
       next  = (copyNoSub-8);
       col   = 2;
     }else if((copyNoSub >= 11) && (copyNoSub <= 12)){
       next  = (copyNoSub-11);
       col   = 3;
     }

      Int_t flag4=1;
      // 26.02.2009
      // "next" means "row", "col" means "col"

      if (next>1  && col>2) flag4=0; // corner's subunit + one below
      if (next>2  && col>1) flag4=0; // + one from the corner to the left
      
      if (flag4!=0){
	if ( (copyNoBox == 0)  || (copyNoBox == 3) ){
	  if(copyNoCrys == 1 || copyNoCrys == 3){ 
	    nCrys = next*4 + 3;
	  }else if (copyNoCrys == 0 || copyNoCrys == 2){
	    nCrys = next*4 + 4;
	  }
	}else if ( (copyNoBox == 1)  || (copyNoBox == 2) ){
	  if(copyNoCrys == 0 || copyNoCrys == 2){ 
	    nCrys = next*4 + 2;
	  }else if (copyNoCrys == 1 || copyNoCrys == 3){
	    nCrys = next*4 + 1; 
	  }
	}
	if ( (copyNoBox == 0)  || (copyNoBox == 2) ){
	  if(copyNoCrys == 0 || copyNoCrys == 3){ 
	    nRow = subrow*col + 4;
	  }else if (copyNoCrys == 1 || copyNoCrys == 2){
	    nRow = subrow*col + 3;
	  }
	}else if ( (copyNoBox == 1)  || (copyNoBox == 3) ){
	  if(copyNoCrys == 0 || copyNoCrys == 3){ 
	    nRow = subrow*col + 2;
	  }else if (copyNoCrys == 1 || copyNoCrys == 2){
	    nRow = subrow*col + 1; 
	  }
	}
      }
      module = 4;
      row = nRow;
      crystal = nCrys;

   }
 else if (node_path.Contains("QuarterNewVol")){
      module = 4;

     TObjArray *subStrL;
	  
     if (node_path.Contains("SubunitVol_")){
       subStrL = TPRegexp("^cave/Emc4_0/QuarterNewVol_(\\d+)/SubunitVol_(\\d+)/BoxVol_(\\d+)/CrystalVol_(\\d+)$").MatchS(node_path);
     }else{
       if (node_path.Contains("BoxVol_")){
	 subStrL = TPRegexp("^cave/Emc4_0/QuarterNewVol_(\\d+)/SubunitVol(\\d+)_(\\d+)/BoxVol_(\\d+)/CrystalVol_(\\d+)$").MatchS(node_path);
       }else{
	 subStrL = TPRegexp("^cave/Emc4_0/QuarterNewVol_(\\d+)/SubunitVol(\\d+)_(\\d+)/BoxVol(\\d+)_(\\d+)/CrystalVol_(\\d+)$").MatchS(node_path);
       }
     }
     
     copy  = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
     
     Int_t copyNoSub;
     Int_t copyNoBox;
     Int_t copyNoCrys;  
     
     if (node_path.Contains("SubunitVol_")){
       copyNoSub = (((TObjString *)subStrL->At(2))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
       copyNoBox  = (((TObjString *)subStrL->At(3))->GetString()).Atoi();
       copyNoCrys  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();
     }else
       if (node_path.Contains("BoxVol_")){  
	 copyNoSub = (((TObjString *)subStrL->At(3))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
	 copyNoBox  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();
	 copyNoCrys  = (((TObjString *)subStrL->At(5))->GetString()).Atoi();
       }else{
	 copyNoSub = (((TObjString *)subStrL->At(3))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
	 copyNoBox  = (((TObjString *)subStrL->At(5))->GetString()).Atoi();
	 copyNoCrys  = (((TObjString *)subStrL->At(6))->GetString()).Atoi();
       }
     
     Int_t col=0, nRow=-1, nCrys=-1;
     Int_t next=0;     // starts (from the middle) next column 
	  
     // Below: for BwEndCap we have 13 subunits
     //  Now, 26.02.2009, 3 crystals in the middle's subunit are added =>
     // => number of Subunits for BwEncCap & straight geometry is the same   
      if((copyNoSub >=  0) && (copyNoSub <=  2)){
	next  = copyNoSub+1;
	col   = 0;
      }else if((copyNoSub >=  3) && (copyNoSub <= 6)){
	next  = (copyNoSub-3);
	col   = 1;
      }else if((copyNoSub >= 7) && (copyNoSub <= 10)){
	next  = (copyNoSub-7);
	col   = 2;
      }else if((copyNoSub >= 11) && (copyNoSub <= 13)){
	next  = (copyNoSub-11);
	col   = 3;
      }
//       cout << "copyNoSub= " << copyNoSub << " copyNoBoxs= " << copyNoBox << " copyNoCrys= " << copyNoCrys << endl;

      Int_t flag4=1;
      // 26.02.2009
      // "next" means "row", "col" means "col"

      if (next==3  && col==3) flag4=0; // corner's subunit + one below
      if (next==0  && col==0) flag4=0; // + one from the corner to the left
//       cout << "next= " << next << " col= " << col << endl;
      if (flag4!=0){
	if ( (copyNoBox == 0)  || (copyNoBox == 3) ){
	  if(copyNoCrys == 1 || copyNoCrys == 3){ 
	    nCrys = next*4 + 3;
	  }else if (copyNoCrys == 0 || copyNoCrys == 2){
	    nCrys = next*4 + 4;
	  }
	}else if ( (copyNoBox == 1)  || (copyNoBox == 2) ){
	  if(copyNoCrys == 0 || copyNoCrys == 2){ 
	    nCrys = next*4 + 2;
	  }else if (copyNoCrys == 1 || copyNoCrys == 3){
	    nCrys = next*4 + 1; 
	  }
	}
	if ( (copyNoBox == 0)  || (copyNoBox == 2) ){
	  if(copyNoCrys == 0 || copyNoCrys == 3){ 
	    nRow = col*4 + 4;
	  }else if (copyNoCrys == 1 || copyNoCrys == 2){
	    nRow = col*4 + 3;
	  }
	}else if ( (copyNoBox == 1)  || (copyNoBox == 3) ){
	  if(copyNoCrys == 0 || copyNoCrys == 3){ 
	    nRow = col*4 + 2;
	  }else if (copyNoCrys == 1 || copyNoCrys == 2){
	    nRow = col*4 + 1; 
	  }
	}
      }
//       cout << "ncrys= " << nCrys << " nRow= " << nRow << endl;
      module = 4;
      row = nRow;
      crystal = nCrys;

//       cout << "QNewVol module= "<< module << " row= "<< row<< " copy= "<<copy<<" crys= "<< crystal<< endl;
   }
	else if(node_path.Contains("EmcProto")){
		module=7;
		copy=1;
		TObjArray *subStrL = TPRegexp("^cave/EmcProto_0/emc07r(\\d+)c(\\d+)_0$").MatchS(node_path);
		if(subStrL->GetLast()<2){
			cout<<"crystal name in  Emc Proto: "<<node_path<<" missmatch pattern"<<endl;
			return false;
		}
		row = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
		crystal = (((TObjString *)subStrL->At(2))->GetString()).Atoi();

	}
	else if(node_path.Contains("Proto60")){
		if(node_path.Contains("Passive") || node_path.Contains(TRegexp(".*PartAss_[0-9]*$"))  ){
			return kFALSE;
		}
		module=7;
		copy=1;
		Int_t type=1;
		TObjArray *subStrL = TPRegexp("^cave/Proto60_0/Active_1/Row(\\d+)_1/CrystalType6aoPartAss_(\\d+)/CrystalType6a_1$").MatchS(node_path);
		if(subStrL->GetLast()<2){
			subStrL = TPRegexp("^cave/Proto60_0/Active_1/Row(\\d+)_1/CrystalType6boPartAss_(\\d+)/CrystalType6b_1$").MatchS(node_path);
			type=2;
		} 
		if(subStrL->GetLast()<2){
			cout<<"crystal name in  Emc Proto: "<<node_path<<" missmatch pattern"<<endl;
			return false;
		}
		row= (((TObjString *)subStrL->At(1))->GetString()).Atoi();
		crystal = (((TObjString *)subStrL->At(2))->GetString()).Atoi();
		crystal = ((crystal-1)%5)*2 +type;
//        printf("found node: %s\n Rowstring: %s crytalstring:%s\nrow: %d crystal: %d\n",node_path.Data(),((TObjString *)subStrL->At(1))->GetName(),((TObjString *)subStrL->At(2))->GetName(),row,crystal);



	}
   //case of endcups of forward calorimeter
        else {

      TObjArray *subStrL = TPRegexp("^cave/Emc\\d_(\\d+)/emc(\\d+)r(\\d+)c(\\d+)_0$").MatchS(node_path);
	  if(subStrL->GetLast()<4){
	    cout<<"crystal name in endcup Emc: "<<node_path<<" missmatch pattern"<<endl;
	    return false;
	  }
	  copy  = (((TObjString *)subStrL->At(1))->GetString()).Atoi();
	  module = (((TObjString *)subStrL->At(2))->GetString()).Atoi();
	  row  = (((TObjString *)subStrL->At(3))->GetString()).Atoi();
	  crystal  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();

     }
	
	return true;
}


PndEmcTwoCoordIndex* 
PndEmcStructure::locateIndex( double theta, double phi ) const
{
	// Find crystal with minimal angular difference with given direction
	TVector3 vec(0,0,10);
	vec.SetTheta(theta);
	vec.SetPhi(phi);
	PndEmcTwoCoordIndex *tci;
	double diff=1000;
	     
// 	std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
        PndEmcTciXtalMap::const_iterator iter=fTciXtalMap.begin();
	while(iter!=(fTciXtalMap).end())
	{
		TVector3 vec2= ((*iter).second)->frontCentre();
		double tmpDiff=vec2.Angle(vec);

		if(tmpDiff<diff)
		{
			tci=(*iter).first;
			diff=tmpDiff;
		}
		
		iter++;
	}
	
	return tci;
}


// Print information on angular position for all the crystall into the file
void PndEmcStructure::Print(string filename, Int_t option) const
{
	ofstream f(filename.c_str());
	//different option corresponds to different details level of output 
	if (option==1)
	{
		f<<"detID"<<"\t"<<"ThetaInd"<<"\t"<<"PhiInd"<<"\t"<<"theta_centre"<<"\t"<<"theta_frontface"<<"\t"<<"dTheta"<<"\t"<<"phi_centre"<<"\t"<<"phi_frontface"<<"\t"<<"dPhi"<<endl;
		TVector3 centre, front_centre;
		double theta_c, theta_f, phi_c, phi_f;
		double dTheta, dPhi;
// 		std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
		PndEmcTciXtalMap::const_iterator iter=fTciXtalMap.begin();
		PndEmcTwoCoordIndex *tci;
		while(iter!=(fTciXtalMap).end())
		{
			tci = (*iter).first;
			centre= ((*iter).second)->centre();
			front_centre=((*iter).second)->frontCentre();
			theta_c=centre.Theta()*TMath::RadToDeg();
			phi_c=centre.Phi()*TMath::RadToDeg();
			theta_f=front_centre.Theta()*TMath::RadToDeg();
			phi_f=front_centre.Phi()*TMath::RadToDeg();
	
			dTheta=theta_c-theta_f;
			dPhi=phi_c-phi_f;
	
			f<<tci->Index()<<"\t"<<tci->XCoord()<<"\t"<<tci->YCoord()<<"\t"<<theta_c<<"\t"<<theta_f<<"\t"<<dTheta<<"\t"<<phi_c<<"\t"<<phi_f<<"\t"<<dPhi<<endl;
			iter++;
		}
	} else if (option==2)
	{
		f<<"detID"<<"\t"<<"ThetaInd"<<"\t"<<"PhiInd"<<"\t"<<"X"<<"\t"<<"Y"<<"\t"<<"Z"<<endl;
		TVector3 front_centre;
		TVector3 centre;
		double x, y, z;
// 		std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
		PndEmcTciXtalMap::const_iterator iter=fTciXtalMap.begin();
		PndEmcTwoCoordIndex *tci;
		while(iter!=(fTciXtalMap).end())
		{
			tci = (*iter).first;
			front_centre=((*iter).second)->frontCentre();
			centre=((*iter).second)->centre();
			//x=front_centre.X();y=front_centre.Y();z=front_centre.Z();
			x=centre.X();y=centre.Y();z=centre.Z();
	
			f<<tci->Index()<<"\t"<<tci->XCoord()<<"\t"<<tci->YCoord()<<"\t"<<x<<"\t"<<y<<"\t"<<z<<endl;
			iter++;
		}
		
	}
       
	f.close();
	cout<<"write emc structure to log file"<<endl;

}

ClassImp(PndEmcStructure)
