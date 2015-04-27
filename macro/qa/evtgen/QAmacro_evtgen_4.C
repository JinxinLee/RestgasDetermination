

void add_mccode(TString &mctxt,Int_t id)
{
	switch(id)
	{
	case 22: mctxt+="#gamma" ; break;
	case 11: mctxt+="e^{-}" ; break;
	case -11: mctxt+="e^{+}" ; break;
	case 13: mctxt+="#mu^{-}" ; break;
	case -13: mctxt+="#mu^{+}" ; break;
	case 111: mctxt+="#pi^{0}" ; break;
	case 211: mctxt+="#pi^{+}" ; break;
	case -211: mctxt+="#pi^{-}" ; break;
	case 310: mctxt+="K^{0}_{S}" ; break;
	case 130: mctxt+="K^{0}_{L}" ; break;
	case 321: mctxt+="K^{+}" ; break;
	case -321: mctxt+="K^{-}" ; break;

	case 12: mctxt+="#nu_{e}"; break;
	case 14: mctxt+="#nu_{#mu}"; break;
	case -12: mctxt+="#bar{#nu}_{e}"; break;
	case -14: mctxt+="#bar{#nu}_{#mu}"; break;

	case 2112: mctxt+="n" ; break;
	case 2212: mctxt+="p" ; break;

	case 3122: mctxt+="#Lambda"; break;
	case 3124: mctxt+="#Lambda(1520)"; break;
	case 13122: mctxt+="#Lambda(1405)"; break;

	case 3222: mctxt+="#Sigma^{+}"; break;
	case 3224: mctxt+="#Sigma^{*+}"; break;
	case 3212: mctxt+="#Sigma^{0}"; break;
	case 3214: mctxt+="#Sigma^{*0}"; break;
	case 3112: mctxt+="#Sigma^{-}"; break;
	case 3114: mctxt+="#Sigma^{*-}"; break;

	case 3312: mctxt+="#Xi^{-}"; break;
	case 3322: mctxt+="#Xi^{0}"; break;
	case 3314: mctxt+="#Xi^{*-}"; break;
	case 3324: mctxt+="#Xi^{*0}"; break;

	case 3334: mctxt+="#Omega^{-}"; break;

	case -2112: mctxt+="#bar{n}" ; break;
	case -2212: mctxt+="#bar{p}" ; break;

	case -3122: mctxt+="#bar{#Lambda}"; break;
	case -3124: mctxt+="#bar{#Lambda}(1520)"; break;
	case -13122: mctxt+="#bar{#Lambda}(1405)"; break;

	case -3222: mctxt+="#bar{#Sigma}^{-}"; break;
	case -3224: mctxt+="#bar{#Sigma}^{*-}"; break;
	case -3212: mctxt+="#bar{#Sigma}^{0}"; break;
	case -3214: mctxt+="#bar{#Sigma}^{*0}"; break;
	case -3112: mctxt+="#bar{#Sigma}^{+}"; break;
	case -3114: mctxt+="#bar{#Sigma}^{*+}"; break;

	case -3312: mctxt+="#bar{#Xi}^{+}"; break;
	case -3322: mctxt+="#bar{#Xi}^{0}"; break;
	case -3314: mctxt+="#bar{#Xi}^{*+}"; break;
	case -3324: mctxt+="#bar{#Xi}^{*0}"; break;

	case -3334: mctxt+="#bar{#Omega}^{+}"; break;

	case 443: mctxt+="J/#Psi"; break;
	case 100443: mctxt+="#Psi(2S)"; break;
	case 441: mctxt+="#eta_c(1S)"; break;
	case 10441: mctxt+="#xi_{c0}(1P)"; break;
	case 20443: mctxt+="#xi_{c1}(1P)"; break;
	case 445: mctxt+="#xi_{c2}(1P)"; break;
	case 100441: mctxt+="#eta_c(2S)"; break;
	case 10443: mctxt+="h_c(1P)"; break;

	default: mctxt+="?" ; break;
	}
}


TString propagate_mclist(Int_t id,TClonesArray * mctrack_array)
{
    PndMCTrack *mct;
    PndMCTrack *mo;

    TLorentzVector sum, tmp;
    TString chtxt="";

    if(id<0) return "none";
    mct=(PndMCTrack *)mctrack_array->At(id);

    if(mct->GetMotherID()==-1){// show only first geant generation
      for(Int_t i=0; i<mctrack_array->GetEntries(); i++){
    	  mo=(PndMCTrack *)mctrack_array->At(i);

    	  if(mo->GetMotherID()==id || (mo->GetMotherID()==-1 && mo->GetSecondMotherID()==id)){
    		  chtxt+=propagate_mclist( i, mctrack_array);
    	  }
      }
    }

    TString mctxt="";
    add_mccode(mctxt,mct->GetPdgCode());

    if(chtxt!="") mctxt+=TString(" (")+chtxt+TString(")");
   return mctxt+" ";
}

// TEST 3: check result
void QAmacro_evtgen_4()
{
  TStopwatch timer;
  timer.Start();

  // defined by what is simulated in qa macros 2
  TString shouldbe="#Psi(2S) (#xi_{c1}(1P) (J/#Psi (#Lambda (p #pi^{-} ) #bar{#Lambda} (#bar{n} #pi^{0} (#gamma e^{+} e^{-} ) ) ) #gamma ) #gamma ) ";

  cout << "-------------------------------------------------"<<endl
		  << "This macro checks the decay tree stored by QAmacro evtgen_2"<<endl
		  << "in file qa_evtgen2.root"<<endl
		  << "Is checks the layout of the (fixed) tree by a string comparison."<<endl
		  << "-----------------------------------------------"<<endl;

  Bool_t fTest=kTRUE;

    PndMCTrack *mo;
  {
	  TChain *abc=new TChain("cbmsim");
	  abc->Add("qa_evtgen2.root");

    TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
    abc->SetBranchAddress("MCTrack",&mctrack_array);

    Int_t ncounts = abc->GetEntries();
    if(ncounts<1) fTest=kFALSE;

    for(int k = 0; k < ncounts; k++){
    	TString mytxt, fullevt;
      abc->GetEntry(k);

      fullevt="";
      cout << "====== Evt "<<k<<" ======="<<endl;
      for(Int_t i=0; i<mctrack_array->GetEntries(); i++){// for all generator generated particles
        mo=(PndMCTrack *)mctrack_array->At(i);
        mytxt="";
        if(mo->GetMotherID()==-1 && mo->GetSecondMotherID()==-1) {
        	mytxt = propagate_mclist(i, mctrack_array);
            cout << "Part "<<i<<": "<<mytxt<<endl;
            if(fullevt!="") fullevt+=" / ";
            fullevt+=mytxt;
        }
      }
      cout << "Full Event: "<<fullevt<<endl;
      if(fullevt!=shouldbe){
          cout << "Event layout differs from the one defined in qa macro!!!"<<endl;
          cout << "Shouldbe: "<<shouldbe<<endl;
          fTest=kFALSE;
      }
      cout << "==================="<<endl;
     }
  }

  if (fTest){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;
  }
 timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  exit(0);
}

