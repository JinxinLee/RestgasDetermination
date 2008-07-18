//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsVertexSelector	    						//
//                                                                      //
// Abstract Selector classes for vertexing				//
//                                                                      //
// Author: Marcel Kunze, RUB, July 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsVertexSelector.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"

ClassImp(VAbsVertexSelector)

#include <iostream>
using namespace std;

VAbsVertexSelector::VAbsVertexSelector(const char *name) :
TNamed(name,name),
fDoca(1.0), fVtxip(3.14159265358979323846), fRmin(0.0), fRmax(1.E8), fPrimaryVertex(0.,0.,0.)
{
}

VAbsVertexSelector::~VAbsVertexSelector() 
{
}

void VAbsVertexSelector::Combine( TCandList& l1, TCandList& l2, TCandList& out) 
{
    TLorentzVector vl;
    Double_t charge;
    Bool_t nearby = kTRUE;
    //fEntries = 0;
    //end of unlock
    Int_t endpos1 = l1.GetLength();
    Int_t endpos2 = l2.GetLength();
    out.Cleanup();
    
    if ( &l1==&l2 ) {
        //combination of a list with itself
        for (Int_t comb_i=0;comb_i<endpos1;++comb_i) {
            for (Int_t comb_k=comb_i+1;comb_k<endpos2;++comb_k) {
                if ( !l1[comb_i].Overlaps( l2[comb_k] ) ) {
                    vl=l1[comb_i].P4()+l2[comb_k].P4();
                    charge=l1[comb_i].Charge()+l2[comb_k].Charge();
		    nearby = Accept(l1[comb_i],l2[comb_k]);
                    if (nearby) {
                        //fill list with new candidate
			TCandidate c(vl,charge);
			c.SetMarker(l1[comb_i].GetMarker(0)|l2[comb_k].GetMarker(0),0);
			c.SetMarker(l1[comb_i].GetMarker(1)|l2[comb_k].GetMarker(1),1);
			c.SetMarker(l1[comb_i].GetMarker(2)|l2[comb_k].GetMarker(2),2);
			c.SetMarker(l1[comb_i].GetMarker(3)|l2[comb_k].GetMarker(3),3);
			c.SetPosition(GetVertex());
			c.SetVect(GetMomentum());
			c.SetE(c.E());
                        out.Put(c);
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
		    nearby = Accept(l1[comb_i],l2[comb_k]);
                    if (nearby) {
                        //fill list with new candidate
			TCandidate c(vl,charge);
                        c.SetMarker(l1[comb_i].GetMarker(0)|l2[comb_k].GetMarker(0),0);
                        c.SetMarker(l1[comb_i].GetMarker(1)|l2[comb_k].GetMarker(1),1);
                        c.SetMarker(l1[comb_i].GetMarker(2)|l2[comb_k].GetMarker(2),2);
                        c.SetMarker(l1[comb_i].GetMarker(3)|l2[comb_k].GetMarker(3),3);
			c.SetPosition(GetVertex());
			c.SetVect(GetMomentum());
			c.SetE(c.E());
                        out.Put(c);
		    }
                }
            }
        }
    } 
}        

void VAbsVertexSelector::PrintOn(std::ostream& o) const 
{
    o << GetName() << " settings:" << endl;
    o << "Distance of closest approach < " << fDoca << " cm" << endl; 
    o << "Angle vertex-IP->momentum    < " << fVtxip << " rad" << endl; 
    o << "Radius of acceptance         = " << fRmin << " ... " << fRmax << " cm" << endl; 
    o << endl;    
}

std::ostream&  operator << (std::ostream& o, const VAbsVertexSelector& a) { a.PrintOn(o); return o; }

