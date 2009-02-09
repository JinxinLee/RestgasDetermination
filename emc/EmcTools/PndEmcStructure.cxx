//======================================================================
// File and Version Information:
// 	$Id: $
//
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
	// PndEmcMapper should be instatiated before first call here with proper input index
   PndEmcMapper *fEmcMap=PndEmcMapper::Instance(0);
  
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
		
		geoRot.SetMatrix(crystal_matrix->GetRotationMatrix());
		
		PndEmcXtal *xtal;
		if (module==5) {
			TGeoBBox *box = (TGeoBBox *) node->GetVolume()->GetShape();
			
			TGeoTrap crystal(box->GetDZ(), 0, 0,
			box->GetDY(), box->GetDX(), box->GetDX(), 0,
			box->GetDY(), box->GetDX(), box->GetDX(), 0);

			xtal=new PndEmcXtal(tci,crystal,pos,geoRot);
		}
		else{
			
			TGeoTrap *trap1 = (TGeoTrap*) node->GetVolume()->GetShape();
			
			TGeoTrap crystal(trap1->GetDz(), trap1->GetTheta(), trap1->GetPhi(),
			trap1->GetH1(), trap1->GetBl1(), trap1->GetTl1(), trap1->GetAlpha1(),
			trap1->GetH2(), trap1->GetBl2(), trap1->GetTl2(), trap1->GetAlpha2());
			xtal=new PndEmcXtal(tci,crystal,pos,geoRot);
		}
		
		fTciXtalMap[tci]=xtal;
   }
}

bool PndEmcStructure::crystal_name_analysis(TString node_path,int &module,int &copy,int &row,int &crystal)
{
        // The following code extract information about module, copy, row and crystal number from the path name of the TGeoNode which corresponds to emc crystal
	// Name convention is according to PndEmc.cxx
	if (!(node_path.Contains("emc") || node_path.Contains("Crystal"))) return false;
	
	// Case of Fsc
	if (node_path.Contains("Fsc")){
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
      TObjArray *subStrL = TPRegexp("^cave/Emc3_0/QuarterVol_(\\d+)/SubunitVol_(\\d+)/BoxVol_(\\d+)/CrystalVol_(\\d+)$").MatchS(node_path);
      if(subStrL->GetLast()<4){
	 cout<<"crystal name in NEW FwEndCap Emc: "<<node_path<<" missmatch pattern"<<endl;
	 return false;
      }

      copy  = (((TObjString *)subStrL->At(1))->GetString()).Atoi();

      Int_t copyNoSub = (((TObjString *)subStrL->At(2))->GetString()).Atoi()-1;//'copyNoSub' should start from zero
      Int_t copyNoBox  = (((TObjString *)subStrL->At(3))->GetString()).Atoi();
      Int_t copyNoCrys  = (((TObjString *)subStrL->At(4))->GetString()).Atoi();

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
      
      if(flag)
	if ( (copyNoBox == 0)  || (copyNoBox == 2) ){
	   if(copyNoCrys == 0 || copyNoCrys == 1){
	      nCrys = ( (next*4) + (copyNoCrys+1) );
	      nRow = (copyNoBox + 1 + subrow*col); 	
	   }else if (copyNoCrys == 2 || copyNoCrys == 3){
	      k1 = copyNoCrys-2;			      
	      nCrys = ( (next*4) + (k1+1) );
	      nRow = (copyNoBox + 2 + subrow*col);		 
	   }
	}else if ( (copyNoBox == 1) || (copyNoBox == 3) ){
	   if(copyNoCrys == 0 || copyNoCrys == 1){
	      nCrys = ( (next*4) + (copyNoCrys+3) );
	      nRow = copyNoBox + subrow*col; 
	   }else if (copyNoCrys == 2 || copyNoCrys == 3){
	      k1 = copyNoCrys-2;
	      nCrys = ( (next*4) + (k1+3) );
	      nRow = (copyNoBox + 1 + subrow*col);
	   } //end of crystal		     
	} //end of box
      
      module = 3;
      row = nRow;
      crystal = nCrys;
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
	     
	std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
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
		std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
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
		std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
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
