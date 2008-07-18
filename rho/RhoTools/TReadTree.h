//==========================================================================
// File and Version Information:
// 	$Id: TReadTree.h,v 1.3 2002-02-01 03:54:58 marcel Exp $
//
//--------------------------------------------------------------------------
// Description:
//    Class TReadTree is a class which can read a TCandidate decay
//    tree from a stream. The function that does that is 
//       TCandidate * read(istream &);
//
//    NOTE: The caller owns the returned pointer.
//    
// TReadTree can use any input format for which there is a TCandReader
// (see TCandReader.h). The default TCandReader is an instance of 
// TCandReaderNameMomVtx, which is booked in the TCandReader static
// registry. There are two ways to change the TCandReader-selection 
// behavior of TReadTree:
// * To force it to use a TCandReader which is not the default one, call
//     void setReader(const TCandReader * reader);
// 
//   You cal also let it search the TCandReader registry for an instance
//   with a particular name:
//     void setReader(const char * readerName);
//
// * To make the TReadTree select the TCandReader from the input to the
//   read(istream&) function, call
//     void setReaderFromInput();
//
//   If this function has been called, then the istream argument of 
//   read(istream&) should start with the word "reader", followed by the 
//   name of a TCandReader which has been booked in the static 
//   TCandReader registry. If the name doesn't exist in the registry, 
//   an error message is printed and the job crashes. An example of legal
//   format would then be (see more examples and details below):
//      "reader NameMomVtx D0 {K- & pi+}"
//   
//   If setReader(...) was called, the istream does not have to start with 
//   a reader specification, in which case the TCandReader specified by the
//   setReader(..) function will be used to read the TCandidates on the 
//   stream. If a reader specification does appear in the beginning of the 
//   stream, read(istream&) will complain and crash if that TCandReader has
//   a different name() from the one set by setReader(..).
//
// You can check the read mode of the TReadTree by calling
//     Bool_t readerFromInput() const;
//
// A return value of kTRUE means that setReaderFromInput() was called, false
// means that setReader(..) was called. By default, a TReadTree is in the 
// latter mode, with TCandReaderNameMomVtx as its TCandReader.
//
// TReadTree uses three types of delimiters to parse the tree. The delimiters
// are determined by the TCandReader used to read individual TCandidates
// in the tree, in order to ensure that the TCandReader doesn't use these
// delimiters for other purposes (such as separating between elements of 
// 3-vectors, etc.). The 3 TCandReader functions that specify these 
// delimiters are 
//   virtual char TCandReader::open() const {return '{';}     
//   virtual char TCandReader::close() const {return '}';}    
//   virtual char TCandReader::separate() const {return '&';} 
//
// The delimiters are used to start (open()) a daughters list, end (close())
// a daughters list, and delimit (separate()) between sisters in a daughters
// list.
// 
// With the default delimiters above, here are examples of legal input:
//
//    "K+ {}" or "K+{}" or "K+{ }" or "K+ { }"
// A K+ with no daughters specified. The "{}" is required to tell the 
// TReadTree that the input has ended. Another way to do that is to end
// the input (with ^d or other end of stream indication). Note that the open()
// and close() delimiters have the same effect regardless of spaces before or 
// after them. This is kTRUE of the separate() delimiter too, so no further 
// examples involving spaces are given below.
//
//    "D0 {K- & pi+}"
// A D0 decaying into a K- and a pi+
//
//    "B- {D0 {K- & pi+} & pi-}"
// A B- decaying into a D0 and a pi-, with the D0 decaying into a K- and a pi+.
// I is also allowed to drop the separate() delimiter after the close() of a 
// daughter list, ie.:
//    "B- {D0 {K- & pi+} pi-}"
//
// 3-vectors and HepPoints can be specified using the format (1,2,3), where
// the brackets are not necessary, the commas can be replaced by spaces, and
// additional spaces can be inserted (this flexibility may disappear if CLHEP
// input operators become more stupid). When using TCandReaderNameMomVtx,
// both the 3-momentum and decayVtx point can (but don't have to) be specified
// for each TCandidate in the tree. For example:
//    "D0 (1 1 1) 2 2 2 {K- 3,3 3 & pi+ (4,4,4)}"
//
// Here, the D0 has momentum (1,1,1) and decay Vtx point (2,2,2), and the 
// momenta of the K- and pi+ are (3,3,3) and (4,4,4), respectively. Despite the
// apparent momentum conservation violation, the D0's momentum is ignored, 
// since TOpMakeTree will set its momentum by adding up the momenta of its 
// daughters. Its decayVtx point, however, will be set from the input. The
// decayVtx points of the K- and pi+ were not given in this exaple, but they
// can be.
//
//--------------------------------------------------------------------------
// Collaborating classes:
//
//--------------------------------------------------------------------------
// Sample User Code:
//
//--------------------------------------------------------------------------
// Compiler Default Member Functions:
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

#ifndef TREADTREE_H
#define TREADTREE_H

#include <iostream>

#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoTools/TCandReader.h"

class TCandidate;
class TString;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TReadTree : public TObject {
    //----------------
    // Static members:
    //----------------
public: 
    // When the TCandRead is specified in the input stream to read(stream),
    // This string is searched for in the beginning of the input to 
    // read(istream&) to indicate that it is followed by the name() of a 
    // TCandRead, which is to be used to read the rest of the stream. For 
    // example, the input could be "reader NameVtx D0 (0.1 0 0) {K- & pi+}",
    // indicating that the TCandReader named "NameVtx" reads the TCandidates
    static const char * ReaderIndicator() {return "reader";}


    //------------------
    // Instance members:
    //------------------
public:
    // Constructors:
    TReadTree();
    TReadTree(const TReadTree & original);

    // Destructor
    virtual ~TReadTree();

    // Assignment operator:
    TReadTree & operator=(const TReadTree &);

    // Accessors:
    // The distribution of Status values obtained during the last read(..) call:
    int NumTimes(TCandReader::Status status) const {return _numTimes[status];}

    // whether to crash on read error or just note the error in numTimes(..):
    Bool_t CrashOnError() const {return _crashOnError;}        // default = kTRUE

    // The current TCandReader used to read nodes in the tree:
    const TCandReader * Reader() const {return _reader;}

    // Whether using a fixed TCandReader or figuring which one to use 
    // from the input to read(stream):
    Bool_t ReaderFromInput() const {return _readerFromInput;}

    // Modifiers
    // Read a TCandidate from a stream:
    virtual TCandidate * Read(istream & stream);

    // whether to crash on read error or just note the error in numTimes(..):
    void SetCrashOnError(Bool_t b) {_crashOnError = b;}  // default = kTRUE

    // Figure out the TCandReader to use from its name specified in the 
    // input to read(stream). This setting implies
    // that the input must begin with the namd() of the TCandReader to use:
    void SetReaderFromInput() {_readerFromInput = kTRUE;}

    // Set the TCanrReader to use for reading nodes of the tree. 
    // The reader can be specified using its name, which should be registered
    // with TCandReader::book(..). Calling these functions sets 
    // _readerFromInput to false. The default constructor calls this functon:
    void SetReader(const TCandReader * reader);
    void SetReader(const char * readerName);


protected:
    // Helper functions
    virtual void ReadFill(istream & stream, TCandList& daughters);

    void Initialize();

    TString Parse(istream & stream, char & delimiter) const;

    void CheckStatus(TCandReader::Status status,
	const char * nodeInfo, const char * readerName);


private:  
    // Data members
    const TCandReader * _reader;    //! not owned
    Bool_t _readerFromInput; //!
    Bool_t _crashOnError; //!
    int _numOpens; //!
    int _numTimes[TCandReader::NUM_STATUS_KINDS]; //!
    TCandList _tree;    // !make memory management easier

public:
    ClassDef(TReadTree,1) // A class which can read a TCandidate decay tree from a stream
};

#endif
