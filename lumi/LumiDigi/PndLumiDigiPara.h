/*
 *
 * PndLumiDigiPara.h
 *
 *    Author: tsito
 *
 */

#ifndef PNDLUMIDIGIPARA_H_
#define PNDLUMIDIGIPARA_H_

#include "FairParGenericSet.h"
#include "FairParamList.h"

#include "TString.h"
#include "TVector2.h"
#include "TMath.h"
#include <iostream>

using namespace std;

class PndLumiDigiPara : public FairParGenericSet
{
public:

	/** Constructor*/
	PndLumiDigiPara(const char* name = "PndLumiDigiParameter",
			const char* title = "PndLumi Strip Digi Parameter",
			const char* context = "PndLumiDefaultContext");

	/** Destructor */
	~PndLumiDigiPara(void){}

	void clean(void){}

	void putParams(FairParamList* list);

	Bool_t getParams(FairParamList* list);

	void Print();

	/** Accessor methods */
	Double_t GetPitch()   const {return fPitch;}
	Double_t GetFrontOrient()   const {return fFrontOrient;}
	Double_t GetBackOrient()    const {return fBackOrient;}
	TVector2 GetFrontAnchor()  const {return fFrontAnchor;}
	TVector2 GetBackAnchor()   const {return fBackAnchor;}
	Double_t GetGausSigma()    const {return fSigma;}
	Double_t GetThreshold()     const {return fThreshold;}
	Double_t GetNoise()         const {return fNoise;}
	const char* GetSensType()   const {return fSensType.Data();}

	/** Assign methods */
	void SetPitch(Double_t x)   {fPitch = x;}
	void SetFrontOrient(Double_t x)    {fFrontOrient = x;}
	void SetBackOrient(Double_t x)   {fBackOrient = x;}
	void SetFrontAnchor(TVector2& x)  {fFrontAnchor = x;}
	void SetBackAnchor(TVector2& x)  {fBackAnchor = x;}
	void SetThreshold(Double_t x)   {fThreshold=x;}
	void SetNoise(Double_t x)         {fNoise=x;}
	void SetGausSigma(Double_t x)     {fSigma = x;}
	void SetSensType(TString x)       {fSensType = x;}


private:

	Double_t fPitch;		//the pitch
	Double_t fFrontOrient;  //strip orientation in the front side
	Double_t fBackOrient;   //strip orientation in the back side
    Double_t fFrontAnchorX;
    Double_t fFrontAnchorY;
    Double_t fBackAnchorX;
    Double_t fBackAnchorY;
    TVector2 fFrontAnchor; 	//anchor point on the front side
    TVector2 fBackAnchor;   //anchor point on the back side
    Double_t fThreshold;    //the threshold of the signal
    Double_t fNoise;        //the noise of the electronics
    Double_t fSigma;        //the sigma of gaussian distribution for charge diffusion
    TString  fSensType;      //the type of sensor: either double-sided or single-sided


    ClassDef(PndLumiDigiPara,1);

};


#endif /* PNDLUMIDIGIPARA_H_ */
