#include "PndTrkMergeSort.h"
#include <iostream>
#include <cmath>


// Root includes
#include "TROOT.h"


using namespace std;


//----------begin of function PndTrkMergeSort::Merge

void PndTrkMergeSort::Merge(
	UShort_t nl,
	Double_t *left,
	Int_t *ind_left,
	UShort_t nr,
	Double_t *right,
	Int_t *ind_right, 
	Double_t *result,
	Int_t *ind
	)
{
   UShort_t i =0, j, nl_curr=0, nr_curr=0;

   while( nl > 0 && nr >0){
     if( left[nl_curr] <= right[nr_curr]){
      result[i] =  left[nl_curr];
      ind[i] = ind_left[nl_curr];
      nl--;
      nl_curr++;
     } else {
      result[i] =  right [nr_curr];
      ind[i] =  ind_right [nr_curr];
      nr--;
      nr_curr++;
     }
    i++;
   }
//--------------------
   if( nl ==0) {
     for(j=0; j<nr; j++){
      result[i+j]= right[nr_curr+j];
      ind[i+j]= ind_right[nr_curr+j];
     }
   }   else {
     for(j=0; j<nl; j++){
      result[i+j]= left[nl_curr+j];
      ind[i+j]= ind_left[nl_curr+j];
     }
   }





}

//----------end of function PndTrkMergeSort::Merge




//----------begin of function PndTrkMergeSort::Merge_Sort

void PndTrkMergeSort::Merge_Sort(
	UShort_t n_ele,
	Double_t *array,
	Int_t *ind)
{

  Int_t nr, nl, middle, i,
	ind_left[n_ele], ind_right[n_ele];

  Double_t left[n_ele], right[n_ele], result[n_ele];

   if( n_ele <= 1)  return;

   middle = n_ele/2 ;
   for(i=0; i<middle; i++){
     left[i]=array[i];
     ind_left[i]= ind[i];
   }
   for(i=middle; i<n_ele; i++){
     right[i-middle]=array[i];
     ind_right[i-middle]= ind[i];
   }

   Merge_Sort( middle,  left, ind_left);
   Merge_Sort(n_ele-middle, right, ind_right);

   if( left[middle-1] > right[0]) {
     Merge(middle, left,ind_left, n_ele-middle, right, ind_right, array, ind);
   }  else {
     //  do the appending
     for(i=0; i<middle; i++){
       array[i]=left[i];
       ind[i]=ind_left[i];
       
     }
     for(i=middle; i<n_ele; i++){
       array[i]=right[i-middle];
       ind[i]=ind_right[i-middle];
     }
   }


}


//----------end of function PndTrkMergeSort::Merge_Sort



ClassImp(PndTrkMergeSort);
