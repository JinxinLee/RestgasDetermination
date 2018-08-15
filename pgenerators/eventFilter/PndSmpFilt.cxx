#include "PndSmpFilt.h"

#include <iostream>

using std::cout;
using std::endl;

// --------------------------------------------------------------------

void PndSmpFilt::Print()
{
		if (compo)
		{
			cout <<"----------------"<<endl<< "mass + count filter \""<<name<<"\""<<endl<<"----------------"<<endl;
			cout << "     dau : ";
			for (int j=0;j<ndau;++j) cout << pdg[j] <<" "; // << " (" << code_name_map[pdg[j]] << ") ";
			if (!nocc) cout << " (+ c.c.)";
			cout << endl;
		}
		else
		{
			cout <<"----------------"<<endl<< "count filter \""<<name<<"\""<<endl<<"----------------" << endl;
			cout << "     pdg : "  << pdg[0] << endl;// << " (" << code_name_map[pdg[0]] << ")" << endl;
		}
		
		if (nmin>0 || nmax<10000){
			cout << "     mul : ";
			if (nmax==10000) cout << " >= " << nmin << endl;
			else if (nmin==nmax) cout <<nmin<<endl;
			else if (nmin==0 ) cout << " <= " << nmax << endl;
			else cout << nmin << " ... " << nmax << endl;
		}		
		
		if (pmin>0 || pmax<1e8)
		{
			cout << "     p   : ";
			if (pmax==1e8) cout << " >= " << pmin <<" GeV/c" << endl;
			else if (pmin==0  ) cout << " <= " << pmax <<" GeV/c"<< endl;
			else cout << pmin << " ... " << pmax <<" GeV/c"<< endl;
		}		
		
		if (ptmin>0 || ptmax<1e8)
		{
			cout << "     pt  : ";
			if (ptmax==1e8) cout << " >= " << ptmin <<" GeV/c"<< endl;
			else if (ptmin==0  ) cout << " <= " << ptmax <<" GeV/c"<< endl;
			else cout << ptmin << " ... " << ptmax <<" GeV/c"<< endl;
		}		
		
		if (pzmin>-1e8 || pzmax<1e8)
		{
			cout << "     pz  : ";
			if (pzmax==1e8) cout << " >= " << pzmin <<" GeV/c"<< endl;
			else if (pzmin==-1e8  ) cout << " <= " << pzmax <<" GeV/c"<< endl;
			else cout << pzmin << " ... " << pzmax <<" GeV/c"<< endl;
		}		
		
		if (thtmin>0 || thtmax<180)
		{
			cout << "     tht : ";
			if (thtmax==180) cout << " >= " << thtmin <<" deg"<< endl;
			else if (thtmin==0  ) cout << " <= " << thtmax <<" deg"<< endl;
			else cout << thtmin << " ... " << thtmax <<" deg"<< endl;
		}		
		
		if (phimin>-1e8 || phimax<1e8)
		{
			cout << "     phi : ";
			if (phimax==1e8) cout << " >= " << phimin <<" deg"<< endl;
			else if (phimin==-1e8  ) cout << " <= " << phimax <<" deg"<< endl;
			else cout << phimin << " ... " << phimax <<" deg"<< endl;
		}		
		
		
		if (compo) cout <<"    mass : " <<mcntr <<" +- 0.5*"<<mwin<<" ["<<mcntr-mwin/2<<" ... "<<mcntr+mwin/2.<<"]  GeV/c2"<<endl;
		if (veto)  cout <<"    VETO FILTER"<<endl;
		
		cout <<"----------------"<<endl;
}

// -------------------------------------------------------------------------

ClassImp(PndSmpFilt)

