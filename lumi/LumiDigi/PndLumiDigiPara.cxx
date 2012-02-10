#include "PndLumiDigiPara.h"

PndLumiDigiPara :: PndLumiDigiPara(const char* name,const char* title,const char* context)
                 : FairParGenericSet(name,title,context)
{
	clear();
}


void PndLumiDigiPara :: putParams (FairParamList* list)
{
	if(!list) return;
	list->add("pitch",fPitch);
	list->add("front_orient",fFrontOrient);
	list->add("back_orient",fBackOrient);
	list->add("front_anchor_x",(Double_t)fFrontAnchor.X());
	list->add("front_anchor_y",(Double_t)fFrontAnchor.Y());
	list->add("back_anchor_x",(Double_t)fBackAnchor.X());
	list->add("back_anchor_y",(Double_t)fBackAnchor.Y());
	list->add("nr_fe_channels",fChannel);
	list->add("nr_fe_top",fFrontFe);
	list->add("nr_fe_bottom",fBackFe);
	list->add("charge_threshold",fThreshold);
	list->add("charge_noise",fNoise);
	list->add("gaus_sigma",fSigma);
}


Bool_t PndLumiDigiPara::getParams(FairParamList* list)
{
	if(!list) return kFALSE;
	if(!list->fill("pitch", &fPitch))                    return kFALSE;
    if(!list->fill("front_orient", &fFrontOrient))       return kFALSE;
    if(!list->fill("back_orient", &fBackOrient))         return kFALSE;
    Double_t x, y;
    if(!list->fill("front_anchor_x", &x))                return kFALSE;
    if(!list->fill("front_anchor_y",&y))                 return kFALSE;
    fFrontAnchor.Set(x,y);
    if(!list->fill("back_anchor_x", &x))                 return kFALSE;
    if(!list->fill("back_anchor_y", &y))                 return kFALSE;
    fBackAnchor.Set(x,y);
    if(!list->fill("nr_fe_channels", &fChannel))    return kFALSE;
    if(!list->fill("nr_fe_top", &fFrontFe))   return kFALSE;
    if(!list->fill("nr_fe_bottom",&fBackFe)) return kFALSE;
    if(!list->fill("charge_threshold", &fThreshold))   	 return kFALSE;
    if(!list->fill("charge_noise", &fNoise))             return kFALSE;
    if(!list->fill("gaus_sigma",&fSigma))                return kFALSE;

    return kTRUE;

}
ClassImp(PndLumiDigiPara);


void PndLumiDigiPara::Print()
{
	cout<<"------------------------------------------------------------------------------ "<<endl;
	cout<<"--------  The Digitization Parameters for The Luminosity Monitor  ------------ "<<endl;
	cout<<"------------------------------------------------------------------------------ "<<endl;
	cout<<"-- Parameters \t \t \t \t \tValue\t \t \tUnits"<<endl;
	cout<<"------------------------------------------------------------------------------ "<<endl;
	cout<<"-- Pitch " <<"\t \t \t \t \t"<<fPitch<<"\t \t \t"<<"cm"<<endl;
    cout<<"-- Front Anchor "<<"\t \t \t \t"<<"( "<<fFrontAnchor.X()<<" , "<<fFrontAnchor.Y()<<" )"<<"\t \t"<<"cm"<<endl;
    cout<<"-- Back Anchor "<<"\t \t \t \t \t"<< "( "<<fBackAnchor.X()<<" , "<<fBackAnchor.Y()<<" )"<<"\t \t"<<"cm"<<endl;
    cout<<"-- Front Orient "<<"\t \t \t \t"<<fFrontOrient/TMath::Pi()*180<<"\t \t \t"<<"degree"<<endl;
    cout<<"-- Back Orient "<<"\t \t \t \t \t"<<fBackOrient/TMath::Pi()*180<<"\t \t \t"<<"degree"<<endl;
    cout<<"-- nr Channel per Fe " <<"\t \t \t \t"<<fChannel<<endl;
    cout<<"-- nr Fe's on Front Side " <<"\t \t \t \t"<<fFrontFe<<endl;
    cout<<"-- nr Fe's on Back Side " <<"\t \t \t \t"<<fBackFe<<endl;
    cout<<"-- Sigma of Gaussian Charge diffusion "<<"\t \t"<<fSigma<<"\t \t \t"<<"cm"<<endl;
    cout<<"-- Threshold "<<"\t \t \t \t \t"<<fThreshold<<"\t \t \t"<<"eV"<<endl;
    cout<<"-- Noise "<<"\t \t \t \t \t"<<fNoise<<"\t \t \t"<<"eV"<<endl;
    cout<<"------------------------------------------------------------------------------ "<<endl;


}

