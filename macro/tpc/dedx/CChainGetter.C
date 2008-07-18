#include <fstream>
#include <iostream>

#include "TTree.h"
#include "TChain.h"
#include "TFile.h"

#include "CChainGetter.h"

using namespace std;

CChainGetter::~CChainGetter()
{	
/*
	if(m_pChain)	{
		m_pChain->Delete();
	}
	*/
}

void CChainGetter::Reset()
{
	if(m_pChain)	{
		m_pChain->Delete();
		m_pChain=NULL;
	}
	m_vFileNames.clear();
}

bool CChainGetter::CheckFile(string filename)
{	//check if file can be opened properly
	bool flag=false;
	{
		fstream fin;
		fin.open(filename.c_str(),ios::in);
		if( fin.is_open() )		{
			flag=true;
		}
		fin.close();
	}
	return flag;
}

bool CChainGetter::CheckTree(string filename)
{
	bool flag=false;
	if(!CheckFile(filename.c_str()))	{
		cout << "File: " << filename << " -> cannot be opened!" << endl;
		return false;
	}
	
	TTree *pTree=NULL;
	TFile *pFile=new TFile(filename.c_str(), "READ");
	if(pFile)	{
		pTree = (TTree*)pFile->Get(m_strTreeName.c_str());
	}
	
	if(pTree)	{	//check if tree can be opened
		flag=true;
		pTree->Delete();
		pFile->Close();
		
	}
	else	{
		cout << "File: " << filename << " -> Tree " << m_strTreeName << " not valid!" << endl;
	}
	return flag;
}


int CChainGetter::ReadFileNames()
{
	int nFiles=0;	// number of lines in the fileListFile
	//________________________________________
	//Read all lines in the fileListFile and store them in vFileNames
	if(m_strFileListFile.empty())	{	// is a name specified?
		return nFiles;
	}
	ifstream ifs( m_strFileListFile.c_str() );
	string temp;
	while( getline( ifs, temp ) )	
	{
		temp=temp+m_strFileExtension;
		
		//Add to list if file exists, only
		bool flag = false;
		flag=CheckTree(temp.c_str());
		if(flag)	{
        		m_vFileNames.push_back( temp );
			nFiles++;
		}
	}
	
	vector<string>::const_iterator cit;
	for(cit=m_vFileNames.begin();cit!=m_vFileNames.end();cit++)
	{
		printf("%s\n", (*cit).c_str());
	}
	return nFiles;
}

void CChainGetter::AddFileToChain(std::string filename)
{
	if(CheckTree(filename))	{
		m_vFileNames.push_back(filename);
	}
}

TChain* CChainGetter::GetChain()
{
	TChain *pTree=NULL;
	pTree=new TChain( m_strTreeName.c_str() );
	assert(pTree);
	vector<string>::const_iterator citFile;
	for(citFile=m_vFileNames.begin();citFile!=m_vFileNames.end();citFile++)
	{
		pTree->Add(citFile->c_str());
	}
	m_pChain=pTree;
	return pTree;
}
