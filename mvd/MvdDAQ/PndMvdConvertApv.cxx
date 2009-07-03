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
  std::vector<Int_t> modules;						// module detec
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

    int triggID,fe,ts,frame,ch,l, moduleID;
    long int ev;
    double q;
        
    hitfile >> ev >> moduleID >> triggID >> frame >> fe >> ts >> ch >> q >> l;

    if (ev!=old_event)
    {
      n=ev;								// count event
      ModulChecker(moduleID, modules);					// check module and may mind
      old_event=ev;
    }
  }
  hitfile.close();
  cout<<n<<" events in File"<<endl;
  cout<<modules.size()<<" modules found   Read Calibration..."<<endl;
  LoadCalibration(CalibFileName, modules);
  fNofEvents=n;
  fHitFileName=HitFileName;
  fDataFile.open(HitFileName);
  fEvent=1;
}

Bool_t PndMvdConvertApv::Init()
{
  fGeoH = new PndMvdGeoHandling(gGeoManager);
  return kTRUE;
}


// -----   count new modules   --------------------------------------------

void PndMvdConvertApv::ModulChecker(Int_t moduleID, std::vector<Int_t>& modules)
{
  for(Int_t i=0;i<modules.size();++i)
  {
    if(moduleID==modules[i]) return;					// already known module
  }
  modules.push_back(moduleID);
  return;
}

// -----   Load calibration for the Modules   --------------------------------------------

void PndMvdConvertApv::LoadCalibration(TString CalibFileName, std::vector<Int_t> modules)
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

    int moduleID, feID, channel;
    double value;

    calibfile >> moduleID >> feID >> channel >> value;
    for(int vec=0;vec<modules.size();vec++)
    {
      if(moduleID==modules[vec])
      {
        fCalibPars[moduleID][feID][channel]=value;
      }
    }
  }
  calibfile.close();
  fNoCalib=false;
  cout<<"Calibration succesfull readed"<<endl;
  return;
}

// -----   Convert adc to e if calibration was loaded  --------------------------------------------

std::vector<PndMvdDigiStrip> PndMvdConvertApv::Calc(std::vector<PndMvdApvHit> hitlist)
{
  std::vector<PndMvdDigiStrip> result;
  TString detPath="";
  for(Int_t hitnumber=0;hitnumber<hitlist.size();hitnumber++)
  {
     Double_t q=0.;
     if(fNoCalib)
     {
       q=1.*hitlist[hitnumber].GetADC();					// no calib adc -> e !!!
     }else{
       if (fCalibPars[hitlist[hitnumber].GetModuleID()][hitlist[hitnumber].GetFeID()].size())
       {
         if (fCalibPars[hitlist[hitnumber].GetModuleID()][hitlist[hitnumber].GetFeID()][hitlist[hitnumber].GetChannel()])
         {
           q=fCalibPars[hitlist[hitnumber].GetModuleID()][hitlist[hitnumber].GetFeID()][hitlist[hitnumber].GetChannel()]*(hitlist[hitnumber].GetADC())*1000.; // in electrons
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
	TGeoVolume* Vol=gGeoManager->FindVolumeFast(detPath);
    if(0!=Vol) {
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
                             fGeoH->GetID(detPath), 
                             hitlist[hitnumber].GetFeID(),
                             hitlist[hitnumber].GetChannel(), 
							 hitlist[hitnumber].GetTimestamp(),
						     q/*/1000/1000*/);
     result.push_back(DigiHit);
    }else{
     PndMvdDigiStrip DigiHit(hitlist[hitnumber].GetEventID(), 
							 hitlist[hitnumber].GetModuleID(),
                             fGeoH->GetID(detPath), 
                             hitlist[hitnumber].GetFeID(),
                             hitlist[hitnumber].GetChannel(), 
                             hitlist[hitnumber].GetTimestamp(), 
                             q/*/1000/1000*/);
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

    int triggID,fe,ts,frame,ch,l,moduleID;
	double q;
    long int ev;

    fDataFile >> ev >> moduleID >> fe >> triggID >> ts >> frame >> ch >> q >> l;

    if ((ev-1)==fEvent)
    {
      if(fhitlist.size()>20) fhitlist.clear();
      digiList = Calc(fhitlist);
      fhitlist.clear();
      fEvent=ev;
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
