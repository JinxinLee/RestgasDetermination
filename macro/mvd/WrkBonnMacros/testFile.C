int testFile (TString nome, TString type){

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");  
  rootlogon();
  
  PndFileNameCreator namecreator(nome.Data());
  
  if (type = "sim")
    {
      TString nome_test = namecreator.GetSimFileName();
    }
  else
    {
      if (type = "digi")
	{
	  TString nome_test = namecreator.GetRecoFileName();
	}
      else
	if (type = "pid")
	  {
	    TString nome_test = namecreator.GetCustomFileName("pidtracks");
	  }
	else 
	  {
	    cout << "-------------------------" << endl;
	    cout << "WRONG TYPE SELECTION!!!!!" << endl;
	    cout << "-------------------------" << endl;
	  }
      
    }

  TFile *f = new TFile(nome_test.Data(),"READ");

  TTree *t = (TTree*) f->Get("cbmsim");

  t->Dump();

  exit(0);


  return 0;
}
