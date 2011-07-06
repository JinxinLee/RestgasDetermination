
#include <map>
#include <vector>
#include <TString.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH3.h>
#include <TH1.h>

#include "../../tpc/PndTpcAlignmentManager.h"
// #include <FairGeoLoader.h>
// #include <FairGeoInterface.h>
// #include <FairGeoMedia.h>
// #include <FairGeoBuilder.h>
// #include <TGeoManager.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <iostream>
// #include <TGeoMatrix.h> 

using namespace std;

Int_t volnum;
volnum=1;
gStyle->SetPalette(1);
// {{{ Main Fieldcage Config

Int_t kNofLayersFc;
Double_t kFcrad;
Double_t kFclen;
Double_t kFcrin, kFcrout;

TString * kLayerNameFc;

Double_t * kLayerThickFc;

Double_t * kZOffsetFc;
Double_t krin, krout;
// }}}
// {{{ Outer Fieldcage Config
const Int_t kNofLayersOfc = 12;
const Double_t kOfcrad = 15.4;
Double_t kOfclen = 72.78;
Double_t kOfcRad, kOfcThick;

TString kLayerNameOfc[kNofLayersOfc] = {"StInner_copper",
					"StMiddle_kapton",
					"StOuter_copper",
					"OFcLast_kapton",
					"OFcFourth_kapton",
					"OFcSecond_rohacell",
					"OFcMiddle_kapton",
					"OFcFirst_glue",
					"OFcFirst_rohacell",
					"OFcSecond_kapton",
					"OFcFirst_kapton",
					"OFcFirst_aluminium"};

Double_t kLayerThickOfc[kNofLayersOfc] = { 5e-4,   //    1 copper
					   25e-4,  // +1=2 kapton
					   5e-4,   // +1=3 copper
					   25.0e-4 ,    //    1 kapton
					   125.0e-4,    // +1=2 kapton
					   0.2,         // +1=3 rohacell
					   125.0e-4,    // +1=4 Kapton
					   1.4e-2,      // +1=5 glue
					   0.2,         // +1=6 rohacell
					   125.0e-4,    // +1=7 Kapton
					   25.0e-4,     // +1=8 Kapton
		     			   200.e-8 };   // +1=9 Aluminium

Double_t kZOffsetOfc[kNofLayersOfc] = { 0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.}; // for a more detailed model, for now put 0.
// }}}
// {{{ Inner Fieldcage Config

const Int_t kNofLayersIfc = 12;
const Double_t kIfcrad = 5.2;
const Double_t kIfclen = 80.5;
Double_t kIfcrin, kIfcrout;
Double_t kIfcRad, kIfcThick;

TString kLayerNameIfc[kNofLayersIfc] = {"IFcFirst_aluminium",
					"IFcFirst_kapton",
					"IFcSecond_kapton",
					"IFcFirst_rohacell",
					"IFcFirst_glue",
					"IFcMiddle_kapton",
					"IFcSecond_rohacell",
					"IFcFourth_kapton",
					"IFcLast_kapton",
					"StInner_copper",
					"StMiddle_kapton",
					"StOuter_copper"};


Double_t kLayerThickIfc[kNofLayersIfc] = { 200.e-8,     //    1 Aluminium
					   25.0e-4,     // +1=2 Kapton
					   125.0e-4,    // +1=3 Kapton
					   0.2,         // +1=4 rohacell
					   1.4e-2,      // +1=5 Glue
					   125.0e-4,    // +1=6 Kapton
					   0.2,         // +1=7 Rohacell
					   125.0e-4,    // +1=8 Kapton
					   25.0e-4,   // +1=9 Kapton
					   5e-4,   //    1 copper
					   25e-4,   // +1=2 kapton
					   5e-4}; // +1=3 copper

Double_t kZOffsetIfc[kNofLayersIfc] = { 0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.}; // for a more detailed model, for now put 0.

// }}}
// {{{ Main Stripfoil Config
Int_t kNofLayersS;
TString * kLayerNameS;
Double_t * kLayerThickS;
Double_t * kLayerZ;
const Double_t kStripPitch = 0.15;		      
// }}}		      
// {{{ Outer Stripfoil Config
const Int_t kNofLayersOs = 3;
Double_t kOsRad, kOsThick;
const TString kLayerNameOs[kNofLayersOs] = {"StInner_copper",
					    "StMiddle_kapton",
					    "StOuter_copper"};

const Double_t kLayerThickOs[kNofLayersOs]= {5e-4,   //    1 copper
					     25e-4,  // +1=2 kapton
					     5e-4}; // +1=3 copper

const Double_t kLayerZOs[kNofLayersOs] = {0.05,kOfclen/2,0.05};


// }}}		      
// {{{ Inner Stripfoil Config

const Int_t kNofLayersIs = 3;
Double_t kIsRad, kIsThick;
TString kLayerNameIs[kNofLayersIs] = {"StInner_copper",
				      "StMiddle_kapton",
				      "StOuter_copper"};

const Double_t kLayerThickIs[kNofLayersIs]= {5e-4,   //    1 copper
					     25e-4,   // +1=2 kapton
					     5e-4}; // +1=3 copper

const Double_t kLayerZIs[kNofLayersIs] = {0.05,kOfclen/2,0.05};

// }}}		      
// {{{ DriftCathode Config
const Int_t kNofLayersDc = 14;
Double_t kDcRad;
const TString kLayerNameDc[kNofLayersDc] = {"DcFirst_aluminium",
					    "DcFirst_kapton",
					    "DcSecond_kapton",
					    "DcThird_kapton",
					    "DcFirst_rohacell",
					    "DcFirstMiddle_kapton",
					    "DcSecond_rohacell",
					    "DCFirst_glue",
					    "DcSecondMiddle_kapton",
					    "DcThird_rohacell",
					    "DcFourth_kapton",
					    "DcFifth_kapton",
					    "DcSixth_kapton",
					    "DcLast_aluminium"
					    
};

const Double_t kLayerThickDc[kNofLayersDc] = { 200e-8, // 1 Aluminium 
					       25e-4,  // 2 kapton
					       25e-4,  // 3 kapton
					       125e-4, // 4 kapton
					       0.2,    // 5 rohacell
					       125e-4, // 6 kapton
 					       0.2,    // 7 rohacell
					       1.8e-2, // 8 glue
					       125e-4, // 9 kapton
					       0.2,    //10 rohacell
					       125e-4, //11 kapton
					       25e-4,  //12 kapton
					       25e-4,  //13 kapton
					       200e-8  //14 Aluminium
};
// }}}
// {{{ ReadoutFlange Config
const Int_t kNofLayersMgf = 4;
const Double_t kZoffPadplaneGem = 0.4;
const Double_t kRinMgf[kNofLayersMgf] = {15.4, // media flange
					 16.3, // gem flange
					 5.45, // padplane copper
					 5.45};// padplane
 
const Double_t KRoutMgf[kNofLayersMgf] = {21, // media flange  
					  18, // gem flange     
					  15.1, // padplane copper
					  18};// padplane         

const TString kLayerNameMgf[kNofLayersMgf] = {"MgfMedia_G10",
					      "MgfGem_G10",
					      "MgfPads_copper",
					      "MgfPadplane_G10"};

const Double_t kLayerThickMgf[kNofLayersMgf] = { 5,   // media flange  
						 1,  // gem flange  
						 35e-4,   // padplane copper
						 0.4 }; // padplane       
// }}}
// {{{ GemFoil Config
const Int_t kNofLayersGf = 7;
const Double_t kRinGf[kNofLayersGf] = {15.1, //upper outer g10ring
				       5.25, //upper inner g10 ring
				       5.45, //upper copper
				       5.25, //kapton
				       5.45, //lower copper
				       5.25, //lower inner g10 ring
				       15.1};//lower outer g10 ring

const Double_t KRoutGf[kNofLayersGf] = {16.3, //upper outer g10 ring
					5.45, //upper inner g10 ring
					16.3, //upper copper	  
					16.3, //kapton		  
					16.3, //lower copper	  
					5.45, //lower inner g10 ring
					16.3};//lower outer g10 ring

const TString kLayerNameGf[kNofLayersGf] = {"GfUpperOuter_G10",
					    "GfUpperInner_G10",
					    "GfUpper_copper",
					    "GfMiddle_kapton",
					    "GfLower_copper",
					    "GfLowerInner_G10",
					    "GfLowerOuter_G10"};

const Double_t kLayerThickGf[kNofLayersGf] = {0.1, //upper outer g10 ring
					      0.1, //upper inner g10 ring
					      4e-4, //upper copper	  
					      50e-4, //kapton		  
					      4e-4, //lower copper	  
					      0.1, //lower inner g10 ring
					      0.1};//lower outer g10 ring
// }}}

// {{{ CreateOuterFieldCage

TGeoVolumeAssembly * CreateOuterFieldCage(TGeoManager* _gGeoMan)
{
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersFc ; ilayer++ )
  //   {
  //     cout << kLayerNameFc[ilayer].Data() << " -> " << kLayerThickFc[ilayer] << endl;
  //   }

  //  vector <*TGeoShape> OfShapes;

  cout<<"outer fieldcage"<<endl;
  TGeoVolumeAssembly * FieldCage = new TGeoVolumeAssembly("Outer_Field_Cage");
  TGeoShape  * OfShapes[kNofLayersOfc];
  TGeoVolume * OfVolumes[kNofLayersOfc];

  Double_t rin  = kFcrad;
  Double_t rout;
  Double_t z = kFclen;
  Double_t totZ=0;
  Double_t radlen, thick;

  TString shapename, layerMaterial;

  thick=0;
  for ( Int_t ilay  =0 ; ilay < kNofLayersFc ; ilay++)
    {
    totZ=totZ + kZOffsetFc[ilay]; 
    rin-=kLayerThickFc[ilay];
    thick+=kLayerThickFc[ilay];
    }
  kFcrin=rin;
  //after this rin is the most inner radius

  TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,-totZ/2);
  z = (z - totZ)/2;

  for ( Int_t icage = 0 ; icage < kNofLayersFc ; icage++ )
    {
      rout = rin + kLayerThickFc[icage];

      shapename=Form("%s_shape",kLayerNameFc[icage]);
      OfShapes[icage] = new TGeoTube(shapename,rin,rout,z);
      
      layerMaterial = kLayerNameFc[icage].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      OfVolumes[icage] =  new TGeoVolume(kLayerNameFc[icage],OfShapes[icage],_gGeoMan->GetMedium(layerMaterial));
  
      FieldCage->AddNode(OfVolumes[icage],volnum++,(TGeoTranslation*)trans1->Clone());

      trans1->SetDz(trans1->GetTranslation()[2]+kZOffsetFc[icage]/2);
      z=z+(kZOffsetFc[icage]/2);
      rin  = rout;
    }
  kFcrout=rin;
  // now rin ist the most outer radius

  SetColors(OfVolumes,kNofLayersFc);

  radlen=GetRadLen(OfVolumes,kLayerThickFc,kNofLayersFc);
 

  if( &kLayerNameFc[0] == &kLayerNameOfc[0]){
    kOfcRad=radlen;
    kOfcThick=thick;
  }
  else if( &kLayerNameFc[0] == &kLayerNameIfc[0]){
    kIfcRad=radlen;
    kIfcThick=thick;
  }

  //  FieldCage->CheckGeometry();

   return FieldCage;
}

// }}}
// {{{ CreateInnerFieldCage

TGeoVolumeAssembly * CreateInnerFieldCage(TGeoManager* _gGeoMan)
{
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersFc ; ilayer++ )
  //   {
  //     cout << kLayerNameFc[ilayer].Data() << " -> " << kLayerThickFc[ilayer] << endl;
  //   }

  //  vector <*TGeoShape> Shapes;

  TGeoShape  * Shapes;
  TGeoVolume * Volumes;
  
  if( &kLayerNameFc[0] == &kLayerNameOfc[0])
    {
      cout<<"outer fieldcage"<<endl;
      TGeoVolumeAssembly * FieldCage = new TGeoVolumeAssembly("Outer_Field_Cage");
      TGeoShape  * OfShapes[kNofLayersOfc];
      TGeoVolume * OfVolumes[kNofLayersOfc];
    }
  else if ( &kLayerNameFc[0] == & kLayerNameIfc[0])
    {
      cout<<"inner fieldcage"<<endl;
      TGeoVolumeAssembly * FieldCage = new TGeoVolumeAssembly("Inner_Field_Cage");
      TGeoShape  * IfShapes[kNofLayersIfc];
      TGeoVolume * IfVolumes[kNofLayersIfc];
    }

  Double_t rin  = kFcrad;
  Double_t rout;
  Double_t z = kFclen;
  Double_t totZ=0;
  Double_t radlen, thick;

  TString shapename, layerMaterial;

  thick=0;
  for ( Int_t ilay  =0 ; ilay < kNofLayersFc ; ilay++)
    {
    totZ=totZ + kZOffsetFc[ilay]; 
    rin-=kLayerThickFc[ilay];
    thick+=kLayerThickFc[ilay];
    }
  kFcrin=rin;
  //after this rin is the most inner radius

  TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,-totZ/2);
  z = (z - totZ)/2;

  for ( Int_t icage = 0 ; icage < kNofLayersFc ; icage++ )
    {
      rout = rin + kLayerThickFc[icage];

      shapename=Form("%s_shape",kLayerNameFc[icage]);
      IfShapes[icage] = new TGeoTube(shapename,rin,rout,z);
      
      layerMaterial = kLayerNameFc[icage].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      IfVolumes[icage] =  new TGeoVolume(kLayerNameFc[icage],IfShapes[icage],_gGeoMan->GetMedium(layerMaterial));
  
      FieldCage->AddNode(IfVolumes[icage],volnum++,(TGeoTranslation*)trans1->Clone());

      trans1->SetDz(trans1->GetTranslation()[2]+kZOffsetFc[icage]/2);
      z=z+(kZOffsetFc[icage]/2);
      rin  = rout;
    }
  kFcrout=rin;
  // now rin ist the most outer radius

  SetColors(IfVolumes,kNofLayersFc);

  radlen=GetRadLen(IfVolumes,kLayerThickFc,kNofLayersFc);
 

  if( &kLayerNameFc[0] == &kLayerNameOfc[0]){
    kOfcRad=radlen;
    kOfcThick=thick;
  }
  else if( &kLayerNameFc[0] == &kLayerNameIfc[0]){
    kIfcRad=radlen;
    kIfcThick=thick;
  }

   return FieldCage;
}

// }}}
// {{{ CreateOuterStripFoil
TGeoVolumeAssembly * CreateStripFoil(TGeoManager* _gGeoMan)
{

  Double_t thick, radlen;
  Int_t kNofStrips  = (Int_t)(kOfclen/kStripPitch);
  //  cout<<"Number of strips:"<<kNofStrips<<endl;

  //  cout<<"Layers for the Strip Foil"<<endl;
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersS ; ilayer++ )
  //   {
  //     cout << kLayerNameS[ilayer].Data() << " -> " << kLayerThickS[ilayer] << endl;
  //   }

  if( &kLayerNameS[0] == &kLayerNameOs[0])
    {
      TGeoVolumeAssembly * StripFoil = new TGeoVolumeAssembly("Outer_Strip_Foil");
      TGeoShape  * ShapesS[kNofLayersOs];
      TGeoVolume * VolumesS[kNofLayersOs];
    }
  else if ( &kLayerNameS[0] == & kLayerNameIs[0])
    {
      TGeoVolumeAssembly * StripFoil = new TGeoVolumeAssembly("Inner_Strip_Foil");
      TGeoShape  * ShapesS[kNofLayersIs];
      TGeoVolume * VolumesS[kNofLayersIs];
    }

  // TGeoShape  *ShapesS [kNofLayersS];
  // TGeoVolume *VolumesS[kNofLayersS];
  TString shapename, layerMaterial;
  Double_t rin,rout,dtot=0,dofctot=0;
  TGeoTranslation * t1 =new TGeoTranslation(0.,0.,kFclen/2);
  Int_t kStrFoCount = 0;



  for ( Int_t ilay = 0 ; ilay < kNofLayersS ; ilay++ )
    {
      dtot = dtot + kLayerThickS[ilay];
    }
 
  for ( Int_t ilay = 0 ; ilay < kNofLayersOfc ; ilay++ )
    dofctot += kLayerThickOfc[ilay];
  

  if( &kLayerNameS[0] == &kLayerNameOs[0])
    rin=kFcrad-dtot-dofctot;
  else
    rin=kFcrad-dtot;
 
  //generate all necessary volumes
  for ( Int_t ilay = 0; ilay < kNofLayersS ; ilay ++ )
    {
      rout=rin+kLayerThickS[ilay];
      shapename = Form("%s_shape",kLayerNameS[ilay]);
      ShapesS[ilay] = new TGeoTube(shapename,rin,rout,kLayerZ[ilay]);
      
      layerMaterial = kLayerNameS[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      
      VolumesS[ilay] =  new TGeoVolume(kLayerNameS[ilay],ShapesS[ilay],_gGeoMan->GetMedium(layerMaterial));
      rin=rout;
    }

  SetColors(VolumesS,kNofLayersS);

  //if ( &kLayerNameS[0] == &kLayerNameIs[0]) t1->SetDz(kIfclen-kOfclen);

  //generate stripfoil strips  
  kStrFoCount=1;
  for(Int_t istrip = 0 ; istrip < kNofStrips ; istrip++)
    {
      for ( Int_t istrlay = 0 ; istrlay < kNofLayersS ; istrlay+=kNofLayersS-1)
	{
	  StripFoil->AddNode((TGeoVolume*)VolumesS[istrlay],volnum++,(TGeoTranslation*)t1->Clone());
	  //	  cout<<(TGeoVolume*)VolumesS[istrlay]->Clone()->GetMaterial()->GetName()<<endl;
	  kStrFoCount++;
	  t1->SetDz(t1->GetTranslation()[2]-kStripPitch/2);
	}
    }
  
  //Add kapton part of stripfoil
  StripFoil->AddNode(VolumesS[1],volnum++);
  kStrFoCount++;

  // SetColors(VolumesS,kNofLayersS);
  // cout<<VolumesS<<endl;
  //  StripFoil->Dump();
 

  radlen=GetRadLen(VolumesS,kLayerThickS,kNofLayersS);

  if( &kLayerNameS[0] == &kLayerNameOs[0]){
    kOsRad=radlen;
    kOsThick=dtot;
  }
  else{
    kIsRad=radlen;
    kIsThick=dtot;
  }

   return StripFoil;
  
}

// }}}
// {{{ CreateInnerStripFoil
TGeoVolumeAssembly * CreateInnerStripFoil(TGeoManager* _gGeoMan)
{

  Double_t thick, radlen;
  Int_t kNofStrips  = (Int_t)(kOfclen/kStripPitch);
  //  cout<<"Number of strips:"<<kNofStrips<<endl;

  //  cout<<"Layers for the Strip Foil"<<endl;
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersS ; ilayer++ )
  //   {
  //     cout << kLayerNameS[ilayer].Data() << " -> " << kLayerThickS[ilayer] << endl;
  //   }

  if( &kLayerNameS[0] == &kLayerNameOs[0])
    {
      TGeoVolumeAssembly * StripFoil = new TGeoVolumeAssembly("Outer_Strip_Foil");
      TGeoShape  * ShapesS[kNofLayersOs];
      TGeoVolume * VolumesS[kNofLayersOs];
    }
  else if ( &kLayerNameS[0] == & kLayerNameIs[0])
    {
      TGeoVolumeAssembly * StripFoil = new TGeoVolumeAssembly("Inner_Strip_Foil");
      TGeoShape  * IsShapesS[kNofLayersIs];
      TGeoVolume * IsVolumesS[kNofLayersIs];
    }

  // TGeoShape  *IsShapesS [kNofLayersS];
  // TGeoVolume *IsVolumesS[kNofLayersS];
  TString shapename, layerMaterial;
  Double_t rin,rout,dtot=0,dofctot=0;
  TGeoTranslation * t1 =new TGeoTranslation(0.,0.,kFclen/2);
  Int_t kStrFoCount = 0;



  for ( Int_t ilay = 0 ; ilay < kNofLayersS ; ilay++ )
    {
      dtot = dtot + kLayerThickS[ilay];
    }
 
  for ( Int_t ilay = 0 ; ilay < kNofLayersOfc ; ilay++ )
    dofctot += kLayerThickOfc[ilay];
  

  if( &kLayerNameS[0] == &kLayerNameOs[0])
    rin=kFcrad-dtot-dofctot;
  else
    rin=kFcrad-dtot;
 
  //generate all necessary volumes
  for ( Int_t ilay = 0; ilay < kNofLayersS ; ilay ++ )
    {
      rout=rin+kLayerThickS[ilay];
      shapename = Form("%s_shape",kLayerNameS[ilay]);
      IsShapesS[ilay] = new TGeoTube(shapename,rin,rout,kLayerZ[ilay]);
      
      layerMaterial = kLayerNameS[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      
      IsVolumesS[ilay] =  new TGeoVolume(kLayerNameS[ilay],IsShapesS[ilay],_gGeoMan->GetMedium(layerMaterial));
      rin=rout;
    }

  SetColors(IsVolumesS,kNofLayersS);

  //if ( &kLayerNameS[0] == &kLayerNameIs[0]) t1->SetDz(kIfclen-kOfclen);

  //generate stripfoil strips  
  kStrFoCount=1;
  for(Int_t istrip = 0 ; istrip < kNofStrips ; istrip++)
    {
      for ( Int_t istrlay = 0 ; istrlay < kNofLayersS ; istrlay+=kNofLayersS-1)
	{
	  StripFoil->AddNode((TGeoVolume*)IsVolumesS[istrlay],volnum++,(TGeoTranslation*)t1->Clone());
	  //	  cout<<(TGeoVolume*)IsVolumesS[istrlay]->Clone()->GetMaterial()->GetName()<<endl;
	  kStrFoCount++;
	  t1->SetDz(t1->GetTranslation()[2]-kStripPitch/2);
	}
    }
  
  //Add kapton part of stripfoil
  StripFoil->AddNode(IsVolumesS[1],volnum++);
  kStrFoCount++;

  // SetColors(IsVolumesS,kNofLayersS);
  // cout<<IsVolumesS<<endl;
  //  StripFoil->Dump();
 

  radlen=GetRadLen(IsVolumesS,kLayerThickS,kNofLayersS);

  if( &kLayerNameS[0] == &kLayerNameOs[0]){
    kOsRad=radlen;
    kOsThick=dtot;
  }
  else{
    kIsRad=radlen;
    kIsThick=dtot;
  }

   return StripFoil;
  
}

// }}}
// {{{ CreateCathode

TGeoVolume * CreateDriftCathode(TGeoManager* _gGeoMan )
{
  // cout<<"Layers for the Drift Cathode"<<endl;
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersDc ; ilayer++ )
  //   {
  //     cout << kLayerNameDc[ilayer].Data() << " -> " << kLayerThickDc[ilayer] << endl;
  //   }

  TGeoShape  *Shapes [kNofLayersDc];
  TGeoVolume *Volumes[kNofLayersDc];
  TString shapename, layerMaterial;
  Double_t rin,rout;
  TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,kFclen/2);

  TGeoVolumeAssembly * DriftCathode = new TGeoVolumeAssembly("Drift_Cathode");

  rin = kIfcrad;
  for(Int_t ilay=0;ilay<kNofLayersIfc;ilay++)
    rin-=kLayerThickIfc[ilay];
  for(Int_t ilay=0;ilay<kNofLayersIs;ilay++)
    rin-=kLayerThickIs[ilay];

  rout = kOfcrad;

  //calc outer fc radius for drift cathode radius
  // for ( Int_t ilay = 0 ; ilay < kNofLayersOfc ; ilay++ )
  //rout += kLayerThickFc[ilay]; 

  for ( Int_t ilay = 0; ilay < kNofLayersDc ; ilay ++ )
    {
      trans1->SetDz(trans1->GetTranslation()[2]+kLayerThickDc[ilay]/2);

      shapename = Form("%s_shape",kLayerNameDc[ilay]);
      Shapes[ilay] = new TGeoTube(shapename,rin,rout,kLayerThickDc[ilay]/2);
      
      layerMaterial = kLayerNameDc[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      
      Volumes[ilay] =  new TGeoVolume(kLayerNameDc[ilay],Shapes[ilay],_gGeoMan->GetMedium(layerMaterial));
      // rout = rout + kLayerThickDc[ilay]; //only for higher detailed model, not implemented now

      DriftCathode->AddNode(Volumes[ilay],volnum++,(TGeoTranslation*)trans1->Clone());
      trans1->SetDz(trans1->GetTranslation()[2]+kLayerThickDc[ilay]/2);
      //      Volumes[ilay]->CheckGeometry();

    }
  
  SetColors(Volumes,kNofLayersDc);

  kDcRad=GetRadLen(Volumes,kLayerThickDc,kNofLayersDc);
  return DriftCathode;
}

// }}}
// {{{ CreateMediaGemFlange
TGeoVolumeAssembly * CreateMediaGemFlange(TGeoManager* _gGeoMan )
{
  // cout<<"Layers for the Media and Gem Flange"<<endl;
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersMgf ; ilayer++ )
  //   {
  //     cout << kLayerNameMgf[ilayer].Data() << " -> " << kLayerThickMgf[ilayer] << endl;
  //   }

  TGeoVolumeAssembly * MediaGemFlange = new TGeoVolumeAssembly("Media_Gem_Flange");

  TGeoShape  *Shapes [kNofLayersMgf];
  TGeoVolume *Volumes[kNofLayersMgf];
  TString shapename, layerMaterial;
  TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,-kFclen/2);

 for ( Int_t ilay = 0; ilay < kNofLayersMgf ; ilay ++ )
    {
      trans1->SetDz(trans1->GetTranslation()[2]-kLayerThickMgf[ilay]/2);

      shapename = Form("%s_shape",kLayerNameMgf[ilay]);
      Shapes[ilay] = new TGeoTube(shapename,kRinMgf[ilay],KRoutMgf[ilay],kLayerThickMgf[ilay]/2);
      
      layerMaterial = kLayerNameMgf[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      
      Volumes[ilay] =  new TGeoVolume(kLayerNameMgf[ilay],Shapes[ilay],_gGeoMan->GetMedium(layerMaterial));
      
      //  if ( ilay < 2 ) 
      //{
      MediaGemFlange->AddNode(Volumes[ilay],volnum++,(TGeoTranslation*)trans1->Clone());
      //}
      trans1->SetDz((trans1->GetTranslation()[2])-kLayerThickMgf[ilay]/2);
	
    }

 //get gem hight
 double kgemheight = 0;
 kgemheight+=kLayerThickGf[0];
 kgemheight+=kLayerThickGf[3];
 kgemheight+=kLayerThickGf[6];
 // for (Int_t iglay = 0 ; iglay < kNofLayersGf ; iglay ++ )
 //   {
 //     kgemheight += kLayerThickGf[iglay];
 //   }

 //get media gemflange height
 double kMgfheight = 0;
 for (Int_t iglay = 0 ; iglay < 2 ; iglay ++ )
   {
     kMgfheight += kLayerThickMgf[iglay];
   }

 // cout<<kgemheight<<" "<<ilay<<endl;
 //trans1->Print();
 // trans1->SetDz(-kFclen/2-kMgfheight+kLayerThickGf[0]/2+kZoffPadplaneGem);
 trans1->SetDz((trans1->GetTranslation()[2])-3*kgemheight/2+kMgfheight-kLayerThickMgf[0]);
 TGeoVolumeAssembly * GemFoil = CreateGemFoil(_gGeoMan);
 for ( Int_t igem = 0 ; igem < 3 ; igem++ )
   {
     //     MediaGemFlange->AddNode(CreateGemFoil(_gGeoMan),ilay++,trans1);
     MediaGemFlange->AddNode((TGeoVolume*)GemFoil,volnum++,(TGeoTranslation*)trans1->Clone());
     //     MediaGemFlange->AddNode((TGeoVolume*)GemFoil,igem+1,(TGeoTranslation*)trans1->Clone());
     trans1->SetDz((trans1->GetTranslation()[2])+kgemheight/2);
   }

 SetColors(Volumes,kNofLayersMgf);

  return MediaGemFlange;

}
// }}}
// {{{ CreateGemFoil
TGeoVolumeAssembly * CreateGemFoil(TGeoManager* _gGeoMan )
{
  // cout<<"Layers for the Gem Foil"<<endl;
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersGf ; ilayer++ )
  //   {
  //     cout << kLayerNameGf[ilayer].Data() << " -> " << kLayerThickGf[ilayer] << endl;
  //   }

  TGeoVolumeAssembly * GemFoil = new TGeoVolumeAssembly("Gem_Foil");

  TGeoShape  *Shapes [kNofLayersGf];
  TGeoVolume *Volumes[kNofLayersGf];
  TString shapename, layerMaterial;
  TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,0.);
  
  for ( Int_t ilay = 0; ilay < kNofLayersGf ; ilay ++ )
    {
      shapename = Form("%s_shape",kLayerNameGf[ilay]);
      Shapes[ilay] = new TGeoTube(shapename,kRinGf[ilay],KRoutGf[ilay],kLayerThickGf[ilay]/2);
      
      layerMaterial = kLayerNameGf[ilay].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      
      Volumes[ilay] =  new TGeoVolume(kLayerNameGf[ilay],Shapes[ilay],_gGeoMan->GetMedium(layerMaterial));
    }

  GemFoil->AddNode(Volumes[0],volnum++,(TGeoTranslation*)trans1->Clone()); //outer g10 ring
  GemFoil->AddNode(Volumes[1],volnum++,(TGeoTranslation*)trans1->Clone()); // inner g10 ring
 
  trans1->SetDz(kLayerThickGf[0]/2-kLayerThickGf[2]/2);  // outer ring/2 - active area/2
  GemFoil->AddNode(Volumes[2],volnum++,(TGeoTranslation*)trans1->Clone()); // upper active area
  
  trans1->SetDz((trans1->GetTranslation()[2])+kLayerThickGf[2]/2+kLayerThickGf[3]/2);  // outer ring/2 + kapton/2
  GemFoil->AddNode(Volumes[3],volnum++,(TGeoTranslation*)trans1->Clone()); // kapton
  
  trans1->SetDz((trans1->GetTranslation()[2])+kLayerThickGf[3]/2+kLayerThickGf[4]/2); //outer ring/2 + kapton + active area/2
  GemFoil->AddNode(Volumes[4],volnum++,(TGeoTranslation*)trans1->Clone()); // lower active area
  
  trans1->SetDz((trans1->GetTranslation()[2])-kLayerThickGf[4]/2+kLayerThickGf[5]/2); //outer ring/2 - active area/2 + kapton + outer/inner ring
  GemFoil->AddNode(Volumes[5],volnum++,(TGeoTranslation*)trans1->Clone()); // inner g10 ring
  GemFoil->AddNode(Volumes[6],volnum++,(TGeoTranslation*)trans1->Clone()); //outer g10 ring

  SetColors(Volumes,kNofLayersGf);

    return GemFoil;
}
// }}}
// {{{ SetColors
void SetColors(TGeoVolume ** _Volumes,Int_t _layers)
{
  TString layerMaterial;
  // cout<<_Volumes<<_layers<<endl;
   // _Volumes[0]->GetMaterial();
  //  cout<<_layers<<endl;
  for ( Int_t igloblay = 0 ; igloblay < _layers ; igloblay ++ )
  {
    //    cout<<igloblay<<endl;
    layerMaterial = _Volumes[igloblay]->GetMaterial()->GetName();
    //    cout<<layerMaterial<<endl;
    if(layerMaterial.Contains("copper"))
      _Volumes[igloblay]->SetLineColor(2);
    if(layerMaterial.Contains("kapton"))
      _Volumes[igloblay]->SetLineColor(3);
    if(layerMaterial.Contains("aluminium"))
      _Volumes[igloblay]->SetLineColor(4);
    if(layerMaterial.Contains("G10"))
      _Volumes[igloblay]->SetLineColor(5);
    if(layerMaterial.Contains("rohacell"))
      _Volumes[igloblay]->SetLineColor(6);
  }
}

// }}}
// {{{ get radlen
Double_t GetRadLen(TGeoVolume** _Volumes, Double_t * _thick, Int_t _layers)
{
  Double_t pradlen;
  Double_t radlen;
  radlen=0;
  for (Int_t icage = 0 ; icage < _layers ; icage++)
    {
      radlen=_thick[icage]/_Volumes[icage]->GetMaterial()->GetRadLen();     
      pradlen+=radlen;
      cout<<_Volumes[icage]->GetName()<<" has a radlen of:"<<_Volumes[icage]->GetMaterial()->GetRadLen()<<" "<<"in per: "<<radlen<<"total: "<<pradlen<<endl;
      cout<<"the material is:"<<_Volumes[icage]->GetMaterial()->GetName()<<endl;
    }
  return pradlen;
}
// }}}
// {{{ print list of volumes
void print_list_volumes(){
Int_t numofvolumes;
TObjArray* volumelist;
cout<<"getting volumelist"<<endl;
volumelist=gGeoMan->GetListOfVolumes();
if(volumelist==NULL){
  cout<<"volumelist empty"<<endl;
 }
 else{
   numofvolumes= volumelist->GetEntries();
   cout<<"num of volumes: "<<numofvolumes<<endl;
   for( Int_t ivol=0; ivol<numofvolumes; ivol++)
     {
       cout<<"volumename:"<<((TGeoVolume*)volumelist->At(ivol))->GetName()<<endl;
     }
 }
}
// }}}
// {{{ print list of nodes
void print_list_nodes(){
  Int_t numofnodes;
  TObjArray* nodelist;
  cout<<"getting nodelist"<<endl;
  nodelist=gGeoMan->GetListOfNodes();
  if(nodelist==NULL){
    cout<<"nodelist empty"<<endl;
  }
  else{
    numofnodes= nodelist->GetEntries();
    cout<<"num of nodes: "<<numofnodes<<endl;
    for( Int_t inode=0; inode<numofnodes; inode++)
     {
       cout<<"nodename:"<<((TGeoNode*)nodelist->At(inode))->GetVolume()->GetName()<<endl;
     }
  }
}
// }}}
// {{{ create_prototype_geom


void create_prototype_geom()
{

  
  //  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("macro/tpc/runRadlenCalc.C");
  
  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  // gSystem->Load("libGeom");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");

  TString outfile = "geometry/tpc_prototype_rot.root";
  //  TFile* fi=new TFile(outfile,"RECREATE");
 TFile fi(outfile,"RECREATE");
  
  FairGeoLoader *geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  //  cout<<$PANDAPATH/geometry/media_pnd.geo<<endl;
  
  geoFace->setMediaFile("geometry/media_pnd.geo");
  geoFace->readMedia();
  //  geoFace->print();
  
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumAluminium   = Media -> getMedium("aluminium");
  FairGeoMedium *CbmMediumKapton      = Media -> getMedium("kapton");
  FairGeoMedium *CbmMediumCopper      = Media -> getMedium("copper");
  FairGeoMedium *CbmMediumG10         = Media -> getMedium("G10");
  FairGeoMedium *CbmMediumRohacell    = Media -> getMedium("rohacell");
  FairGeoMedium *CbmMediumTPCFOPI_mix = Media -> getMedium("TPCFOPI_mix");
  FairGeoMedium *CbmMediumTPCmixture  = Media -> getMedium("TPCmixture");
  FairGeoMedium *CbmMediumTPCGlue     = Media -> getMedium("glue");
  

  

  Int_t nmed;
  nmed=geobuild->createMedium(CbmMediumAluminium);
  nmed=geobuild->createMedium(CbmMediumKapton);
  nmed=geobuild->createMedium(CbmMediumCopper);
  nmed=geobuild->createMedium(CbmMediumG10);
  nmed=geobuild->createMedium(CbmMediumRohacell);
  nmed=geobuild->createMedium(CbmMediumTPCFOPI_mix);
  nmed=geobuild->createMedium(CbmMediumTPCmixture);
  nmed=geobuild->createMedium(CbmMediumTPCGlue);


  TGeoManager* gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
  TGeoVolume *toptop = new TGeoVolumeAssembly("TPC_Prot_top");
  TGeoVolumeAssembly *top = new TGeoVolumeAssembly("TPC_Prot");

  TList* mediaList = (TList*)gGeoMan->GetListOfMedia();
  for (Int_t itemp = 0 ; itemp < mediaList->GetEntries() ; itemp++ )
    {
      TGeoMedium *medium = (TGeoMedium*)mediaList->At(itemp);
      cout<<"medium "<< itemp << " is: "<<medium->GetName() <<endl;
    }



  Int_t ofc, osf, ifc, isf, dc, gmm, gas, draw, ivol, checkover;
  
  ofc  = 1;
  osf  = 0;
  ifc  = 1;
  isf  = 0;
  dc   = 1;
  gmm  = 1;
  gas  = 1;

  draw = 0;
  checkover = 1;
  alignman = 0;

 //create gas
  TGeoVolume *Gas;
  TGeoShape  *GasShape;

  krin=kIfcrad;

  //  for ( Int_t ilay = 0 ; ilay < kNofLayersIs ; ilay++ )
  //  {
  //  krin = krin - kLayerThickIs[ilay];
  //  }

  krout=kOfcrad;
  
  for( Int_t ilay=0;ilay<kNofLayersOfc;ilay++)
    krout-=kLayerThickOfc[ilay];
  for (Int_t ilay=0;ilay<kNofLayersOs;ilay++)
    krout-=kLayerThickOs[ilay];

  ivol=1;
  
  Double_t globoff;
  globoff=-kOfclen/2+10.78;

  PndTpcAlignmentManager* fAlignmentManager=PndTpcAlignmentManager::getInstance("macro/tpc/proto_alignment.txt");
  fAlignmentManager->getTransformation(1)->Print();

  if (alignman == 1){
    TGeoCombiTrans trans1;
    trans1 = fAlignmentManager->getTransformation(1);
    trans1->SetDz(trans1->GetTranslation()[2]+globoff);
  }else{
    TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,globoff);
    trans1->SetDz(globoff);
  }

  trans1->Print();

  GasShape = new TGeoTube("GasShape",krin,krout,kOfclen/2);
  // ArCo2 90/10
  //Gas = new TGeoVolume("gasArCO2",GasShape,gGeoMan->GetMedium("TPCFOPI_mix"));
  // NeCo2 90/10
  Gas = new TGeoVolume("gasNeCO2",GasShape,gGeoMan->GetMedium("TPCmixture"));
  Gas->SetLineColor(7);
  Gas->SetTransparency(10);
  if(gas==1) top->AddNode(Gas,volnum++,(TGeoMatrix*)trans1->Clone());

  // outer fieldcage
  kNofLayersFc = kNofLayersOfc;
  kFcrad = kOfcrad;
  kFclen = kOfclen;
  kLayerNameFc = kLayerNameOfc;
  kLayerThickFc = kLayerThickOfc;
  kZOffsetFc = kZOffsetOfc;
  if(ofc==1) top->AddNode(CreateOuterFieldCage(gGeoMan),volnum++,(TGeoMatrix*)trans1->Clone());
  

  //outer stripfoil
  kNofLayersS = kNofLayersOs;
  kLayerNameS = kLayerNameOs;
  kLayerThickS = kLayerThickOs;
  kLayerZ = kLayerZOs;
  if(osf==1) top->AddNode(CreateStripFoil(gGeoMan),volnum++,(TGeoMatrix*)trans1->Clone());

  //inner fieldcage
  TGeoMatrix* trans2 = (TGeoMatrix*)trans1->Clone(); 
  trans2->SetDz(trans2->GetTranslation()[2]+(kOfclen-kIfclen)/2);
  kNofLayersFc = kNofLayersIfc;
  kFcrad = kIfcrad;
  kFclen = kIfclen;
  kLayerNameFc = kLayerNameIfc;
  kLayerThickFc = kLayerThickIfc;
  kZOffsetFc = kZOffsetIfc;
  if(ifc==1) top->AddNode(CreateInnerFieldCage(gGeoMan),volnum++,(TGeoMatrix*)trans2->Clone());
  kIfcrin=kFcrin;
  kIfcrout=kFcrout;
  kFclen=kOfclen;

  //inner stripfoil
  //  trans1->SetDz(trans1->GetTranslation()[2]+kOfclen/8);
  //  trans1->SetDz(globoff);
  kNofLayersS = kNofLayersIs;
  kLayerNameS = kLayerNameIs;
  kLayerThickS = kLayerThickIs;
  kLayerZ = kLayerZIs;
  if(isf==1) top->AddNode(CreateStripFoil(gGeoMan),volnum++,(TGeoMatrix*)trans1->Clone());
  
  if(dc==1) top->AddNode(CreateDriftCathode(gGeoMan),volnum++,(TGeoMatrix*)trans1->Clone());
  TGeoMatrix* trans3 = (TGeoMatrix*)trans1->Clone();
  trans3->SetDz(trans3->GetTranslation()[2]+kLayerThickMgf[0]);
  if(gmm==1) top->AddNode(CreateMediaGemFlange(gGeoMan),volnum++,(TGeoMatrix*)trans3->Clone());

  TGeoVoxelFinder* voxels = top->GetVoxels();
  if (voxels) { voxels->SetNeedRebuild(); }
  
  //  top->AddNode(topAss, 0);
  
  if(checkover==1)
    {
      gGeoMan->CheckOverlaps(0.01);
      gGeoMan->GetListOfOverlaps()->ls();
    }

  top->cd(0);
  Int_t numofnodes;
  TObjArray* nodelist;
  nodelist=gGeoMan->GetListOfNodes();
  numofnodes= nodelist->GetEntries();
  for( Int_t inode=0; inode<numofnodes; inode++)
    {
      cout<<"name of node "<<inode<<" : "<<((TGeoNode*)nodelist->At(inode))->GetVolume()->GetName()<<endl;
    }



  toptop->AddNode(top,0,fAlignmentManager->getTransformation(1));
  //  toptop->AddNode(top,0);
  

  gGeoMan->SetTopVolume(toptop);
  gGeoMan->CloseGeometry();



  fi.cd();
  toptop->Write();
  //  gGeoMan->Write();
  fi.Close();
  top->cd(0);

  Double_t totperrad;

  cout<<"Outer Fieldcage radlen="<<kOfcRad<<endl;
  cout<<"Strip Foil radlen="<<kIsRad<<endl;
  cout<<"Drift Cathode radlen="<<kDcRad<<endl;

  totperrad=(kOfcRad+kIfcRad+kOsRad+kIsRad)*100;
  cout<<"Radlen perp in percentage="<<totperrad<<endl;
  totperrad=(kIfcRad+kIsRad+kDcRad)*100;
  cout<<"Radlen long in percentage="<<totperrad<<endl;

  //  runRadlenCalc(gGeoMan,0);

  if(draw==1){
  TEveManager::Create();
  TGeoNode* node = gGeoMan->GetNode(0);

  TEveGeoTopNode* en = new TEveGeoTopNode(gGeoMan, node);
  //  TEveGeoTopNode* en = new TEveGeoTopNode(gGeoMan, toptop->GetNode(0));
  //  TEveGeoTopVolume *en = new TEveGeoTopVolume(gGeoMan,toptop);
  gEve->AddGlobalElement(en);
  

  gEve->Redraw3D(kTRUE);
  }
}

// }}}
