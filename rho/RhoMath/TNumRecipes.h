#ifndef TNUMRECIPES_H
#define TNUMRECIPES_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TNumRecipes				    				//
//                                                                      //
// Functions from num. recipes in C					//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

#define NUMREC_ITMAX 100
#define NUMREC_EPS 3.0e-7

class TNumRecipes : public TObject {
public:
    
    // log of gamma function
    static Double_t Gammln(Double_t x);
    
    static Double_t Gammq(Double_t a, Double_t x);  
    
    static Double_t Gammp(Double_t a, Double_t x);
    
    static void Gcf(Double_t* gammcf, Double_t a, Double_t x, Double_t* gln);
    
    static void Gser(Double_t* gamser, Double_t a, Double_t x, Double_t* gln);
    
private:
    static void RecipesErr(const char* c);
public:
    ClassDef(TNumRecipes,1) // Functions from "Num. recipes in C"
};

#endif
