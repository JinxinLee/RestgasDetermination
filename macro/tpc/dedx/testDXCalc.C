#include "PndTpcDXCalculator.h"
#include <vector>
#include <utility>
#include <iostream>
#include "assert.h"

using namespace std;

void testDXCalc()
{
	double de=0.;
	double dx=0.;
	
	PndTpcDXCalculator Calc;
	
	Calc.SetNextPoint(1., 0., 0., 2.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(2., 0., 0., 2.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(3., 0., 0., 2.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(4., 0., 0., 2.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(5., 0., 0., 2.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
}


void testCenteredDXCalc()
{
	double de=0.;
	double dx=0.;
	
	PndTpcDXCalculator Calc;
	
	Calc.SetNextPoint(1., 0., 0., 2.);
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(2., 0., 0., 2.);
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(3., 0., 0., 2.);
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(4., 0., 0., 2.);
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.SetNextPoint(5., 0., 0., 2.);
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
	
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx << endl;
}

void testCenteredDXCalc2()
{
	double de=0.;
	double dx=0.;
	
	PndTpcDXCalculator Calc;
	
	Calc.SetNextPoint(1., 0., 0., 1.);
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert( dx == 0. && de == 0. );
	
	Calc.SetNextPoint(5., 0., 0., 3.);	// 2+1
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 0. && de == 0. );
	
	Calc.SetNextPoint(7., 0., 0., 2.);	//1+3
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 3. && de == 3. );
	
	Calc.SetNextPoint(13., 0., 0., 5.);	//3+1.5
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 4. && de == 2. );
	
	Calc.SetNextPoint(16., 0., 0., 4.);
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 4.5 && de == 5. );
	
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	
	Calc.GetCenteredDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
}

void testDXCalc2()
{
	double de=0.;
	double dx=0.;
	
	PndTpcDXCalculator Calc;
	
	Calc.SetNextPoint(1., 0., 0., 1.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert( dx == 0.);
	
	Calc.SetNextPoint(5., 0., 0., 3.);	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 4.);
	
	Calc.SetNextPoint(7., 0., 0., 2.);	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 2.);
	
	Calc.SetNextPoint(13., 0., 0., 2.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 6.);
	
	Calc.SetNextPoint(16., 0., 0., 2.);
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	assert(dx == 3.);
	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
	
	Calc.GetDEDX(de,dx);
	cout << "de: " << de << " dx: " << dx <<"--" << Calc << endl;
}

void testAll()
{
	testDXCalc();
	cout << " ----" << endl;
	testDXCalc2();
	cout << " ----" << endl;
	testCenteredDXCalc();
	cout << " ----" << endl;
	testCenteredDXCalc2();
}
