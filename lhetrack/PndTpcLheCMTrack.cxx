#include "TObjArray.h"
#include "TClonesArray.h"

#include "PndTpcLheCMTrack.h"
#include "PndTpcLheCMPoint.h"

ClassImp(PndTpcLheCMTrack)

#define PR(x) cout << #x " = " << x << "\n";
//#define PR(x) ;

//___________________________________________________________
  PndTpcLheCMTrack::PndTpcLheCMTrack() : PndTpcLheTrack() {
  // Creates a ObjArray of the hits belonging to the track.

  fNMapHits = 0;
  //  fRadius = 0.;

  fMappingHits = new TObjArray(0);

} ///:~

//___________________________________________________________
PndTpcLheCMTrack::PndTpcLheCMTrack(Int_t tracknumber) :
  PndTpcLheTrack(tracknumber) {
  // Same as default constructor except that the track number is set.
  
  fNMapHits = 0;
  //  fRadius = 0.;
  fMappingHits = new TObjArray(0);

  SetTrackID(tracknumber);
}

//___________________________________________________________
PndTpcLheCMTrack::PndTpcLheCMTrack(Int_t tracknumber, Int_t nhits) :
  PndTpcLheTrack(tracknumber) {
  // constructor for nhits
  
  fNMapHits = 0;
  //  fRadius = 0.;
  fMappingHits = new TObjArray(nhits,0);

  SetTrackID(tracknumber);
}

//___________________________________________________________
PndTpcLheCMTrack::~PndTpcLheCMTrack() {
  //
  //  fMappingHits->Delete(); 

  //  cout << " Destructor for CMTrack " << endl;
  // fMappingHits->IsOwner() << endl;

  if (fMappingHits) {
    //   cout << " inside " << fMappingHits << endl;
    //   //    fMappingHits->Delete();
    delete fMappingHits;
    //   fMappingHits = NULL;
    //   cout << " inside " << fMappingHits << endl;
  }

}

//___________________________________________________________
void PndTpcLheCMTrack::SetTrackID(Int_t number) {

  // Sets the tracknumber.  If the track has already some hits
  // assigned the track number of the hits is also set.

  SetTrackNumber(number); // fTrackNumber = number;

  for (Int_t i = 0; i < fMappingHits->GetEntriesFast(); i++) {
    ((PndTpcLheHit*)fMappingHits->At(i))->SetTrackID(number);    
  }

}

//___________________________________________________________
void PndTpcLheCMTrack::Copy( const PndTpcLheCMTrack* src) {
  // copy 

  fNMapHits = src->GetNumberOfPoints();
  //  PR(fNMapHits);

  fMappingHits->Expand(fNMapHits);
  fRealHits->Expand(fNMapHits);
  TObjArray *hits = src->GetCMHits();

  for (Int_t i = 0; i < fNMapHits; i++) {
    fMappingHits->AddAt(hits->At(i), i); 
    fRealHits->AddAt(dynamic_cast <PndTpcLheHit *> (hits->At(i)), i); 

  }
}

//___________________________________________________________
void PndTpcLheCMTrack::AddPoint(PndTpcLheCMPoint* point, Bool_t backward) {
  // Adds a given point to the track.

  Int_t num = fNMapHits;
  PndTpcLheHit *hit = dynamic_cast <PndTpcLheHit *> (point);

  if(backward) {
    fMappingHits->Expand(num+1);
    fRealHits->Expand(num+1);

    for (Int_t i = num-1; i >= 0; i--) {
      fMappingHits->AddAt(fMappingHits->At(i), i+1);
      fRealHits->AddAt(fRealHits->At(i), i+1);
    }
  
    fMappingHits->AddFirst(point);
    fRealHits->AddFirst(hit);
  }
  else {
    fMappingHits->AddLast(point);
    fRealHits->AddLast(hit);
  }

  fNMapHits++;

}

//___________________________________________________________
void PndTpcLheCMTrack::SetPointsUsage() {

  //  Release track points

//   cout << " map points " << fMappingHits->GetEntriesFast() << endl;

  for (Int_t i = 0; i < fMappingHits->GetEntriesFast(); i++) {    
    PndTpcLheCMPoint *p = (PndTpcLheCMPoint *)fMappingHits->At(i);
    p->SetUsage(kTRUE);
  }

}

//___________________________________________________________
void PndTpcLheCMTrack::Clear() {
  //---  

//   cout << " map points " << fMappingHits->GetEntriesFast() << endl;

  if(fMappingHits && fMappingHits->GetEntriesFast() != 0) {
    for (Int_t i = 0; i < fMappingHits->GetEntriesFast(); i++) {    
      PndTpcLheCMPoint *p = (PndTpcLheCMPoint *)fMappingHits->At(i);
      p->SetUsage(kFALSE);
    }

    fMappingHits->Clear();
    fMappingHits = NULL;
  }

}

//______________________________________________________________
void PndTpcLheCMTrack::Print() {
  //

    Int_t nhit =  fMappingHits->GetEntries();

    cout << "\n Track " << fTrackNumber << "  nhits " << nhit;
    cout << " Primary "  << fFromMainVertex << endl;

    cout << " vertex XYZ: " << setw(5) << fVertex.GetX();
    cout << " " << setw(5) << fVertex.GetY();
    cout << " " << setw(5) << fVertex.GetZ();

    cout << " Pxyz: ";
    cout.precision(3);
    cout << " " << setw(7) <<  fP.X();
    cout << " " << setw(7) <<  fP.Y();
    cout << " " << setw(7) <<  fP.Z();
    cout << endl;

    PndTpcLheCMPoint * ghit = NULL;

    for (Int_t j=0; j < nhit; j++) {
      ghit = (PndTpcLheCMPoint *) fMappingHits->At(j);
      ghit->Print();
    }

}


