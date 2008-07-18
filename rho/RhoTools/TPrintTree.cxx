//==========================================================================
// File and Version Information:
// 	$Id: TPrintTree.cxx,v 1.3 2002-02-01 23:00:19 marcel Exp $
//
//--------------------------------------------------------------------------
// Description:
//	See TPrintTree.h
//
//--------------------------------------------------------------------------
// Sample User Code:
//
//--------------------------------------------------------------------------
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
//--------------------------------------------------------------------------
// Author List:
//	Abi Soffer              (Original author)
//  Marcel Kunze             Adaptation to T
//
//--------------------------------------------------------------------------
// Copyright Information:
//	Copyright (C) 1998	Colorado State University
//
// ROOT Version by Marcel Kunze, RUB
//==========================================================================

#include "TList.h"
#include "TObjString.h"
#include "RhoTools/TPrintTree.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsTruth.h"
#include "RhoBase/VAbsVertex.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"

ClassImp(TPrintTree)

TBuffer &operator>>(TBuffer &buf, TPrintTree *&obj)
{
   obj = (TPrintTree *) buf.ReadObject(TPrintTree::Class());
   return buf;
}

#include <iostream>
#include <iomanip>
#include <strstream>
using namespace std;

//-----------------------------------------------
//-- Static Data & Function Member Definitions --
//-----------------------------------------------
const char* 
TPrintTree::PrintName(const TCandidate & cand) {
    TString result;
    const TParticlePDG * pdtEntry = cand.PdtEntry();
    if (0 != pdtEntry){
	result += pdtEntry->GetName();
    }
    return result.Data();
}

//------------------------------------------------------------------
const char* 
TPrintTree::PrintP4(const TCandidate & cand) {
    ostrstream stream;
    TLorentzVector q = cand.P4();
    // Reproduce the LorentzVector printout since the effect of setw(..) is lost
    // after each floating point output:
    stream << "(" 
	<<  q.X() << "," 
	<<  q.Y() << ","
	<<  q.Z() << ";" 
	<<  q.T() << ")" << ends;
    
    TString result(stream.str());
    delete [] stream.str();           // must take ownership here
    return result.Data();
}

//------------------------------------------------------------------
const char* 
TPrintTree::PrintVertex(const TCandidate & cand) {
    TString result;
    const VAbsVertex * vertex = cand.DecayVtx();
    if (0 != vertex){
	TVector3 q = vertex->Point();
	ostrstream stream;
	stream << "(" 
	    << q.X() << "," 
	    << q.Y() << "," 
	    << q.Z() << ")" << ends;
	
	result += stream.str();
	delete [] stream.str();      // must take ownership here
    }
    return result.Data();
}

//----------------------------------------
//-- Public Function Member Definitions --
//----------------------------------------

//----------------
// Constructors --
//----------------
//-----------------------------------------------------------
TPrintTree::TPrintTree() {
    Initialize();
    fTerm = 0;
}

//--------------
// Destructor --
//--------------
TPrintTree::~TPrintTree() {
    _lines.Delete();  
}


//-------------
// Accessors --
//-------------
//-----------------------------------------------------------
TStringLong
TPrintTree::Print(const TCandidate & cand) {
    MakeLines(cand);
    return PrintAgain();
}

TStringLong
TPrintTree::Print(const VAbsTruth & cand) {
    MakeLines(cand);
    return PrintAgain();
}

//--------------------------------------------------------------------  
TStringLong 
TPrintTree::PrintAgain() const {
    TStringLong result;
    for (int l = 0; l < _lines.GetSize(); ++l){
	TObjString *string = (TObjString *) _lines.At(l);
	result = result + string->GetString()  + TString("\n");
    }
    result = result + TString("\0");
    return result;
}

//-------------
// Modifiers --
//-------------
void 
TPrintTree::SetSeparator(const char * div) {
    _separator = div;
}

//-----------------------------------------------------------
void 
TPrintTree::SetLastMark(const char * div) {
    _lastMark = div;
}

//-----------------------------------------------------------
void 
TPrintTree::SetAligner(const char * div) {
    _aligner = div;
}

//-----------------------------------------------------------
void 
TPrintTree::SetSpaceFiller(char spa) {
    _spaceFiller = spa;
}

//-----------------------------------------------------------
void 
TPrintTree::SetMissingInfo(const char * info) {
    _missingInfo = info;
}

//-------------------------------------------------------------
void 
TPrintTree::AddTerminal(Int_t particle) {
    if (fTerm<100) _terminals[fTerm++] = particle;
}

void 
TPrintTree::AddTerminal(TString particles) {
    // Don't print the daughters of terminal particles:
    istrstream terminalsStream((char*)particles.Data());
    char terminal[128];
    while (terminalsStream >> terminal){
	const TParticlePDG * entry = TRho::Instance()->GetPDG()->GetParticle(terminal);
	if (entry != 0){
	    AddTerminal(entry->PdgCode());
	}
    }
}

//-------------------------------------------------------------
void 
TPrintTree::RemoveTerminal(Int_t particle) {
}


//-----------
// Globals --
//-----------

//-------------------------------------------
//-- Protected Function Member Definitions --
//-------------------------------------------

void 
TPrintTree::MakeLines(const TCandidate & cand) {
    _lines.Delete();  
    _daughtersLineNum = 0;
    MakeLines(cand, 0, kFALSE, kTRUE, 0);
}

//-------------------------------------------------------------
void
TPrintTree::MakeLines(const TCandidate & cand, int lineNumber,
			Bool_t firstDaughter, Bool_t lastDaughter,
			int numInitialSpaces) {
    
    // See if the line number exists in _lines. If not, new it:
    TObjString *line = 0;
    if (_lines.GetSize() > lineNumber){
	line = (TObjString*)_lines.At(lineNumber);
    }
    else{
	line = new TObjString();
    }
    
    TString stringLine = line->GetString();
    
    // If cand is the first daughter, align it with is parent.
    // Otherwise, push it beyond the longest line below it:
    if (kTRUE == firstDaughter){
	// Regardless of _spaceFiller, always use ' ' for first daughter:
	char localSpaceFiller = ' ';
	
	stringLine = stringLine + SpaceFillerString(numInitialSpaces - stringLine.Length(), 
	    localSpaceFiller);
    }
    else {
	// Use spaceFillers only of cand needs space for daughters in next line:
	if (cand.NDaughters() > 0){
	    int longestLineLength = 0;
	    for (int l = lineNumber; l < _lines.GetSize(); ++l){
		if (longestLineLength < (((TObjString*)_lines.At(l))->GetString()).Length()){
		    longestLineLength = (((TObjString*)_lines.At(l))->GetString()).Length();
		}
	    }
	    stringLine = stringLine + SpaceFillerString(longestLineLength - stringLine.Length(), 
		_spaceFiller);
	}
    }
    
    // If first daughter, indicate decay from parent using aligner:
    if (kTRUE == firstDaughter){
	stringLine = stringLine + TString(_aligner);
    }
    
    // numInitialSpaces for next recursive calls:
    const int nextNumInitialSpaces = stringLine.Length();
    
    // print the information of interest on the line:
    TString funcOutput;
    if (funcOutput==""){
	///		funcOutput += _missingInfo;
	if(cand.PdtEntry()) {
	    funcOutput += cand.PdtEntry()->GetName();
	}
	else {
	    funcOutput += _missingInfo;
	}
    }
    
    // Add separators:
    if (kTRUE == lastDaughter) {
	stringLine = stringLine + funcOutput + TString(_lastMark);
    }
    else {
	stringLine = stringLine + funcOutput + TString(_separator);
    }
    
    // If line is not in _lines yet, append it to the list:
    if (_lines.GetSize() <= lineNumber){
	_lines.Add(new TObjString(stringLine));
    }
    else
	line->SetString((char*)stringLine.Data());
    
    // If cand is a terminal particle, ignore its daughters:
    const TParticlePDG * candEntry = cand.PdtEntry();
    if (0 != candEntry){
	const int candType = candEntry->PdgCode();
	for (int t = 0; t < fTerm; ++t){
	    if (_terminals[t] == candType){
		return;
	    }
	}
    }
    
    // Otherwise, recursively call this function for daughters:
    Bool_t nextFirstDaughter = kTRUE;
    int nextLineNumber = lineNumber + 1;
    TCandListIterator iter(cand.DaughterIterator());
    const TCandidate * daughter;
    
    int sisterNum = 0;
    
    if(cand.NDaughters()>0) {
	while (daughter = iter.Next()){
	    Bool_t nextLastDaughter = kFALSE;
	    if (++sisterNum == cand.NDaughters()){
		nextLastDaughter = kTRUE;
	    }
	    
	    MakeLines(*daughter, nextLineNumber, nextFirstDaughter, 
		nextLastDaughter, nextNumInitialSpaces);
	    nextFirstDaughter = kFALSE;
	}
    }
}

//-----------------------------------------------------------
void 
TPrintTree::MakeLines(const VAbsTruth & cand) {
    _lines.Delete();  
    _daughtersLineNum = 0;
    MakeLines(cand, 0, kFALSE, kTRUE, 0);
}

//-------------------------------------------------------------
void
TPrintTree::MakeLines(const VAbsTruth & cand, int lineNumber,
			Bool_t firstDaughter, Bool_t lastDaughter,
			int numInitialSpaces) {
    
    // See if the line number exists in _lines. If not, new it:
    TObjString *line = 0;
    if (_lines.GetSize() > lineNumber){
	line = (TObjString*)_lines.At(lineNumber);
    }
    else{
	line = new TObjString();
    }
    
    TString stringLine = line->GetString();
    
    // If cand is the first daughter, align it with is parent.
    // Otherwise, push it beyond the longest line below it:
    if (kTRUE == firstDaughter){
	// Regardless of _spaceFiller, always use ' ' for first daughter:
	char localSpaceFiller = ' ';
	
	stringLine = stringLine + SpaceFillerString(numInitialSpaces - stringLine.Length(), 
	    localSpaceFiller);
    }
    else {
	// Use spaceFillers only of cand needs space for daughters in next line:
	if (cand.GetNumberOfDaughters() > 0){
	    int longestLineLength = 0;
	    for (int l = lineNumber; l < _lines.GetSize(); ++l){
		if (longestLineLength < (((TObjString*)_lines.At(l))->GetString()).Length()){
		    longestLineLength = (((TObjString*)_lines.At(l))->GetString()).Length();
		}
	    }
	    stringLine = stringLine + SpaceFillerString(longestLineLength - stringLine.Length(), 
		_spaceFiller);
	}
    }
    
    // If first daughter, indicate decay from parent using aligner:
    if (kTRUE == firstDaughter){
	stringLine = stringLine + TString(_aligner);
    }
    
    // numInitialSpaces for next recursive calls:
    const int nextNumInitialSpaces = stringLine.Length();
    
    // print the information of interest on the line:
    TString funcOutput;
    if (funcOutput==""){
	///		funcOutput += _missingInfo;
	if(cand.GetPdtEntry()) {
	    funcOutput += cand.GetPdtEntry()->GetName();
	}
	else {
	    funcOutput += _missingInfo;
	}
    }
    
    // Add separators:
    if (kTRUE == lastDaughter) {
	stringLine = stringLine + funcOutput + TString(_lastMark);
    }
    else {
	stringLine = stringLine + funcOutput + TString(_separator);
    }
    
    // If line is not in _lines yet, append it to the list:
    if (_lines.GetSize() <= lineNumber){
	_lines.Add(new TObjString(stringLine));
    }
    else
	line->SetString((char*)stringLine.Data());
    
    // If cand is a terminal particle, ignore its daughters:
    const TParticlePDG * candEntry = cand.GetPdtEntry();
    if (0 != candEntry){
	const int candType = candEntry->PdgCode();
	for (int t = 0; t < fTerm; ++t){
	    if (_terminals[t] == candType){
		return;
	    }
	}
    }
    
    // Otherwise, recursively call this function for daughters:
    Bool_t nextFirstDaughter = kTRUE;
    int nextLineNumber = lineNumber + 1;
    
    int sisterNum = 0;
    
    if(cand.GetNumberOfDaughters()>0) {
	for (int i=0;i<cand.GetNumberOfDaughters();i++) {
	    const VAbsTruth* daughter = cand.GetDaughter(i);
	    Bool_t nextLastDaughter = kFALSE;
	    if (++sisterNum == cand.GetNumberOfDaughters()){
		nextLastDaughter = kTRUE;
	    }
	    
	    MakeLines(*daughter, nextLineNumber, nextFirstDaughter, 
		nextLastDaughter, nextNumInitialSpaces);
	    nextFirstDaughter = kFALSE;
	}
    }
}

//-----------------------------------------------------------
TString 
TPrintTree::SpaceFillerString(int length, char spaceFiller) {
    TString result;
    if (length <= 0){
	return result;
    }
    else {
	char * spaces = new char[length + 1];  
	for (int s = 0; s < length; ++s){
	    spaces[s] = spaceFiller;
	}
	spaces[length] = '\0';
	result += spaces;
	delete[] spaces;
	return result;
    }
}

//-----------------------------------------
//-- Private Function Member Definitions --
//-----------------------------------------
// Do work common to different constructors:
void 
TPrintTree::Initialize() {
    _aligner = "|_ ";
    _separator = " ";
    _lastMark = " ";
    _spaceFiller = '-';
    _missingInfo = "NoInfo";
    _daughtersLineNum = 0;
}

//-----------------------------------
//-- Internal Function Definitions --
//-----------------------------------
