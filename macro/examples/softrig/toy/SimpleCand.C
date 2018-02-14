#include "TLorentzVector.h"

class SimpleCand {
public:
	SimpleCand() 
		{fLV.SetXYZT(0.,0.,0.,0.); fPid = 0; fId = 0; fCh = 0; fMotherIdx=-1; 
		fMct=false; fMcPid=false; fNSiblings=1; fNFS=1; fMarker=1<<fId; fDauF=-1; fDauL=-1; fNDau = 0; fEvtId=-1;fDaus.clear();}
		
	SimpleCand(TLorentzVector lv, int pid, int id, int ch) 
		{Set(lv,pid,id,ch); fMotherIdx=-1; fMct=false; fMcPid=false; 
		fNSiblings=1; fNFS=1;  fMarker=1<<fId; fDauF=-1; fDauL=-1;; fEvtId=-1;fNDau = 0; fDaus.clear();}
		
	SimpleCand(double x, double y, double z, double t, int pid, int id, int ch) 
		{fLV.SetXYZT(x,y,z,t); fPid = pid; fId = id; fCh=ch; fMotherIdx=-1; 
		fMct=false; fMcPid=false; fNSiblings=1;fNFS=1;  fMarker=1<<fId; fDauF=-1; fDauL=-1;fNDau = 0 ; fEvtId=-1;fDaus.clear();}
	
	~SimpleCand(){};
	
	void Set(TLorentzVector lv, int pid, int id, int ch) {fLV = lv; fPid = pid; fId = id; fCh = ch; fMarker=1<<fId;}
	void SetP4(TLorentzVector lv) {fLV = lv;}
	void SetP4(double x, double y, double z, double t) {fLV.SetXYZT(x,y,z,t);}
	void SetMass(double mass) { fLV.SetVectM(fLV.Vect(),fabs(mass)); }
	
	void SetPid(int pid) {fPid = pid;}
	void SetPdg(int pdg) {fPdg = pdg;}
	void SetId(int id) {fId = id;}
	void SetCharge(int ch) {fCh = ch;}
	void SetMotherIdx(int idx) {fMotherIdx=idx;}
	void SetMct(bool mct=true) { fMct=mct; }
	void SetMcPid(bool mct=true) { fMcPid=mct; }
	void SetNSiblings(int n) { fNSiblings=n; }
	void SetNFS(int n) { fNFS=n; }
	void SetMarker(unsigned int i) {fMarker = i;}
	void SetDau(int n, int m=0) {fDauF=n; fDauL=m==0?n:m;}
	void SetEvtId(int ev) {fEvtId = ev;}
	
	void AddDau(SimpleCand *c) {fDaus.push_back(c); fNDau++;}
	
	TLorentzVector P4() const {return fLV;}
	int   Pid()const  {return fPid;}
	int   Pdg()const  {return fPdg;}
	int   Id() const  {return fId;}
	int   EvtId() const {return fEvtId;}
	int   Charge() const  {return fCh;}
	int   MotherIdx()const  {return fMotherIdx;}
	int   NSiblings()const  {return fNSiblings;}
	int   NFS()const  {return fNFS;}
	bool  Mct() const {return fMct;}
	bool  McPid() const {return fMcPid;}
	unsigned int Marker() const {return fMarker;} 
	int   DauF() const {return fDauF;}
	int   DauL()const  {return fDauL;}
	int   NDau() const { return (fDauF>0 && fDauL>0) ? fDauL-fDauF+1 : 0;}	
	bool  IsDau(int idx)  const {return NDau()>0 ? (idx>=fDauF && idx<=fDauL) : false;}
	SimpleCand* Dau(int idx) {return fDaus[idx];}

private:
	TLorentzVector fLV;
	int			   fPid;
	int			   fPdg;
	int            fId;
	int 		   fEvtId;	
	int 		   fCh;
	int 		   fMotherIdx;
	int 		   fNSiblings;
	int			   fNFS;
	bool		   fMct;
	bool		   fMcPid;
	unsigned int   fMarker;
	int 		   fDauF;
	int 		   fDauL;
	int			   fNDau;
	std::vector<SimpleCand*> fDaus;
};

typedef std::vector<SimpleCand> CandList;
