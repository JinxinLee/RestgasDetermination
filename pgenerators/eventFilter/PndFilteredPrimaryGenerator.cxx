#include "PndFilteredPrimaryGenerator.h"

#include "FairGenericStack.h"           // for FairGenericStack
#include "FairMCEventHeader.h"          // for FairMCEventHeader

#include "TDatabasePDG.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TClonesArray.h"
#include "TParticle.h"


// -------------------------------------------------------------------------

PndFilteredPrimaryGenerator::PndFilteredPrimaryGenerator() 
: FairPrimaryGenerator(),

  fEvtFilterStat(FairEvtFilterParams()),
  fVerbose(3),
  fEventNrFiltered(0),
  fEventPrintFreq(100)
{
	fdbPdg = TDatabasePDG::Instance();
	
	std::vector<TString>    tmpfPartNames = {"e+", "e-", "mu+", "mu-", "pi+", "pi-", "k+", "k-", "p+",  "p-", "n0", "n0b", "e+-",   "mu+-",   "pi+-",   "k+-",    "p+-",   "n00b"  , "t+", "t-", "t+-", "nt", "any" };
	std::vector<int>        tmpfNamePdg   = {-11 , 11  , -13  , 13   , 211  , -211 , 321 , -321, 2212, -2212, 2112, -2112, 11000  , 13000   , 211000  , 321000 ,  2212000, 2112000,   1   , -1  ,  2    , 0   ,  3   };
	std::vector<int>        tmpfCombFsPdg = {-11 , 11  , -13  , 13   , 211  , -211 , 321 , -321, 2212, -2212 , 22};
	
	// initialize sets and vectors with names and codes
	fPartNames  = tmpfPartNames;
	fNamePdg    = tmpfNamePdg;
	fCombFsPdg  = tmpfCombFsPdg;

	std::unordered_set<int> tmpfSetFsPdg(fCombFsPdg.begin(), fCombFsPdg.end());
	fSetFsPdg   = tmpfSetFsPdg;
	
	// initialize maps between names and codes
	for (int i=0;i<(int)fPartNames.size();++i) fNameCodeMap[fPartNames[i]] = fNamePdg[i];
	for (int i=0;i<(int)fPartNames.size();++i) fCodeNameMap[fNamePdg[i]]   = fPartNames[i];
}

// -------------------------------------------------------------------------

PndFilteredPrimaryGenerator::PndFilteredPrimaryGenerator(TString filterset)
: FairPrimaryGenerator(),

  fEvtFilterStat(FairEvtFilterParams()),
  fVerbose(3),
  fEventNrFiltered(0),
  fEventPrintFreq(100)
{
	fdbPdg = TDatabasePDG::Instance();
	
	std::vector<TString>    tmpfPartNames = {"e+", "e-", "mu+", "mu-", "pi+", "pi-", "k+", "k-", "p+",  "p-", "n0", "n0b", "e+-",   "mu+-",   "pi+-",   "k+-",    "p+-",   "n00b"  , "t+", "t-", "t+-", "nt", "any" };
	std::vector<int>        tmpfNamePdg   = {-11 , 11  , -13  , 13   , 211  , -211 , 321 , -321, 2212, -2212, 2112, -2112, 11000  , 13000   , 211000  , 321000 ,  2212000, 2112000,   1   , -1  ,  2    , 0   ,  3   };
	std::vector<int>        tmpfCombFsPdg = {-11 , 11  , -13  , 13   , 211  , -211 , 321 , -321, 2212, -2212 , 22};
	
	// initialize sets and vectors with names and codes
	fPartNames  = tmpfPartNames;
	fNamePdg    = tmpfNamePdg;
	fCombFsPdg  = tmpfCombFsPdg;
	
	std::unordered_set<int> tmpfSetFsPdg(fCombFsPdg.begin(), fCombFsPdg.end());
	fSetFsPdg   = tmpfSetFsPdg;

	// initialize maps between names and codes
	for (int i=0;i<(int)fPartNames.size();++i) fNameCodeMap[fPartNames[i]] = fNamePdg[i];
	for (int i=0;i<(int)fPartNames.size();++i) fCodeNameMap[fNamePdg[i]]   = fPartNames[i];
	
	AddFilter(filterset);
}

// -------------------------------------------------------------------------

Bool_t PndFilteredPrimaryGenerator::Init()
{
	FairPrimaryGenerator::Init();

	if (fVerbose) 
		for (int ifs = 0; ifs < (int)fFilterSets.size(); ++ifs)
		{
			cout <<"FILTER SET "<< ifs<<endl<<endl;
			for (int i = 0; i < (int)fFilterSets[ifs].size(); ++i) fFilterSets[ifs][i].Print();
			cout <<endl;
		}

	return kTRUE;
}

// --------------------------------------------------------------------

StrVec PndFilteredPrimaryGenerator::SplitString(TString s, TString delim)
{
	StrVec toks;
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
	{
		TString token = (((TObjString*)tok->At(i))->String()).Strip(TString::kBoth);
		toks.push_back(token);
	}
	return toks;
}

// --------------------------------------------------------------------

bool PndFilteredPrimaryGenerator::CheckKinematic(const PndSmpFilt &f, const TLorentzVector &p4)
{
	bool acc_trk = true;
	
	double curr_p   = p4.P(); acc_trk = (curr_p   >= f.pmin   && curr_p   <= f.pmax);   	
	if (acc_trk)  { double curr_pt  = p4.Pt();            acc_trk = (curr_pt  >= f.ptmin  && curr_pt  <= f.ptmax);  } 
	if (acc_trk)  { double curr_pz  = p4.Pz();            acc_trk = (curr_pz  >= f.pzmin  && curr_pz  <= f.pzmax);  }
	if (acc_trk)  { double curr_tht = p4.Theta()*57.2958; acc_trk = (curr_tht >= f.thtmin && curr_tht <= f.thtmax); }
	if (acc_trk)  { double curr_phi = p4.Phi()*57.2958;   acc_trk = (curr_phi >= f.phimin && curr_phi <= f.phimax); }	
	
	return acc_trk;
} 

// --------------------------------------------------------------------

int PndFilteredPrimaryGenerator::AntiPdgCode(int pdg)
{
	if (fdbPdg->GetParticle(pdg)!=0x0 && fdbPdg->GetParticle(pdg)->AntiParticle()!=0x0) 
		return fdbPdg->GetParticle(pdg)->AntiParticle()->PdgCode(); 
	
	return pdg;
}

// --------------------------------------------------------------------

void PndFilteredPrimaryGenerator::GetRangeDouble(TString s, double &a, double &b, TString delim, bool forceset)
{
	int len = delim.Length();
	a = (TString(s(0,s.Index(delim)))).Atof();
	double tmpb = (TString(s(s.Index(delim)+len,10000))).Atof();
	if (tmpb>0. || forceset) b = tmpb;
}

// --------------------------------------------------------------------

void PndFilteredPrimaryGenerator::GetRangeInt(TString s, int &a, int &b, TString delim)
{
	int len = delim.Length();
	a = (TString(s(0,s.Index(delim)))).Atoi();
	int tmpb = (TString(s(s.Index(delim)+len,10000))).Atoi();
	if (tmpb>0) b=tmpb;
}



// -------------------------------------------------------------------------
// Parses the definition string for a filter
// It may be provided a string of the form 'Filt_1.1 && Filt_1.2  ||  Filt_2.1  ||  Filt_3.1 && Filt_3.2 && Filt_3.2'
// where && (AND) has higher priority than || (OR) 

void PndFilteredPrimaryGenerator::AddFilter(TString filterStr)
{
	// some replacements
	filterStr.ToLower();
	filterStr.ReplaceAll(":","&&");
	filterStr.ReplaceAll("gamma","nt");
	filterStr.ReplaceAll("gam","nt");
	filterStr.ReplaceAll("ch0","nt");
	filterStr.ReplaceAll("tr","t");
	filterStr.ReplaceAll("ch","t");
	filterStr.ReplaceAll("mass","m");


	// split filter string
	StrVec filtersets_str = SplitString(filterStr,"||");
	
	// *** loop over event type filters (the filter sets)
	for (int ifs=0; ifs < (int) filtersets_str.size(); ++ ifs)
	{
		StrVec filters_str = SplitString(filtersets_str[ifs],"&&");
		
		// --------------------
		// parse filter strings
		// --------------------
		
		PndSmpFilterSet filters;
		
		for (int i=0;i<(int) filters_str.size();++i)
		{
			// some filter checks
			TString fs = filters_str[i];
			if ( (!fs.EndsWith(")")) || (fs.Contains(",") && !fs.Contains("[")) ) 
			{
				cout <<" --> Please check filter string '"<<fs<<"'"<<endl;
				exit(0);
			}
			
			// a copy without the X() stuff
			TString tmpfs = fs(fs.Index("(")+1,fs.Length()-fs.Index("(")-2);
			
			StrVec cuts = SplitString(tmpfs,";");

			if (fVerbose>3) cout<<endl<<fs<<endl<<tmpfs<<endl;
			
			PndSmpFilt f;
			f.name = fs;

			// negate filter?
			if (fs.BeginsWith("!")) {f.veto = true; fs.ReplaceAll("!","");}
			
			// if the filter string has the form "M(...)" it's a mass filter, else a multiplicity filter ("(...)") 
			if (fs.BeginsWith("m(")) f.compo=true;
			
			for (int j=0;j<(int)cuts.size();++j)
			{
				TString ct = cuts[j];
				//cout <<"  "<<ct<<endl;

				// if specifier (p, tht, pt, ...) store it and remove the '[]'
				TString key = "";
				if (ct.Contains("[")) 
				{
					key = ct(0,ct.Index("["));
					ct = ct(ct.Index("[")+1,ct.Length()-ct.Index("[")-2);
				}

				// either multiplicity or particle names
				if (key=="")
				{
					// mult, either range delimiter '..' found
					if      (ct.Contains("..")) GetRangeInt(ct, f.nmin, f.nmax);
					// or single number, explicitely allowed to be 0 (kind of veto filtering)
					else if (ct.Atoi()!=0 || ct=="0")      f.nmin = f.nmax = ct.Atoi();
					// particle name of the particle to be counted or a list of particles for inv. mass filter (in case f.compo = true)
					else
					{
						StrVec parts = SplitString(ct);
						for (int jj=0;jj<(int)parts.size();++jj)
						{
							if (fNameCodeMap.find(parts[jj])!=fNameCodeMap.end() && f.ndau<5) f.pdg[f.ndau++] = fNameCodeMap[parts[jj]];
							
							// the key word 'nocc' prevents automatic charged conjugates for 
							if (parts[jj]=="nocc") f.nocc = true;
						}
						for (int jj=0;jj<(int)parts.size();++jj) if (f.pdg[jj]==0) f.pdg[jj] = 22; 
					}
				}
				
				// other pdg code
				else if (key=="pdg")  {f.pdg[0] = ct.Atoi(); f.ndau=1;}
								
				// kinematic ranges
				else if (key=="p")    GetRangeDouble(ct, f.pmin,   f.pmax);
				else if (key=="pt")   GetRangeDouble(ct, f.ptmin,  f.ptmax);
				else if (key=="pz")   GetRangeDouble(ct, f.pzmin,  f.pzmax);
				else if (key=="tht")  GetRangeDouble(ct, f.thtmin, f.thtmax);
				else if (key=="phi")  GetRangeDouble(ct, f.phimin, f.phimax, ",", 1);
				else if (key=="m")    GetRangeDouble(ct, f.mcntr,  f.mwin);
			}
			
			if ( (f.compo && f.ndau<2) || (!f.compo && f.ndau!=1))
			{
				cout <<" --> Please check filter string '"<<fs<<"'"<<endl;
				exit(0);				
			}
			else filters.push_back(f);
		}
		
		fFilterSets.push_back(filters); 
	}
}


// -------------------------------------------------------------------------

PndSmpCandList PndFilteredPrimaryGenerator::CombineList(int pdg, PndSmpCandList *l0, PndSmpCandList *l1, PndSmpCandList *l2, PndSmpCandList *l3, PndSmpCandList *l4)
{
	PndSmpCandList res;

	//-------------
	for (int i0=0; i0<(int)l0->size(); ++i0)
	{
		PndSmpCand &c0 = (*l0)[i0];
		
		// if l0 == l1, start at i1 = i0+1 to avoid double counting
		int st1 = 0;
		if ((*l1) == (*l0)) st1 = i0+1;		
		
		// -------------
		for (int i1=st1; i1<(int)l1->size(); ++i1)
		{
			PndSmpCand &c1 = (*l1)[i1];
			if (c1.Overlap(c0)) continue;
			
			// only two lists? done!
			if (l2==0) res.push_back(PndSmpCand(pdg, c0, c1));
			else
			{
				// if l2 == l0 or l1, start at i2 = i0/i1+1 to avoid double counting
				int st2 = 0;
				if (*l2==*l1) st2 = i1+1;
				else if (*l2==*l0) st2 = i0+1;
				
				// -------------
				for (int i2=st2; i2<(int)l2->size(); ++i2)
				{
					PndSmpCand &c2 = (*l2)[i2];
					if (c2.Overlap(c1) || c2.Overlap(c0)) continue;
					
					// only three lists? done!
					if (l3==0) res.push_back(PndSmpCand(pdg, c0, c1, c2));
					else
					{
						// if l3 == l0, l1 or l2, start at i3 = i0/i1/i2+1 to avoid double counting
						int st3 = 0;
						if (*l3==*l2) st3 = i2+1; 
						else if (*l3==*l1) st3 = i1+1; 
						else if (*l3==*l0) st3 = i0+1;
						
						// -------------
						for (int i3=st3; i3<(int)l3->size(); ++i3)
						{
							PndSmpCand &c3 = (*l3)[i3];
							if (c3.Overlap(c2) || c3.Overlap(c1) || c3.Overlap(c0)) continue;
							
							// only four lists? done!
							if (l4==0) res.push_back(PndSmpCand(pdg, c0, c1, c2, c3));
							else
							{
								// if l3 == l0, l1 or l2, start at i3 = i0/i1/i2+1 to avoid double counting
								int st4 = 0;
								if (*l4==*l3) st4 = i3+1; 
								else if (*l4==*l2) st3 = i2+1; 
								else if (*l4==*l1) st3 = i1+1;
								else if (*l4==*l0) st3 = i0+1;
								
								// -------------
								for (int i4=st4; i4<(int)l4->size(); ++i4)
								{
									PndSmpCand &c4 = (*l4)[i4];
									if (c4.Overlap(c3) || c4.Overlap(c2) || c4.Overlap(c1) || c4.Overlap(c0)) continue;
									
									res.push_back(PndSmpCand(pdg, c0, c1, c2, c3, c4));
								} // for i4
							}  // else i3
						}  // for i3
					}  // else i2
				}  // for i2		
			}  // else i1
		}  // for i1	
	}  // for i0
	
	return res;
} 

// --------------------------------------------------------------------

void PndFilteredPrimaryGenerator::PrintSmpCandList(PndSmpCandList l, TString name)
{
	printf("LIST %s with %d candidates\n",name.Data(), (int)l.size());
	for (int i=0;i<(int)l.size(); ++i) {printf(" %2d : ",i); l[i].Print();}
}


// --------------------------------------------------------------------
// -----   Public method GenerateEvent   -----------------------------------

Bool_t PndFilteredPrimaryGenerator::GenerateEvent(FairGenericStack* pStack)
{
	Int_t iTry = 0; // number of attempts to find the next event that suits your filter
	
	bool acc_glob = false; // is kTRUE if the event is finally accepted

	bool active_filters = (fFilterSets.size()>0);

	while( !acc_glob && iTry < fEvtFilterStat.fFilterMaxTries)
	{
		++iTry; // count how often we run the generators before we accept an event
		++fEvtFilterStat.fGeneratedEvents; // total number of generated events

		pStack->Reset(); // Clean the stack
		FairPrimaryGenerator::GenerateEvent(pStack); // fill the stack
		
		// no filtering --> accept event
		if (!active_filters) {acc_glob = true; continue;}
		
		TClonesArray* fParticleList = pStack->GetListOfParticles();
		
		
		fEventNr = fEventNrFiltered; // Fix event numbering in FairPrimaryGenerator (otherwise fRun will stop too early...)

		PndSmpCandList all;
		
		// init all lists to empty list
		std::map<int,PndSmpCandList> pdgList;
		for (unsigned int i=0;i<fCombFsPdg.size();++i)   pdgList[fCombFsPdg[i]] = all;

		int cnt = 0;
		
		// --------------
		// Fill the lists
		// --------------
		
		//for (int i=0;i<*nTrk;++i)
		//{
		for (Int_t iPart=0; iPart<fParticleList->GetEntries(); ++iPart) 
		{
			TParticle *particle = (TParticle*)fParticleList->At(iPart);
			
			// the current MCT pdg code
			Int_t pdg = particle->GetPdgCode();

			// do we want to count multiplicity for this code?
			//if (set_mult_pdg.find(pdg)==set_mult_pdg.end()) continue;
			
			// is yes, create SmpCand
			TLorentzVector l(particle->Px(), particle->Py(), particle->Pz(), particle->Energy());
			Float_t ch = fdbPdg->GetParticle(pdg) ? fdbPdg->GetParticle(pdg)->Charge()/3. : 0;
			
			// and push to to all-list
			if (fSetFsPdg.find(pdg)==fSetFsPdg.end())
			{
				// push to all list, but with uid = -1 for non-final states, since not needed for combinatorics
				all.push_back(PndSmpCand(l, ch, pdg, -1));
				continue;
			}

			// create candidate with marker (uid >= 0) only for final states used for combinatorics
			PndSmpCand sc(l, ch, pdg, cnt++);
			
			// and push to to all-list
			all.push_back(sc);
			
			// add particle to all lists with the same charge
			for (unsigned int j=0;j<fCombFsPdg.size();++j)
			{
				int    pdg_ch = fdbPdg->GetParticle(fCombFsPdg[j])->Charge()/3.;
				double pdg_m  = fdbPdg->GetParticle(fCombFsPdg[j])->Mass();
				
				if (pdg_ch == (int)ch) 
				{
					l.SetVectM(l.Vect(), pdg_m);
					sc.SetP4(l);
					pdgList[fCombFsPdg[j]].push_back(sc);
				}
			}		
		}

		// --------------
		// print all lists
		// --------------
		
		if (fVerbose>3)
		{
			PrintSmpCandList(all,"All particles:");
			
			if (fVerbose>4)
			for (int i=0;i<(int)fCombFsPdg.size();++i) 
			{
				int pdg = fCombFsPdg[i];
				int nc = pdgList[pdg].size();
				if (nc>0) PrintSmpCandList(pdgList[pdg],Form("%s",fCodeNameMap[pdg].Data()));
			}
			cout <<endl<<endl;
		}
				
		
		// -------------------
		// Loop over filters
		// -------------------

		acc_glob=false;
		
		for (int ifs=0; ifs < (int) fFilterSets.size(); ++ifs)
		{
			// did another filterset accept? then we don't need to check anymore
			if (acc_glob) continue;
			
			// initialize filter set accept with true
			bool acc_fset = true;

			// -------------------
			// *** loop over count filters in filter set
			// -------------------
			for (int i=0; i<(int)fFilterSets[ifs].size(); ++i)
			{
				// if the event cannot be accepted anymore, skip the next filter test
				if (!acc_fset) continue;

				// event accept flag
				bool acc_evt = true;
				
				PndSmpFilt &f = fFilterSets[ifs][i];

				// if mass filter, skip here (first apply count filters, which are probably much faster)
				if (f.compo) continue;
				
				int cnt_matches = 0;
				
				// *** loop over particles
				for (int j=0 ; j<(int)all.size()  ; ++j)
				{
					// *** check particle type first
					//vector<TString> names = {"e+", "e-", "mu+", "mu-", "pi+", "pi-", "k+", "k-", "p+",  "p-", "n0", "n0b", "e+-", "mu+-", "pi+-", "k+-",  "p+-", "n"  , "t+", "t-", "t+-", "nt", "any" };
					//vector<int> name_pdg  = {-11 , 11  , -13  , 13   , 211  , -211 , 321 , -321, 2212, -2212, 2112, -2112, 11000  , 13000   , 211000  , 321000 ,  2212000, 2112000, 1    , -1   ,  2    ,  0   ,  3   };
					
					// tracks pdg and charge
					int trpdg = all[j].Pdg();	
					int trchg = (int) all[j].Charge(); 
					
					// filter pdg (or special) code
					int flpdg = f.pdg[0];

					// the counters for any, t+-, t+, t- and nt/gam only for final states
					if (all[j].Marker()>0)
					{
						// any particle: code = 3
						if (flpdg == 3)   trpdg = flpdg;

						// all charged tracks t+- : code = 2
						if (flpdg == 2)   trpdg = abs(trchg)*2;

						// simple track+- and neutral counting (t+,t-,nt) : code = charge (-1, +1, 0); 0 also used for photons with pdg = 22 
						if (abs(flpdg)<2) trpdg = trchg;
					}
					
					// the pdg code ignoring charge : code = 1000*pdg, e.g. K+- = 321000, pi+- = 211000
					if (flpdg%1000==0) trpdg = abs(trpdg)*1000;
					
					// *** check code and kinematic variables and count track
					if ( (trpdg==flpdg)  &&  CheckKinematic(f,  all[j].P4()) )
					{
						cnt_matches++;
						if (fVerbose>2) cout  << "\033[1;34m ** ";
					}
					else if (fVerbose>2) cout << "    ";
					
					// *** (and print if verbose)
					if (fVerbose>2) {all[j].Print(); cout<<"\033[0;0m";}
				}
				
				// does multiplicity match?
				if (cnt_matches<f.nmin || cnt_matches>f.nmax) acc_evt = false;
				
				// is this a veto filter -> negate result
				acc_evt = acc_evt^f.veto;
				
				if (fVerbose>2) cout <<"Filterset "<<ifs<<" Filter "<<i<<" : N_matched = "<<cnt_matches<<" : event "<<(!acc_evt?"not ":"")<<"accepted"<<endl<<endl<<endl; 

				// global accept (of all filters)
				acc_fset = (acc_fset && acc_evt);		
				
			} // **** count filters
			
			
			// -------------------
			// Loop over mass filters
			// -------------------
			for (int i=0; i<(int)fFilterSets[ifs].size(); ++i)
			{
				// if the event cannot be accepted anymore, skip the next filter test
				if (!acc_fset) continue;

				// event accept flag
				bool acc_evt = true;
				
				PndSmpFilt &f = fFilterSets[ifs][i];

				// now only mass filters
				if (!f.compo) continue;
						
				// ----------------
				// do combinatorics
				// ----------------

				// prepare lists of particles and the charged conjugation
				PndSmpCandList *l[5] = {0}, *al[5] = {0};
				std::vector<int> vpdg, vapdg;
				
				for (int j=0; j<f.ndau; ++j)
				{
					vpdg.push_back(f.pdg[j]); 
					vapdg.push_back(AntiPdgCode(f.pdg[j]));
					
					l[j]  = &(pdgList[f.pdg[j]]);
					al[j] = &(pdgList[AntiPdgCode(f.pdg[j])]);
				}
								
				// *** do combinatorics
				PndSmpCandList comblist = CombineList(0, l[0], l[1], l[2], l[3], l[4]);

				// do we want c.c. and is the cc'd list not the same as the original one (= a permutation)?
				if (!f.nocc && !is_permutation(vpdg.begin(), vpdg.end(), vapdg.begin())) 
				{
					PndSmpCandList anticomblist = CombineList(0, al[0], al[1], al[2], al[3], al[4]);
					comblist.insert(comblist.end(), anticomblist.begin(), anticomblist.end());
				}
				
				int cnt_matches = 0;
				
				// *** loop over composite particles
				for (int j=0 ; j<(int)comblist.size()  ; ++j)
				{
					TLorentzVector p4 = comblist[j].P4();
					
					// *** check mass window and kinematics of composite
					bool acc_cand = (fabs(p4.M() - f.mcntr) < f.mwin/2.)  &&  CheckKinematic(f, p4);   

					// *** count track
					if (acc_cand)
					{
						cnt_matches++;
						if (fVerbose>2) cout  << "\033[1;35m ** ";
					}
					else if (fVerbose>2) cout << "    ";
					
					// *** (and print if verbose)
					if (fVerbose>2) { comblist[j].Print(); cout<<"\033[0;0m";}
				}		
				
				// does multiplicity match?
				if (cnt_matches<f.nmin || cnt_matches>f.nmax) acc_evt = false;
				
				// is this a veto filter -> negate result
				acc_evt = acc_evt^f.veto;
				
				if (fVerbose>2) cout <<"Filterset "<<ifs<<" Filter "<<i<<" : N_matched = "<<cnt_matches<<" : event "<<(!acc_evt?"not ":"")<<"accepted"<<endl<<endl<<endl; 

				// global accept (of all filters)
				acc_fset = (acc_fset && acc_evt);		
				
			} // **** mass filters
			
			
			// the global event accept, being an OR connection of all filter sets ('event types' to be accepted)
			
			acc_glob = (acc_glob || acc_fset);
		} // **** filter sets
		
		
		//if (fVerbose>1) cout <<" --> EVENT "<<entry<<" "<<(!acc_glob?"NOT ":"")<<"ACCEPTED"<<endl<<endl<<endl; 
		
		//if (acc_glob) {cnt_acc++;}		
	} 


	// Set the event number ALWAYS when filtering
	++fEventNrFiltered;
	fEvent->SetEventID(fEventNrFiltered);

	if ( !acc_glob ){
		++fEvtFilterStat.fFailedFilterEvents;
		cout << "\n -E PndFilteredPrimaryGenerator: No event was found within " << iTry << " tries which satisfies your event filter.\n ";
		cout << "I accept a random event as evtNr " << fEventNrFiltered << " to avoid infinite loops. \n";
		cout <<  "Try increasing the max. number of tries or change your filter\n\n";
		if (fVerbose > 3 ){
			cout << iTry << " events simulated until I found a good one.\n";
			cout << fEvtFilterStat.fGeneratedEvents << " events generated for finding " << fEventNr << " accepted events.\n";
			cout << fEvtFilterStat.fFailedFilterEvents << " unsuccessful attempts in total to find an event that suits your filters\n\n";
		}
	}

	if (0 < fVerbose && active_filters && (fEventNrFiltered%fEventPrintFreq)==0) cout <<"[PndFilteredPrimaryGenerator] " << fEventNrFiltered << " / " << fEvtFilterStat.fGeneratedEvents << " generated events accepted.\n";

	return kTRUE;
}
// -------------------------------------------------------------------------

ClassImp(PndFilteredPrimaryGenerator)
