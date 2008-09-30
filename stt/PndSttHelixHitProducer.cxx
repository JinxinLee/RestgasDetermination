/////////////////////////////////////////////////////////////
// PndSttHelixHitProducer
//
// Class for hit on helix track 
//
/////////////////////////////////////////////////////////////

#include "TClonesArray.h"

#include "CbmRootManager.h"
#include "PndSttHelixHitProducer.h"
#include "PndSttHit.h"
#include "PndSttTrack.h"
#include "PndSttPoint.h"
#include "PndSttHelixTrackFitter.h"
#include "PndSttHelixHit.h"
#include "TGeoManager.h"
// #include "TGeoVolume.h"
// #include "TGeoNode.h"
// #include "TGeoMatrix.h"
#include "TVector3.h"
#include "PndSttSingleStraw.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include <iostream>
#include <cmath>
#include "TGeoVolume.h"
#include "TGeoTube.h"


using namespace std;

// -----   Default constructor   -------------------------------------------
PndSttHelixHitProducer::PndSttHelixHitProducer() :
  CbmTask("STT HELIX Hit Producer") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttHelixHitProducer::~PndSttHelixHitProducer() { 
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttHelixHitProducer::Init() {
  
  hx = new TH1F("hx", "x: mc - reco", 100, -1, 1);
  hy = new TH1F("hy", "y: mc - reco", 100, -1, 1);
  hz = new TH1F("hz", "z: mc - reco", 100, -3, 3);
  
  hxs = new TH1F("hxs", "x: mc - reco", 100, -1, 1);
  hys = new TH1F("hys", "y: mc - reco", 100, -1, 1);
  hzs = new TH1F("hzs", "z: mc - reco", 100, -3, 3);

  hzresvsslope = new TH2F("hzresvsslope", "z: mc - reco vs slope", 100, -3.5, 3.5, 100, -3., 3.);

  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSttHelixHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get SttTrack array
  fTrackArray  = (TClonesArray*) ioman->GetObject("STTTrack"); 
  if ( ! fTrackArray) 
    {
      cout << "-E- CbmSttFitTracks::Init: No SttTrack array!"
	   << endl;
      return kERROR;
    }
	
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndSttHelixHitProducer::Init: "
	 << "No STTPoint array!" << endl;
    return kERROR;
  }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if ( ! fHitArray ) {
    cout << "-W- PndSttHelixHitProducer::Init: "
	 << "No STTHit array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHelixHitArray = new TClonesArray("PndSttHelixHit");
  ioman->Register("SttHelixHit","STT",fHelixHitArray,kTRUE);
    


  cout << "-I- PndSttHelixHitProducer: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSttHelixHitProducer::Exec(Option_t* opt) {

  // Reset output array
  if ( ! fHelixHitArray ) Fatal("Exec", "No HelixHitArray");
  
  fHelixHitArray->Clear();

  // Declare some variables
  PndSttTrack* pTrack  = NULL;

  if ( ! fTrackArray ) Fatal("Exec", "No TrackArray");
 
   
  // Loop over tracks
  Int_t nTracks = fTrackArray->GetEntriesFast();

  for (Int_t j = 0; j < nTracks; j++) {
    pTrack = (PndSttTrack *) fTrackArray->At(j);
    if(!pTrack) continue;

    //    if(pTrack->GetFlag() < 3) continue; // only prefit-fit-zfit CHECK
    // --------------------------- THE TRACK ----------------------------
    // xy
    Int_t hh = -(Int_t) pTrack->GetParamLast()->GetQp(); // CHECK in realta' dovrebbe essere q/p e non solo q
    Double_t d0 = pTrack->GetParamLast()->GetX();
    Double_t phi0 = pTrack->GetParamLast()->GetY();
    Double_t Rad =  pTrack->GetParamLast()->GetTx();
    // z    
    Double_t z0 = pTrack->GetParamLast()->GetZ();
    Double_t zslope = pTrack->GetParamLast()->GetTy();
    // -------------------------------------------------------------------

    Int_t hitcounter = pTrack->GetNofHits();
    Int_t hotcounter = 0;
    TVector2 point; // point
    Double_t radius = 0;

    PndSttHelixHit *helixhit = NULL;
    for (Int_t k = 0; k < hitcounter; k++) {
      Int_t iHit = pTrack->GetHitIndex(k);
      PndSttHit *currenthit = (PndSttHit*) fHitArray->At(iHit);
      Int_t refindex = currenthit->GetRefIndex(); 
      // get point
      PndSttPoint *iPoint = (PndSttPoint*) fPointArray->At(refindex);

      TClonesArray& clref = *fHelixHitArray;
      Int_t size = clref.GetEntriesFast();
      //  cout << "filling HelixHit" << endl;

      // CHECK remember to SET the errors on position!!!!!!!
      helixhit = new(clref[size]) PndSttHelixHit();
      helixhit->CopyHitToHelixHit(currenthit, iHit);

      pTrack->AddHelixHit(hitcounter, k, size);
      
      //      helixhit->Print();
     
      TVector3 wiredirection = currenthit->GetWireDirection();
      
      // ================= NON SKEWED =======================
      if(wiredirection == TVector3(0.,0.,1.)) 
	{
	  
	  // x y plane / non skewed tubes -------------------------------------------------
	  
	  // centre of curvature of helix
	  TVector2 vec((pTrack->GetParamLast()->GetX() + pTrack->GetParamLast()->GetTx()) * cos(pTrack->GetParamLast()->GetY()), (pTrack->GetParamLast()->GetX() + pTrack->GetParamLast()->GetTx()) * sin(pTrack->GetParamLast()->GetY()));
	  
	  //==========
	  // POINT ----------------------------------------------------
	  // 1. find the cooordinates of the point fired wire of the track
	  // get point
	 
	  // [xp, yp] point = coordinates xy of the centre of the firing tube
	  point.Set(currenthit->GetX(), currenthit->GetY());
	  radius = currenthit->GetIsochrone();

	  // the coordinates of the point are taken from the intersection
	  // between the circumference from the drift time and the R radius of
	  // curvature. -------------------------------------------------------
	  // 2. find the intersection between the little circle and the line // R
	  TVector2 first;
	  TVector2 second;
	  // 2.a
	  // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
	  // y = mx + q
	  Double_t m = (point.Y() - vec.Y())/(point.X() - vec.X());
	  Double_t q = point.Y() - m*point.X();
      
	  /**                             CHECK!!! **/
	  // cut on radius
	  // if the simulated radius is too small, the pMhit
	  // is not used for the fit 
	  if(radius < 0.1) {
	    /**
	       marray.AddAt(-999, k);
	       pMhit->SetXint(-999);
	       pMhit->SetYint(-999);
	       continue;  // CHECK throw away the hit
	    **/
	  }
	  
      
	  // 2.b
	  // intersection little circle and line --> [x1, y1]
	  // + and - refer to the 2 possible intersections
	  // +
	  Double_t x1 = (-(m*(q - point.Y()) - point.X()) + sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - radius*radius))) / (m*m + 1);
	  Double_t y1 = m*x1 + q;
	  first.Set(x1, y1);
   
	  // - 
	  Double_t x2 = (-(m*(q - point.Y()) - point.X()) - sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - radius*radius))) / (m*m + 1);
	  Double_t y2 = m*x2 + q;
	  second.Set(x2, y2);
      
	  // 2.c intersection between line and circle
	  // +
	  Double_t xb1 = (-(m*(q - vec.Y()) - vec.X()) + sqrt((m*(q - vec.Y()) - vec.X())*(m*(q - vec.Y()) - vec.X()) - (m*m + 1)*((q - vec.Y())*(q - vec.Y()) + vec.X()*vec.X() - (pTrack->GetParamLast()->GetTx()) *(pTrack->GetParamLast()->GetTx()) ))) / (m*m + 1);
	  Double_t yb1 = m*xb1 + q;
	  // -
	  Double_t xb2 = (-(m*(q - vec.Y()) - vec.X()) - sqrt((m*(q - vec.Y()) - vec.X())*(m*(q - vec.Y()) - vec.X()) - (m*m + 1)*((q - vec.Y())*(q - vec.Y()) + vec.X()*vec.X() - (pTrack->GetParamLast()->GetTx()) *(pTrack->GetParamLast()->GetTx())))) / (m*m + 1);
	  Double_t yb2 = m*xb2 + q;
      
	  // calculation of the distance between [xb, yb] and [xp, yp]
	  Double_t distb1 = sqrt((yb1 - point.Y())*(yb1 - point.Y()) + (xb1 - point.X())*(xb1 - point.X()));
	  Double_t distb2 = sqrt((yb2 - point.Y())*(yb2 - point.Y()) + (xb2 - point.X())*(xb2 - point.X()));
      
	  // choice of [xb, yb]
	  TVector2 xyb;
	  if(distb1 > distb2) xyb.Set(xb2, yb2); 
	  else xyb.Set(xb1, yb1); 
      
	  // calculation of the distance between [x, y] and [xb. yb]
	  Double_t dist1 = sqrt((xyb.Y() - y1)*(xyb.Y() - y1) + (xyb.X() - x1)*(xyb.X() - x1));
	  Double_t dist2 = sqrt((xyb.Y() - y2)*(xyb.Y() - y2) + (xyb.X() - x2)*(xyb.X() - x2));
      
	  // choice of [x, y]
	  TVector2 *xy;
	  if(dist1 > dist2) xy = new TVector2(x2, y2);
	  else xy = new TVector2(x1, y1);   // <========= THIS IS THE NEW POINT to be used for the fit
      
	  // set to helix hit x and y
	  helixhit->SetX(xy->X());
	  helixhit->SetY(xy->Y());
	  //	  cout << "helix final hit "<<k << " "  << helixhit->GetX() << " " << helixhit->GetY() << endl;

	  //=====================
	  // z plane / non skewed tubes -------------------------------------------------
	  PndSttHelixTrackFitter fitter;
	  Double_t scosl = fitter.CalculateScosl(hh, d0, phi0, Rad, helixhit->GetX(), helixhit->GetY());
	  Double_t zcoord = z0 + zslope * scosl;
	  helixhit->SetZ(zcoord);


	  hx->Fill(iPoint->GetXtot() - helixhit->GetX());
	  hy->Fill(iPoint->GetYtot() - helixhit->GetY());
	  hz->Fill(iPoint->GetZtot() - helixhit->GetZ());


	  // 	  cout << "hit on helix " << helixhit->GetX() << " " << helixhit->GetY() << " " << helixhit->GetZ() << endl;
	  // 	  cout << "mc point     " << iPoint->GetXtot() << " "<< iPoint->GetYtot() << " " << iPoint->GetZtot() << endl;
	  
	  //	  helixhit->Print();
	  

	}
      else { // =========== SKEWED TUBE ==================

	if(currenthit->GetZint() != -999)
	  { 
	    // get the track fit result
	    helixhit->SetX(currenthit->GetXint());
	    helixhit->SetY(currenthit->GetYint());
	    helixhit->SetZ(currenthit->GetZint());
	  }
	else
	  {
	    // CHECK the reason why these are different from the previous ones!

	    TVector3 *tofit, *tofit2;
	
	    // centre of curvature // CHECK gia' trovato!!!!!!!!!!
	    Double_t x_0 = (pTrack->GetParamLast()->GetX() + pTrack->GetParamLast()->GetTx()) * cos(pTrack->GetParamLast()->GetY());
	    Double_t y_0 = (pTrack->GetParamLast()->GetX() + pTrack->GetParamLast()->GetTx()) * sin(pTrack->GetParamLast()->GetY());
	    // radius of curvature // CHECK gia' trovato!!!!!!!!!!
	    Double_t R = pTrack->GetParamLast()->GetTx();
	

	    wiredirection *= 75.; // CEHCK lunghezza vera!
	    TVector3 cenposition(currenthit->GetX(), currenthit->GetY(), currenthit->GetZ());  // CHECK! z = 35!!
	
	    TVector3 min, max;
	    min = cenposition - wiredirection;
	    max = cenposition + wiredirection;
	
	    // first extremity
	    Double_t x_1= min.X(); 
	    Double_t y_1= min.Y(); 
	    Double_t z_1= min.Z(); 
	
	    // second extremity
	    Double_t x_2= max.X();
	    Double_t y_2= max.Y();
	    Double_t z_2= max.Z();
	
	    Double_t rcur = currenthit->GetIsochrone(); // CHECK gia' trovato!!!!!!!!!!

	    Double_t x1 = -9999.;
	    Double_t y1 = -9999.;
	    Double_t x2 = -9999.;
	    Double_t y2 = -9999.;
	
	    // from xy plane fit
	    Double_t phi0 = pTrack->GetParamLast()->GetY();// CHECK gia' trovato!!!!!!!!!!
	    Double_t d0 = pTrack->GetParamLast()->GetX();// CHECK gia' trovato!!!!!!!!!!
	    Double_t x0 = d0*TMath::Cos(phi0);
	    Double_t y0 = d0*TMath::Sin(phi0);
	    // in xy plane: angle of the PCA to the origin
	    // with respect to the curvature center
	    Double_t Phi0 = TMath::ATan2((y0 - y_0),(x0 - x_0));

	    Double_t a = -999;
	    Double_t b = -999;
      
	    // intersection point between the reconstructed 
	    // circumference and the line joining the centres
	    // of the reconstructed circle and the i_th drift circle
	    if(fabs(x_2-x_1)>0.0001) {
	      a =(y_2-y_1)/(x_2-x_1);
	      b =(y_1-a*x_1);
	      Double_t A = a*a+1;
	      Double_t B = x_0+a*y_0-a*b;
	      Double_t C = x_0*x_0+y_0*y_0+b*b-R*R-2*y_0*b;
	      if((B*B-A*C)>0) {
		x1= (B+TMath::Sqrt(B*B-A*C))/A;
		x2= (B-TMath::Sqrt(B*B-A*C))/A;
		y1=a*x1+b;
		y2=a*x2+b;
	      }
	    }
	    else if(fabs(y_2-y_1)>0.0001) {
	      Double_t A = 1;
	      Double_t B = y_0;
	      Double_t C = y_0*y_0 +(x_1-x_0)*(x_1-x_0) -R*R;

	      if((B*B-A*C)>0) {
		y1= (B+TMath::Sqrt(B*B-A*C))/A;
		y2= (B-TMath::Sqrt(B*B-A*C))/A;
		x1=x2=x_1;
	      }
	    }
	    else {
	      cout << "-E- intersection point not found" << endl;
	      continue;
	    }
	
	    //x1 and x2 are the 2 intersection points
	    Double_t d1=TMath::Sqrt((x1-cenposition.X())*(x1-cenposition.X())+
				    (y1-cenposition.Y())*(y1-cenposition.Y()));
	    Double_t d2=TMath::Sqrt((x2-cenposition.X())*(x2-cenposition.X())+
				    (y2-cenposition.Y())*(y2-cenposition.Y()));
	
	    Double_t x_ = x1;
	    Double_t y_ = y1;
	
	    // the intersection point nearest to the drift circle's centre is taken
	    if(d2<d1) {x_=x2;y_=y2;}    
	
    
	    // now we need to find the actual centre of the drift circle,
	    // by translating the drift circle until it becomes tangent
	    // to the reconstructed circle. 
	    // Two solutions are possible (left rigth abiguity), 
	    // they are both kept, only the following zed fit will discard the wrong ones.
	    // Using the parametric equation of the 3d-straigth line and taking the
	    // x points just obtained, the zed coordinate of the skewed tube centre is calculated.
	
	    //solving the equation to find out the centre of the tangent circle
	    Double_t A = a*a+1;
	    Double_t B = -(a*b-a*y_-x_);
	    Double_t C = x_*x_+ y_*y_+b*b-2*b*y_-rcur*rcur;
	    if((B*B-A*C)>0) {
	      x1= (B+TMath::Sqrt(B*B-A*C))/A;
	      x2= (B-TMath::Sqrt(B*B-A*C))/A;
	      y1=a*x1+b;
	      y2=a*x2+b;
	    }
	    else if((B*B-A*C)==0){          // CHECK forse da scommentare
	      x1= B/A;
	      x2 = x1;
	      y1=a*x1+b;
	      y2=a*x2+b;
	    }
	    else {
	      cout << "NO WAY2" << endl;
	      continue;
	    }	


	    d1=TMath::Sqrt((x1-cenposition.X())*(x1-cenposition.X())+(y1-cenposition.Y())*(y1-cenposition.Y()));
	    d2=TMath::Sqrt((x2-cenposition.X())*(x2-cenposition.X())+(y2-cenposition.Y())*(y2-cenposition.Y()));
	
	    Double_t xcen0=x1;
	    Double_t xcen1=x2;
	    Double_t ycen0=y1;
	    Double_t ycen1=y2;
	
	    if(d2<d1) { // z2 contains the points nearest (in x-y) to the initial centre of the skewed tube
	      xcen0=x2;
	      xcen1=x1;
	      ycen0=y2;
	      ycen1=y1;
	  
	    }
	
	    // zed association
	    if(fabs(x_2-x_1)<0.001) {
	      helixhit->SetPosition(TVector3(-999,-999,-999));
	      cout << "helix hit skewed1 " << helixhit->GetX() 
		   << " "                 << helixhit->GetY() 
		   << " "                 << helixhit->GetZ() << endl; // CHECK the procedure!!
	      continue;
	    }
	    Double_t t_      =(xcen0-x_1)/(x_2-x_1); // x= a_x*t + x_1 [t=1 x=x_2]
	    Double_t z_      =(z_2-z_1)*t_ +z_1;     // z= a_z*t + z_1 [t=1 z=z_2]

	    Double_t t_bis   =(xcen1-x_1)/(x_2-x_1); // from x_'s (the 2 solutions of the 2nd order equation)
	    Double_t z_bis   =(z_2-z_1)*t_bis +z_1;  // and the 2 parametric equations the z coord. are obtained 


	    //	tofit = new TVector3(xcen0,ycen0,z_);
	    tofit = new TVector3(x_,y_,z_);
	    //	tofit2 = new TVector3(xcen1,ycen1,z_bis);
	    tofit2 = new TVector3(x_,y_,z_bis);

	    // I have 2 choices, I prefer the nearest to the previous one!
	    TVector3 *previouspos = new TVector3(currenthit->GetXint(), currenthit->GetYint(), currenthit->GetZint());

	    //  	// ------- HOUGH TRANSFORM ------------
	    // 	// FIRST CHOICE
	    // 	if(tofit) Hough(tofit, Phi0, x0, y0, R);
	
	    // 	// SECOND CHOICE
	    // 	if(tofit2) Hough(tofit2, Phi0, x0, y0, R);

	    double distance = sqrt((tofit->X() - previouspos->X())*(tofit->X() - previouspos->X())
				   + (tofit->Y() - previouspos->Y())*(tofit->Y() - previouspos->Y())
				   + (tofit->Z() - previouspos->Z())*(tofit->Z() - previouspos->Z()));

	    double distance2 = sqrt((tofit2->X() - previouspos->X())*(tofit2->X() - previouspos->X())
				    + (tofit2->Y() - previouspos->Y())*(tofit2->Y() - previouspos->Y())
				    + (tofit2->Z() - previouspos->Z())*(tofit2->Z() - previouspos->Z()));


	    if(distance < distance2) helixhit->SetPosition(*tofit);
	    else helixhit->SetPosition(*tofit2);

	    //	    cout << "previous center  " << currenthit->GetX() << " " << currenthit->GetY() << " " << currenthit->GetZ() << endl; // CHECK the procedure!!
	    // 	    cout << "helix hit " << helixhit->GetX() << " " << helixhit->GetY() << " " << helixhit->GetZ() << endl; // CHECK the procedure!!
// 	    cout << "previous reco  " << previouspos->X() << " " << previouspos->Y() << " " << previouspos->Z() << endl; // CHECK the procedure!!
	    
	    hxs->Fill(iPoint->GetXtot() - helixhit->GetX());
	    hys->Fill(iPoint->GetYtot() - helixhit->GetY());
	    hzs->Fill(iPoint->GetZtot() - helixhit->GetZ());

	    delete previouspos;
	  }


	
	//=====================
	// z plane / non skewed tubes -------------------------------------------------
	PndSttHelixTrackFitter fitter;
	Double_t scosl = fitter.CalculateScosl(hh, d0, phi0, Rad, helixhit->GetX(), helixhit->GetY());
	Double_t zcoord = z0 + zslope * scosl;
	//	helixhit->SetZ(zcoord);

	//	cout << "helix hit skewed " << helixhit->GetX() << " " << helixhit->GetY() << " " << helixhit->GetZ() << endl; // CHECK the procedure!!
	//  	cout << "mc point     " << iPoint->GetXtot() << " "<< iPoint->GetYtot() << " " << iPoint->GetZtot() << endl;

	hxs->Fill(iPoint->GetXtot() - helixhit->GetX());
	hys->Fill(iPoint->GetYtot() - helixhit->GetY());
	hzs->Fill(iPoint->GetZtot() - helixhit->GetZ());
	hzresvsslope->Fill(zslope, (iPoint->GetZtot() - helixhit->GetZ()));
      }
      

      // dE/dx calculation ================== 
      if(currenthit->GetdEdx() != -999) helixhit->SetdEdx(currenthit->GetdEdx()); // if MC is used
      else {
	TString tubename; 
	TGeoVolume *gastube;
	TObjArray *volumeArray = gGeoManager->GetListOfVolumes();

	for(int i = 0; i < volumeArray->GetEntriesFast() ; i++)
	  {
	    tubename = volumeArray->At(i)->GetName();
	    if(tubename.Contains("stt") && tubename.Contains("gas")) 
	      {  
		gastube = (TGeoVolume*) volumeArray->At(i);
		break;
	      }
	  }
	TGeoTube *tube = (TGeoTube*) gastube->GetShape();
	Double_t tuberadius = tube->GetRmax();
	Double_t distance = 2 * sqrt(tuberadius * tuberadius - radius * radius); // cm
	    
	Double_t coslam = TMath::Cos(TMath::ATan(zslope));
	distance = distance / coslam;    
	  
	// just to check, delete it!
	TVector3 diff3(iPoint->GetXInLocal() - iPoint->GetXOutLocal(),
		       iPoint->GetYInLocal() - iPoint->GetYOutLocal(),
		       iPoint->GetZInLocal() - iPoint->GetZOutLocal());
	//	cout << "true " << diff3.Mag() << " sim " << distance << endl;

	Double_t dedx = 0.;
	if (distance != 0)  dedx = currenthit->GetDepCharge()/(1000000 * distance);  // in arbitrary units
  
	helixhit->SetdEdx(dedx);
      }
	  
      hotcounter++;
    }
    
    // Track summary
    cout << "-I- PndSttHelixHitProducer: " << j << " track " << hitcounter << " SttHits, "
 	 << hotcounter << " HelixHits created." << endl;
    cout << "----------------------------------------" << endl;
    
  }


}

void PndSttHelixHitProducer::WriteHistograms(){
  TFile* file = CbmRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("PndSttHelixHit");
  file->cd("PndSttHelixHit");
  
  hx->Write();
  delete hx;
  hy->Write();
  delete hy;
  hz->Write();
  delete hz;

  hxs->Write();
  delete hxs;
  hys->Write();
  delete hys;
  hzs->Write();
  delete hzs;

  hzresvsslope->Write();
  delete hzresvsslope;


}



ClassImp(PndSttHelixHitProducer)
