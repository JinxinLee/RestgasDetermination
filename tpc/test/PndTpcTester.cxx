//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcTester
//      see PndTpcTester.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcTester.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <string>

// Collaborating Class Headers --------

#include "TGraph.h"
#include "TFile.h"
#include "TRandom.h"
#include "TH1I.h"
#include "TGraph2D.h"
#include "DebugLogger.h"
#include "dbgstream.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcDigi.h"
#include "PndTpcCluster.h"
#include "PndTpcClusterCOG.h"
#include "McIdCollection.h"
#include "TCovEllipse.h"
//#include "BiHarmSpline.h"
#include "PndTpcDigitizationPolicy.h"
#include "PndTpcSignal.h"
#include "PndTpcSample.h"
#include "PndTpcFrontend.h"
#include "PndTpcAbsPulseshape.h"
#include "PndTpcCRRCPulseshape.h"
#include "TCanvas.h"

// Class Member definitions -----------

ClassImp(PndTpcTester)

void
PndTpcTester::testDebugStream(){
  //DebugLogger::Instance();

  //LOG<<"blablabla"<<std::endl;

  //odbgstream dbg(std::cout.rdbuf());
  std::ofstream outfile("log.log");
  DebugLogger::Instance()->addOutput(outfile);
  logrule rule1("PndTpcTester.cxx",0,64,1);
  logrule rule2("PndTpcTester.cxx",65,68,2);
  logrule rule3("PndTpcTester.cxx",69,69,1);

  DebugLogger::Instance()->addRule(&rule1);
  DebugLogger::Instance()->addRule(&rule2);
  DebugLogger::Instance()->addRule(&rule3);
  DebugLogger::Instance()->addRule("tpc/test/PndTpcTester.cxx","testDebugStreamFunctionBlock",1);
DebugLogger::Instance()->addRule("tpc/test/PndTpcTester.cxx","anyExpressionWhatSoEver",1);
  DebugLogger::Instance()->setLevel(1);
  
  LOG<<"Hi! this has to low level"<<std::endl;
  LOG<<level(1)<<"an error message to the screen" <<std::endl;

  LOG<<"an error msg to the file, level too low"<<std::endl;
  LOG<<level(1)<<"an error msg to the file level 1" <<std::endl;
  LOG<<level(2)<<"an error msg to the file level 2" <<std::endl;

  LOG<<level(1)<<" this should go to the screen"<<std::endl;
  LOG<<level(1)<<" this is outside range"<<std::endl;

  testDebugStreamFunctionBlock();

  outfile.close();

  bool anyExpressionWhatSoEver=true;
  if(anyExpressionWhatSoEver){
    if(4){
      LOG<<level(1)<<" this goes to the screen to show that we can address expressions"<<std::endl;
    }
  }

} // end testDebugStream()



void 
PndTpcTester::testDebugStreamFunctionBlock(){
  LOG<<level(10)<<" this should only appear when there is a rule to activate debug in testDebugStreamFunctionBlock()"<<std::endl;
}


TMatrixD
PndTpcTester::testCOG(){
  McIdCollection mc;
  McId id(0,0);
  mc.AddID(id);
  std::vector<PndTpcDigi*> digis;
  digis.push_back(new PndTpcDigi(200,2415,20,mc));
  digis.push_back(new PndTpcDigi(300,2415,21,mc));
  digis.push_back(new PndTpcDigi(220,2416,36,mc));
  digis.push_back(new PndTpcDigi(280,2417,37,mc));
  digis.push_back(new PndTpcDigi(90,2416,35,mc));
  
  PndTpcClusterCOG myCOG;

  PndTpcCluster* cl=myCOG.cog(&digis);

  cl->pos().Print();
  cl->cov().Print();

  return cl->cov();

}

void
PndTpcTester::testSpline(){
  /*  std::vector<TVector2*> v;
  std::vector<double> w;

  TVector2* a=new TVector2(1,1);v.push_back(a);
  TVector2* b=new TVector2(1,2);v.push_back(b);
  TVector2* c=new TVector2(2,1);v.push_back(c);
  
  w.push_back(1);
  w.push_back(4);
  w.push_back(-2);

  BiHarmSpline spline;

  spline.setCenters(v);
  spline.setWeights(w);

  double x=0;
  double y=0;
  double d=0.4;
  TGraph2D* devmap=new TGraph2D(100);

  for(int i=0;i<10;++i){
    y=0;
    x+=d;
    for(int j=0;j<10;++j){
      y+=d;
      TVector2 p(x,y);
      devmap->SetPoint(j+i*10,x,y,spline.eval(p));
      std::cout<<spline.eval(p)<<std::endl;
    }
  }

  devmap->Draw("SURF");
  */
}


void
PndTpcTester::SetUpTestSampling()
{
	pSignal1=new PndTpcSignal;
	pSignal1->sett(20);
	pSignal1->setamp(1000);
	pSignal1->setpadId(100);
	pSignal1->setmcTrackId(0);
	pSignal1->setmcHitId(2);
	pSignal1->setmcEventId(1);
	
     pSignal2=new PndTpcSignal;
	pSignal2->sett(100);
	pSignal2->setamp(1000);
	pSignal2->setpadId(100);
	pSignal2->setmcTrackId(0);
	pSignal2->setmcHitId(6);
	pSignal2->setmcEventId(1);	
	
     pSignal3=new PndTpcSignal;
	pSignal3->sett(120);
	pSignal3->setamp(5000);
	pSignal3->setpadId(100);
	pSignal3->setmcTrackId(0);
	pSignal3->setmcHitId(6);
	pSignal3->setmcEventId(1);		
	
	const double tint=50.;
	const double tdiff=50.;
	const double tsig=2.;
	const double adcThreshold=10.;
	const double adcmax=100000;
	const int adcbits=12;
	const double samplingfreq_MHz=40.;
	const double t0=0;
	const int timebits=28;
	const unsigned int PSAthreshold=50;
	
	pFrontend=new PndTpcFrontend(tint, tdiff, tsig, adcThreshold, adcmax, adcbits, samplingfreq_MHz, t0, timebits, PSAthreshold);
						
	pPulseshape=new PndTpcCRRCPulseshape(tint, tdiff, tsig);
}

void PndTpcTester::SetUpTestSampling(unsigned int adcthreshold, double amp1, double amp2, double t1, double t2 )
{
	SetUpTestSampling();
	pFrontend->setAdcThreshold(adcthreshold);
	pSignal1->sett(t1);
	pSignal2->sett(t2);
	pSignal1->setamp(amp1);
	pSignal2->setamp(amp2);
}

void
PndTpcTester::testSampling(std::vector<PndTpcSignal*> *TestSignals, TGraph *pGraph)
{	
	using std::vector;
	using std::cout;
	using std::endl;
	
	vector< PndTpcSample* > Samples;
	
	PndTpcDigitizationPolicy testSampler;
	testSampler.Digitize(TestSignals,&Samples,pFrontend,pPulseshape);
		
	vector< PndTpcSample* >::const_iterator cit;
	int i=0;
	
	if(!pGraph || pGraph->GetMaxSize()<Samples.size())	{
		cout << "Graph Error" << endl;
	}
	for(cit=Samples.begin(); cit!=Samples.end(); cit++)
	{
		cout << *(*cit) << endl;
		cout.flush();
		if(pGraph)	{
			cout << "setting: " << i << " (" << (*cit)->t() << ", " << (*cit)->amp() << ")" << endl;
			cout.flush();
			pGraph->SetPoint(i,(*cit)->t(),(*cit)->amp());
		}
		i++;
	}

}

void PndTpcTester::testSampling_Signal1(TGraph *pGraph)
{
	std::vector< PndTpcSignal* > TestSignals;
	TestSignals.push_back(pSignal1);
	testSampling(&TestSignals, pGraph);
}

void PndTpcTester::testSampling_Signal2(TGraph *pGraph)
{
	std::vector< PndTpcSignal* > TestSignals;
	TestSignals.push_back(pSignal2);
	testSampling(&TestSignals, pGraph);
}

void PndTpcTester::testSampling_Signal12(TGraph *pGraph)
{
	std::vector< PndTpcSignal* > TestSignals;
	TestSignals.push_back(pSignal1);	
	TestSignals.push_back(pSignal2);
	testSampling(&TestSignals, pGraph);
}

void PndTpcTester::testSampling_Signal123(TGraph *pGraph)
{
	std::vector< PndTpcSignal* > TestSignals;
	TestSignals.push_back(pSignal1);	
	TestSignals.push_back(pSignal2);
	TestSignals.push_back(pSignal3);
	testSampling(&TestSignals, pGraph);
}

void
PndTpcTester::testMcId(){

  McId a(1,1,1);
  McId b(1,1,2);
  McId c(1,2,1);
  McId d(1,3,1);

  McIdCollection col;
  col.AddID(a);
  std::cout<<col<<std::endl;
  col.AddID(b);
  std::cout<<col<<std::endl;
  col.AddID(c);
  std::cout<<col<<std::endl;
  col.AddID(a,10);
  std::cout<<col<<std::endl;
  col.AddID(d,20);
  std::cout<<col<<std::endl;

  // here we have (1,1,13)(1,2,1)(1,3,20)

  McIdCollection col2;
  col2.AddID(a,3);
  col2.AddID(c,7);

  col.AddIDCollection(col2,5);
 std::cout<<col<<std::endl;

 std::cout<<"here we should have (1,1,28)(1,2,36)(1,3,20)"<<std::endl;

}
