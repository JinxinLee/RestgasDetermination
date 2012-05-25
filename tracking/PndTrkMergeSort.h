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
	Short_t nl,
	Double_t *left,
	Int_t *ind_left,
	Short_t nr,
	Double_t *right,
	Int_t *ind_right,
	Double_t *result,
	Int_t *ind
	);



  void  Merge_Sort(
	Short_t n_ele,
	Double_t *array,
	Int_t *ind
	);





  ClassDef(PndTrkMergeSort,1);

};

#endif
