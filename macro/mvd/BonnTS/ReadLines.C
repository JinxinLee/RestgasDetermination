/*
 * ReadLines.C
 *
 *  Created on: Apr  12, 2010
 *      Author: Simone Bianco
 *  Really verbose and slow macro, just for debug purposes
 */

#include <vector>;

int ReadLines(){


	// load libs

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	//
	TString LinFile = "fit.root";

	TString directory = gSystem->Getenv("VMCWORKDIR");
	TString geomFile = directory + "/geometry/TrackingStation.root";

	TFile *geo = new TFile(geomFile);

	TGeoManager *myGeo = geo->Get("FAIRGeom");

	fGeoH = new PndGeoHandling();


	// Loading the geometry and defining the geo handler
	
	TFile *f = new TFile(LinFile);

	TTree *t=(TTree *) f->Get("cbmsim") ;
	
	TClonesArray* tr_array=new TClonesArray("PndLinTrack");
	t->SetBranchAddress("MVDTrack",&tr_array);//Branch names

    	cout << "Events: " << t->GetEntries() << endl;

	for (Int_t j = 0 ; j < t->GetEntries() ; j++) // loop on events

	  {

	    t->GetEvent(j);

	    cout << "Event " << j << " Entries " << tr_array->GetEntries() << endl;

	    for (Int_t y = 0 ; y < tr_array->GetEntries() ; y++) // loop on hits
	      {
			
		PndLinTrack* line = (PndLinTrack*)tr_array->At(y);
		
		std::vector<TString> names = line->GetSens();
		
		Int_t siz = names.size();

		cout << " Hit " << y << " size " << siz << endl;
		
		for (Int_t p = 0 ; p < siz ; p++)
		  {
		    cout << "Det: " << fGeoH->GetPath(names[p]) << endl;
		  }
	      }
	    
	  } // end loop on events

  return 0;

}

