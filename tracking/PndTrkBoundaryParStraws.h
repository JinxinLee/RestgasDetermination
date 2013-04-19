#ifndef PndTrkBoundaryParStraws_H
#define PndTrkBoundaryParStraws_H 1

// Root includes
#include "TROOT.h"


class PndTrkBoundaryParStraws : public TObject
{


 public:

 /** Default constructor **/
 PndTrkBoundaryParStraws(){};
 /** Destructor **/
 ~PndTrkBoundaryParStraws(){};


  bool Set(
	int straw_number
	);


  ClassDef(PndTrkBoundaryParStraws,1);

};

#endif
