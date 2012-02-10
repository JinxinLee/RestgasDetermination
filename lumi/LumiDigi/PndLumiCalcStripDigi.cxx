#include "PndLumiCalcStripDigi.h"

TRandom3 *fRND = new TRandom3();

PndLumiCalcStripDigi::PndLumiCalcStripDigi()
{
	 fPitch = 0.0;
	 fOrient = 0.0;
	 fThreshold = 0.0;
	 fNoise = 0.;
	 fSigma = -1.;
	 fStripZeroId = TVector2 (0.0, 0.0);
}


PndLumiCalcStripDigi::PndLumiCalcStripDigi(const PndLumiDigiPara *digipar, SensorSide side)
{
	if(side == kTOP){
		fOrient = digipar->GetFrontOrient();
		fStripZeroId= digipar->GetFrontAnchor();

	}else if(side == kBOTTOM){
		fOrient = digipar->GetBackOrient();
		fStripZeroId= digipar->GetBackAnchor();
	}

	fPitch = digipar->GetPitch();
	fThreshold = digipar->GetThreshold();
	fNoise = digipar->GetNoise();
	fSigma = digipar->GetGausSigma();
}


PndLumiCalcStripDigi::~PndLumiCalcStripDigi()
{

}


std::vector<PndLumiStrip> PndLumiCalcStripDigi::GetStrips(FairGeoVector in,
		FairGeoVector out, Double_t eloss)
{
	// Get the 2d-Projection of trajectory
    TVector2 in_2d  (in.getX(),in.getY());
    TVector2 out_2d (out.getX(),out.getY());

    std::vector<PndLumiStrip> strip;

    Double_t inStripId  = CalcStripFromHit(in.getX() , in.getY());
    Double_t outStripId = CalcStripFromHit(out.getX() , out.getY());

    Int_t nuIn = ceil(inStripId);
    Int_t nuOut = ceil(outStripId);

    Double_t dQ; // fraction charge collected in one strip
    Int_t nstrips = abs(nuIn-nuOut);
    Int_t dir = (nuIn-nuOut)<0?1:-1;
    Int_t stripId;
    Double_t charge;

    //*** Collect info in central strips
    for(int istrip = 0; istrip < nstrips+1; istrip++){
    	stripId = nuIn + dir*istrip;
    	dQ = getchargefraction(in_2d, out_2d, stripId, eloss);
    	charge= AddNoise(dQ*3.68);
    	if(AboveThreshold(charge)){
    		strip.push_back(PndLumiStrip( stripId, charge ));
    	}
    }

    //*** Collect info in edge
    for(int istrip = 1; istrip <6; istrip++){
     	stripId = nuIn-dir*istrip;
     	dQ = getchargefraction(in_2d, out_2d, stripId, eloss);
     	charge= AddNoise(dQ*3.68);
     	if(AboveThreshold(charge)){
     		strip.push_back(PndLumiStrip( stripId, charge ));
     	}else{
     		break;
     	}
     }

    for(int istrip = 1; istrip < 6; istrip++){
    	stripId = nuOut+dir*istrip;
    	dQ = getchargefraction(in_2d, out_2d, stripId, eloss);
    	charge= AddNoise(dQ*3.68);
    	if(AboveThreshold(charge)){
    		strip.push_back(PndLumiStrip( stripId, charge ));
    	}else{
    		break;
    	}
	}
    return strip;
}

Double_t PndLumiCalcStripDigi::getchargefraction(TVector2 in, TVector2 out,
		Int_t stripId, Double_t eloss)
{
	Double_t charge = eloss/3.68;

	Double_t a = (out.Y()-in.Y())/(out.X()-in.X());
	Double_t b = in.Y()-a*in.X();
	Double_t path=out.X()-in.X();
	Int_t dq = 107;
	Int_t nstep = ceil(charge/dq);
	Double_t ds = path/nstep;
	Double_t xmin = (stripId-1)*fPitch;
	Double_t xmax = stripId*fPitch;
	Int_t n;
	Double_t dx, dy, xs;
	Double_t dQ;
	Double_t DQ=0.0;

	Double_t xr, yr;
	Double_t kstrip;
	Int_t k =0;
	for (n=0 ; n<nstep; n++){
		dx =in.X()+(n+0.5)*ds;
		dy = a*dx+b;
		xs = -dx*sin(fOrient)+dy*cos(fOrient);
		dQ = 0.5*(TMath::Erf((xmax-xs)/(fSigma*sqrt(2.)))-TMath::Erf((xmin-xs)/(fSigma*sqrt(2.))));
		DQ += (dQ * dq);
	}
	return DQ;

}
Double_t PndLumiCalcStripDigi:: AddNoise(Double_t charge)
{
	Double_t Q_n = fRND->Gaus(charge, fNoise);
    return Q_n;
}


Double_t PndLumiCalcStripDigi::CalcStripFromHit(Double_t x, Double_t y)
{

	Double_t a = fStripZeroId.X();
	Double_t b = fStripZeroId.Y();
	Double_t zero;
	zero =TMath::Abs(( -a*sin(fOrient) + b*cos(fOrient))/fPitch);
	Double_t nr = (-x*sin(fOrient) + y *cos(fOrient))/fPitch;

	return zero + nr;

}


bool PndLumiCalcStripDigi::AboveThreshold(Double_t Q)
{
	if (Q >= fThreshold){
		return true;
	} else {
		return false;
	}
}


void PndLumiCalcStripDigi::Print() const
{
    std::cout<<"-I- PndLumiCalcStripDigi Info :"<<std::endl;
    std::cout<<"     pitch                  = "<<fPitch<<" um"<<std::endl;
    std::cout<<"     orientation angle      = "<<fOrient/TMath::Pi()*180.<<" deg"<<std::endl;

}
ClassImp(PndLumiCalcStripDigi)
