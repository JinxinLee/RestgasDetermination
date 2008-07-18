//===================================================================
// File and Version Information:
// 	$Id: TCandReader.cxx,v 1.3 2002-02-01 23:00:07 marcel Exp $
//
//-----------------------------------------------------------------
// Description:
//	See TCandReader.h
//
//-------------------------------------------------------------------
// Sample User Code:
//
//------------------------------------------------------------------
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
//------------------------------------------------------------------
// Author List:
//	Abi Soffer              (Original author)
//
//--------------------------------------------------------------------------
// Copyright Information:
//	Copyright (C) 1998	Colorado State University
//
// ROOT Version by Marcel Kunze, RUB
//==========================================================================

//---------------
// C++ Headers --
//---------------
#include <strstream>
#include <ctype.h>

//-----------------------
// This Class's Header --
//-----------------------

#include "RhoTools/TCandReader.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "TVector3.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TFactory.h"
#include "RhoBase/TCandidate.h"
#include "RhoTools/TOpAdd4.h"
#include "RhoTools/TOpMakeTree.h"
#include "RhoTools/TAdd4Vertexer.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

ClassImp(TCandReader)

TBuffer &operator>>(TBuffer &buf, TCandReader *&obj)
{
   obj = (TCandReader *) buf.ReadObject(TCandReader::Class());
   return buf;
}

#include <iostream>
using namespace std;

//---------------------------
//-- Static Data definitions:
//---------------------------
// This is how subclasses of TCandReader register their instances with the 
// static registry:


Bool_t TCandReaderName::_registered = kFALSE;
Bool_t TCandReaderNameMom::_registered = kFALSE;
Bool_t TCandReaderNameVtx::_registered = kFALSE;
Bool_t TCandReaderNameMomVtx::_registered = kFALSE;

//------------------------------------------------
// Class TCandReader Static Function Definitions:
//------------------------------------------------

Bool_t 
TCandReader::IsDelimiter(char c) const {
    return (c == Open() || c == Separate() || c == Close());
}

//-------------------------------------------------------------------
const TCandReader * 
TCandReader::Find(const char * name) {
    return modifyRegistry().Find(name);
}

//----------------------------------------------------------------------
Bool_t 
TCandReader::Book(TCandReader * instance, const char * file, int line) {
    Bool_t result = modifyRegistry().Book(instance, file, line);
    if (kFALSE == result){
	const TCandReader * otherReader = registry().Find(instance->Name());
	int otherLine = registry().Line(otherReader);
	const char * otherFile = registry().File(otherReader);
	
	if (instance != otherReader){
	    cerr 
		<< "ERROR: TCandReader: Attempt to book a TCandReader named \""
		<< instance->Name() << "\"" << endl
		<< "       in line " << line << " of " << file << endl
		<< "       when a TCandReader with this name was already booked in "
		<< "line " << otherLine << " of" << endl
		<< "       " << otherFile << "." << endl
		<< "       The second TCandReader is deleted and not registered."
		<< endl;
	    
	    delete instance;
	    instance = 0;      
	}
	else { 
	    cerr
		<< "ERROR: TCandReader: Attempt to book the TCandReader named \""
		<< instance->Name() << "\"" << endl
		<< "       in line " << line << " of " << file << endl
		<< "       when it was already booked in line " << otherLine
		<< " of" << endl
		<< "       " << otherFile << "." << endl
		<< "       Booking attempt ignored."
		<< endl;
	}
    }
    return result;
}

//----------------------------------------------------------------------
TCandReaderRegistry & 
TCandReader::modifyRegistry() {
    static TCandReaderRegistry result;
    return result;
}

//----------------------------------------------------------------------
const TCandReaderRegistry & 
TCandReader::registry() {
    return modifyRegistry();
}

//---------------------------------------------------------------
// Class TCandReaderRegistry Public Member Function Definitions:
//---------------------------------------------------------------
TCandReaderRegistry::~TCandReaderRegistry() {
    _items.Delete();
}

//-----------------------------------------------------------------------
Bool_t 
TCandReaderRegistry::Book(TCandReader * reader, 
			  const char * file, int line) {
    // First, make sure no other instance of name exists:
    if (0 != Find(reader->Name())){
	return kFALSE;
    }
    else{
	_items.Add((TObject*)new Item(reader, reader->Name(), file, line));
	return kTRUE;
    }
}

//----------------------------------------------------------------------
const TCandReader * 
TCandReaderRegistry::Find(const char * name) const {
    for (int item = 0; item < _items.GetSize(); ++item){
	if (name == ((Item*)_items.At(item))->_name){
	    return ((Item*)_items.At(item))->_reader;
	}
    }
    return 0;
}

//----------------------------------------------------------------------
const char * 
TCandReaderRegistry::File(const TCandReader * reader) const {
    const Item * it = item(reader);
    if (0 != it){
	return it->_file;
    }
    else {
	return 0;
    }
}

//----------------------------------------------------------------------
int 
TCandReaderRegistry::Line(const TCandReader * reader) const {
    const Item * it = item(reader);
    if (0 != it){
	return it->_line;
    }
    else {
	return -1;
    }
}

//-----------------------------------------------------------
// Class TCandReaderName Public Member Function Definitions:
//-----------------------------------------------------------
// This function expects str to contain the name of the candidate:
TCandidate *
TCandReaderName::Read(TCandReader::Status & status, 
		      const TString & str,
		      TCandListIterator* input) const {
    TCandidate * result = 0;
    
    // The first substring in str with no white spaces:
    TString strNoWhite = str;//.Car();
    
    // If the iterator is valid, make the candidate from the daughters:
    if (0 != input){
	TOpAdd4 add4;
	result = add4.CreateFromList(*input);
    }
    else {
	result = TFactory::Instance()->NewCandidate();
    }
    
    const TParticlePDG * pdtEntry = TRho::Instance()->GetPDG()->GetParticle(strNoWhite);
    if (0 == pdtEntry){
	status = UNKNOWN;
    }
    else{
	result->SetType(pdtEntry);
	status = SUCCESS;
    }
    
    return result;
}

//--------------------------------------------------------------
// Class TCandReaderNameMom Public Member Function Definitions:
//--------------------------------------------------------------
//---------------------------------------------------------------
// This function expects str to contain the name of the particle, followed 
// by a 3-momentum in the format "(x,y,z)". Spaces are allowed, and commas
// can be replaced by spaces. Brackets must exist.
TCandidate *
TCandReaderNameMom::Read(TCandReader::Status & status, 
			 const TString & str,
			 TCandListIterator* input) const {
    TCandidate * result = 0;
    
    // Read the name, up to the '(' of the 3-momentum or another delimiter
    // or space:
    TString name;
    istrstream stream((char*)(const char*)str);
    char c;
    while (EOF != (c = stream.get()) && stream.good()){
	// If start of vector or delimiter, then end of name, push delimiter back:
	if ('(' == c || IsDelimiter(c) || (isspace(c) && !name.IsNull())) {
	    stream.putback(c);
	    break;
	}
	else{
	    if (0 == isspace(c)){
		name += c;
	    }
	}
    }
    
    TVector3 mom3 = ReadVector(stream);
    //stream >> mom3;
    TLorentzVector mom(mom3, 0.0);
    
    // If the iterator is valid, make the candidate from the daughters.
    // Don't take the momentum from the string, but from the daughters:
    if (0 != input){
	TOpAdd4 add4;
	result = add4.CreateFromList(*input);
    }
    else { // take the momentum from the string for non-composites:
	result = TFactory::Instance()->NewCandidate(TCandidate(mom, 0.0));
    }
    
    const TParticlePDG * pdtEntry = TRho::Instance()->GetPDG()->GetParticle(name);
    if (0 == pdtEntry){
	status = UNKNOWN;
    }
    else{
	result->SetType(pdtEntry);
	status = SUCCESS;
    }
    
    return result;
}


//----------------------------------------------------------------
// Class TCandReaderNameVtx Public Member Function Definitions:
//----------------------------------------------------------------
//---------------------------------------------------------------
// This function expects str to contain the name of the particle, followed 
// by a HepPoint in the format "(x,y,z)". Spaces are allowed, and commas
// can be replaced by spaces. Brackets must exist.
TCandidate *
TCandReaderNameVtx::Read(TCandReader::Status & status, 
			 const TString & str,
			 TCandListIterator* input) const {
    TCandidate * result = 0;
    
    // Read the name, up to the '(' of the point or another delimiter or space:
    TString name;
    istrstream stream((char*)(const char*)str);
    char c;
    while (EOF != (c = stream.get()) && stream.good()){
	// If start of point or delimiter, then end of name, push delimiter back:
	if ('(' == c || IsDelimiter(c) || (isspace(c) && !name.IsNull())) {
	    stream.putback(c);
	    break;
	}
	else{
	    if (0 == isspace(c)){
		name += c;
	    }
	}
    }
    
    TVector3 point = ReadVector(stream);;
    //stream >> point;
    //ghm  TSimpleVertex vertex(point);
    TAdd4Vertexer vertexer(point);
    
    // If the iterator is valid, make the candidate from the daughters.
    // Don't take the momentum from the string, but from the daughters:
    if (0 != input){
	//ghm    TFixedVertexer vertexer(vertex);
	TOpMakeTree makeTree(vertexer);
	result = makeTree.CreateFromList(*input);
    }
    else { // take the point from the string for non-composites:
	result = TFactory::Instance()->NewCandidate(
	    TCandidate(TLorentzVector(), 0.0, 
	    TFactory::Instance()->NewVertex(TSimpleVertex(point))));
    }
    
    const TParticlePDG * pdtEntry = TRho::Instance()->GetPDG()->GetParticle(name);
    if (0 == pdtEntry){
	status = UNKNOWN;
    }
    else{
	result->SetType(pdtEntry);
	status = SUCCESS;
    }
    
    return result;
}

//-------------------------------------------------------------------
// Class TCandReaderNameMomVtx Public Member Function Definitions:
//-------------------------------------------------------------------
//---------------------------------------------------------------
// This function expects str to contain the name of the particle, followed 
// by a HepPoint in the format "(x,y,z)". Spaces are allowed, and commas
// can be replaced by spaces. Brackets must exist.
TCandidate *
TCandReaderNameMomVtx::Read(TCandReader::Status & status, 
			    const TString & str,
			    TCandListIterator* input) const {
    TCandidate * result = 0;
    
    // Read the name, up to the '(' of the 3-momentum or another delimiter 
    // or space
    TString name;
    istrstream stream((char*)(const char*)str);
    char c;
    while (EOF != (c = stream.get()) && stream.good()){
	// If start of mom or delimiter, then end of name, push delimiter back:
	if ('(' == c || IsDelimiter(c) || (isspace(c) && !name.IsNull())) {
	    stream.putback(c);
	    break;
	}
	else{
	    if (0 == isspace(c)){
		name += c;
	    }
	}
    }
    
    TVector3 mom3 = ReadVector(stream);
    //stream >> mom3;
    TLorentzVector mom(mom3, 0.0);
    
    TVector3 point= ReadVector(stream);;
    //stream >> point;
    //ghm  TSimpleVertex vertex(point);
    TAdd4Vertexer vertexer(point);
    
    // If the iterator is valid, make the candidate from the daughters.
    // Don't take the momentum from the string, but from the daughters:
    if (0 != input){
	//ghm    TFixedVertexer vertexer(vertex);
	TOpMakeTree makeTree(vertexer);
	result = makeTree.CreateFromList(*input);
    }
    else { // take 3-momentum and point from the string for non-composites:
	result = TFactory::Instance()->NewCandidate(
	    TCandidate(mom, 0.0, 
	    TFactory::Instance()->NewVertex(TSimpleVertex(point))));
    }
    
    const TParticlePDG * pdtEntry = TRho::Instance()->GetPDG()->GetParticle(name);
    if (0 == pdtEntry){
	status = UNKNOWN;
    }
    else{
	result->SetType(pdtEntry);
	status = SUCCESS;
    }
    
    return result;
}

			    TVector3 TCandReader::ReadVector(std::istream & s) {
    TVector3 q;
    // Parse:
    TString cleanString;
    char c;
    int numRead = 0;
    Bool_t previosIsSpace = kFALSE;
    
    while(EOF != (c = s.get()) && c != ')' && s.good()) {
	// ignore initial bracket or spaces
	if (!cleanString.IsNull() || ('(' != c && 0 == isspace(c))){            
	    if (',' == c){          // replace ',' with ' '
		c = ' ';
	    }
	    cleanString += c;     // take c
	    if (isspace(c)){      // count # of white spaces (excluding initials)
		if (kFALSE == previosIsSpace) {
		    ++numRead;
		}
		previosIsSpace = kTRUE;
		if (numRead >= 12){  // read enough
		    break;
		}
	    }
	    else {
		previosIsSpace = kFALSE;
	    }
	}
    }
    
    istrstream newStream((char*)(const char*)cleanString);
    double xx(q.X()), yy(q.Y()), zz(q.Z());
    newStream >> xx >> yy >> zz;
    q.SetX(xx);
    q.SetY(yy);
    q.SetZ(zz);
    
    return q;
}

