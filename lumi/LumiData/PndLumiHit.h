#ifndef PNDLUMIHIT_H
#define PNDLUMIHIT_H 1


#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"

#include "PndLumiStrip.h"


class PndLumiHit : public FairHit
{
	friend std::ostream& operator<< (std::ostream& out, const PndLumiHit& hit){
		out << "PndLumiHit in " << hit.GetDetName() << " at" << std::endl;
		out << "(" << hit.GetX() << ", " << hit.GetY() << ", " << hit.GetZ() << ") cm "
		<< " with " << hit.GetCharge() << " e" << ", Cluster No. " << hit.GetClusterIndex();
		if (hit.GetBotIndex() > -1)
			out << " and bottom " << hit.GetBotIndex();
		if (hit.GetRefIndex() > -1)
			out << ", mc point id = " << hit.GetRefIndex();
		else
			out << ", noise hit without mc point";
		out << std::endl;

		out <<"hit.GetClusterIndex() "<<hit.GetClusterIndex() <<std::endl;
		out <<" hit.GetBotIndex() "<< hit.GetBotIndex() <<std::endl;
		out <<"hit.GetRefIndex() "<< hit.GetRefIndex()<<std::endl;

		return out;
	}

public:

	PndLumiHit();

	/** Standard constructor with all parameters
	 *    *@param detID   Detector ID
	 *    *@param detName Detector Name
	 *    *@param pos     Position vector [cm]
	 *    *@param dpos    Position error vector [cm]
	 *    *@param index   Array index of ClusterCand
	 *    *@param charge  Charge measured in hit
	 *    *@param NDigiHits Number of pixels fired for this event
	 *     **/

	PndLumiHit(Int_t detID, TString detName,TVector3& pos, TVector3& dpos,
			Int_t clindex, Double_t charge, Int_t NDigiHits, Int_t mcindex,
			Bool_t IsDoubleSided);

	PndLumiHit(Int_t detID, TString detName, Double_t &slope, Double_t& slopeErr,
			Double_t& intercept, Double_t &intErr, Int_t clindex, Double_t charge,
			Int_t NDigiHits, Int_t mcindex,	Bool_t IsDoubleSided);


	virtual ~PndLumiHit();

	/** Accessors **/

	void SetDetName(TString name)  { fDetName   = name;}
	void SetCharge(Double_t charge){ fCharge    = charge;}
	void SetNDigiHits(Int_t pixel) { fNDigiHits = pixel;}
	void SetClusterIndex(Int_t id) { fClusterIndex = id;}
	void SetBotIndex(Int_t id)     { fBotIndex  = id;}

	Bool_t IsDoubleSided()       const{return fIsDoubleSided;}

	TString GetDetName() const { return fDetName;}
	Double_t GetCharge() const { return fCharge;}
	Int_t GetNDigiHits() const { return fNDigiHits;}
	TVector3 GetPosition() const { return TVector3(fX,fY,fZ);}
	Int_t GetClusterIndex() const { return fClusterIndex;}
	Int_t GetTopIndex() const { return GetClusterIndex();}
	Int_t GetBotIndex() const { return fBotIndex;}
	//Int_t GetMCIndex() const {return fMCIndex;}
	Double_t GetEloss() const { return (fCharge * 3.61e-9);}  // 3.6 eV/Electron in Silicon
	Double_t GetSlope() const {return fSlope;}
	Double_t GetYIntercept() const {return fIntercept;}



	/** Screen output **/

	virtual void Print(const Option_t* opt = 0) const;

private:

	Int_t fMCIndex;

	TString fDetName;  // Detector name
	Double_t fCharge; /// deposited Charge
	Int_t fNDigiHits; /// number of fired Digis for this hit,
	Int_t fClusterIndex; /// top/pixel cluster index
	Int_t fBotIndex; /// bottom side of strip clusters
	Double_t fSlope; //slope of the straight line (for only single-sided sensor)
	Double_t fIntercept; //y-intercept of the straight line (for only single-sided sensor)

	Bool_t fIsDoubleSided;//Identify is the sensor is double-sided active or not

	ClassDef(PndLumiHit,1);

};
#endif
