//================================================================
// File and Version Information:
// 	$Id: TCandReader.h,v 1.4 2008-02-25 15:27:23 marcel Exp $
//
//----------------------------------------------------------------
// Description:
//	Class TCandReader is a base class for objects which read a 
// single TCandidate from a TString, with different subclasses 
// being able to read different kinds of information. Some 
// subclasses are also declared in this file. The main purpose of 
// TCandReader objects is to be used by TReadTree to read the 
// nodes (single TCandidates) of a decay tree. See TReadTree.h for
// mode details.
//      A static registry of subclass instantiations is
// maintained by the base class, allowing TReadTree to choose 
// the TCandReader which is appropriate for a particular input.
// To book a subclass instance, call
//
//    static Bool_t book(TCandReader * instance, const char * file, int line);
//
// NOTE: The registry owns the TCandReader * argument.
// This call is simplified by using the following macro, which automatically
// inserts the file and line:
//
//    book(TCandReader * instance)
//
// A TCandReader instance can be retreived from the registry using
//
//    static const TCandReader * find(const char * name);
//
// You can also see the # of TCandReaders booked and their names, using
//    static inline int numInstances();
//    static inline TString instanceName(int ins);
//
//
// The reading of a TCandidate is done thgouth the pure virtual 
//
//  virtual TCandidate *
//  Read(TCandReader::Status & status, 
//       const TString & str,
//       HepAListIterator<TCandidate> * iter) const = 0;
//
// The caller owns the returned pointer.
// Instance or class identification information is oTined through the pure
// virtual 
//
//  virtual const char * name() const = 0;
//
// which is used to indicate to TReadTree which TCandReader should be used
// to read a particular input.
//
// Additional functions of significance are 
//   virtual char open() const {return '{';}     // start a daughter list
//   virtual char close() const {return '}';}    // end a daughter list
//   virtual char separate() const {return '&';} // separate sisters
//
// These functions determine the delimiters which should be used in the 
// input stream to TReadTree to separate between sisters, and to open
// and close daughters lists. These delimiters are determined by the 
// TCandReader rather than the TReadTree, since there might be 
// characters which the TCandReader uses as its own delimiters (such as
// commans in 3-vectors). On the other hand, the open(), close() and separate()
// delimiters are guaranteed to be usable with a particular TCandReader.
//
//--------------------------------------------------------------------
// Input format of subclasses declared in this file:
//    The TCandReader subclasses declared here are 
// TCandReaderName: Reads only the pdt.table name of the particle, 
//   which determines its type
// TCandReaderNameMom: Reads the name and the 3-momentum
// TCandReaderNameVtx: Reads the name and the decay vertex position
// TCandReaderNameMomVtx: Reads the name, 3-momentum, and the decay vertex 
//   position
//
// 3-momenta and vertex points are specified using the format specified
// by HepPoint and Hep3Vector.
// 
//--------------------------------------------------------------------
// Collaborating classes:
//
//--------------------------------------------------------------------
// Sample User Code:
//
//--------------------------------------------------------------------
// Compiler Default Member Functions:
//
//--------------------------------------------------------------------
// Environment:
//    Software developed for the BaBar Detector at the SLAC B-Factory.
//
//-------------------------------------------------------------------
// Author List:
//	Abi Soffer              (Original author)
//
//-------------------------------------------------------------------
// Copyright Information:
//	Copyright (C) 1998	Colorado State University
//
// ROOT Version by Marcel Kunze, RUB
//===================================================================

#ifndef TCANDREADER_H
#define TCANDREADER_H

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------

//-----------------
// BaBar Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "TObject.h"
#include "TString.h"
#include "TList.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"

//--------------------------------------------
// Collaborating Class Forward Declarations --
//--------------------------------------------
class TCandidate;
class TCandReaderRegistry;

//--------------------------------------------
// The following macro calls the TCandReader::book(..) function, 
// automatically inserting the file name and line number for convenience:
#ifndef TCandReaderBook
#define TCandReaderBook(reader) TCandReader::Book(reader, __FILE__, __LINE__)
#endif // TCandReaderBook

//		---------------------
// 		-- Class Interface --
//		---------------------


class TCandReader : public TObject {
    //------------------
    // Static Members --
    //------------------
public:
    // Enums:
    // enum Status report success/failure of reading TCandidate 
    // information from a string. Here are the meanings of the values. 
    // Note that not all values are valid in all cases:
    //
    // SUCCESS: the string was successfully read and the TCandidate 
    // was successfully created to incorporate the information.
    //
    // UNKNOWN: The information in the string was not understood. 
    //
    // MISSING: Some expected information was missing from the string.
    //
    // INVALID: The information in the string was understood, but was 
    //   incompatible with something and therefore unusable.
    //
    // NO_READER: This flag is specified by TReadTree if it is asked to 
    //   use a TCandReader that is not registered.
    //
    enum Status {
	SUCCESS = 0, 
	    UNKNOWN = 1, 
	    MISSING = 2, 
	    INVALID = 3, 
	    NO_READER = 4,
	    NUM_STATUS_KINDS = 5
    };
    
    // Accessors:
    // The number of registered subclass instances and their names:
    static inline int NumInstances();
    static inline TString InstanceName(int ins);
    
    // Modifiers:
    // Registring an instance of a subclass with the static registry.
    // NOTE: The registry owns the pointer.
    static Bool_t Book(TCandReader * instance, const char * file, int line);
    
    // Find a subclass instance:
    static const TCandReader *Find(const char * name);

    static TVector3 ReadVector(istream & s); 
    
private:
    // Static data member held in a function to ensure availability:  
    static TCandReaderRegistry & modifyRegistry();
    
    // An accessor to that member:
    static const TCandReaderRegistry & registry();
    
    
    //--------------------
    // Instance Members --
    //--------------------
    
public:
    // Constructors
    TCandReader() {}
    
    // Destructor
    virtual ~TCandReader() {}
    
    // Modifiers:
    // The main function of this class: Create a TCandidate from a 
    // string, and report the status of the operation. If 0 != iter, 
    // then create the the candidate from its daughters in iter and 
    // use additional relevant information in the string.
    // This function is not const to allow subclasses to have member data
    // that help with parsing, etc.
    virtual TCandidate *
	Read(TCandReader::Status & status, 
	const TString & str,
	TCandListIterator* iter) const = 0;
    
    // Accessors:
    // A name unique to each subclass, to be used for registering an instance:
    virtual const char *Name() const = 0;
    
    // Delimiters which this class doesn't need, and hence allows 
    // TReadTree to use. Different subclasses may not be able to use 
    // these default delimiters, since they appear naturally in the 
    // information they read about a TCandidate.
    virtual char Open() const {return '{';}     // start a daughter list
    virtual char Close() const {return '}';}    // end a daughter list
    virtual char Separate() const {return '&';} // separate sisters
    
    // Check if c is one of the above delimiters:
    virtual Bool_t IsDelimiter(char c) const; 
    
    ClassDef(TCandReader,1) // read a single TCandidate from a TString
};

//====================================================================
// Class TCandReaderRegistry is a by-value map which associates a 
// TCandReader* with a string, and also takes care of memory 
// management. The user doesn't need to know about it, only 
// TCandReaderBase, which uses it to hold subclass instances:

class TCandReaderRegistry {
public:
    // Constructors:
    TCandReaderRegistry() {}
    
    // Destructor:
    ~TCandReaderRegistry();
    
    // Accessors:
    // Retrieve a TCandReader* given its name. Returns 0 if not found.
    const TCandReader *Find(const char * name) const;
    
    // Get the file and line in which a reader was booked:
    const char *File(const TCandReader * reader) const;
    int Line(const TCandReader * reader) const;
    
    // The number of registered subclass instances and their names:
    int NumInstances() const {return _items.GetSize();}
    TString InstanceName(int inst) const {return ((Item*)_items.At(inst))->_name;}
    
    // Modifiers:
    // Register and take ownership of a TCandReader*. Does nothing 
    // and returns false if the name already belongs to someone. 
    // Otherwise returns true:
    Bool_t Book(TCandReader * reader, const char * file, int line);	    
    
    
private:
    // Nested class:
    struct Item {
	Item(TCandReader * reader, const char * name, const char * file,
	    int line) :
	_reader(reader), _name(name), _file(file), _line(line) {}
	
	~Item() {
	    if (0 != _reader){
		delete _reader;
	    }
	}
	
	TCandReader * _reader;
	TString _name;
	TString _file;
	
	// Even though this is a private nested class
	// don't allow copies, assignment
	// desva 21-Oct 1999
	//Item( const Item& ) { ::abort(); }
	//Item& operator= ( const Item& ) {::abort(); return *this;}
	
	int _line;
    };
    
    // Helpers: CC wants this to be expanded here since Item is private:
    const TCandReaderRegistry::Item * 
	item(const TCandReader * reader) const {
	for (int i = 0; i < _items.GetSize(); ++i){
	    if (((Item*)_items.At(i))->_reader == reader) {
		return (Item*)_items.At(i);
	    }
	}
	return 0;
    }
    
    // Data:
    TList _items;
};

//==================================================================
// A subclas of TCandReader, which reads only the name (and hence 
// type) of the TCandidate it creates from a string:

class TCandReaderName : public TCandReader {
public:
    // Constructors:
    TCandReaderName() { if (!_registered) _registered = TCandReaderBook(new TCandReaderName());}
    
    // Destructor:
    // Should be Virtual - desva 21-Oct 1999
    virtual ~TCandReaderName() {}
    
    // Accessors:
    virtual const char *Name() const {return "Name";}
    
    // Modifiers:
    virtual TCandidate * 
	Read(TCandReader::Status & status, 
	const TString & str,
	TCandListIterator* iter) const;
    
    
public:
    static Bool_t Registered() {return _registered;} 
    
private:  
    // static data used to register an instance:
    static Bool_t _registered; //! Do not stream
};

//==================================================================
// A subclas of TCandReader, which reads the name (and hence type) 
// and 3-momentum of the TCandidate it creates from a string:

class TCandReaderNameMom : public TCandReader {
public:
    // Constructors:
    TCandReaderNameMom()  { if (!_registered) _registered = TCandReaderBook(new TCandReaderNameMom());}
    
    // Destructor:
    // Should be Virtual - desva 21-Oct 1999
    virtual ~TCandReaderNameMom() {}
    
    // Accessors:
    virtual const char *Name() const {return "NameMom";}
    
    // Modifiers:
    virtual TCandidate * 
	Read(TCandReader::Status & status, 
	const TString & str,
	TCandListIterator* iter) const;
    
    
public:
    static Bool_t Registered() {return _registered;} 
    
private:  
    // static data used to register an instance:
    static Bool_t _registered; //! Do not stream
};

//==================================================================
// A subclas of TCandReader, which reads the name (and hence type) 
// and decay point of the TCandidate it creates from a string:

class TCandReaderNameVtx : public TCandReader {
public:
    // Constructors:
    TCandReaderNameVtx() {}
    
    // Destructor:
    // Should be Virtual - desva 21-Oct 1999
    virtual ~TCandReaderNameVtx() { if (!_registered) _registered = TCandReaderBook(new TCandReaderNameVtx());}
    
    // Accessors:
    virtual const char *Name() const {return "NameVtx";}
    
    // Modifiers:
    virtual TCandidate * 
	Read(TCandReader::Status & status, 
	const TString & str,
	TCandListIterator * iter) const;
    
    
public:
    static Bool_t Registered() {return _registered;} 
    
private:  
    // static data used to register an instance:
    static Bool_t _registered; //! Do not stream
};

//==================================================================
// A subclas of TCandReader, which reads the name (and hence type), 
// 3-momentum, and decay point of the TCandidate it creates from a 
// string:

class TCandReaderNameMomVtx : public TCandReader {
public:
    // Constructors:
    TCandReaderNameMomVtx() { if (!_registered) _registered = TCandReaderBook(new TCandReaderNameMomVtx());}
    
    // Destructor:
    // Should be Virtual - desva 21-Oct 1999
    virtual ~TCandReaderNameMomVtx() {}
    
    // Accessors:
    virtual const char *Name() const {return "NameMomVtx";}
    
    // Modifiers:
    virtual TCandidate * 
	Read(TCandReader::Status & status, 
	const TString & str,
	TCandListIterator * iter) const;
    
    
public:
    static Bool_t Registered() {return _registered;} 
    
private:  
    // static data used to register an instance:
    static Bool_t _registered; //! Do not stream
};

//-------------------------------------------------------------------
// Inline implementations
//-------------------------------------------------------------------
inline int 
TCandReader::NumInstances() {
    return registry().NumInstances();
}

//-------------------------------------------------------------------
inline TString 
TCandReader::InstanceName(int ins) {
    return registry().InstanceName(ins);
}


#endif
