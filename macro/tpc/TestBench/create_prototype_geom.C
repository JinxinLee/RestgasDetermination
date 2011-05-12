#include <map>
#include <vector>
#include <TString.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
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
const Int_t kNofLayersOfc = 8;
const Double_t kOfcrad = 15.4;
Double_t kOfclen = 72.78;

TString kLayerNameOfc[kNofLayersOfc] = {"FcFirst_aluminium",
					"FcFirst_kapton",
					"FcSecond_kapton",
					"FcFirst_rohacell",
					"FcMiddle_kapton",
					"FcSecond_rohacell",
					"FcFourth_kapton",
					"FcLast_kapton"};

Double_t kLayerThickOfc[kNofLayersOfc] = { 200.e-8,     //    1 Aluminium
					   25.0e-4,     // +1=2 Kapton
					   125.0e-4,    // +1=3 Kapton
					   0.2,         // +1=4 rohacell
					   125.0e-4,    // +1=5 Kapton
					   0.2,         // +1=6 Rohacell
					   125.0e-4,    // +1=7 Kapton
					   25.0e-4 };   // +1=8 Kapton

Double_t kZOffsetOfc[kNofLayersOfc] = { 0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.,
					0.}; // for a more detailed model, for now put 0.
// }}}
// {{{ Inner Fieldcage Config

const Int_t kNofLayersIfc = 8;
const Double_t kIfcrad = 5.2;
const Double_t kIfclen = 80.5;
Double_t kIfcrin, kIfcrout;
TString kLayerNameIfc[kNofLayersIfc] = {"FcFirst_aluminium",
					"FcFirst_kapton",
					"FcSecond_kapton",
					"FcFirst_rohacell",
					"FcMiddle_kapton",
					"FcSecond_rohacell",
					"FcFourth_kapton",
					"FcLast_kapton"};


Double_t kLayerThickIfc[kNofLayersIfc] = { 200.e-8,     //    1 Aluminium
					   25.0e-4,     // +1=2 Kapton
					   125.0e-4,    // +1=3 Kapton
					   0.2,         // +1=4 rohacell
					   125.0e-4,    // +1=5 Kapton
					   0.2,         // +1=6 Rohacell
					   125.0e-4,    // +1=7 Kapton
					   25.0e-4 };   // +1=8 Kapton

Double_t kZOffsetIfc[kNofLayersIfc] = { 0.,
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
const TString kLayerNameOs[kNofLayersOs] = {"StInner_copper",
					    "StMiddle_kapton",
					    "StOuter_copper"};

const Double_t kLayerThickOs[kNofLayersOs]= {5e-4,   //    1 copper
					     25e-4,  // +1=2 kapton
					     5e-4,}; // +1=3 copper

const Double_t kLayerZOs[kNofLayersOs] = {0.05,kOfclen/2,0.05};


// }}}		      
// {{{ Inner Stripfoil Config

const Int_t kNofLayersIs = 3;
TString kLayerNameIs[kNofLayersIs] = {"StInner_copper",
				      "StMiddle_kapton",
				      "StOuter_copper"};

const Double_t kLayerThickIs[kNofLayersIs]= {5e-4,   //    1 copper
					     25e-4,   // +1=2 kapton
					     5e-4}; // +1=3 copper

const Double_t kLayerZIs[kNofLayersIs] = {0.05,kOfclen/2,0.05};

// }}}		      
// {{{ DriftCathode Config
  const Int_t kNofLayersDc = 13;
const TString kLayerNameDc[kNofLayersDc] = {"DcFirst_aluminium",
					    "DcFirst_kapton",
					    "DcSecond_kapton",
					    "DcThird_kapton",
					    "DcFirst_rohacell",
					    "DcFirstMiddle_kapton",
					    "DcSecond_rohacell",
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
					       125e-4, // 8 kapton
					       0.2,    // 9 rohacell
					       125e-4, //10 kapton
					       25e-4,  //11 kapton
					       25e-4,  //12 kapton
					       200e-8  //13 Aluminium
};
// }}}
// {{{ ReadoutFlange Config
const Int_t kNofLayersMgf = 4;
const Double_t kZoffPadplaneGem = 0.4;
const Double_t kRinMgf[kNofLayersMgf] = {15.4, // media flange
					 16.2, // gem flange
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
					      5e-4, //upper copper	  
					      50e-4, //kapton		  
					      5e-4, //lower copper	  
					      0.1, //lower inner g10 ring
					      0.1};//lower outer g10 ring
// }}}

// {{{ CreateFieldCage
TGeoVolumeAssembly * CreateFieldCage(TGeoManager* _gGeoMan)
{
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersFc ; ilayer++ )
  //   {
  //     cout << kLayerNameFc[ilayer].Data() << " -> " << kLayerThickFc[ilayer] << endl;
  //   }




  //  vector <*TGeoShape> Shapes;

  if( &kLayerNameFc[0] == &kLayerNameOfc[0])
    {
      TGeoVolumeAssembly * FieldCageTop = new TGeoVolumeAssembly("Outer_Field_Cage_Top");
      TGeoVolumeAssembly * FieldCage = new TGeoVolumeAssembly("Outer_Field_Cage");
      TGeoShape  * Shapes[kNofLayersOfc];
      TGeoVolume * Volumes[kNofLayersOfc];
    }
  else if ( &kLayerNameFc[0] == & kLayerNameIfc[0])
    {
      TGeoVolumeAssembly * FieldCageTop = new TGeoVolumeAssembly("Inner_Field_Cage_Top");
      TGeoVolumeAssembly * FieldCage = new TGeoVolumeAssembly("Inner_Field_Cage");
      TGeoShape  * Shapes[kNofLayersIfc];
      TGeoVolume * Volumes[kNofLayersIfc];
    }

  Double_t rin  = kFcrad;
  Double_t rout;
  Double_t z = kFclen;
  Double_t totZ=0; 

  TString shapename, layerMaterial;


  for ( Int_t ilay  =0 ; ilay < kNofLayersFc ; ilay++)
    {
    totZ=totZ + kZOffsetFc[ilay]; 
    rin-=kLayerThickFc[ilay];
    }
  kFcrin=rin;
  //after this rin is the most inner radius

  TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,-totZ/2);
  z = (z - totZ)/2;

  for ( Int_t icage = 0 ; icage < kNofLayersFc ; icage++ )
    {
      rout = rin + kLayerThickFc[icage];

      shapename=Form("%s_shape",kLayerNameFc[icage]);
      Shapes[icage] = new TGeoTube(shapename,rin,rout,z);
      
      layerMaterial = kLayerNameFc[icage].Data();
      layerMaterial.Remove(0,layerMaterial.Last('_')+1);
      Volumes[icage] =  new TGeoVolume(kLayerNameFc[icage],Shapes[icage],_gGeoMan->GetMedium(layerMaterial));
  
      FieldCage->AddNode(Volumes[icage],0,(TGeoTranslation*)trans1->Clone());

      trans1->SetDz(trans1->GetTranslation()[2]+kZOffsetFc[icage]/2);
      z=z+(kZOffsetFc[icage]/2);
      rin  = rout;
    }
  kFcrout=rin;
  // now rin ist the most outer radius

  SetColors(Volumes,kNofLayersFc);
  
  FieldCageTop->AddNode(FieldCage,0);
  return FieldCageTop;
}
// }}}
// {{{ CreateStripFoil
TGeoVolumeAssembly * CreateStripFoil(TGeoManager* _gGeoMan)
{
  
  Int_t kNofStrips  = (Int_t)(kOfclen/kStripPitch);
  //  cout<<"Number of strips:"<<kNofStrips<<endl;

  //  cout<<"Layers for the Strip Foil"<<endl;
  // for ( Int_t ilayer = 0 ; ilayer < kNofLayersS ; ilayer++ )
  //   {
  //     cout << kLayerNameS[ilayer].Data() << " -> " << kLayerThickS[ilayer] << endl;
  //   }

  if( &kLayerNameS[0] == &kLayerNameOs[0])
    {
      TGeoVolumeAssembly * StripFoilTop = new TGeoVolumeAssembly("Outer_Strip_Foil_Top");
      TGeoVolumeAssembly * StripFoil = new TGeoVolumeAssembly("Outer_Strip_Foil");
      TGeoShape  * ShapesS[kNofLayersOs];
      TGeoVolume * VolumesS[kNofLayersOs];
    }
  else if ( &kLayerNameS[0] == & kLayerNameIs[0])
    {
      TGeoVolumeAssembly * StripFoilTop = new TGeoVolumeAssembly("Inner_Strip_Foil_Top");
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

  //if ( &kLayerNameS[0] == &kLayerNameIs[0]) t1->SetDz(kIfclen-kOfclen);

  //generate stripfoil strips  
  kStrFoCount=1;
  for(Int_t istrip = 0 ; istrip < kNofStrips ; istrip++)
    {
      for ( Int_t istrlay = 0 ; istrlay < kNofLayersS ; istrlay+=kNofLayersS-1)
	{
	  StripFoil->AddNode((TGeoVolume*)VolumesS[istrlay]->Clone(),kStrFoCount,(TGeoTranslation*)t1->Clone());
	  kStrFoCount++;
	  t1->SetDz(t1->GetTranslation()[2]-kStripPitch/2);
	}
    }
  
  //Add kapton part of stripfoil
  StripFoil->AddNode(VolumesS[1],0);
  kStrFoCount++;

  // SetColors(VolumesS,kNofLayersS);
  // cout<<VolumesS<<endl;
  //  StripFoil->Dump();
  SetColors(VolumesS,kNofLayersS);

  StripFoilTop->AddNode(StripFoil,0);
  return StripFoilTop;
  
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

  TGeoVolume * DriftCathodeTop = new TGeoVolumeAssembly("Drift_Cathode_Top");
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

      DriftCathode->AddNode(Volumes[ilay],0,(TGeoTranslation*)trans1->Clone());
      trans1->SetDz(trans1->GetTranslation()[2]+kLayerThickDc[ilay]/2);
    }
  
  SetColors(Volumes,kNofLayersDc);

  DriftCathodeTop->AddNode(DriftCathode,0);
  return DriftCathodeTop;
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

  TGeoVolumeAssembly * MediaGemFlangeTop = new TGeoVolumeAssembly("Media_Gem_Flange_Top");
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
      MediaGemFlange->AddNode(Volumes[ilay],0,(TGeoTranslation*)trans1->Clone());
      //}
      trans1->SetDz((trans1->GetTranslation()[2])-kLayerThickMgf[ilay]/2);
	
    }

 //get gem hight
 double kgemheight = 0;
 for (Int_t iglay = 0 ; iglay < kNofLayersGf ; iglay ++ )
   {
     kgemheight += kLayerThickGf[iglay];
   }

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
     MediaGemFlange->AddNode((TGeoVolume*)GemFoil->Clone(),igem+1,(TGeoTranslation*)trans1->Clone());
     trans1->SetDz((trans1->GetTranslation()[2])+kgemheight/2);
   }

 SetColors(Volumes,kNofLayersMgf);

 MediaGemFlangeTop->AddNode(MediaGemFlange,0);
 return MediaGemFlangeTop;

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

  TGeoVolumeAssembly * GemFoilTop = new TGeoVolumeAssembly("Gem_Foil_Top");
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

  GemFoil->AddNode(Volumes[0],0,(TGeoTranslation*)trans1->Clone()); //outer g10 ring
  GemFoil->AddNode(Volumes[1],0,(TGeoTranslation*)trans1->Clone()); // inner g10 ring
 
  trans1->SetDz(kLayerThickGf[0]/2-kLayerThickGf[2]/2);  // outer ring/2 - active area/2
  GemFoil->AddNode(Volumes[2],0,(TGeoTranslation*)trans1->Clone()); // upper active area
  
  trans1->SetDz((trans1->GetTranslation()[2])+kLayerThickGf[2]/2+kLayerThickGf[3]/2);  // outer ring/2 + kapton/2
  GemFoil->AddNode(Volumes[3],0,(TGeoTranslation*)trans1->Clone()); // kapton
  
  trans1->SetDz((trans1->GetTranslation()[2])+kLayerThickGf[3]/2+kLayerThickGf[4]/2); //outer ring/2 + kapton + active area/2
  GemFoil->AddNode(Volumes[4],0,(TGeoTranslation*)trans1->Clone()); // lower active area
  
  trans1->SetDz((trans1->GetTranslation()[2])-kLayerThickGf[4]/2+kLayerThickGf[5]/2); //outer ring/2 - active area/2 + kapton + outer/inner ring
  GemFoil->AddNode(Volumes[5],0,(TGeoTranslation*)trans1->Clone()); // inner g10 ring
  GemFoil->AddNode(Volumes[6],0,(TGeoTranslation*)trans1->Clone()); //outer g10 ring

  SetColors(Volumes,kNofLayersGf);

  GemFoilTop->AddNode(GemFoil,0);
  return GemFoilTop;
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
// {{{ create_prototype_geom

void create_prototype_geom()
{

  
  //gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");

  TString outfile = "geometry/tpc_prototype.root";
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

  

  Int_t nmed;
  nmed=geobuild->createMedium(CbmMediumAluminium);
  nmed=geobuild->createMedium(CbmMediumKapton);
  nmed=geobuild->createMedium(CbmMediumCopper);
  nmed=geobuild->createMedium(CbmMediumG10);
  nmed=geobuild->createMedium(CbmMediumRohacell);
  nmed=geobuild->createMedium(CbmMediumTPCFOPI_mix);
  nmed=geobuild->createMedium(CbmMediumTPCmixture);


  TGeoManager* gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
  TGeoVolume *toptop = new TGeoVolumeAssembly("TPC_Prot_top");
  TGeoVolumeAssembly *top = new TGeoVolumeAssembly("TPC_Prot");

  TList* mediaList = (TList*)gGeoMan->GetListOfMedia();
  //  cout << "known media: "<< mediaList->GetEntries() <<"."<<endl;
  for (Int_t itemp = 0 ; itemp < mediaList->GetEntries() ; itemp++ )
    {
      TGeoMedium *medium = (TGeoMedium*)mediaList->At(itemp);
      cout<<"medium "<< itemp << " is: "<<medium->GetName() <<endl;
    }

  TGeoTranslation * trans1 =new TGeoTranslation(0.,0.,0.);

  Int_t ofc, osf, ifc, isf, dc, gmm, gas, draw, ivol;
  
  ofc  = 1;
  osf  = 1;
  ifc  = 1;
  isf  = 1;
  dc   = 1;
  gmm  = 1;
  gas  = 1;

  draw = 0;

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

  GasShape = new TGeoTube("GasShape",krin,krout,kOfclen/2);
  // ArCo2 90/10
  //Gas = new TGeoVolume("gasARCO2",GasShape,gGeoMan->GetMedium("TPCFOPI_mix"));
  // NeCo2 90/10
  Gas = new TGeoVolume("gasNECO2",GasShape,gGeoMan->GetMedium("TPCmixture"));
  Gas->SetLineColor(7);
  Gas->SetTransparency(10);
  if(gas==1) top->AddNode(Gas,0);

  // outer fieldcage
  kNofLayersFc = kNofLayersOfc;
  kFcrad = kOfcrad;
  kFclen = kOfclen;
  kLayerNameFc = kLayerNameOfc;
  kLayerThickFc = kLayerThickOfc;
  kZOffsetFc = kZOffsetOfc;
  if(ofc==1) top->AddNode(CreateFieldCage(gGeoMan),0);
  

  //outer stripfoil
  kNofLayersS = kNofLayersOs;
  kLayerNameS = kLayerNameOs;
  kLayerThickS = kLayerThickOs;
  kLayerZ = kLayerZOs;
  if(osf==1) top->AddNode(CreateStripFoil(gGeoMan),0);

  //inner fieldcage
  trans1->SetDz((kOfclen-kIfclen)/2);
  kNofLayersFc = kNofLayersIfc;
  kFcrad = kIfcrad;
  kFclen = kIfclen;
  kLayerNameFc = kLayerNameIfc;
  kLayerThickFc = kLayerThickIfc;
  kZOffsetFc = kZOffsetIfc;
  if(ifc==1) top->AddNode(CreateFieldCage(gGeoMan),0,(TGeoTranslation*)trans1->Clone());
  kIfcrin=kFcrin;
  kIfcrout=kFcrout;
  kFclen=kOfclen;

  //inner stripfoil
  //  trans1->SetDz(trans1->GetTranslation()[2]+kOfclen/8);
  trans1->SetDz(0);
  kNofLayersS = kNofLayersIs;
  kLayerNameS = kLayerNameIs;
  kLayerThickS = kLayerThickIs;
  kLayerZ = kLayerZIs;
  if(isf==1) top->AddNode(CreateStripFoil(gGeoMan),0,(TGeoTranslation*)trans1->Clone());
  
  if(dc==1) top->AddNode(CreateDriftCathode(gGeoMan),0);
  trans1->SetDz(kLayerThickMgf[0]);
  if(gmm==1) top->AddNode(CreateMediaGemFlange(gGeoMan),0,(TGeoTranslation*)trans1->Clone());
  //top->AddNode(CreateGemFoil(gGeoMan),7);

  //top->Draw("test.pdf");

  gGeoMan->SetTopVolume(top);
  toptop->AddNode(top,0);
  

  fi.cd();
  toptop->Write();
  //gGeoMan->Write();
  fi.Close();

  if(draw==1){
  TEveManager::Create();
  TGeoNode* node = gGeoMan->GetNode(0);
  
  TEveGeoTopNode* en = new TEveGeoTopNode(gGeoMan, node);
  //TEvePlot3D * plotter = new TEvePlot3D("plotter","");
  //  en->SetVisLevel(4);
  //en->GetNode()->GetVolume()->SetVisibility(kFALSE);
  
  //  plotter->SetPlot(node);
  //plotter->Paint();
  gEve->AddGlobalElement(en);
  

  gEve->Redraw3D(kTRUE);
  }
}

// }}}

