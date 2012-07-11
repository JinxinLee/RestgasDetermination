#include "PndTrkPrintouts.h"

#include "FairMCPoint.h"
#include "PndSttTube.h"
#include <iostream>
#include <cmath>


// Root includes
#include "TROOT.h"


using namespace std;


//----------begin of function PndTrkPrintouts::stampaSttHits

void PndTrkPrintouts::stampaSttHits(
	Short_t iHit,
	Short_t ipunto,
	Double_t dradius,
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	FairMCPoint* puntator,
	PndSttTube * pSttTube
	)
{

	Short_t i = iHit;

	cout <<"da PndTracking, Stt iHit "<< i << " e n. punto MC ottenuto con RefIndex = "
      	<<ipunto<<endl;
      if(ipunto<0) {
      	cout<<"hit di background da mixing!\n";
      	cout <<"             hit wire pos. in middle "   << pSttTube->GetPosition().X() << " " <<
             pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z() 
           << "; R = "<<sqrt(pSttTube->GetPosition().X()*pSttTube->GetPosition().X()+
	   pSttTube->GetPosition().Y()*pSttTube->GetPosition().Y())<<
	   ", suo drift radius = "<<dradius <<endl;
      	cout <<"             wire direction, X, Y, Z (Z direction set always positive)"
      	<< WDX[i]<<"  "<<WDY[i]<<"  "<<WDZ[i] <<endl;
      } else{
      cout <<"             hit X, Y, Z space position "   << puntator->GetX() << " " <<
                       puntator->GetY() << " " << puntator->GetZ()<<endl; 
      cout <<"             hit wire pos. in middle "   << pSttTube->GetPosition().X() << " " <<
             pSttTube->GetPosition().Y() << " " << pSttTube->GetPosition().Z() 
           << "; R = "<<sqrt(pSttTube->GetPosition().X()*pSttTube->GetPosition().X()+
	   pSttTube->GetPosition().Y()*pSttTube->GetPosition().Y())<<
	   ", suo drift radius = "<<dradius <<endl;
      cout <<"             wire direction, X, Y, Z (Z direction set always positive)"
      << WDX[i]<<"  "<<WDY[i]<<"  "<<WDZ[i] <<endl
           <<"             this hit belongs to MC track n. "<<puntator->GetTrackID()<<endl;
      }

 }


//----------end of function PndTrkPrintouts::stampaSttHits



ClassImp(PndTrkPrintouts);
