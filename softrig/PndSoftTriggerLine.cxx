// ************************************************************************
//
// Online SoftTrigger TriggerLine class
//
// K.Goetzen 08/2014
//
// ************************************************************************

// The header file
#include "PndSoftTriggerLine.h"

// C++ headers
#include <iostream>

// ROOT headers
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TObjArray.h"
#include "TObjString.h"

// RHO headers
#include "RhoMassParticleSelector.h"
#include "RhoTuple.h"

using std::cout;
using std::endl;

// -------------------------------------------------------------------------
// Default constructor
// -------------------------------------------------------------------------
PndSoftTriggerLine::PndSoftTriggerLine() : 
	fMode(-1), fName(""), fDecay(""),	fPrefix(""), fNTupleName(""), fWriteQA(false), fActive(true), fCC(false), fAux(false),			
	fQAMassMin(0.), fQAMassMax(20.), fMean(0.), fSigma(0.01), fThresh(0.), fTagNSig(5.), 
	fQASelector(0), fSelector(0), fNTuple(0)		
{
	fPdg = TDatabasePDG::Instance();	
}

// -------------------------------------------------------------------------
//  constructor
// -------------------------------------------------------------------------
PndSoftTriggerLine::PndSoftTriggerLine(int mode, TString name, TString dec, TString pre, TString ntpname) : 
	fMode(mode), fName(name), fDecay(dec),	fPrefix(pre), fNTupleName(ntpname), fWriteQA(false), fActive(true), fCC(false),	fAux(false),			
	fQAMassMin(0.), fQAMassMax(20.), fMean(0.), fSigma(0.01), fThresh(0.), fTagNSig(5.), 
	fQASelector(0), fSelector(0), fNTuple(0)		
{
	fPdg = TDatabasePDG::Instance();
}

// -------------------------------------------------------------------------
// Destructor 
// -------------------------------------------------------------------------
PndSoftTriggerLine::~PndSoftTriggerLine()
{
	if (fQASelector) delete fQASelector;
	if (fSelector) delete fSelector;
}

// -------------------------------------------------------------------------
// creates the Ntuple; user is responsible to call this function
// -------------------------------------------------------------------------
void PndSoftTriggerLine::Init()
{
	// initialize RhoTuple according to flag
	if (fWriteQA && fNTuple==0) fNTuple = new RhoTuple(fNTupleName, fDecay);
	
	// parse the decay pattern string and transform to fPdgList
	ParseDecayString();
	
	// create the selectors
	SetQASelector((fQAMassMax+fQAMassMin)/2., (fQAMassMax-fQAMassMin));
	SetSelector(fMean, fSigma*2.*fTagNSig);
}


// -------------------------------------------------------------------------
// convert the decay pattern string to a list of pdg codes (fPdgList)
// E.g.: "D0 -> K- pi+" gets converted to 421 -321 211
// Example with auxiliary resonance: "D*0 -> D0 [K- pi+] pi0" converts to 432 421 -99 -321 211 -98 111
// -------------------------------------------------------------------------
bool PndSoftTriggerLine::ParseDecayString()
{
	if (fDecay=="") return false;
	
	fPdgList.clear();
	
	TString toks[20];
	int n=SplitString(fDecay," ",toks,20);
	
	if (toks[1]!="->" || n<4)
	{
		cout <<"[PndSoftTriggerLine] **** Invalid decay pattern '"<<fDecay.Data()<<"'"<<endl;
		return false;
	}
	
	bool flag=false; // store if end of aux list codes
	
	for (int i=0;i<n;++i)
	{
		if (toks[i] == "cc") {fCC = true; continue;}                  // switch charged conjugation
		if (toks[i] == "->") continue;                                // ignore the arrow
		if (toks[i] == "pbp0") {fPdgList.push_back(88880); continue;}  // treat shortcut for pbarpSystem0
		if (toks[i] == "pbp")  {fPdgList.push_back(88888); continue;}  // treat shortcut for pbarpSystem
		if (toks[i] == "[") {fPdgList.push_back(-99);fAux=true; continue;} // auxiliary resonance list starts
		if (toks[i] == "]") {fPdgList.push_back(-98); continue;}           // auxiliary resonance list ends
		
		if (toks[i].BeginsWith("[")) {toks[i]=toks[i](1,1000); fAux=true; fPdgList.push_back(-99);} // auxiliary resonance list starts
		if (toks[i].EndsWith("]")) {toks[i]=toks[i](0,toks[i].Length()-1); flag = true;}            // auxiliary resonance list ends
			
		TParticlePDG *part = fPdg->GetParticle(toks[i]);
		if (part) fPdgList.push_back(part->PdgCode());
		else 
		{
			cout <<"[PndSoftTriggerLine] **** Unknown particle name '"<<toks[i].Data()<<"'"<<endl;
			return false;
		}
		if (flag) {fPdgList.push_back(-98); flag = false;} // auxiliary resonance list ends
	}
	
	return true;
}


// -------------------------------------------------------------------------
// set new parameters for the QA selector
// -------------------------------------------------------------------------
void PndSoftTriggerLine::SetQASelector(double m, double w)
{
	if (fQASelector) delete fQASelector;
	fQASelector = new RhoMassParticleSelector("QASel"+fName, m, w); 
}


// -------------------------------------------------------------------------
// set new parameters for the mass selector
// -------------------------------------------------------------------------
void PndSoftTriggerLine::SetSelector(double m, double w)
{
	if (fSelector) delete fSelector;
	fSelector = new RhoMassParticleSelector("Sel"+fName, m, w); 
}


// -------------------------------------------------------------------------
// splits a string into tokens; toks is the return array and has to created by the calling entity
// -------------------------------------------------------------------------
int PndSoftTriggerLine::SplitString(TString s, TString delim, TString *toks, int maxtoks)
{
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) if (i<maxtoks) toks[i] = (((TObjString*)tok->At(i))->String()).Strip(TString::kBoth);
	return N;
}


// -------------------------------------------------------------------------
// prints summary of this trigger line
// -------------------------------------------------------------------------
void PndSoftTriggerLine::Print()
{
	cout << "PndSoftTriggerLine: " << fName.Data() << " ("<<fPrefix.Data()<<")"<< endl;
	cout << "  Mode code       : " << fMode << endl;
	cout << "  Decay pattern   : " << fDecay.Data();
	if ( fPdgList.size()>0 )
	{
		cout << " ( " << fPdgList[0]<<" -> ";
		for (int i=1;i<fPdgList.size();++i)
		{
			if (fPdgList[i]==-99) cout <<" [";
			else if (fPdgList[i]==-98) cout <<" ]";
			else cout <<" "<<fPdgList[i];
		}
		if (fCC) cout <<" (+cc)";
		cout <<" )"<< endl;
	}else cout <<endl;
	cout << "  QA Window       : " << fQAMassMin << " < m < " << fQAMassMax << endl;
	cout << "  Selector Window : " << "| m - " << fMean << " | < " << fTagNSig << " x " << fSigma << endl;
	cout << "  Tag threshold   : " << fThresh << endl;
	cout << "  QA enabled      : " << fWriteQA << endl;
	cout << "  Tag active      : " << fActive << endl;
}


