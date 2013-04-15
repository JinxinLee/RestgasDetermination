#include "PndTrkBoundaryParStraws.h"
#include "PndSttTube.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <stdlib.h>
#include <iostream>
#include <cmath>


// Root includes
#include "TROOT.h"


using namespace std;

//------------------ begin function  PndTrkBoundaryParStraws::CalculateSpecialRegion
void  PndTrkBoundaryParStraws::CalculateSpecialRegion(
	Double_t RSTRAWDETECTORMIN,
	Double_t APOTEMAMAXINNERPARSTRAW,
	Double_t APOTEMAMINOUTERPARSTRAW,
	Double_t VERTICALGAP,
	Double_t &x,
	Double_t &y
	)
{
	double a1 = 16., a2 = 23., a3 = 31.86, delta = 2.;


	double px, py, qx, qy;
	double alfa, beta, D, A, B, x1, y1, x2, y2;

	a1 = RSTRAWDETECTORMIN;
	a2 = APOTEMAMAXINNERPARSTRAW;
	a3 = APOTEMAMINOUTERPARSTRAW;
	delta = VERTICALGAP;
	px = delta/2.;
	qx = -px;
	py = (2.*a2-0.5*delta)/sqrt(3.);
	qy = (-0.5*delta + 2.*a1)/sqrt(3.);
	
	beta = -(px*px+py*py+qx*qx+qy*qy)/(py+qy);
	
	alfa = (-beta*py-px*px-py*py)/px;

	A = -4.*a3*sqrt(3.) - alfa*sqrt(3.) + beta ;
	B = 4.*a3*a3+2.*alfa*a3;

	D = A*A-16.*B;
	if( D <0. ) { cout<<"delta = "<<D<<", stop!\n"; exit(-1);};
	D = sqrt(D);
	cout<<"A "<<A<<", B "<<B<<", delta "<<D<<endl;
	y1 = (-A + D)/8. ;
	y2 = (-A - D)/8. ;

	x1 = 2.*a3 - sqrt(3.)*y1;
	x2 = 2.*a3 - sqrt(3.)*y2;

	// il punto nel 1 quadrante ha coordinate (x1,y1);
	// la soluzione (x2,y2) e' l'intersezione che non interessa;

	cout<<"soluzione 1 , x1 "<<x1<<", y1 "<<y1<<endl;
	cout<<"soluzione 2 , x2 "<<x2<<", y2 "<<y2<<endl;

	x = x1;
	y = y1;

	return ;

}
//------------------ end function  PndTrkBoundaryParStraws::CalculateSpecialRegion



//----------begin of function PndTrkBoundaryParStraws::Set

void PndTrkBoundaryParStraws::Set(
	// inputs :
	Double_t APOTEMAMAXINNERPARSTRAW,
	Double_t APOTEMAMINOUTERPARSTRAW,
	Short_t	 NUMBER_STRAWS,
	Double_t RSTRAWDETECTORMIN,
	Double_t RSTRAWDETECTORMAX,
	bool stampa,
	TClonesArray *SttTubeArray,
	Double_t STRAWRADIUS,
	Double_t VERTICALGAP,
	// outputs :
	Short_t * StrawCode,
	Short_t * StrawCode2
	)
{
	// return true for those parallel Straws that are at the boundary
	// of the Stt central tracker;

	// the Stt Straw original number goes from 1 to 4542 included;


// StrawCode convention (in the following left or right is looking to the beam from downstream) :
//   -1 = not a boundary straw;
//   10= inner axial boundary left;
//   20= inner axial boundary right;
//   12= outer VERTICAL (BUT NOT OUTERMOST) axial boundary left;
//   22= outer VERTICAL (BUT NOT OUTERMOST)  axial boundary right;
//   13= outermost axial boundary left;
//   23= outermost axial boundary right;



	for(int i=0;i<NUMBER_STRAWS; i++){
		StrawCode[i] = -1 ;
		StrawCode2[i] = -1 ;
	}


	SttTubeList(
	// inputs :
	SttTubeArray,
	RSTRAWDETECTORMIN,
	RSTRAWDETECTORMAX,
	APOTEMAMAXINNERPARSTRAW,
	APOTEMAMINOUTERPARSTRAW,
	VERTICALGAP,
	STRAWRADIUS,
	NUMBER_STRAWS,
	stampa,
	// outputs :
	StrawCode,
	StrawCode2
	);

	return;

}
//----------end of function PndTrkBoundaryParStraws::Set


//------------------ begin function  PndTrkBoundaryParStraws::SttTubeList
void  PndTrkBoundaryParStraws::SttTubeList(
	// inputs :
	TClonesArray *SttTubeArray,
	Double_t RSTRAWDETECTORMIN,
	Double_t RSTRAWDETECTORMAX,
	Double_t APOTEMAMAXINNERPARSTRAW,
	Double_t APOTEMAMINOUTERPARSTRAW,
	Double_t VERTICALGAP,
	Double_t STRAWRADIUS,
	Short_t NUMBER_STRAWS,
	bool stampa,
	// outputs :
	Short_t *StrawCode, // goes from 0 to NUMBER_STRAWS-1 included;
	Short_t *StrawCode2 // goes from 0 to NUMBER_STRAWS-1 included;
					)
{
	int	code,
		i,
		tubeID;
	double
		apotema,
		Rrr,
		x,
		y;

	PndSttTube *pSttTube ;
	TVector3 center;
	TVector3 wiredirection;


//-----------------------------
	int num = NUMBER_STRAWS;

// calcolo delle regioni 'speciali';

 CalculateSpecialRegion(
	RSTRAWDETECTORMIN,
	APOTEMAMAXINNERPARSTRAW,
	APOTEMAMINOUTERPARSTRAW,
	VERTICALGAP,
	x, // ascissa of intersection point in 1st quadrant;
	y  // ordinate of intersection point in 1st quadrant;
	);


//---------------------------------

 if(stampa){
	cout<<"numero totale di tubi "<<num<<endl;
	// il primo tubo e' il n. 1;
	tubeID=1;
	pSttTube = (PndSttTube*) SttTubeArray->At(tubeID);
	center = pSttTube->GetPosition();
	wiredirection = pSttTube->GetWireDirection();
	Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
	cout<<"\tFirst STT straw tubeID;  n.  "<<tubeID<<", centro X "<< center.X()
	<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr<<endl;
	//	double HL = pSttTube->GetHalfLength();
	// ultimo tubo;
	tubeID=num;
	pSttTube = (PndSttTube*) SttTubeArray->At(tubeID);
	center = pSttTube->GetPosition();
	wiredirection = pSttTube->GetWireDirection();
	Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
	cout<<"\tLast STT straw tubeID,  n.  "<<tubeID<<", centro X "<< center.X()
	<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr<<endl<<endl;
 }
//---------------------------------


//  Stt a sinistra (col beam in faccia) -->  + 10 nello StrawCode;
//  Stt a destra (col beam in faccia) -->  + 20 nello StrawCode;


//   Verticali al centro :
	// abs( X ) < 3 ;
	// 16 < abs(Y) < 25 -->  inner Stt axial section;
	// 35 < abs(Y) < 40 -->  outer Stt axial section;


	// STT  al bordo verticale centrale;
	cout<<"\tBOUNDARY Straws : Central Vertical Boundary, axial STT straws Center positions :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		if(fabs(center.X())>3.)  continue;
		 if(stampa){
			cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
			<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr;
		 }
		if( center.X() < 0. ) {
		  if( fabs( center.Y() ) < 30. ) { code = 10;} else {code = 12;};
		} else {
		  if( fabs( center.Y() ) < 30. ) { code = 20;} else {code = 22;};
		}

			if(stampa) cout<<", SttCode = "<<code<<endl;

			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}


	}  // end for(i=1;i<=num;i++)


	// STT  al bordo esterno;
	if(stampa)cout<<"\n\tExternal Round Boundary || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;

		if( Rrr <RSTRAWDETECTORMAX-2*STRAWRADIUS)  continue;
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr;
		if( center.X() < 0. ) {
			code = 13;
			if(stampa)cout<<", SttCode = 13\n";
		} else {
			code = 23;
			if(stampa)cout<<", SttCode = 23\n";
		}

			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


	// boundary Inner delle Stt parallele a sinistra;


	//  apotema = DISTANZA DA (0,0) del lato dell'esagono;

	// Stt lato inner, del 2 quadrante; code 0 + n*10;
	apotema = RSTRAWDETECTORMIN;
	// equazione del bordo : x - sqrt(3)*y +2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary (smaller) with code 10; 2nd quadrant || STT straws :\n";
	double distanza;
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()>0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(sqrt(3.)*center.Y()-center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()< -apotema - 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 10\n";
		code = 10;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


	// Stt lato inner, lato verticale; code 0 + n*10;
	apotema = RSTRAWDETECTORMIN;
	// equazione del bordo :  x = -apotema ;
	if(stampa)cout<<"\n\tInner Vertical Boundary (smaller) with code 10; 2nd-3rd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		if(center.X()>0.)  continue;
		if(center.X()< -apotema - 2.*STRAWRADIUS)  continue;
		if(fabs(center.Y())> apotema/sqrt(3.))  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 10\n";
		code = 10;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)



	// Stt lato inner, del 3 quadrante; code 0 + n*10;
	apotema = RSTRAWDETECTORMIN;
	// equazione del bordo : x + sqrt(3)*y +2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary (smaller) with code 10; 3nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()>0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(-sqrt(3.)*center.Y()-center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()< -apotema - 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 10\n";
		code = 10;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)

	//  parte a destra dell'inner Stt boundary; considero la parte a sinistra e cambio il segno a tutte le X;

	// Stt lato inner, del 1 quadrante; code 0 + n*10;
	apotema = RSTRAWDETECTORMIN;
	// equazione del bordo : x +  sqrt(3)*y -2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary (smaller) with code 20; 1nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()<0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(sqrt(3.)*center.Y()+center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()> apotema + 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 20\n";
		code = 20;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


	// Stt lato inner, lato verticale + n*10;
	apotema = RSTRAWDETECTORMIN;
	// equazione del bordo :  x = apotema ;
	if(stampa)cout<<"\n\tInner Vertical Boundary (smaller) with code 20; 1st-2nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		if(center.X()<0.)  continue;
		if(center.X()> apotema + 2.*STRAWRADIUS)  continue;
		if(fabs(center.Y())> apotema/sqrt(3.))  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 20\n";
		code = 20;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)



	// Stt lato inner, del 4 quadrante + n*10;
	apotema = RSTRAWDETECTORMIN;
	// equazione del bordo : x -sqrt(3)*y -2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary (smaller) with code 20; 4nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()<0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(-sqrt(3.)*center.Y()+center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()> apotema + 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 20\n";
		code = 20;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


//--------------

// la rimanente parte Inner a destra : parte obliqua del 1 quadrante; code 1 + n*10;
	// Stt lato inner, del 1 quadrante;
	apotema = APOTEMAMAXINNERPARSTRAW;	// = 23.25 cm;
	// equazione del bordo : x +  sqrt(3)*y -2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary (the larger), the one with code 21; 1nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()<0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(sqrt(3.)*center.Y()+center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()> apotema + 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 21\n";
		code = 21;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


// la rimanente parte Inner a destra : parte verticale a cavallo tra il 1 ed il 4 quadrante; code 1 + n*10;
	// Stt lato inner, lato verticale;
	apotema = APOTEMAMAXINNERPARSTRAW;	// = 23.25 cm;
	// equazione del bordo :  x = apotema ;
	if(stampa)cout<<"\n\tInner Vertical Boundary (the larger) the one with code 21; 1st-4th quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		if(center.X()<0.)  continue;
		if( fabs(center.X() - apotema) >  2.*STRAWRADIUS )  continue;
		if(fabs(center.Y())> apotema/sqrt(3.))  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 21\n";
		code = 21;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


// la rimanente parte Inner a destra : parte obliqua del 4 quadrante; code 1 + n*10;
	// Stt lato inner, del 4 quadrante;
	apotema = APOTEMAMAXINNERPARSTRAW;	// = 23.25 cm;
	// equazione del bordo : x -sqrt(3)*y -2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary (the larger) the one with code 21; 4nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()<0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(-sqrt(3.)*center.Y()+center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()> apotema + 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 21\n";
		code = 21;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)

// la rimanente parte Inner a sinistra : parte obliqua del 2 quadrante; code 1 + n*10;
	// Stt lato inner (the larger) the one with code 11, del 2 quadrante;
	apotema = APOTEMAMAXINNERPARSTRAW;	// = 23.25 cm;
	// equazione del bordo : x - sqrt(3)*y +2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary (the larger) the one with code 11, 2nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()>0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(sqrt(3.)*center.Y()-center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()< -apotema - 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 11\n";
		code = 11;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


// la rimanente parte Inner a sinistra : parte verticale a cavallo tra il 2 e 3 quadrante; code 1 + n*10;
	// Stt lato inner, lato verticale (larger) the one with code 11;
	apotema = APOTEMAMAXINNERPARSTRAW;	// = 23.25 cm;
	// equazione del bordo :  x = -apotema ;
	if(stampa)cout<<"\n\tInner Vertical Boundary (the larger) the one with code 11; 2nd-3rd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		if(center.X()>0.)  continue;
		if( fabs( center.X() + apotema ) > 2.*STRAWRADIUS  )  continue;
		if(fabs(center.Y())> apotema/sqrt(3.))  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 11\n";
		code = 11;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)



// la rimanente parte Inner a sinistra : parte obliqua del 3 quadrante; code 1 + n*10;
	// Stt lato inner, del 3 quadrante (larger);
	apotema = APOTEMAMAXINNERPARSTRAW;	// = 23.25 cm;
	// equazione del bordo : x + sqrt(3)*y +2*apotema = 0 ;
	if(stampa)cout<<"\n\tInner Boundary 3nd quadrant (the larger) the one with code 11;  || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()>0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(-sqrt(3.)*center.Y()-center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()< -apotema - 2.*STRAWRADIUS)  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 11\n";
		code = 11;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)



//--------------------------------------------------------------------------------------------------------------

// la rimanente parte Outer a destra : parte obliqua del 1 quadrante; code 25 per un piccolo tratto che
// puo' essere investito da tracce che provengono da (0,0) senza aver intersecato prima gli Straws
// assiali, e code  24 per il resto;
	// Stt lato inner, del 1 quadrante;
	apotema = APOTEMAMINOUTERPARSTRAW;	// = 31.86 cm;
	// equazione del bordo : x +  sqrt(3)*y -2*apotema = 0 ;
	if(stampa)cout<<"\n\tOuter Boundary (the larger), the one with code 24 or 25; 1nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()<0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(sqrt(3.)*center.Y()+center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()> apotema + 2.*STRAWRADIUS)  continue;

		// condition to belong to the special section;
		if( center.X() - STRAWRADIUS < x ) { code = 25; } else { code = 24; }

		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = "<<code<<endl;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


// la rimanente parte Outer a destra : parte verticale a cavallo tra il 1 ed il 4 quadrante;
	// Stt lato Outer, lato verticale;
	apotema = APOTEMAMINOUTERPARSTRAW;	// = 31.86 cm;
	// equazione del bordo :  x = apotema ;
	if(stampa)cout<<"\n\tOuter Vertical Boundary (the larger) the one with code 24; 1st-4th quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		if(center.X()<0.)  continue;
		if(fabs( center.X()- apotema ) >  2.*STRAWRADIUS )  continue;
		if(fabs(center.Y())> apotema/sqrt(3.))  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 24\n";
		code = 24;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


// la rimanente parte Outer a destra : parte obliqua del 4 quadrante;
	// Stt lato Outer, del 4 quadrante;
	apotema = APOTEMAMINOUTERPARSTRAW;	// = 31.86 cm;
	// equazione del bordo : x -sqrt(3)*y -2*apotema = 0 ;
	if(stampa)cout<<"\n\tOuter Boundary (the larger) the one with code 24 or 25; 4nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()<0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(-sqrt(3.)*center.Y()+center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()> apotema + 2.*STRAWRADIUS)  continue;

		// condition to belong to the special section;
		if( center.X() - STRAWRADIUS < x ) { code = 25; } else { code = 24; }

		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = "<<code<<endl;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)

// la rimanente parte Outer a sinistra : parte obliqua del 2 quadrante;
	// Stt lato Outer (the larger) the one with code 12, del 2 quadrante;
	apotema = APOTEMAMINOUTERPARSTRAW;	// = 31.86 cm;
	// equazione del bordo : x - sqrt(3)*y +2*apotema = 0 ;
	if(stampa)cout<<"\n\tOuter Boundary (the larger) the one with code 14 or 15, 2nd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()>0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(sqrt(3.)*center.Y()-center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()< -apotema - 2.*STRAWRADIUS)  continue;

		// condition to belong to the special section;
		if( center.X() + STRAWRADIUS > -x ) { code = 15; } else { code = 14; }

		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = "<<code<<endl;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)


// la rimanente parte Outer a sinistra : parte verticale a cavallo tra il 2 e 3 quadrante;
	// Stt lato Outer, lato verticale (larger) the one with code 14;
	apotema = APOTEMAMINOUTERPARSTRAW;	// = 31.86 cm;
	// equazione del bordo :  x = -apotema ;
	if(stampa)cout<<"\n\tOuter Vertical Boundary (the larger) the one with code 12; 2nd-3rd quadrant || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		if(center.X()>0.)  continue;
		if(fabs(center.X()+apotema)  > 2.*STRAWRADIUS)  continue;
		if(fabs(center.Y())> apotema/sqrt(3.))  continue;
		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = 14\n";
		code = 14;
			if( StrawCode[i-1] > -1  && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)



// la rimanente parte Outer a sinistra : parte obliqua del 3 quadrante;
	// Stt lato Outer, del 3 quadrante (larger);
	apotema = APOTEMAMINOUTERPARSTRAW;	// = 31.86 cm;
	// equazione del bordo : x + sqrt(3)*y +2*apotema = 0 ;
	if(stampa)cout<<"\n\tOuter Boundary 3nd quadrant (the larger) the one with code 14 or 15;  || STT straws :\n";
	for(i=1;i<=num;i++){
		pSttTube = (PndSttTube*) SttTubeArray->At(i);
		center = pSttTube->GetPosition();
		wiredirection = pSttTube->GetWireDirection();
		// solo STT parallel;
		if(center.X()>0.)  continue;
		if(!(fabs( wiredirection.X() )< 0.00001 && fabs( wiredirection.Y() )< 0.00001))continue;
		distanza = fabs(-sqrt(3.)*center.Y()-center.X() -2*apotema)/2.;
		if(distanza>2.*STRAWRADIUS || center.X()< -apotema - 2.*STRAWRADIUS)  continue;

		// condition to belong to the special section;
		if( center.X() + STRAWRADIUS > -x ) { code = 15; } else { code = 14; }

		Rrr = sqrt( center.X()*center.X()+center.Y()*center.Y());
		if(stampa)cout<<"\tSTT straw || tubeID n.  "<<i<<", centro X "<< center.X()
		<<", centro Y "<< center.Y()<<", centro Z "<< center.Z()<<", fR = "<<Rrr
		<<", SttCode = "<<code<<endl;
			if( StrawCode[i-1] > -1 && StrawCode[i-1] != code) { // to this Tube a StrawCode was already previously assigned;
				// therefore set StrawCode2;
				StrawCode2[i-1] = code;
			} else {
				StrawCode[i-1] = code;
			}
	}  // end for(i=1;i<=num;i++)




	return;
}

//------------------ end function  PndTrkBoundaryParStraws::SttTubeList


ClassImp(PndTrkBoundaryParStraws);
