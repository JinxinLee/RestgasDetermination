///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// PndTpcLheSegments class - divide $\phi$- $\theta$-station in cells           //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#include "PndTpcLheSegments.h"
#include "PndLheCMPoint.h"

#include "TObjArray.h"
#include "TMath.h"

#include <iomanip>

ClassImp(PndTpcLheSegments)

//#define TRACKSEG

//#define SEGMENTS

//________________________________________________________________
PndTpcLheSegments::PndTpcLheSegments() {
  //---

  fVertex = NULL;

  //  fNumOfStation = 1;       // The number of stations
  fNumThetaSegment = 30;
  fNumPhiSegment = 20;
  fBounds = fNumThetaSegment * fNumPhiSegment;

  fPhiMax = 2.* TMath::Pi();
  fPhiMin = 0.;
  fThetaMax = TMath::Pi();
  fThetaMin = -TMath::Pi();

}

//________________________________________________________________
PndTpcLheSegments::PndTpcLheSegments( Int_t nTheta, Int_t nPhi) {
  //
  fVertex = NULL;

  fNumThetaSegment = nTheta;    //
  fNumPhiSegment = nPhi;  //
  fBounds = fNumThetaSegment * fNumPhiSegment;


}

//_________________________________________________________________
PndTpcLheSegments::~PndTpcLheSegments() {
  //
  delete fVertex;

  if(fSegments) fSegments->Delete();
  delete fSegments;

}

//________________________________________________________________
void  PndTpcLheSegments::Init() {
  //---


  fSegments = new TObjArray(fBounds);     //

  for (Int_t iv = 0; iv < fBounds; iv++) {
    fSegments->AddAt(new TObjArray(0), iv);
  }

}

//________________________________________________________________
void  PndTpcLheSegments::Clear() {
  //---

  TIter next(fSegments);

  TObjArray *cell;
  while((cell = (TObjArray*) next())) {
    cell->Clear();
  }

}

//________________________________________________________________
void  PndTpcLheSegments::FillSegments(TClonesArray *fCMHits) {
  //---

  Clear();

#ifdef TRACKSEG
  Int_t cur_track = 0;
  cout << "\n track " <<  " station " << 
    "  theta "  << " theta segm " <<
    "  phi " << " phi_seg "  <<
    " seg " <<  "\n";
#endif
  
  for (Int_t i = 0; i < fCMHits->GetEntriesFast(); i++) {
    PndLheCMPoint* h = (PndLheCMPoint *)fCMHits->At(i);
    h->SetUsage(kFALSE);
    //    h->Print();
    Int_t iseg =
      GetSegm(GetThetaSegm(h), GetPhiSegm(h));
    ((TObjArray *)fSegments->At(iseg))->AddLast(h);

#ifdef TRACKSEG
    if (cur_track != h->GetTrackID()) {
      cur_track = h->GetTrackID();
      cout << endl;
    }
    cout.width(5);
    cout <<  h->GetTrackID() << " " <<  GetRadiusSegm(h) << " ";
    cout << setw(10) << setprecision(4) << h->GetTheta() << " ";
    cout.width(4);  cout << GetThetaSegm(h) << " ";
    cout << setw(10) << setprecision(4) <<  h->GetPhi() << " ";
    cout.width(4);  cout << GetPhiSegm(h);
    cout.width(10);  cout <<  iseg << "\n";
#endif
  }
  
#ifdef SEGMENTS
  PrintSegments();
#endif

}

//________________________________________________________________
void PndTpcLheSegments::PrintSegments() {

  cout << "PndTpcLheSegments::PrintSegments()\n";


// This function loops over all hits in segment

  TObjArray *segment;
  PndLheCMPoint *hit;

  Int_t st_num = 0;

    // loop over theta segments
    for (Int_t theta_segm_counter = 0;
	 theta_segm_counter < fNumThetaSegment;
	 theta_segm_counter++) {

      // go over theta in two directions, one segment in each direction alternately
      Int_t theta_segm;

        theta_segm = theta_segm_counter;

      // loop over phi segments
      for(Int_t phi_segm = 0; phi_segm < fNumPhiSegment; phi_segm++) {
	
	// loop over entries in one segment
        segment = (TObjArray *)fSegments->At(GetSegm(theta_segm, phi_segm));
        Int_t entries = segment->GetEntriesFast();

        if (entries) {
	    cout << "\n segment " <<
	      GetSegm(theta_segm, phi_segm) << ": " <<
	      " entries " << entries << endl;

          for (Int_t hit_num = 0; hit_num < entries; hit_num++) {
            hit = (PndLheCMPoint *)segment->At(hit_num);

	    cout << "\n hit #" << hit->GetHitNumber();
	    cout << " trackID " << hit->GetTrackID();
	    //	    cout << " station " << hit->GetStation();
	    cout << " theta =  " << theta_segm;
	    cout << " phi = " << phi_segm;
          }
        }    // if (entries)

      }
    }
    //  } // station

}

//________________________________________________________________
void PndTpcLheSegments::PrintSegmentContents(Int_t n_seg) {
  //---

  TObjArray *segment = (TObjArray *)fSegments->At(n_seg);
  Int_t entries = segment->GetEntriesFast();

//   cout << "\n segment " <<
//     GetSegm(theta_segm, phi_segm) << ": ";

  if (entries) {
    
    for (Int_t hit_num = 0; hit_num < entries; hit_num++) {
      PndLheCMPoint* hit = (PndLheCMPoint *)segment->At(hit_num);

      cout << "\n hit ";
      cout << " geant # " << hit->GetTrackID();
      //      cout << " station " << hit->GetStation();
//       cout << " theta =  " << theta_segm;
//       cout << " phi = " << phi_segm;
      cout << " is used = " << Int_t (hit->GetUsage());

    }
  }    // if (entries)
  else {
      cout << "\n segment is empty\n ";
  }

}

