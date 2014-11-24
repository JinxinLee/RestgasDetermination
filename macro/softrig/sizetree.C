#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TString.h"
#include "TKey.h"

#include <iostream>

void sizetree(TString fname)
{
	int totsize = 0;
	TString name[1000];
	int size[1000];
	int n[1000];
	int br[1000];
	int cnt=0;
	
	TFile *f=new TFile(fname,"READ");
	if (!f->IsZombie())
	{		
		TKey *key;
		TIter next(f->GetListOfKeys());
		while ( (key = (TKey*)next()) )
		{
			TObject *obj = key->ReadObj();
			
			if (!obj->InheritsFrom("TTree")) continue;
			
			name[cnt] = obj->GetName();
			TTree* t = (TTree*) obj; 
			
			n[cnt]  = t->GetEntries();
			br[cnt] = t->GetNbranches();
			int s = 0;
						
			TObjArray* branches = t->GetListOfBranches();
			for(int i=0; i<=branches->GetLast(); ++i)
			{
				TBranch* branch = (TBranch*)branches->UncheckedAt(i);
				s += branch->GetZipBytes();
			}
			size[cnt++]=s;
			
			totsize += s;
		}
	}
	f->Close();
	
	cout <<cnt<<endl;
	
	int ar[1000];
	for (int i=0;i<cnt;++i) ar[i]=i;
	
	for (int i=cnt;i>1;--i)
	{
		for (int j=0;j<i-1;++j) 
		{
			if (size[ar[j]]<size[ar[j+1]])
			{
				int tmp=ar[j];ar[j]=ar[j+1];ar[j+1]=tmp;
			}
		}
	}
	
	for (int i=0;i<cnt;++i)
		printf("%2d. %10s : N = %8d, BR = %5d, N*BR = %7dk, Size = %7dk\n", 
			ar[i],name[ar[i]].Data(), n[ar[i]], br[ar[i]], (n[ar[i]]*br[ar[i]])/1000, size[ar[i]]/1000);
	
	cout <<"total size: "<<totsize/1000<<"k"<<endl;
}
