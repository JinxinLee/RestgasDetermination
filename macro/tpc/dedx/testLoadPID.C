#include "tpc/PndTpcDEDXFits.h"

void testLoadPID()
{
	PndTpcDEDXFits Fits;
	cout << "Loading Resolution: " << endl;
	//Fits.SetAdditionalCoordinate('T', 40);
	//Fits.LoadResolution("test4R.txt");
	Fits.LoadResolution("E2t40R.txt");
	cout << "Loading BB-Fit Parameter: " << endl;
	//Fits.SetAdditionalCoordinateBB('T',40);	//40 sollte hier nicht stehen, das lade ich doch erst!!!!
	//Fits.LoadFile("test4B.txt");
	Fits.LoadFile("E2t40B.txt");
	cout << "--------" << endl;
	Fits.Print();
	double reso_40=Fits.GetResolution("K+",40);
	double reso_42=Fits.GetResolution("K+",42);
	double reso_50=Fits.GetResolution("K+",50);
	double reso_55=Fits.GetResolution("K+",55);
	double reso_60=Fits.GetResolution("K+",60);
	cout << "reso_40: " << reso_40 << endl;
	cout << "reso_42: " << reso_42 << endl;	
	cout << "reso_50: " << reso_50 << endl;	
	cout << "reso_55: " << reso_55 << endl;	
	cout << "reso_60: " << reso_60 << endl;	
	TF1 *p =Fits.GetFunction("K+");
	p->Draw();
}
