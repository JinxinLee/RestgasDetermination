//==========================================================================
// File and Version Information:
// 	$Id: TReadTree.cxx,v 1.4 2002-02-01 23:00:19 marcel Exp $
//
//--------------------------------------------------------------------------
// Description:
//	See TReadTree.h
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
//
//--------------------------------------------------------------------------
// Copyright Information:
//	Copyright (C) 1998	Colorado State University
//
// ROOT Version by Marcel Kunze, RUB
//==========================================================================

#include <ctype.h>
#include "RhoTools/TReadTree.h"
#include "RhoTools/TOpAdd4.h"

ClassImp(TReadTree)

TBuffer &operator>>(TBuffer &buf, TReadTree *&obj)
{
   obj = (TReadTree *) buf.ReadObject(TReadTree::Class());
   return buf;
}

#include <iostream>
#include <strstream>
#include <string>
using namespace std;


TReadTree::TReadTree() :
  _crashOnError(kTRUE)
{
  SetReader(TCandReaderNameMomVtx().Name());
  Initialize();
}

//-------------------------------------------------------------------
TReadTree::TReadTree(const TReadTree & original) {
  *this = original;
}

//--------------
// Destructor --
//--------------
TReadTree::~TReadTree() {
}

//-------------
// Operators --
//-------------
TReadTree & 
TReadTree::operator=(const TReadTree & original) {
  Initialize();
  _reader = original._reader;
  _readerFromInput = original._readerFromInput;
  _crashOnError = original._crashOnError;
  return *this;
}

//-------------
// Accessors --
//-------------
    
//-------------
// Modifiers --
//-------------

void 
TReadTree::SetReader(const TCandReader * reader) {
  if (0 == reader){
    cerr
      << "ERROR: TReadTree::setReader(const TCandReader * reader):"
      << endl
      << "       argument is null. Aborting." << endl;
    abort();
  }

  _readerFromInput = kFALSE;  
  _reader = reader;
}

//---------------------------------------------------------------
void 
TReadTree::SetReader(const char * readerName) {
  SetReader(TCandReader::Find(readerName));
}

//---------------------------------------------------------------
TCandidate * 
TReadTree::Read(std::istream & stream) {
  TCandidate * result = 0;
/*
  // Clean up first:
  Initialize();
  
  // See if the TCandReader is specified in the input:
  string indicator;
  stream >> indicator;
  if (indicator.IsNull()) {
    return 0;  // nothing to read
  }
  string indicatorNoWhite = indicator;
  indicatorNoWhite.ToLower();

  if (ReaderIndicator() != indicatorNoWhite){
    // indicator is not a reader indication, so put in back into stream:
    for (int i = indicator.Length() - 1; i > -1; --i){
      stream.putback(indicator(i));
    }
    if (0 == stream.good()){
      cerr 
	<< "ERROR: TReadTree::read(istream&):" << endl
	<< "       Unable to putback \"" << indicator
	<< "\" into the stream." << endl;
      abort();
    }
  }
  else {
    // Read the name of the TCandReader from the stream:
    string name;
    stream >> name;
    
    if (name.IsNull()) {
      return 0;    // not much to do there
    }
    
    const TCandReader * reader = TCandReader::Find(name.c_str());
    if (0 == reader){
      CheckStatus(TCandReader::NO_READER, 0, name.c_str());
      return 0;
    }

    if (kTRUE == _readerFromInput){
      // Set the TCandReader:
      _reader = reader;
    }
    else {
      // Check that the name is compatible with the current _reader:
      if (_reader->Name() != name.c_str()){
	cerr 
	  << "ERROR: TReadTree:read(istream&):" << endl
	  << "       input specified TCandReader \""
	  << name.c_str() << "\", which is different from" << endl
	  << "       \"" << _reader->Name() 
	  << "\", which was set by a call to setReader(..)/" << endl;
	abort();
      }
    }
  }

  // Pass a list to hold the candidate's daughters to the recursive function:
  TCandList daughtersList;
  ReadFill(stream, daughtersList);

  if (1 == daughtersList.GetNumberOfCandidates()){
    result = &daughtersList[0];
  }
  else if (0 != daughtersList.GetNumberOfCandidates()){
    cerr << "TReadTree::read(istream&): daughtersList length = " 
		  << daughtersList.GetNumberOfCandidates() 
		  << " when expecting 1 or 0. Call Abi." 
		  << endl;
    ::abort();
  }

  // remove result from the _tree, which owns all the temporary candidates,
  // and then delete everything on _tree:
  _tree.Remove(_tree[_tree.GetNumberOfCandidates() - 1]);
  _tree.Cleanup();
*/
  return result;
}

//-----------
// Globals --
//-----------

//-------------------------------------------
//-- Protected Function Member Definitions --
//-------------------------------------------

void 
TReadTree::ReadFill(std::istream & stream, 
		      TCandList& daughtersList) {
  // Read a particle from the stream, and use the delimiter to determine
  // open/close of daughter lists, separation betwen sisters, or EOF:
  char delimiter = EOF;
  string nodeInfo = (const char*) Parse(stream, delimiter);

  if (_reader->Open() == delimiter){   
    ++_numOpens;                              // count # of open lists
    TCandList newDaughtersList;
    ReadFill(stream, newDaughtersList);      // read daughters into new list

    // Create the candidate and give it its nodeInfo:
    TCandListIterator newDaughterIter(newDaughtersList);
    TCandListIterator * newDaughterIterPtr = &newDaughterIter;
    if (0 == newDaughtersList.GetNumberOfCandidates()){
      newDaughterIterPtr = 0; // send 0 pointer to signal no daughters
    }

    TCandReader::Status status;
    TCandidate * cand = _reader->Read(status, nodeInfo.c_str(), newDaughterIterPtr);
    CheckStatus(status, nodeInfo.c_str(), _reader->Name());
    
    // put it on the owning list and its mother's daughters list:
    _tree.Add(*cand);
    daughtersList.Add(*cand);
    
    // The readFill(..) call above may return due to a close(). If there are as
    // many close()'s as open()'s, then this is the end of the input:
    if (0 == _numOpens){
      return;
    }
    else {
      ReadFill(stream, daughtersList);  // keep reading for sisters or close()
    }
  }
  else if (_reader->Separate() == delimiter){
    // If there is nodeInfo, create a candidate and put it on the owning list 
    // and on its mother's daughters list. Otherwise, we must be at the 
    // end of a close(), so no new candidate created:
    if (kFALSE == nodeInfo.empty()){
      TCandReader::Status status;
      TCandidate * cand = _reader->Read(status, nodeInfo.c_str(), 0);
      CheckStatus(status, nodeInfo.c_str(), _reader->Name());
      _tree.Add(*cand);
      daughtersList.Add(*cand);
    }
    // In any case, continue to read the sister:
    ReadFill(stream, daughtersList);
  }
  else if (_reader->Close() == delimiter){
    // end of a daughters list. Count # of open()'s:
    --_numOpens;

    // If there is nodeInfo, create a candidate amd put it on the owning list 
    // and on its mother's daughters list. Otherwise, we must be at the 
    // end of a close(), so no new candidate created:
    if (kFALSE == nodeInfo.empty()){
      TCandReader::Status status;
      TCandidate * cand = _reader->Read(status, nodeInfo.c_str(), 0);
      CheckStatus(status, nodeInfo.c_str(), _reader->Name());
      _tree.Add(*cand);
      daughtersList.Add(*cand);
    }
    return;
  }
  else if (EOF == delimiter){
    if (0 != _numOpens){
      cerr << "TReadTree::readFill(...): end of file with "
		    << _numOpens << " Open() (\'" << _reader->Open()
		    << "\') characters " << endl;
      ::abort();
    }
    return;
  }
  else {
    cerr << "TReadTree::read(...): unexpectd condition" << endl;
    ::abort();
  }
}

//------------------------------------------------------------------
TString 
TReadTree::Parse(std::istream & stream, char & delimiter) const {
  TString result;

  // Read the stream, looking for one of the delimiters. If not found,
  // append the character to the result, ignoring white spaces:
  while (kTRUE) {
    delimiter = stream.get();
    if (_reader->IsDelimiter(delimiter) || EOF == delimiter){
      return result;      
    }
    else {// take the character, discarding initial white spaces:
      if (0 == isspace(delimiter) || !result.IsNull()){
	result += delimiter;
      }
    }
  }
}
    
//-------------------------------------------------------------------
void 
TReadTree::CheckStatus(TCandReader::Status status,
			 const char * nodeInfo, const char * readerName) {
  // Update error statistics, complain and crash if asked to:
  ++_numTimes[status];

  if (TCandReader::SUCCESS == status) {
    return;
  }

  cerr << "ERROR: TReadTree read error:" << endl;
  switch(status) {
  case TCandReader::UNKNOWN:
    cerr << "     Input information was not understood."
		  << endl;
    break;
  case TCandReader::MISSING:
    cerr << "     Expected information was missing from the input"
		  << endl;
    break;
  case TCandReader::INVALID:
    cerr << "     Input information understood but incompatible"
		  << endl;
    break;
  case TCandReader::NO_READER:
    cerr << "     Unregistered TCandReader \"" << readerName
		  << "\" requested by input." 
		  << endl;
    break;
  default:
    cerr << "     Unknown error status " << status << endl;
  }
  
  cerr << "     Requested TCandReader was \"" 
		<< readerName << "\"." << endl
		<< "     Last TCandidate info was \""
		<< nodeInfo << "\"." << endl;

  if (kTRUE == _crashOnError) {
    abort();
  }
}


//-----------------------------------------
//-- Private Function Member Definitions --
//-----------------------------------------

void 
TReadTree::Initialize() {
  _numOpens = 0;
  for (int s = 0; s < TCandReader::NUM_STATUS_KINDS; ++s){
    _numTimes[s] = 0;
  }
}
  

//-----------------------------------
//-- Internal Function Definitions --
//-----------------------------------
