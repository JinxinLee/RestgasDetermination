#include "PndLumiCalcStrip.h"

TRandom3 *fRND = new TRandom3();

PndLumiCalcStrip::PndLumiCalcStrip()
{
	fPitch = 0.;
	fOrient = 0.;
	fThreshold = 0.;
	fNoise = 0.;
	fSensorWidth = -1.;
	fSensorLength= -1.;
}


PndLumiCalcStrip::PndLumiCalcStrip(Double_t pitch, Double_t orient, Double_t width,
		Double_t length, Double_t threshold, Double_t noise)
{
	fPitch = pitch;
	fOrient = orient;
	fSensorWidth = width;
	fSensorLength = length;
	fNoise = noise;
	fThreshold = threshold;
}

std::vector<PndLumiStrip>PndLumiCalcStrip::GetStrips(FairGeoVector in, FairGeoVector out,
		Double_t eLoss)
{
	// Number total of strip in a sensor
	Double_t diag = CalcDiag(fSensorWidth, fSensorLength);
	Int_t nStrips =  Int_t((diag * fabs(cos(fabs(atan(fSensorWidth/fSensorLength)
			-fabs(fOrient)))))/ fPitch) + 1;

	// 2d-Projection of trajectory
    TVector2 vin(in.getX(),in.getY());
    TVector2 vout(out.getX(),out.getY());
    TVector2 vpath=vout-vin;

    Double_t path = vpath.Mod();

    std::vector<PndLumiStrip> strip;

    Double_t Q_eff;

    //Calcul of the strip indices from the entry and exit point
    Double_t nuIn  = CalcStripFromPoint(in.getX(),in.getY());
    Double_t nuOut = CalcStripFromPoint(out.getX(),out.getY());


    Double_t dir = 0.;
    if (nuOut>nuIn) dir=1.;
    else dir = -1.;

    Double_t Q = eLoss;

    //***********Information in each strip fired

    // if only one strip fired
    if (Int_t(nuIn) == Int_t(nuOut)){

    	Q_eff = AddNoise(Q);
    	// this strip collected the entire charge
    	if (Q_eff>= fThreshold){

    		strip.push_back(PndLumiStrip(Int_t(nuOut),path,Q_eff));
            cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;

    	}

    } else{

    	// Information at the first strip

        Int_t nrHits = 0;
        if (dir>0.){

        	Double_t fract_path = (std::floor(nuIn+1.)-nuIn)/(nuOut-nuIn);
        	Double_t Q1 = Q*(std::floor(nuIn+1.)-nuIn)/(nuOut-nuIn);
        	Q_eff = AddNoise(Q1);

        	if (Q_eff >= fThreshold){

        		strip.push_back(PndLumiStrip(Int_t(nuIn),(fract_path * path),Q_eff));
                cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;

        	}

        	nrHits++;
            Q -= Q1;
            nuIn = std::floor(nuIn + 1.);

        } else{

        	Double_t fract_path = (nuIn-(std::floor(nuIn)))/(-nuOut+nuIn);
        	Double_t Q1 = Q*(nuIn-(std::floor(nuIn)))/(-nuOut+nuIn);
            Q_eff = AddNoise(Q1);

            if (Q_eff>= fThreshold){

            	strip.push_back(PndLumiStrip(Int_t(nuIn),(fract_path * path), Q_eff));
                cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;

            }

            nrHits++;
            Q -= Q1;
            nuIn = std::floor(nuIn);

        }

        // Information at the last strip

        if (dir<0.){

        	Double_t fract_path = (std::floor(nuOut+1.)-nuOut)/(nuIn-nuOut);
        	Double_t Q2 = Q*(std::floor(nuOut+1.)-nuOut)/(nuIn-nuOut);
            Q_eff = AddNoise(Q2);

            if (Q_eff>= fThreshold){
                strip.push_back(PndLumiStrip(Int_t(nuOut),(fract_path * path),Q_eff));
                cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
            }

            nrHits++;
            Q -= Q2;
            nuOut = std::floor(nuOut+1.);

        } else{

        	Double_t fract_path = (nuOut-(std::floor(nuOut)))/(-nuIn+nuOut);
        	Double_t Q2 = Q*(nuOut-(std::floor(nuOut)))/(-nuIn+nuOut);
        	Q_eff = AddNoise(Q2);

        	if (Q_eff>= fThreshold){

                strip.push_back(PndLumiStrip(Int_t(nuOut),(fract_path * path),Q_eff));
                cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
        	}

        	nrHits++;
            Q -= Q2;
            nuOut = std::floor(nuOut);

        }

        // Distribute the left charge among the intermediate strips

        Double_t fract_path = 1./(std::fabs(nuOut-nuIn));
        Double_t dQ=Q/(std::fabs(nuOut-nuIn));
        Q_eff = AddNoise(dQ);

        if (dir>0.){

        	for (Int_t n= Int_t(nuIn); n<Int_t(nuOut); n++){
        		if (Q_eff>= fThreshold){

        			strip.push_back(PndLumiStrip(n,(fract_path * path),Q_eff));
        			cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;

        		}

        		nrHits++;
        	}

        } else{

        	for (Int_t n = Int_t(nuOut) ; n<Int_t(nuIn); n++){
        		if (Q_eff>= fThreshold){

        			strip.push_back(PndLumiStrip(n,(fract_path * path),Q_eff));
        			 cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
        		}

        		nrHits++;
        	}
        }
    }

   return strip;
}

Double_t PndLumiCalcStrip::AddNoise(Double_t charge)
{
	Double_t q = fRND->Gaus(charge,fNoise);
     return q;
}

Double_t PndLumiCalcStrip::CalcStripFromPoint(Double_t x, Double_t y)
{
	Double_t Norme = CalcDiag(x,y);
	return Norme * fabs(cos(fabs(atan(fabs(x/y))-fabs(fOrient))))/fPitch;
}

Double_t PndLumiCalcStrip::CalcDiag(Double_t a, Double_t b)
{
	return sqrt(a*a + b*b);
}
ClassImp(PndLumiCalcStrip)
