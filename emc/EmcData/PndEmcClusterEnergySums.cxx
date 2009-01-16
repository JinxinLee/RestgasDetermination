//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class PndEmcClusterEnergySums
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Helmut Marsiske
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
// Dima Melnichuk, adaption for pandaroot
//------------------------------------------------------------------------
#include "PndEmcClusterEnergySums.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include <iostream>



using std::endl;
using std::ostream;

PndEmcClusterEnergySums::PndEmcClusterEnergySums( const PndEmcCluster& toUse ):
	PndEmcAbsClusterProperty( toUse )
{
}

PndEmcClusterEnergySums::PndEmcClusterEnergySums(const PndEmcClusterEnergySums& other):
	PndEmcAbsClusterProperty( other )		
{}

//--------------
// Destructor --
//--------------

PndEmcClusterEnergySums::~PndEmcClusterEnergySums()
{}

//-------------
// Methods   --
//-------------

Double_t
PndEmcClusterEnergySums::energy( Int_t n ) const 
{
	Double_t sum = 0;

   std::vector<PndEmcDigi*> fDigiList = MyCluster().DigiList();
	std::vector<PndEmcDigi*>::iterator digipos;

  // If asked for too many digis summed return the whole energy
	if ( n > MyCluster().NumberOfDigis() ) return ( MyCluster().energy() );

	for (digipos=fDigiList.begin();digipos<fDigiList.end();++digipos){
		sum += (*digipos)->GetEnergy();
	}

	return( sum );
}

Double_t
PndEmcClusterEnergySums::energy( Double_t distance ) const 
{
	Double_t sum = 0;
   std::vector<PndEmcDigi*> fDigiList = MyCluster().DigiList();
	std::vector<PndEmcDigi*>::iterator digipos;
	
	for (digipos!=fDigiList.begin();digipos<fDigiList.end();++digipos){
		if ( MyCluster().DistanceToCentre( *digipos) < distance )
			sum += (*digipos)->GetEnergy();
	}  
	
	return( sum );
}

Double_t
PndEmcClusterEnergySums::E1() const
{
  const PndEmcDigi* current = MyCluster().Maxima();
  return( current->GetEnergy() );
}

Double_t
PndEmcClusterEnergySums::E9() const
{
	PndEmcDigi* maxDigi = const_cast<PndEmcDigi*> (MyCluster().Maxima());
	
	Double_t sum = maxDigi->GetEnergy();

	std::vector<PndEmcDigi*> fDigiList = MyCluster().DigiList();
	std::vector<PndEmcDigi*>::iterator current;
	
	for (current=fDigiList.begin();current!=fDigiList.end();++current){
		if ( (*current) == maxDigi) continue;
		
      if ( (*current)->isNeighbour(maxDigi))
		{
			sum += (*current)->GetEnergy();
		}
	}		
  
	return sum;
}

Double_t
PndEmcClusterEnergySums::E25() const
{
	const PndEmcDigi* maxDigi = MyCluster().Maxima();
	Double_t sum = E9();
	
	std::vector <const PndEmcDigi*> tmpDigiList;

	std::vector<PndEmcDigi*> fDigiList = MyCluster().DigiList();
	std::vector<PndEmcDigi*>::const_iterator current_i, current_j;
	
	for (current_i=fDigiList.begin();current_i!=fDigiList.end();++current_i){
		if ( (*current_i) == maxDigi) continue;
      if ((*current_i)->isNeighbour(maxDigi))
		{
			for (current_j=fDigiList.begin();current_j!=fDigiList.end();++current_j){
				if ( ((*current_j) == maxDigi) || ((*current_j)->isNeighbour(maxDigi) ) ) continue; 
				if ( (*current_j)->isNeighbour(*current_i) ) {
					std::vector<const PndEmcDigi*>::iterator iter;
					iter = std::find(tmpDigiList.begin(), tmpDigiList.end(), *current_j);
					if (iter == tmpDigiList.end()) {
						sum += (*current_j)->GetEnergy();
						tmpDigiList.push_back(*current_j);
					}
				}
			}
		}
	}	

  return sum;

}

Double_t PndEmcClusterEnergySums::E1E9() const
{
  Double_t e9 = E9();
  //Double_t e1e9 = ( e9 != 0. )? e1e9 = E1()/e9 : -1.;
   /*
   Double_t e1e9 = -1;
   if( e9 != 0.0){
   	e1e9 = E1()/e9;
   	}
  return( e1e9 );
  */
  return ( e9 != 0. )? E1()/e9 : -1.;
}

Double_t PndEmcClusterEnergySums::E9E25() const
{
  Double_t e25 = E25();
  //Double_t e9e25 = ( e25 != 0. )? e9e25 = E9()/e25 : -1.;
  /*
  Double_t e9e25 = -1;
  if( e25 != 0.0){
  	e9e25 = E9()/e25;
  	}
  return( e9e25 );
  */
  return ( e25 != 0. )? E9()/e25 : -1.;
}

void
PndEmcClusterEnergySums::Print(const Option_t* opt) const
{
  std::cout << " E1=" << E1() << ", E9=" << E9() << ", E25=" << E25();
  std::cout << ", E1/E9=" << E1E9() << ", E9/E25=" << E9E25();
  std::cout << endl;
}

ClassImp(PndEmcClusterEnergySums)
