using namespace std;

void parfiletest(string Filename, Int_t iMaxValues)
{
	string InputBuffer;
	char* pEnd;
	Double_t Value1;
	Double_t *Values = new Double_t[16];
	
	ifstream InputFile;
	InputFile.open(Filename.data());
	
	if (!InputFile)
	{

		cout << "File does not exist" << endl;
	}
	else if (InputFile.good())
	{
		getline(InputFile,InputBuffer);
		cout << InputBuffer << endl;
		pEnd = InputBuffer.data();
		for (Int_t iValues = 0; iValues < iMaxValues; iValues++)
		{
		Values[iValues = strtod(pEnd,&pEnd);
		
		cout << "Value_" << iValues <<": " <<Value1 << endl<<endl;
		//cout << pEnd << endl;
		};
	}
	
	
}


GetParameters(ifstream InputFile,Int_t iMaxValues)


