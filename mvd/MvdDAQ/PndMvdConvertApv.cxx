#include "PndMvdConvertApv.h"

#include "PndMvdApvHit.h"
#include "PndMvdDigiStrip.h"
#include "TString.h"
#include <fstream>
#include <vector>
#include <map>

using namespace std;

// -----   Constructor   --------------------------------------------

PndMvdConvertApv::PndMvdConvertApv(const TString& CalibFileName, const TString& HitFileName)
{
  fFake=false;
  cout<<"Scan HitFile..."<<endl;
  std::ifstream hitfile(HitFileName);
  std::vector<Int_t> fes;						// module detecor
  std::vector<Int_t> nEvents;
  if(!hitfile)
  {
    cout<<"Hitfile not found!"<<endl;
    return;
  }
  long int n=0;								// event counter
  long int old_event=-1;
  while (!hitfile.eof())  						// read data
  {
    char c;
    hitfile>>c;
    if (hitfile.eof()) break;
    if (!isdigit(c))    
    {
      char str[256];
      hitfile.getline(str,256);
      continue;
    }
    hitfile.putback(c);

    //int triggID,ts,frame,moduleID;
    int fe,ch,l;
    long int ev;
    double q;
        
    //    moduleID=0;
    hitfile >> ev >> fe >> ch >> q >> l;
    //cout<<"event "<<ev<<" fe:"<<fe<<" channel:"<<ch<<" adc:"<<q<<endl;

    Int_t found = 0;

    for(Int_t i=0;i<fes.size();++i)
      {
	if(fe==fes[i])found=1;
			 
      }


    if (!found){
      cout << "Found frontend ID:" << fe << endl;
        fes.push_back(fe);
    }

    if (ev!=old_event)
    {
      n++;								// count event
      nEvents.push_back(n);					// check module and may mind
      old_event=ev;
    }
  }
  hitfile.close();
  cout<<nEvents.size()<<" events in File"<<endl;
  cout<<" counted "<<n<<" events"<<endl;
  cout<<fes.size()<<" modules found   Read Calibration..."<<endl;
  LoadCalibration(CalibFileName, fes);
  fNofEvents=n;
  fHitFileName=HitFileName;
  fDataFile.open(HitFileName);
  fLastEvent=0;
  fEvent=-1;
  cout<<"** end of PndMvdConvertApv::PndMvdConvertApv(const TString& , const TString&) **"<<endl;
}

Bool_t PndMvdConvertApv::Init()
{
  fGeoH = new PndGeoHandling(gGeoManager);
  return kTRUE;
}


// -----   Load calibration for the Modules   --------------------------------------------

void PndMvdConvertApv::LoadCalibration(TString CalibFileName, std::vector<Int_t> fes)
{
  std::ifstream calibfile(CalibFileName);
  if(!calibfile)
  {
    cout<<"Calibration file not found"<<endl;
    fNoCalib=true;
    return;
  }
  while (!calibfile.eof())  						// read data
  {
    char c;
    calibfile>>c;
    if (calibfile.eof()) break;
    if (!isdigit(c))    
    {
      char str[256];
      calibfile.getline(str,256);
      continue;
    }
    calibfile.putback(c);

    int feID, channel;
    double value;

    calibfile >> feID >> feID >> channel >> value;

    for(int vec=0;vec<fes.size();vec++)
    {
      if(feID==fes[vec])
      {
        fCalibPars[feID][channel]=value;
      }
    }
  }
  calibfile.close();
  fNoCalib=false;
  cout<<"Calibration succesfully read"<<endl;
  return;
}

// -----   Convert adc to e if calibration was loaded  --------------------------------------------

std::vector<PndMvdDigiStrip> PndMvdConvertApv::Calc(std::vector<PndMvdApvHit> hitlist)
{
  std::vector<PndMvdDigiStrip> result;
  for(Int_t hitnumber=0;hitnumber<hitlist.size();hitnumber++)
  {
     Double_t q=0.;
     if(fNoCalib)
     {
       q=1.*hitlist[hitnumber].GetADC();					// no calib adc -> e !!!
     }else{
       if (fCalibPars[hitlist[hitnumber].GetFeID()].size())
       {
         if (fCalibPars[hitlist[hitnumber].GetFeID()][hitlist[hitnumber].GetChannel()])
         {
           q=fCalibPars[hitlist[hitnumber].GetFeID()][hitlist[hitnumber].GetChannel()]*(hitlist[hitnumber].GetADC())*1000.; // in electrons
         }
       }
     }

//FIXME: Welche DetId braucht das Framework? "2" fuer Strips?
//TODO: Detektornamen mit Geometrie sinnvoll verheiraten. 
// 	string detPath="SiliconTestStation_1/DummysensorAss_0/";
//     detPath+="Module";
    TString detPath="Module";
    Int_t modId=-1;
    if(fFake)
    {
      if(fTopModuleID==hitlist[hitnumber].GetModuleID() || fBottomModuleID==hitlist[hitnumber].GetModuleID())
        modId = 1;
      if(fBottomModuleID==hitlist[hitnumber].GetModuleID()) hitlist[hitnumber].SetFeID(hitlist[hitnumber].GetFeID()+3);
    }else{
      modId = hitlist[hitnumber].GetModuleID();
    }
    detPath+=modId;
    detPath+="Rect";
//     std::cout<<detPath.Data()<<"   |    "<<modId<<std::endl;
	//TGeoVolume* Vol=gGeoManager->FindVolumeFast(detPath);
    TGeoVolume* Vol=0;
    if(Vol!=0) {
// 		std::cout<<Vol->GetName()<<std::endl;
// 		Vol->GetNode(-1)->cd();
        detPath="/SiliconTestStation_1/DummysensorAss_0/";
		detPath+=Vol->GetName();
		detPath+="_0";
		gGeoManager->cd(detPath.Data());
	}
	else {
// 		std::cout<<" -E- PndMvdConvertApv::Calc(): "<<detPath.Data()<<" does not exist"<<std::endl;
        detPath+="_nonexistent";
	}
//     std::cout<<detPath.Data()<<std::endl;
//     std::cout<<gGeoManager->GetPath()<<std::endl;
//     if (0==Vol) std::cout<<"0"; 
//     else std::cout<<"1";
//     std::cout<<Vol->GetName();

//     detPath = Vol->GetName();
// 	std::cout << "write Digi with "<< detPath.Data()<<" ( "<<fGeoH->GetID(detPath)<<" )"<<std::endl;
    if(fFake)
    {
    PndMvdDigiStrip DigiHit(hitlist[hitnumber].GetEventID(), 
							 1,
                             /*fGeoH->GetID(detPath)*/"", 
                             hitlist[hitnumber].GetFeID(),
			    hitlist[hitnumber].GetChannel(), q/*/1000/1000*/,
							 hitlist[hitnumber].GetTimestamp()
						     );
     result.push_back(DigiHit);
    }else{
     PndMvdDigiStrip DigiHit(hitlist[hitnumber].GetEventID(), 
							 hitlist[hitnumber].GetModuleID(),
                             /*fGeoH->GetID(detPath)*/"", 
                             hitlist[hitnumber].GetFeID(),
                             hitlist[hitnumber].GetChannel(),
			     q/*/1000/1000*/,
                             hitlist[hitnumber].GetTimestamp() 
                             );
     result.push_back(DigiHit);
    }
  }
  return result;
}

// -----   Returns the number of events in the hit file   --------------------------------------------

long int PndMvdConvertApv::GetNofEvents()
{
  return fNofEvents;
}

// -----   read the next event from hitfile   --------------------------------------------

std::vector<PndMvdDigiStrip> PndMvdConvertApv::ReadNext()
{
//  cout<<"** PndMvdConvertApv::ReadNext() **"<<endl;
  std::vector<PndMvdDigiStrip> digiList;
  bool work=true;
  while (!fDataFile.eof() && work)  					// read data
  {
    char c;
    fDataFile>>c;
    if (fDataFile.eof()) break;
    if (!isdigit(c))    
    {
      char str[256];
      fDataFile.getline(str,256);
      continue;
    }
    fDataFile.putback(c);

    int triggID=0;
    int fe=0;
    int ts=0;
    int frame=0;
    int ch=0;
    int l=0;
    int moduleID=0;
    double q=0.;
    long int ev=0;

    fDataFile >> ev >> fe >>  ch >> q >> l;
    //cout<<"event "<<fEvent<<" event id:"<<ev<<" last event id:"<<fLastEvent<<" fe:"<<fe<<" channel:"<<ch<<" adc:"<<q<<endl;


    if (fEvent==-1) { fLastEvent=ev; fEvent=0; }
    if (ev!=fLastEvent)
    {
      if(fhitlist.size()>20) fhitlist.clear();
      digiList = Calc(fhitlist);
      fhitlist.clear();
      fLastEvent=ev;
      fEvent++;
      work=false;
    }
    PndMvdApvHit Hit(ev, moduleID, fe, triggID, ts, frame, ch, q, l);
    fhitlist.push_back(Hit);
  }
  if(!(fEvent%10000)) cout<<"[ "<<(fEvent*100)/fNofEvents<<" %] "<<fEvent<<" events converted ..."<<endl;
  if(fEvent==fNofEvents) cout<<"[100 %] "<<fEvent<<" events converted"<<endl;
  return digiList;
}

// -----   read all events from hitfile   --------------------------------------------

std::vector<PndMvdDigiStrip> PndMvdConvertApv::ReadAll()
{
  std::vector<PndMvdDigiStrip> result;
  while(fEvent!=fNofEvents)
  {
    std::vector<PndMvdDigiStrip> dummy=ReadNext();
    for(int i=0;i<dummy.size();++i) result.push_back(dummy[i]);
  }
  return result;
}

void PndMvdConvertApv::SetFakePair(Int_t TopModuleID, Int_t BottomModuleID)
{
  fFake=true;
  fTopModuleID=TopModuleID;
  fBottomModuleID=BottomModuleID;
  return;
}

// PndMvdDigiStrip PndMvdConvertApv::DigiHit(hitlist [])
// {
// }

