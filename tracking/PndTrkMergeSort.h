#ifndef PndTrkMergeSort_H
#define PndTrkMergeSort_H 1

// Root includes
#include "TROOT.h"


class PndTrkMergeSort : public TObject
{


 public:

 /** Default constructor **/
 PndTrkMergeSort(){};
 /** Destructor **/
 ~PndTrkMergeSort(){};


  void Merge(
	UShort_t nl,
	Double_t *left,
	Int_t *ind_left,
	UShort_t nr,
	Double_t *right,
	Int_t *ind_right,
	Double_t *result,
	Int_t *ind
	);



  void  Merge_Sort(
	UShort_t n_ele,
	Double_t *array,
	Int_t *ind
	);





  ClassDef(PndTrkMergeSort,1);

};

#endif
