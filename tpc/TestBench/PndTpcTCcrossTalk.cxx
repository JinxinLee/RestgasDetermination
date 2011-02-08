//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     Crosstalk correction for the t2k data of the test chamber TPC
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------


// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcTCcrossTalk.h"

// C/C++ Headers ----------------------
#include <vector>
#include <map>
#include <algorithm>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "PndTpcSample.h"
#include "PndTpcSampleAmp.h"

#include "TCmappingManager.h"

#include "McIdCollection.h"

#include "QAPlotCollection.h"
#include <TH1D.h>

#include <cmath>

 
using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcTCcrossTalk)


  PndTpcTCcrossTalk::PndTpcTCcrossTalk(): fqa(NULL)
{

  //  A[0]=0.0; 
  // A[1]=0.0;
  // A[2]=0.0;
  // A[3]=0.0;

  for (int i = 0; i< 10;i++)
    {AX[i]=0;A[i]=0.4;}

 
  //TC MAPPING MANAGER
  MappingManager::Instance().mapSystem();
  if(!MappingManager::Instance().good()){
    cout <<"PndTpcTCcrossTalk::MappingManager :" << MappingManager::Instance() << std::endl;
  }

  
}

PndTpcTCcrossTalk::~PndTpcTCcrossTalk()
{
}

void PndTpcTCcrossTalk::Init()
{
  //Create Histograms for recording spread of Avalanche-centre to hit Pads
  if(fqa!=NULL){
    for (int h=0;h<10;h++)
      {char buf [20];
      sprintf(buf,"CrossTalk at dist %d",h);
      cHist[h] = fqa->getTH1D(buf,buf,100,0,1);
      }
  }
  
    else std::cout << "PndTpcTCcrossTalk::Init() QAcollection null, can't save histograms"
		   << std::endl;

}


void PndTpcTCcrossTalk::CalcAmp(std::vector<PndTpcSample*>* _sa)
{
  if(!_sa->size()) 
    {
      std::cout<< "PndTpcTCcrossTalk::CalcAmp called with empty Sample vector"<<std::endl;
      return;
    }
  //reset amplitudes
  for (int i=0;i<10;i++) A[i]=0.0;

  sort(_sa->begin(),_sa->end(),PndTpcSampleAmp());//sorting by amplitude, last has the largest amp
  
  int le = _sa->size()-1;//index of amp max
  double maxamp=_sa->at(le)->amp(); 
  int pinref = MappingManager::Instance().fFECmap[
						  MappingManager::Instance().fpadId2FEC[
											_sa->at(le)->padId()]];
  int tref = _sa->at(le)->t();
  
	      std::cout << " PINREF " << pinref
	    	    << " PADREF " << _sa->at(0)->padId()
	    	    << " TIMEREF "<<tref
	    	    << " AMPREF " <<tref 
	    	    << " CHIP " << _sa->at(le)->chipId()
			<< " CHIP2 "<<
	        MappingManager::Instance().fpadId2FEC[_sa->at(le)->padId()].chipId 
	    	    << " CHANNEL "
		      <<   MappingManager::Instance().fpadId2FEC[_sa->at(le)->padId()].channelId
		      <<   std::endl;
  int ADCID = _sa->at(le)->adcId();
  int CHIPID= MappingManager::Instance().fpadId2FEC[_sa->at(le)->padId()].chipId;
  for (int i=le-1;i>=0;i--)
    //  {std::cout << "i "<< i << "chipid"<<MappingManager::Instance().fpadId2FEC[_sa->at(i)->padId()].chipId;
    if (MappingManager::Instance().fpadId2FEC[_sa->at(i)->padId()].chipId ==CHIPID)

	//&&_sa->at(i)->adcId()==ADCID)//same chip
      if (_sa->at(i)->t()==tref)//same time
	{

	  PndTpcSample * sa = new PndTpcSample(*_sa->at(i));
	  int padId= sa->padId();
	  int pin = MappingManager::Instance().fFECmap[
						     MappingManager::Instance().fpadId2FEC[padId]];

	  int a = (int)fabs(pin-pinref);
	  if(a<10)//1<=a<=4
	    {
	      int dist = a;//a/2-1;
	      
	      A[dist]=sa->amp()/maxamp;
	  
	      	    std::cout << " PIN " << pin
	      		      << " PAD " << padId
	      		      << " TIME "<<sa->t()
	      		      << " AMPF " <<sa->amp()/maxamp
	      		      << " CHIP " 
	      		      <<   MappingManager::Instance().fpadId2FEC[padId].chipId 
	      		      << " CHANNEL " 
	      		      <<   MappingManager::Instance().fpadId2FEC[padId].channelId
	      		      << " DIST " << a/2
	      		      <<   std::endl;    
	    }
	  delete sa;
	}
  // }
}
  
void PndTpcTCcrossTalk::Fill()
{
  std :: cout << "FQA "<<fqa << std::endl;
  for(int h=0;h<10;h++)
    if (A[h]) 
      cHist[h]->Fill(A[h]);
}

void PndTpcTCcrossTalk::Print()
{
  double AD[10];
  
    for (int i=0;i<10;i++)
      if(AX[i])//if calcultation, normalized
	AD[i]=A[i]/AX[i];
      else
	AD[i]=A[i];
  std::cout << "PndTpcTCcrossTalk CrosstalkAmplitude : "<< std::endl;
  for (int i=0;i<10;i++)
    {
      std::cout << "A[" << i <<"]= " <<AD[i]<< std::endl;
    }
}
 

//EXEC
void  PndTpcTCcrossTalk::ProcessCut(std::vector<PndTpcSample*>* _sa)
{
  std::cout << "PndTpcTCcrossTalk::ProcessCut" << std::endl;

  if(!_sa->size()) 
    {
      std::cout<< "PndTpcTCcrossTalk::ProcessCut called with empty Sample vector"<<std::endl;
      return ;
    }
  
  sort(_sa->begin(),_sa->end(),PndTpcSampleAmp());//sorting by amplitude
  
  int I = _sa->size()-1;
  for (int i=I;i>=0;i--)
    { 
      int pinref = MappingManager::Instance().fFECmap[
				 MappingManager::Instance().fpadId2FEC[
					 _sa->at(i)->padId()]];

      
      int maxamp = _sa->at(i)->amp();
      int adcid  =  _sa->at(i)->adcId();
      int chipid =  _sa->at(i)->chipId();
      int tref = _sa->at(i)->t();
  
  
      for (int j=0;j<i;j++)
	if(_sa->at(j)->t()==tref &&//if same time/adc/chip
	   _sa->at(j)->chipId()==chipid && 
	   _sa->at(j)->adcId()==adcid )
	    {
	    int pin = MappingManager::Instance().fFECmap[
		        MappingManager::Instance().fpadId2FEC[_sa->at(j)->padId()]];
	    int dist = abs(pin-pinref);
	  	    if(dist<10)//neighboring on the card
	      _sa->at(j)->setAmp((int)_sa->at(j)->amp()-A[dist]*maxamp);
	    }
    } 
  return;
}

void  PndTpcTCcrossTalk::Process(std::vector<PndTpcSample*>* _sa)
{
  std::cout << "PndTpcTCcrossTalk::Process" << std::endl;

  if(!_sa->size()) 
    {
      std::cout<< "PndTpcTCcrossTalk::Process called with empty Sample vector"<<std::endl;
      return ;
    }
  
  sort(_sa->begin(),_sa->end(),PndTpcSampleAmp());//sorting by amplitude


  int I = _sa->size()-1;
  for (int i=I;i>=0;i--)
    { 
      int pinref = MappingManager::Instance().fFECmap[
				 MappingManager::Instance().fpadId2FEC[
					 _sa->at(i)->padId()]];

      
      int maxamp = _sa->at(i)->amp();
      int adcid  =  _sa->at(i)->adcId();
      int chipid =  _sa->at(i)->chipId();
      int tref = _sa->at(i)->t();
  
  
      for (int j=0;j<i;j++)
        if(!_sa->at(j)->ct())//if not tagged
	   if(_sa->at(j)->t()==tref &&//if same time/adc/chip
	     _sa->at(j)->chipId()==chipid && 
	     _sa->at(j)->adcId()==adcid )
	    {
	    int pin = MappingManager::Instance().fFECmap[
		        MappingManager::Instance().fpadId2FEC[_sa->at(j)->padId()]];
	    int dist = abs(pin-pinref);
	    	    if(dist<10)//neighboring on the card
	      if(_sa->at(j)->amp()<A[dist]*maxamp)//if under the cut
		_sa->at(j)->setCt();//tagging the sample
	    }

    }
  
}
