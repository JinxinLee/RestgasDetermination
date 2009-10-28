
#include <iostream>


using namespace std;

//**
// Some classes to put into the dictionary.  We're going to pull them
// out and call their virtual print() methods.  We sandwhich this between
// two other virtuals.  The placement in the vtab of prnt() is differnt
// for each class.  If we ever get one the wrong way we'll call the wrong
// fn and get an error message.
//
//**

class B {
public:
  B( int i ) : _id( i ) {}
  virtual void print(void) const { cout << "B[" << _id << "] "; }
  virtual void err1( void) const { cout << "ERROR: B probably as C"; }
  virtual void err2( void) const { cout << "ERROR: B probably as D"; }

private:
  int _id;
  friend ostream& operator <<( ostream& o, const B& a);
};

class C {
public:
  C( int i ) : _id( i ) {}
  virtual void err1( void) const { cout << "ERROR: C probably as B"; }
  virtual void print(void) const { cout << "C[" << _id << "] "; }
  virtual void err2( void) const { cout << "ERROR: C probably as D"; }

private:
  int _id;
  friend ostream& operator <<( ostream& o, const C& a);

};

class D {
public:
  D( int i ) : _id( i ) {}
  virtual void err1( void) const { cout << "ERROR: D probably as B"; }
  virtual void err2( void) const { cout << "ERROR: D probably as C"; }
  virtual void print(void) const { cout << "D[" << _id << "] "; }

private:
  int _id;
  friend ostream& operator <<( ostream& o, const D& d);
};

//
// some classes to use in the test
//
class boogie{};
class woogie{};

class ListA {
public:
  ListA() : _myID( _instanceScaler++ ) {}
  virtual ~ListA() { --_instanceScaler; }
  static void numInstances(void) {
    cout << "Number of ListA objects=" <<  _instanceScaler;

  }
private:
  static int _instanceScaler;
  int _myID;
};


class NoIfd {
public:
  NoIfd( int id ) : _myID( id ) {}
  virtual void print(void) const { cout << "instance " << _myID
				        <<" of NoIfd"; }
private:
  int _myID;
  friend ostream& operator <<( ostream& o, const NoIfd& d);
};
