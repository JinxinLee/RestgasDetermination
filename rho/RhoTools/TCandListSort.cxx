//////////////////////////////////////////////////////////////////////////
//                                                                      //
// File and Version Information:					//
//   TCandListSort.h							//
//                                                                      //
// Description:								//
//   Operates on a list of TCandidates to sort them according to	//
//   momentum or cluster energy, highest first.				//
//                                                                      //
// Usage:								//
//   TCandidList aList;							//
//   // ..and fill your list....					//
//   TCandListSort mylistsort; // to instantiate a TCandListSort object	//
//   mylistsort.p(aList); // to sort list according to momentum		//
//   mylistsort.Ecal(aList); // sort list according to cluster energy	//
//									//
//   Clearly, the momentum sort is very general. However, the Ecal sort	//
//   requires the presence of the CalQual info at Micro level.		//
//                                                                      //
// Original Author     Theresa Champion					//
//                                                                      //
// ROOT Version by Marcel Kunze, RUB					//
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/TCandListSort.h"
#include <assert.h>
#include <math.h>
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandidate.h"

ClassImp(TCandListSort)

TBuffer &operator>>(TBuffer &buf, TCandListSort *&obj)
{
   obj = (TCandListSort *) buf.ReadObject(TCandListSort::Class());
   return buf;
}

#include <iostream>
#include <iomanip>
using namespace std;

// function to sort list of TCandidates according to momentum
int 
SortMom(const TCandidate **trkptr1, const TCandidate **trkptr2)
{
  if ((*trkptr1)->P() < (*trkptr2)->P()) return 1;
  return -1;
} 

// function to sort list of TCandidates according to EMC energy
int 
SortEcal(const TCandidate **trkptr1, const TCandidate **trkptr2)
{
  double e1(0.), e2(0.);

  const VAbsMicroCandidate& CalQual1 = (*trkptr1)->GetMicroCandidate();
  const VAbsMicroCandidate& CalQual2 = (*trkptr2)->GetMicroCandidate();
  if (&CalQual1!=0) { e1 = CalQual1.GetEmcCalEnergy();}
  if (&CalQual2!=0) { e2 = CalQual2.GetEmcCalEnergy();}

  if (e1 < e2) return 1;
  return -1;
}
//---------------
// Constructors -
//---------------

TCandListSort::TCandListSort()
{
}

//--------------
// Destructor --
//--------------

TCandListSort::~TCandListSort()
{
}

//-------------------
// Member functions -
//-------------------

void
TCandListSort::P(TCandList& theList)
{
  theList.Sort(SortMom);
}
void
TCandListSort::Ecal(TCandList& theList)
{
  theList.Sort(SortEcal);
}
