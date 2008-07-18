//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCandList								//
//                                                                      //
// Container class for TCandidates					//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Feb. 99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "RhoBase/TCandList.h"
#include "RhoBase/TFactory.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsPidSelector.h"
#include "RhoBase/VAbsVertexSelector.h"

ClassImp(TCandList)

#include <iostream>
using namespace std;

TCandList::TCandList(const char *name, UInt_t capacity) : 
TNamed(name,name), fFast(kFALSE)
{
    fOwnList = new TObjArray(capacity);
}


// Perform a deep copy
TCandList::TCandList(TCandList& l) 
{
    fFast = l.fFast;
    
    Cleanup();
    const Int_t n = l.GetNumberOfTracks();
    for (int i=0;i<n;++i) {
	Put(*l.Get(i));
    }
}

//--------------
// Destructor --
//--------------
// Deletes the list and the owned objects 
TCandList::~TCandList( ) 
{
    delete fOwnList;
}

//----------------------
//-- public Functions --
//----------------------

void TCandList::Cleanup( ) 
{
    fOwnList->Clear();
}

void TCandList::SetNumberOfTracks(Int_t n) 
{
    cerr << "TCandList::SetNumberOfTracks is deprecated" << endl;
}

Int_t TCandList::GetNumberOfTracks() const 
{
    return fOwnList->GetLast()+1;
}

void TCandList::Put(const TCandidate& c, Int_t i )  
{
    TCandidate *newCand = TFactory::Instance()->NewCandidate(c);
    if (i<0)
	fOwnList->Add(newCand);
    else
	(*fOwnList)[i] = newCand;
}        

void TCandList::InsertAt(Int_t i, const TCandidate& c)
{
    fOwnList->AddAtAndExpand((TObject*)&c,i);
    Put(c,i);
}        


TCandidate* TCandList::Get(Int_t i)  
{
    if (i >= GetNumberOfTracks()) return 0;
    return (TCandidate*) (fOwnList->UncheckedAt(i));
}

TCandidate* TCandList::GetConst(Int_t i) const
{
    if (i >= GetNumberOfTracks()) return 0;
    return (TCandidate*) (fOwnList->UncheckedAt(i));
}

// Compare the marker and remove corresponding entry (MK,12/99)
// This allows to remove objects in several lists
Int_t TCandList::Remove(TCandidate& c )  {
    Int_t nRemoved = 0;
    Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *b = Get(i);
	if (b->Equals(c)) {
	    fOwnList->RemoveAt(i);
	    nRemoved++; 
	}
    }
    fOwnList->Compress();
    return nRemoved;
}        

Int_t TCandList::RemoveFamily(TCandidate& c )  
{
    Int_t nRemoved = 0;
    Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *b = Get(i);
	if (b->Overlaps(c)) {
	    fOwnList->RemoveAt(i);
	    nRemoved++; 
	}
    }
    fOwnList->Compress();
    return nRemoved;
}        

Int_t TCandList::RemoveClones()  
{
    Int_t nRemoved = 0;
    Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n-1;++i) {
	TCandidate *b = Get(i);
	if (b==0) continue;
	for (Int_t j=i+1;j<n;++j) {
	    TCandidate *c = Get(j);
	    if (c==0) continue;
	    if (b->Equals(*c)) {
		fOwnList->RemoveAt(j);
		nRemoved++; 
	    }
	}
    }
    fOwnList->Compress();
    return nRemoved;
}        

Int_t TCandList::OccurrencesOf(TCandidate& c )  {
    Int_t nCand = 0;
    const Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *b = Get(i);
	if (b->Equals(c)) nCand++; 
    }
    return nCand;
}        

Double_t TCandList::GetTotalEnergy(Double_t emin)  
{
    Double_t e = 0.0;
    const Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *c = Get(i);
	Double_t energy = c->Energy();
	if (energy > emin) e += energy;
    }
    return e;
}        

TVector3 TCandList::GetTotalMomentum(Double_t pmin)  
{
    TVector3 p(0.0,0.0,0.0);
    const Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *c = Get(i);
	TVector3 p3 = c->P3();
	if (p3.Mag() > pmin) p = p + p3;
    }
    return p;
}        

void TCandList::Boost(const TVector3& p )  
{
    const Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
        Get(i)->Boost(p);
    }
}        

void TCandList::PrintOn(std::ostream& o) const 
{
    const Int_t n = GetNumberOfTracks();
    o << "content: " << n << " entries." << endl;
    if ( n!=0 ) {
	for (Int_t i=0;i<n;i++) {
	    const TCandidate *c = GetConst(i);
	    o << "[" << i << "] " << *c;
	    o << endl;
	}
    }
}        

void TCandList::Remainder(TCandList& l) 
{
    const Int_t n = l.GetNumberOfTracks();
    for (int i=0;i<n;++i) {
	const TCandidate *c = l.Get(i);
	if ( c->GetMarker()!=0 ) fOwnList->RemoveAt(i);
    }
    fOwnList->Compress();
}

// Makes now a deep copy rather than just cpoying pointers (MK,12/99)
void TCandList::operator =(const TCandList& l ) 
{
    fFast = l.fFast;
    
    Cleanup();
    const Int_t n = l.GetNumberOfTracks();
    for (int i=0;i<n;++i) {
	Put(*l.GetConst(i));
    }
}                      

TCandidate& TCandList::operator[](Int_t i) 
{
    return *Get(i);
}

void TCandList::Combine( TCandList& l1, TCandList& l2,VAbsVertexSelector* s)
{    
    Cleanup();
    CombineAndAppend(l1, l2, s);  
}

void TCandList::Combine( TCandList& l1, TCandList& l2, TCandList& l3)
{    
    Cleanup();
    CombineAndAppend(l1, l2, l3);  
}

void TCandList::Combine( TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4)
{    
    Cleanup();
    CombineAndAppend(l1, l2, l3, l4);  
}

void TCandList::Combine( TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4, TCandList& l5)
{    
    Cleanup();
    CombineAndAppend(l1, l2, l3, l4, l5);  
}

void TCandList::CombineAndAppend( TCandList& l1, TCandList& l2,VAbsVertexSelector* selector) 
{
    TLorentzVector vl;
    Double_t charge;
    Bool_t nearby = kTRUE;

	const int len1=l1.GetLength();
	const int len2=l2.GetLength();
	
	int i1,i2;
	int st2;
	
	for (i1=0; i1<len1; ++i1)
	{
		st2=0;
		if (&l1==&l2) st2=i1+1;
		
		for (i2=st2; i2<len2; ++i2)
		{
			if (l1[i1].Overlaps(l2[i2])) continue;
			
			vl=l1[i1].P4()+l2[i2].P4();
			charge=l1[i1].Charge()+l2[i2].Charge();
			if (selector) nearby = selector->Accept(l1[i1],l2[i2]);
			if (!nearby) continue;
			
			TCandidate c(vl,charge);
			c.SetCovP4(l1[i1].P4Cov()+l2[i2].P4Cov());
			
			c.SetMarker(l1[i1].GetMarker(0)|l2[i2].GetMarker(0),0);
			c.SetMarker(l1[i1].GetMarker(1)|l2[i2].GetMarker(1),1);
			c.SetMarker(l1[i1].GetMarker(2)|l2[i2].GetMarker(2),2);
			c.SetMarker(l1[i1].GetMarker(3)|l2[i2].GetMarker(3),3);
			
			c.AddDaughterLinkSimple(&(l1[i1]));
			c.AddDaughterLinkSimple(&(l2[i2]));
			
			if (selector!=0)
			{
				c.SetPosition(selector->GetVertex());
				c.SetVect(selector->GetMomentum());
				c.SetEnergy(c.E());
			}
			
			Put(c);
		}
	}
	
	/*
    const Int_t endpos1 = l1.GetNumberOfTracks();
    const Int_t endpos2 = l2.GetNumberOfTracks();
    
    if ( &l1==&l2 ) {
        //combination of a list with itself
        for (Int_t comb_i=0;comb_i<endpos1;++comb_i) {
            for (Int_t comb_k=comb_i+1;comb_k<endpos2;++comb_k) {
                if ( !l1[comb_i].Overlaps( l2[comb_k] ) ) {
                    vl=l1[comb_i].P4()+l2[comb_k].P4();
                    charge=l1[comb_i].Charge()+l2[comb_k].Charge();
		    if (selector) nearby = selector->Accept(l1[comb_i],l2[comb_k]);
                    if (nearby) {
                        //fill list with new candidate
                        TCandidate c(vl,charge);
			c.SetMarker(l1[comb_i].GetMarker(0)|l2[comb_k].GetMarker(0),0);
			c.SetMarker(l1[comb_i].GetMarker(1)|l2[comb_k].GetMarker(1),1);
			c.SetMarker(l1[comb_i].GetMarker(2)|l2[comb_k].GetMarker(2),2);
			c.SetMarker(l1[comb_i].GetMarker(3)|l2[comb_k].GetMarker(3),3);
			
			c.SetCovP4(l1[comb_i].P4Cov()+l2[comb_k].P4Cov());
			//cout <<"TCandList:"<<endl;
			//c.Cov7().Print();
			
                        if (selector!=0){
                          c.SetPosition(selector->GetVertex());
                          c.SetVect(selector->GetMomentum());
			  c.SetEnergy(c.E());
                        }
			// *************** modified by K Goetzen
			c.AddDaughterLinkSimple(&(l1[comb_i]));
			c.AddDaughterLinkSimple(&(l2[comb_k]));
			// ****************
			Put(c);
		    }
                }
            }
        }     
    }
    else {
        //combination of 2 different lists
        for (Int_t comb_i=0;comb_i<endpos1;++comb_i) {
            for (Int_t comb_k=0;comb_k<endpos2;++comb_k) {
                if ( !l1[comb_i].Overlaps( l2[comb_k] ) ) {
		    vl=l1[comb_i].P4()+l2[comb_k].P4();
		    charge=l1[comb_i].Charge()+l2[comb_k].Charge();
		    if (selector) nearby = selector->Accept(l1[comb_i],l2[comb_k]);
                    if (nearby) {
                        //fill list with new candidate
                        TCandidate c(vl,charge);
			c.SetMarker(l1[comb_i].GetMarker(0)|l2[comb_k].GetMarker(0),0);
			c.SetMarker(l1[comb_i].GetMarker(1)|l2[comb_k].GetMarker(1),1);
			c.SetMarker(l1[comb_i].GetMarker(2)|l2[comb_k].GetMarker(2),2);
			c.SetMarker(l1[comb_i].GetMarker(3)|l2[comb_k].GetMarker(3),3);
			
			c.SetCovP4(l1[comb_i].P4Cov()+l2[comb_k].P4Cov());
			
                        if (selector!=0){
                          c.SetPosition(selector->GetVertex());
                          c.SetVect(selector->GetMomentum());
			  c.SetEnergy(c.E());
                        }
			// *************** modified by K Goetzen
			c.AddDaughterLinkSimple(&(l1[comb_i]));
			c.AddDaughterLinkSimple(&(l2[comb_k]));
			// ****************
			Put(c);
		    }
                }
            }
        }
    } 
	*/
}        

void TCandList::CombineAndAppend( TCandList& l1, TCandList& l2, TCandList& l3) 
{
    TLorentzVector vl;
    Double_t charge;

	const int len1=l1.GetLength();
	const int len2=l2.GetLength();
	const int len3=l3.GetLength();
	
	int i1,i2,i3;
	int st2,st3;
	
	for (i1=0; i1<len1; ++i1)
	{
		st2=0;
		if (&l2==&l1) st2=i1+1;
		
		for (i2=st2; i2<len2; ++i2)
		{
			if (l1[i1].Overlaps(l2[i2])) continue;
			
			st3=0;
			if (&l3==&l2)      st3=i2+1;
			else if (&l3==&l1) st3=i1+1;
			
			for (i3=st3; i3<len3; ++i3)
			{
				if ( l3[i3].Overlaps(l2[i2]) || l3[i3].Overlaps(l1[i1]) ) continue;
				
				vl=l1[i1].P4()+l2[i2].P4()+l3[i3].P4();
				charge=l1[i1].Charge()+l2[i2].Charge()+l3[i3].Charge();
				
				TCandidate c(vl,charge);
				c.SetCovP4(l1[i1].P4Cov()+l2[i2].P4Cov()+l3[i3].P4Cov());
				
				c.SetMarker(l1[i1].GetMarker(0)|l2[i2].GetMarker(0)|l3[i3].GetMarker(0),0);
				c.SetMarker(l1[i1].GetMarker(1)|l2[i2].GetMarker(1)|l3[i3].GetMarker(1),1);
				c.SetMarker(l1[i1].GetMarker(2)|l2[i2].GetMarker(2)|l3[i3].GetMarker(2),2);
				c.SetMarker(l1[i1].GetMarker(3)|l2[i2].GetMarker(3)|l3[i3].GetMarker(3),3);
				
				c.AddDaughterLinkSimple(&(l1[i1]));
				c.AddDaughterLinkSimple(&(l2[i2]));
				c.AddDaughterLinkSimple(&(l3[i3]));
						
				Put(c);
			}
		}
	}
}

void TCandList::CombineAndAppend( TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4) 
{
    TLorentzVector vl;
    Double_t charge;

	const int len1=l1.GetLength();
	const int len2=l2.GetLength();
	const int len3=l3.GetLength();
	const int len4=l4.GetLength();
	
	int i1,i2,i3,i4;
	int st2,st3,st4;
	
	for (i1=0; i1<len1; ++i1)
	{
		st2=0;
		if (&l2==&l1) st2=i1+1;
		
		for (i2=st2; i2<len2; ++i2)
		{
			if (l1[i1].Overlaps(l2[i2])) continue;
			
			st3=0;
			if (&l3==&l2)      st3=i2+1;
			else if (&l3==&l1) st3=i1+1;
			
			for (i3=st3; i3<len3; ++i3)
			{
				if (l3[i3].Overlaps(l2[i2]) || l3[i3].Overlaps(l1[i1]) ) continue;
				
				st4=0;
				if (&l4==&l3)      st4=i3+1;
				else if (&l4==&l2) st4=i2+1;
				else if (&l4==&l1) st4=i1+1;
				
				for (i4=st4; i4<len4; ++i4)
				{
					if (l4[i4].Overlaps(l3[i3]) || l4[i4].Overlaps(l2[i2]) || l4[i4].Overlaps(l1[i1]) ) continue;
					
					vl=l1[i1].P4()+l2[i2].P4()+l3[i3].P4()+l4[i4].P4();
					charge=l1[i1].Charge()+l2[i2].Charge()+l3[i3].Charge()+l4[i4].Charge();
					
					TCandidate c(vl,charge);
					c.SetCovP4(l1[i1].P4Cov()+l2[i2].P4Cov()+l3[i3].P4Cov()+l4[i4].P4Cov());
					
					c.SetMarker(l1[i1].GetMarker(0)|l2[i2].GetMarker(0)|l3[i3].GetMarker(0)|l4[i4].GetMarker(0),0);
					c.SetMarker(l1[i1].GetMarker(1)|l2[i2].GetMarker(1)|l3[i3].GetMarker(1)|l4[i4].GetMarker(1),1);
					c.SetMarker(l1[i1].GetMarker(2)|l2[i2].GetMarker(2)|l3[i3].GetMarker(2)|l4[i4].GetMarker(2),2);
					c.SetMarker(l1[i1].GetMarker(3)|l2[i2].GetMarker(3)|l3[i3].GetMarker(3)|l4[i4].GetMarker(3),3);
					
					c.AddDaughterLinkSimple(&(l1[i1]));
					c.AddDaughterLinkSimple(&(l2[i2]));
					c.AddDaughterLinkSimple(&(l3[i3]));
					c.AddDaughterLinkSimple(&(l4[i4]));
							
					Put(c);
				}
			}
		}
	}
}

void TCandList::CombineAndAppend( TCandList& l1, TCandList& l2, TCandList& l3, TCandList& l4, TCandList& l5)
{
    TLorentzVector vl;
    Double_t charge;

	const int len1=l1.GetLength();
	const int len2=l2.GetLength();
	const int len3=l3.GetLength();
	const int len4=l4.GetLength();
	const int len5=l5.GetLength();
	
	int i1,i2,i3,i4,i5;
	int st2,st3,st4,st5;
	
	for (i1=0; i1<len1; ++i1)
	{
		st2=0;
		if (&l2==&l1) st2=i1+1;
		
		for (i2=st2; i2<len2; ++i2)
		{
			if (l1[i1].Overlaps(l2[i2])) continue;
			
			st3=0;
			if (&l3==&l2)      st3=i2+1;
			else if (&l3==&l1) st3=i1+1;
			
			for (i3=st3; i3<len3; ++i3)
			{
				if (l3[i3].Overlaps(l2[i2]) || l3[i3].Overlaps(l1[i1]) ) continue;
				
				st4=0;
				if (&l4==&l3)      st4=i3+1;
				else if (&l4==&l2) st4=i2+1;
				else if (&l4==&l1) st4=i1+1;
				
				for (i4=st4; i4<len4; ++i4)
				{
					if (l4[i4].Overlaps(l3[i3]) || l4[i4].Overlaps(l2[i2]) || l4[i4].Overlaps(l1[i1]) ) continue;
					
					st5=0;
					if (&l5==&l4)      st5=i4+1;
					else if (&l5==&l3) st5=i3+1;
					else if (&l5==&l2) st5=i2+1;
					else if (&l5==&l1) st5=i1+1;
					
					for (i5=st5; i5<len5; ++i5)
					{
						if (l5[i5].Overlaps(l4[i4]) || l5[i5].Overlaps(l3[i3]) 
							|| l5[i5].Overlaps(l2[i2]) || l5[i5].Overlaps(l1[i1]) ) continue;
					
						vl=l1[i1].P4()+l2[i2].P4()+l3[i3].P4()+l4[i4].P4()+l5[i5].P4();
						charge=l1[i1].Charge()+l2[i2].Charge()+l3[i3].Charge()+l4[i4].Charge()+l5[i5].Charge();
						
						TCandidate c(vl,charge);
						c.SetCovP4(l1[i1].P4Cov()+l2[i2].P4Cov()+l3[i3].P4Cov()+l4[i4].P4Cov()+l5[i5].P4Cov());
						
						c.SetMarker(l1[i1].GetMarker(0)|l2[i2].GetMarker(0)
									|l3[i3].GetMarker(0)|l4[i4].GetMarker(0)|l5[i5].GetMarker(0),0);
						c.SetMarker(l1[i1].GetMarker(1)|l2[i2].GetMarker(1)
									|l3[i3].GetMarker(1)|l4[i4].GetMarker(1)|l5[i5].GetMarker(1),1);
						c.SetMarker(l1[i1].GetMarker(2)|l2[i2].GetMarker(2)
									|l3[i3].GetMarker(2)|l4[i4].GetMarker(2)|l5[i5].GetMarker(2),2);
						c.SetMarker(l1[i1].GetMarker(3)|l2[i2].GetMarker(3)
									|l3[i3].GetMarker(3)|l4[i4].GetMarker(3)|l5[i5].GetMarker(3),3);
						
						c.AddDaughterLinkSimple(&(l1[i1]));
						c.AddDaughterLinkSimple(&(l2[i2]));
						c.AddDaughterLinkSimple(&(l3[i3]));
						c.AddDaughterLinkSimple(&(l4[i4]));
						c.AddDaughterLinkSimple(&(l5[i5]));
								
						Put(c);
					}
				}
			}
		}
	}
}

std::ostream&  operator << (std::ostream& o, const TCandList& a) { a.PrintOn(o); return o; }

//extern "C" void qsort(void *, size_t, size_t, int (const void *,const void *));

typedef int compare_function(const void*, const void*);

void TCandList::Sort(int (*compfunc)(const TCandidate **, const TCandidate **) )
{
   qsort( fOwnList, GetNumberOfTracks(), sizeof(void *), (compare_function *)compfunc);
}


void TCandList::Select(TCandList& l, Bool_t (*selfunc)(TCandidate&) )
{
    Cleanup();
    const Int_t n = l.GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *c = Get(i);
	if (selfunc(*c)) {
	    Put(*c);
       	}
    }
}

// Destructive selection

void TCandList::Select(VAbsPidSelector* pidmgr) 
{
    const Int_t n = GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *c = Get(i);
	if (!pidmgr->Accept(*c)) fOwnList->RemoveAt(i);
    }
    fOwnList->Compress();
}

// Non-destructive selection  

void TCandList::Select(TCandList& l, VAbsPidSelector* pidmgr) 
{
    Cleanup();
    const Int_t n = l.GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *c = l.Get(i);
	if (pidmgr->Accept(*c)) {
	    Put(*c);
       	}
    }
}

void TCandList::Append(TCandList& l, VAbsPidSelector* pidmgr) 
{
    const Int_t n = l.GetNumberOfTracks();
    for (Int_t i=0;i<n;++i) {
	TCandidate *c = l.Get(i);
	if (0==pidmgr || pidmgr->Accept(*c)) {
	    Put(*c);
       	}
    }
}
