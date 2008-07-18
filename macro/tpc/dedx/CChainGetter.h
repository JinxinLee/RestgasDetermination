#ifndef CCHAINGETTER_HH
#define CCHAINGETTER_HH

#include <string>
#include <vector>
#include "TString.h"

class TString;
class TChain;
class TFile;
class TTree;


class CChainGetter	{
public:
	CChainGetter(std::string TreeName="cbmsim") : m_strTreeName(TreeName), m_pChain(NULL)	{}
	~CChainGetter();
	
	//Set a File in which every line specifies a tree file
	void SetFileListFile(std::string filename)	{ m_strFileListFile=filename; } 
	//Set an extension that is added to all filenames
	void SetFileExtension(std::string ext)	{ m_strFileExtension=ext; }
   	//Reads the Filenames listet in m_strFileListFile and stores them in a vector
	int ReadFileNames();	
	
	void AddFileToChain(std::string filename);
	void Reset();
	TChain * GetChain();
	
	//check if file exists
	bool CheckFile(std::string filename);
	
	//check if tree is sane
	bool CheckTree(std::string filename);
	
private:
	std::string m_strTreeName;
	std::string m_strFileListFile; //a name of the text file, where every line is the name of a root tree file 
	std::string m_strFileExtension; //a string to be added at the end of every name in vFileNames
	std::vector<std::string> m_vFileNames;	//a list of files out of which the tree consists
	TChain *m_pChain;
};

#endif
