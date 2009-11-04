#include <iostream>
#include <strings.h>

#include <string>
using std::string;

#include "Tokenize.h"
#include "String.h"
using std::cerr;
using std::cout;
using std::endl;
using namespace panda::String;

void failed(int i, const string& stmp) {
  cerr << "Failed at " << i << " : " ;
  cerr << stmp << endl;
}

void TokenizeTest(){
// Starting tests
  cout << "Starting tests ... " << endl;

  string str("Something is rotten in the state of Denmark.");
  string str_a[8];
  str_a[0] = "Something";
  str_a[1] = "is";
  str_a[2] = "rotten";
  str_a[3] = "in";
  str_a[4] = "the";
  str_a[5] = "state";
  str_a[6] = "of";
  str_a[7] = "Denmark.";

  Tokenize next(str);

  string token;
  int ctr;
  ctr = 0;
  while (!(token=next()).empty() ) {
    if (token != str_a[ctr] ) failed(ctr,token);
    ctr++;
  }

  string str2("Something  = is = rotten =");
  string str_a2[3];
  str_a2[0] = "Something  ";
  str_a2[1] = " is ";
  str_a2[2] = " rotten ";
  Tokenize next2(str2);
  ctr = 0;
  while (!(token=next2("=")).empty() ) {
    if (token != str_a2[ctr] ) failed(ctr+100,token);
    ctr++;
  }

  cout << "Finished tests." << endl;

  exit(0);    
}
