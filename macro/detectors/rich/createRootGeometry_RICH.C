// macro that creates the RICH geometry
// to be able to run this macro please be sure you have the following configuration in gconfig/g4Config.C file:
// TG4RunConfiguration* runConfiguration 
//           = new TG4RunConfiguration("geomRoot", "QGSP_BERT_EMV+optical", "stepLimiter+specialCuts+specialControls");


// B E F O R E   R U N N I N G   T H E   P R O G R A M
// C H E C K   T H E   N A M E   O F   T H E   O U T P U T   F I L E ! ! !

#include <vector>

const Double_t pi = 3.1415926535;

void createRootGeometry_RICH(UInt_t ver = 0, bool useBeamPipeHole = true){

   UInt_t verm = ver%100;
   
  const Double_t pi = 3.1415926535;

//  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  TString vmcWorkdir = getenv("VMCWORKDIR");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libEve");
  
  // variable to achieve the RICH basic parameters
  PndRichGeo* fGeo = new PndRichGeo();
  fGeo->init(ver);
  
  // units = cm

  //bool useBeamPipeHole = true;

  TVector3 richOffset               = fGeo->richOffset();
  TVector3 alBoxSize                = fGeo->alBoxSize();
  Double_t alBoxWallThickness       = fGeo->alBoxWallThickness();
  TVector3 aerogelSize              = fGeo->aerogelSize();
  TVector3 aerogelOffset            = fGeo->aerogelOffset();
  std::vector<Double_t> nOpt        = fGeo->nOpt();
  std::vector<Double_t> aerogelLayers = fGeo->aerogelLayers();
  Double_t ka1                      = fGeo->angleExtansionInner();
  Double_t ka2                      = fGeo->angleExtansionOuter();
  Double_t beta                     = fGeo->mirrorCurvature();
  Double_t alpha                    = fGeo->angleOfMirrorPosition();
  Double_t mirrorThickness          = fGeo->mirrorThickness();
  Double_t mirrorLength             = fGeo->mirrorLength();
  Double_t phDetLength              = fGeo->phDetLength();
  Double_t phDetThickness           = fGeo->phDetThickness();
  Double_t beamPipeHoleX            = fGeo->beamPipeHoleX();
  Double_t beamPipeHoleY            = fGeo->beamPipeHoleY();

   std::vector<Double_t> phDetY = fGeo->phDetY();
   std::vector<Double_t> phDetZ = fGeo->phDetZ();
   std::vector<Double_t> flatMirrorY = fGeo->flatMirrorY();
   std::vector<Double_t> flatMirrorZ = fGeo->flatMirrorZ();
   for(size_t i=0;i<flatMirrorY.size();i++)
      std::cout << i << " " << flatMirrorY[i] << " " << flatMirrorZ[i] << std::endl;
   for(i=0;i<phDetY.size();i++)
      std::cout << i << " " << phDetY[i] << " " << phDetZ[i] << std::endl;
   for(i=0;i<nOpt.size();i++)
      std::cout << i << " " << nOpt[i] << std::endl;

  //----------------------------------------------------------

  if (useBeamPipeHole)
      TString fGeoFile= Form("../../geometry/rich_v%d.root",ver);
  else
      TString fGeoFile= Form("../../geometry/rich_v%d_cal.root",ver);
  TFile* fi = new TFile(fGeoFile,"RECREATE");
  cout<<"Output file = "<<fGeoFile<<endl;
  
   if (!useBeamPipeHole) {
      beamPipeHoleX = 0;
      beamPipeHoleY = 0;
      useBeamPipeHole = true;
   }
   std::cout << beamPipeHoleX << " " << beamPipeHoleY << std::endl;
    
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();
  
  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
   
  FairGeoMedium *RichAl       	= Media->getMedium("aluminium");
  FairGeoMedium *RichAir       	= Media->getMedium("RichAir");
  FairGeoMedium *RichAerogel    = Media->getMedium("FusedSil");
  FairGeoMedium *RichAerogel0   = Media->getMedium("RichAerogel0");
  FairGeoMedium *RichAerogel1   = Media->getMedium("RichAerogel1");
  FairGeoMedium *RichAerogel2   = Media->getMedium("RichAerogel2");
  FairGeoMedium *RichAerogel3   = Media->getMedium("RichAerogel3");
  FairGeoMedium *RichAerogel4   = Media->getMedium("RichAerogel4");
  FairGeoMedium *RichPcbmvd     = Media->getMedium("pcbmvd");
  FairGeoMedium *RichPDWindow   = Media->getMedium("RichPDWindow");
  FairGeoMedium *RichSilicon    = Media->getMedium("silicon");
  FairGeoMedium *RichMirror     = Media->getMedium("Mirror");
  
  Int_t nmed;
  nmed=geobuild->createMedium(RichAir);
  nmed=geobuild->createMedium(RichAl);
  nmed=geobuild->createMedium(RichAerogel);
  nmed=geobuild->createMedium(RichAerogel0);
  nmed=geobuild->createMedium(RichAerogel1);
  nmed=geobuild->createMedium(RichAerogel2);
  nmed=geobuild->createMedium(RichAerogel3);
  nmed=geobuild->createMedium(RichAerogel4);
  nmed=geobuild->createMedium(RichPcbmvd);
  nmed=geobuild->createMedium(RichPDWindow);
  nmed=geobuild->createMedium(RichSilicon);
  nmed=geobuild->createMedium(RichMirror);

   
   //-------------------------------------------------------------------------------------------------------------
   // Rich system
   // Al box
    TGeoBBox* lRichBox = new TGeoBBox("richBox", alBoxSize.X()/2, alBoxSize.Y()/2, alBoxSize.Z()/2 );
    TGeoVolume *alRichBox = new TGeoVolume("RichAlBox",lRichBox, gGeoManager->GetMedium("aluminium"));
     
    // Air box
    TGeoBBox* lRichBoxAir = new TGeoBBox("richBoxAir", 
                                         alBoxSize.X()/2 - alBoxWallThickness, 
                                         alBoxSize.Y()/2 - alBoxWallThickness, 
                                         alBoxSize.Z()/2 - alBoxWallThickness );
    TGeoVolume *alRichBoxAir = new TGeoVolume("RichAlBoxAir",lRichBoxAir, gGeoManager->GetMedium("RichAir"));
    alRichBoxAir->SetLineColor(kGreen);
    alRichBoxAir->SetTransparency(90); 
    alRichBox->AddNode(alRichBoxAir, 0, new TGeoCombiTrans( 0, 0, 0, new TGeoRotation(0) ) );
     
    TGeoBBox* lRichBeamPipeHole = new TGeoBBox("richBeamPipeHole", 
                                               2*beamPipeHoleX/2,
                                               2*beamPipeHoleY/2,
                                               alBoxSize.Z()/2 );
   
   // MultiLayer Aerogel
   UInt_t numberOfBlocks = nOpt.size();
   TString aerogelUnity = Form("( ");
   std::vector<TGeoBBox*> lRichAerogel(numberOfBlocks);
   std::vector<TGeoTranslation*> trans(numberOfBlocks);
   std::vector<TGeoCompositeShape*> richAerogelCS(numberOfBlocks);
   std::vector<TGeoVolume*> richAerogel(numberOfBlocks);
   Double_t za = aerogelOffset.Z()-alBoxSize.Z()/2;
   for(UInt_t ia=0; ia<numberOfBlocks; ia++) {
      Double_t aerogelThickness = aerogelSize.Z()*aerogelLayers[ia];
      za += aerogelThickness/2;
      
      TString richAerogelName = Form("richAerogel%d",ia);
      TString transName = Form("trans%d",ia);
      TString richAerogelMedia = Form("RichAerogel%d",ia);
      TString richAerogelCSName = Form("RichAerogelCS%d",ia);
      TString richAerogelSensorName = Form("RichAerogelSensor%d",ia);

      if (useBeamPipeHole)
         aerogelUnity = "( " + richAerogelName + ":" + transName + " ) - ( richBeamPipeHole ) ";
      else
         aerogelUnity = "( " + richAerogelName + ":" + transName + " ) ";
      std::cout << aerogelUnity << " " << richAerogelMedia << " " <<
         gGeoManager->GetMedium(richAerogelMedia) << " " <<
      gGeoManager->GetMedium("FusedSil") << std::endl;
      
      lRichAerogel.at(ia) = new TGeoBBox(richAerogelName, 
                                         aerogelSize.X()/2,
                                         aerogelSize.Y()/2,
                                         aerogelThickness/2 );
      trans.at(ia) = new TGeoTranslation(transName,
                                         aerogelOffset.X(),
                                         aerogelOffset.Y(),
                                         za);
      trans.at(ia)->RegisterYourself();
      richAerogelCS.at(ia) = new TGeoCompositeShape(richAerogelCSName,aerogelUnity);
      richAerogel.at(ia) = new TGeoVolume(richAerogelSensorName,
                                          richAerogelCS.at(ia),
                                          gGeoManager->GetMedium(richAerogelMedia));
      richAerogel.at(ia)->SetLineColor(kCyan);
      richAerogel.at(ia)->SetTransparency(40);
      alRichBoxAir->AddNode(richAerogel.at(ia), 0, new TGeoCombiTrans( 0, 0, 0, new TGeoRotation(0) ) );
      
      za += aerogelThickness/2;
   }
       
    // Mirror
    Double_t za = richOffset.Z() /*+ alBoxWallThickness*/ + aerogelOffset.Z();
    Double_t ya = aerogelSize.Y()/2;
    Double_t wa = aerogelSize.Z();
    Double_t yp1 = ya + wa;
    Double_t zp1 = wa;
    alpha = alpha < 45 - beta ? alpha : 45 -beta;
    alpha *= pi/180;
    beta *= pi/180;
    Double_t thetaCh = acos(1/nOpt[0]);
    Double_t theta = atan(ya/za);
    Double_t alpha1 = 2*(alpha+beta) - ka1*thetaCh;
    Double_t alpha2 = 2*(alpha-beta) + ka2*thetaCh + theta;
    Double_t alpham = (alpha1+alpha2)/2;
    Double_t zm1 = zp1 + yp1/tan(alpha1);
    Double_t ym1 = 0;
    Double_t zm2 = (zm1+ya*tan(alpha))/(1-tan(theta+ka2*thetaCh)*tan(alpha));
    Double_t ym2 = (zm2-zm1)/tan(alpha);
    Double_t zp2 = ((ym2-yp1+zm2*tan(alpha2))*tan(alpham)+zp1)/(1+tan(alpha2)*tan(alpham));
    Double_t yp2 = yp1 + (zp2-zp1)/tan(alpham);
    if (yp2<ym2) {
       yp2 = ym2;
       zp2 = zm2;
       alpham = atan((zp2-zp1)/(yp2-yp1));
    }
    Double_t phDetWidth = sqrt((zp1-zp2)*(zp1-zp2)+(yp1-yp2)*(yp1-yp2));
    Double_t zmc = (zm1+zm2)/2;
    Double_t ymc = (ym1+ym2)/2;
    Double_t wm = sqrt((zm1-zm2)*(zm1-zm2)+(ym1-ym2)*(ym1-ym2));
    Double_t rm = wm/2/sin(beta);
    Double_t hm = rm*cos(beta);
//    Double_t zm0 = zmc-hm*cos(alpha)-(alBoxSize.Z()/2-alBoxWallThickness-aerogelOffset.Z())*2;
    Double_t zm0 = zmc-hm*cos(alpha)-alBoxSize.Z()/2+aerogelOffset.Z();
    Double_t ym0 = ymc+hm*sin(alpha);
    Double_t theta1 = 360-(alpha+beta)*180/pi;
    Double_t theta2 = 360-(alpha-beta)*180/pi;
    Double_t theta3 = 360-theta2;

    if (verm==0) {
    TGeoPcon* lRichMirrorUpper = new TGeoPcon("richMirrorUpper", theta1, theta2-theta1, 2);
    lRichMirrorUpper->DefineSection(0, -mirrorLength/2, rm, rm + mirrorThickness );
    lRichMirrorUpper->DefineSection(1, +mirrorLength/2, rm, rm + mirrorThickness );
    TGeoTranslation trans1(0,ym0,zm0);
    TGeoRotation mrot1;
    mrot1.SetAngles(90,270,270); 
    TGeoCombiTrans *combi1 = new TGeoCombiTrans("combi1",0,ym0,zm0,&mrot1);
    combi1->RegisterYourself();
    TGeoPcon* lRichMirrorLower = new TGeoPcon("richMirrorLower", theta3, theta2-theta1, 2);
    lRichMirrorLower->DefineSection(0, -mirrorLength/2, rm, rm + mirrorThickness );
    lRichMirrorLower->DefineSection(1, +mirrorLength/2, rm, rm + mirrorThickness );
    TGeoTranslation trans2(0,-ym0,zm0);
    TGeoRotation mrot2;
    mrot2.SetAngles(90,270,270); 
    TGeoCombiTrans *combi2 = new TGeoCombiTrans("combi2",0,-ym0,zm0,&mrot2);
    combi2->RegisterYourself();
    if (useBeamPipeHole)
          TGeoCompositeShape* richMirrorCS = new TGeoCompositeShape("RichMirrorCS",
                                                                    "( richMirrorUpper:combi1 + richMirrorLower:combi2 ) - ( richBeamPipeHole ) ");
    else
          TGeoCompositeShape* richMirrorCS = new TGeoCompositeShape("RichMirrorCS",
                                                                    "( richMirrorUpper:combi1 + richMirrorLower:combi2 ) ");
    TGeoVolume* richMirror = new TGeoVolume("RichMirror",richMirrorCS,
                                              gGeoManager->GetMedium("Mirror"));
    richMirror->SetLineColor(kBlue);
    richMirror->SetTransparency(40);
    alRichBoxAir->AddNode(richMirror, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
    }
    if (verm==1) {
       TVector3 mirrorAxis     = fGeo->mirrorAxis();
       Double_t mirrorRadius   = fGeo->mirrorRadius();
       Double_t mirrorThetaMin = fGeo->mirrorThetaMin()*180/pi;
       Double_t mirrorThetaMax = fGeo->mirrorThetaMax()*180/pi;

       TGeoPcon* lRichMirrorUpper = new TGeoPcon("richMirrorUpper", mirrorThetaMin, mirrorThetaMax-mirrorThetaMin, 2);
       lRichMirrorUpper->DefineSection(0, -mirrorLength/2, mirrorRadius, mirrorRadius + mirrorThickness );
       lRichMirrorUpper->DefineSection(1, +mirrorLength/2, mirrorRadius, mirrorRadius + mirrorThickness );
       Double_t zShift = - alBoxSize.Z()/2 + aerogelOffset.Z();
       std::cout << " " << mirrorAxis.Z() << " " << zShift << std::endl;
       TGeoTranslation trans1(0,mirrorAxis.Y(),mirrorAxis.Z()+zShift);
       TGeoRotation mrot1;
       mrot1.SetAngles(90,270,270); 
       TGeoCombiTrans *combi1 = new TGeoCombiTrans("combi1",0,mirrorAxis.Y(),mirrorAxis.Z()+zShift,&mrot1);
       combi1->RegisterYourself();
       TGeoPcon* lRichMirrorLower = new TGeoPcon("richMirrorLower", -mirrorThetaMax, mirrorThetaMax-mirrorThetaMin, 2);
       lRichMirrorLower->DefineSection(0, -mirrorLength/2, mirrorRadius, mirrorRadius + mirrorThickness );
       lRichMirrorLower->DefineSection(1, +mirrorLength/2, mirrorRadius, mirrorRadius + mirrorThickness );
       TGeoTranslation trans2(0,-mirrorAxis.Y(),mirrorAxis.Z()+zShift);
       TGeoRotation mrot2;
       mrot2.SetAngles(90,270,270); 
       TGeoCombiTrans *combi2 = new TGeoCombiTrans("combi2",0,-mirrorAxis.Y(),mirrorAxis.Z()+zShift,&mrot2);
       combi2->RegisterYourself();
       if (useBeamPipeHole)
          TGeoCompositeShape* richMirrorCS = new TGeoCompositeShape("RichMirrorCS",
                                                                    "( richMirrorUpper:combi1 + richMirrorLower:combi2 ) - ( richBeamPipeHole ) ");
       else
          TGeoCompositeShape* richMirrorCS = new TGeoCompositeShape("RichMirrorCS",
                                                                    "( richMirrorUpper:combi1 + richMirrorLower:combi2 ) ");
       TGeoVolume* richMirror = new TGeoVolume("RichMirror",richMirrorCS,
                                               gGeoManager->GetMedium("Mirror"));
       richMirror->SetLineColor(kBlue);
       richMirror->SetTransparency(40);
       alRichBoxAir->AddNode(richMirror, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
       
       // PhotoDet pars
       yp1 = phDetY[0];
       yp2 = phDetY[1];
       zp1 = phDetZ[0];
       zp2 = phDetZ[1];
       alpham = atan((zp2-zp1)/(yp2-yp1));
       phDetWidth = sqrt((zp1-zp2)*(zp1-zp2)+(yp1-yp2)*(yp1-yp2));
          
    }
   // flat mirror
    if (((verm>=11)&&(verm<=19))||((verm>=21)&&(verm<=29))) {
       UInt_t numberOfSegments = flatMirrorY.size()-1;
       TString richMirrorCSUnity = Form("( ");
       Double_t sag = 0.0;
       // upper side of the mirror
       std::vector<TGeoPcon*> lRichFlatMirrorSag(2*numberOfSegments);
       std::vector<TGeoBBox*> lRichFlatMirror(2*numberOfSegments);
       std::vector<TGeoCombiTrans*> combi(2*numberOfSegments);
       std::vector<TGeoRotation> mrot(2*numberOfSegments);
       for(UInt_t is=0; is<numberOfSegments; is++) {
          
          TString richFlatMirrorName = Form("richFlatMirror%du",is);
          TString combiName = Form("combi%du",is);
          
          richMirrorCSUnity += richFlatMirrorName + ":" + combiName;
          if (is<numberOfSegments)
             richMirrorCSUnity += " + ";
          
          Double_t y1 = flatMirrorY[is];
          Double_t y2 = flatMirrorY[is+1];
          Double_t z1 = flatMirrorZ[is];
          Double_t z2 = flatMirrorZ[is+1];
          Double_t alpha = std::atan((z2-z1)/(y2-y1));
          Double_t mirrorWidth = std::sqrt((y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
          if ( sag == 0 ) {
             Double_t yc = (y1+y2)/2 - mirrorThickness/2*sin(alpha);
             Double_t zc = (z1+z2)/2 - alBoxSize.Z()/2 + aerogelOffset.Z() + mirrorThickness/2*cos(alpha);
             
             lRichFlatMirror.at(is) = new TGeoBBox(richFlatMirrorName, 
                                                   mirrorLength/2,
                                                   mirrorThickness/2,
                                                   mirrorWidth/2 );
             mrot.at(is).SetAngles(0,-90+alpha*180/pi,0);
             combi.at(is) = new TGeoCombiTrans(combiName,0,yc,zc,&mrot.at(is));
             combi.at(is)->RegisterYourself();
          }
          else {
             Double_t rm = ( 4*sag*sag + 1 )*mirrorWidth/8/sag;
             Double_t ym = (y1+y2)/2 + (z2-z1)*(rm/mirrorWidth-sag);
             Double_t zm = (z1+z2)/2 + (y1-y2)*(rm/mirrorWidth-sag);
             Double_t theta1 = std::atan2(y1-ym,z1-zm)*180/pi;
             Double_t theta2 = std::atan2(y2-ym,z2-zm)*180/pi;
             zm += -alBoxSize.Z()/2 + aerogelOffset.Z();
             
             lRichFlatMirrorSag.at(is) = new TGeoPcon(richFlatMirrorName, theta1, theta2-theta1, 2);
             lRichFlatMirrorSag.at(is)->DefineSection(0, -mirrorLength/2, rm, rm + mirrorThickness );
             lRichFlatMirrorSag.at(is)->DefineSection(1, +mirrorLength/2, rm, rm + mirrorThickness );
             
             mrot.at(is).SetAngles(90,270,270);
             combi.at(is) = new TGeoCombiTrans(combiName,0,ym,zm,&mrot.at(is));
             combi.at(is)->RegisterYourself();
          }
       }
       // lower side of the mirror
       for(UInt_t is=0; is<numberOfSegments; is++) {
             
          TString richFlatMirrorName = Form("richFlatMirror%dd",is);
          TString combiName = Form("combi%dd",is);
          
          richMirrorCSUnity += richFlatMirrorName + ":" + combiName;
          if (is<flatMirrorY.size()-2)
             richMirrorCSUnity += " + ";
          
          UInt_t iss = is + numberOfSegments;
          
          Double_t y1 = -flatMirrorY[is];
          Double_t y2 = -flatMirrorY[is+1];
          Double_t z1 = flatMirrorZ[is];
          Double_t z2 = flatMirrorZ[is+1];
          Double_t alpha = std::atan((z2-z1)/(y2-y1));
          Double_t mirrorWidth = std::sqrt((y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
          if ( sag == 0 ) {
             Double_t yc = (y1+y2)/2 - mirrorThickness/2*sin(alpha);
             Double_t zc = (z1+z2)/2 - alBoxSize.Z()/2 + aerogelOffset.Z() + mirrorThickness/2*cos(alpha);
             
             lRichFlatMirror.at(iss) = new TGeoBBox(richFlatMirrorName, 
                                                    mirrorLength/2,
                                                    mirrorThickness/2,
                                                    mirrorWidth/2 );
             mrot.at(iss).SetAngles(0,-90+alpha*180/pi,0);
             combi.at(iss) = new TGeoCombiTrans(combiName,0,yc,zc,&mrot.at(iss));
             combi.at(iss)->RegisterYourself();
          }
          else {
             Double_t rm = ( 4*sag*sag + 1 )*mirrorWidth/8/sag;
             Double_t ym = (y1+y2)/2 + (z1-z2)*(rm/mirrorWidth-sag);
             Double_t zm = (z1+z2)/2 + (y2-y1)*(rm/mirrorWidth-sag);
             Double_t theta1 = std::atan2(y2-ym,z2-zm)*180/pi;
             Double_t theta2 = std::atan2(y1-ym,z1-zm)*180/pi;
             zm += -alBoxSize.Z()/2 + aerogelOffset.Z();
             
             lRichFlatMirrorSag.at(iss) = new TGeoPcon(richFlatMirrorName, theta1, theta2-theta1, 2);
             lRichFlatMirrorSag.at(iss)->DefineSection(0, -mirrorLength/2, rm, rm + mirrorThickness );
             lRichFlatMirrorSag.at(iss)->DefineSection(1, +mirrorLength/2, rm, rm + mirrorThickness );
             
             mrot.at(iss).SetAngles(90,270,270);
             combi.at(iss) = new TGeoCombiTrans(combiName,0,ym,zm,&mrot.at(iss));
             combi.at(iss)->RegisterYourself();
          }
       }
       if (useBeamPipeHole)
          richMirrorCSUnity += " ) - ( richBeamPipeHole ) ";
       else
          richMirrorCSUnity += " ) ";
       std::cout << richMirrorCSUnity << std::endl;
       TGeoCompositeShape* richMirrorCS = new TGeoCompositeShape("RichMirrorCS",richMirrorCSUnity);
       TGeoVolume* richMirror = new TGeoVolume("RichMirror",richMirrorCS,
                                               gGeoManager->GetMedium("Mirror"));
       richMirror->SetLineColor(kBlue);
       richMirror->SetTransparency(40);
       alRichBoxAir->AddNode(richMirror, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
       
       // PhotoDet pars
       yp1 = phDetY[0];
       yp2 = phDetY[1];
       zp1 = phDetZ[0];
       zp2 = phDetZ[1];
       for(UInt_t is=0; is<numberOfSegments; is++)
          zp2 = zp2>flatMirrorZ[is] ? zp2 : flatMirrorZ[is];
       alpham = atan((zp2-zp1)/(yp2-yp1));
       phDetWidth = sqrt((zp1-zp2)*(zp1-zp2)+(yp1-yp2)*(yp1-yp2));
          
       // Side parts of mirror
       if ((verm>=21)&&(verm<=29)) {
          // left
          Double_t x_min = mirrorLength/2;
          Double_t x_max = x_min + mirrorThickness;
          Double_t y_min = -yp2;
          Double_t y_max = yp2;
          Double_t z_min = zp1;
          Double_t z_max = zp2;
          Double_t x_size = x_max - x_min;
          Double_t y_size = y_max - y_min;
          Double_t z_size = z_max - z_min;
          Double_t x_mid = (x_max + x_min)/2;
          Double_t y_mid = (y_max + y_min)/2;
          Double_t z_mid = (z_max + z_min)/2 - alBoxSize.Z()/2 + aerogelOffset.Z();
          TGeoBBox* lRichMirrorLeft = new TGeoBBox("richMirrorLeft",
                                                   x_size/2,
                                                   y_size/2,
                                                   z_size/2 );
          TGeoVolume *richMirrorLeft = new TGeoVolume("RichMirrorLeft",
                                                      lRichMirrorLeft, 
                                                      gGeoManager->GetMedium("Mirror"));
          richMirrorLeft->SetLineColor(kBlue);
          richMirrorLeft->SetTransparency(40);
          alRichBoxAir->AddNode(richMirrorLeft, 0, new TGeoCombiTrans(x_mid, y_mid, z_mid, new TGeoRotation (0)));
       
          // right
          x_max = -mirrorLength/2;
          x_min = x_max - mirrorThickness;
          x_mid = (x_max + x_min)/2;
          TGeoBBox* lRichMirrorRight = new TGeoBBox("richMirrorRight",
                                                    x_size/2,
                                                    y_size/2,
                                                    z_size/2 );
          TGeoVolume *richMirrorRight = new TGeoVolume("RichMirrorRight",
                                                       lRichMirrorRight, 
                                                       gGeoManager->GetMedium("Mirror"));
          richMirrorRight->SetLineColor(kBlue);
          richMirrorRight->SetTransparency(40);
          alRichBoxAir->AddNode(richMirrorRight, 0, new TGeoCombiTrans(x_mid, y_mid, z_mid, new TGeoRotation (0)));
       }
   
    }

    // Photodetertors
    Double_t windowThickness = 0.02;
    Double_t siThickness = 0.025;
    Double_t stefThickness = 0.1;
    Double_t gapThickness = 0.5;
    Double_t stef2Thickness = 0.3;
    // Upper
    TGeoBBox* lRichPhDetUpper = new TGeoBBox("richPhDetUpper", 
                                             phDetLength/2,
                                             phDetThickness/2,
                                             phDetWidth/2 );
    TGeoVolume *richPhDetUpper = new TGeoVolume("RichPhDetUpper",
                                                lRichPhDetUpper, 
                                                gGeoManager->GetMedium("RichAir"));
    richPhDetUpper->SetLineColor(19);
    richPhDetUpper->SetTransparency(40);
    TGeoRotation prot1;
    prot1.SetAngles(0,-90+alpham*180/pi,0);
    Double_t ypc = (yp1+yp2)/2+phDetThickness/2*sin(alpham);
    Double_t zpc = (zp1+zp2)/2 - alBoxSize.Z()/2 + aerogelOffset.Z() -phDetThickness/2*cos(alpham);
    alRichBoxAir->AddNode(richPhDetUpper, 0, 
                          new TGeoCombiTrans( 0, 
                                              ypc, 
                                              zpc,
                                              new TGeoRotation(prot1) ) );
    // Inner layers
    Double_t dir = 1;
    Double_t y_shift = -dir*phDetThickness/2;
    Double_t thickness = 0;
    // Window
    y_shift += dir*thickness/2;
    thickness = windowThickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetWindowUpper = new TGeoBBox("richPhDetWindowUpper",
                                                   phDetLength/2,
                                                   thickness/2,
                                                   phDetWidth/2 );
    TGeoVolume *richPhDetWindowUpper = new TGeoVolume("RichPhDetWindowUpper",lRichPhDetWindowUpper, gGeoManager->GetMedium("RichPDWindow"));
    richPhDetWindowUpper->SetLineColor(kGreen);
    richPhDetWindowUpper->SetTransparency(40); 
    richPhDetUpper->AddNode(richPhDetWindowUpper, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );
    // Si
    y_shift += dir*thickness/2;
    thickness = siThickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetSiUpper = new TGeoBBox("richPhDetSiUpper",
                                               phDetLength/2,
                                               thickness/2,
                                               phDetWidth/2 );
    TGeoVolume *richPhDetSiUpper = new TGeoVolume("RichPhDetSiUpperSensor",lRichPhDetSiUpper, gGeoManager->GetMedium("silicon"));
    richPhDetSiUpper->SetLineColor(kRed);
    richPhDetSiUpper->SetTransparency(40); 
    richPhDetUpper->AddNode(richPhDetSiUpper, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );
    // STEF
    y_shift += dir*thickness/2;
    thickness = stefThickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetSTEFUpper = new TGeoBBox("richPhDetSTEFUpper",
                                                 phDetLength/2,
                                                 thickness/2,
                                                 phDetWidth/2 );
    TGeoVolume *richPhDetSTEFUpper = new TGeoVolume("RichPhDetSTEFUpper",lRichPhDetSTEFUpper, gGeoManager->GetMedium("pcbmvd"));
    richPhDetSTEFUpper->SetLineColor(kYellow);
    richPhDetSTEFUpper->SetTransparency(40); 
    richPhDetUpper->AddNode(richPhDetSTEFUpper, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );
    // electronics STEF
    y_shift += dir*(thickness/2+gapThickness);
    thickness = stef2Thickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetSTEF2Upper = new TGeoBBox("richPhDetSTEF2Upper",
                                                  phDetLength/2,
                                                  thickness/2,
                                                  phDetWidth/2 );
    TGeoVolume *richPhDetSTEF2Upper = new TGeoVolume("RichPhDetSTEF2Upper",lRichPhDetSTEF2Upper, gGeoManager->GetMedium("pcbmvd"));
    richPhDetSTEF2Upper->SetLineColor(kYellow);
    richPhDetSTEF2Upper->SetTransparency(40); 
    richPhDetUpper->AddNode(richPhDetSTEF2Upper, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );
   
    // Lower
    TGeoBBox* lRichPhDetLower = new TGeoBBox("richPhDetLower", 
                                             phDetLength/2,
                                             phDetThickness/2,
                                             phDetWidth/2 );
    TGeoVolume *richPhDetLower = new TGeoVolume("RichPhDetLower",
                                                lRichPhDetLower, 
                                                gGeoManager->GetMedium("RichAir"));
    richPhDetLower->SetLineColor(19);
    richPhDetLower->SetTransparency(40);
    TGeoRotation prot2;
    prot2.SetAngles(0,+90-alpham*180/pi,0);
    alRichBoxAir->AddNode(richPhDetLower, 0, 
                          new TGeoCombiTrans( 0, 
                                              -ypc, 
                                              zpc,
                                              new TGeoRotation(prot2) ) );
    // Inner layers
    dir = -1;
    y_shift = -dir*phDetThickness/2;
    thickness = 0;
    // Window
    y_shift += dir*thickness/2;
    thickness = windowThickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetWindowLower = new TGeoBBox("richPhDetWindowLower",
                                                   phDetLength/2,
                                                   thickness/2,
                                                   phDetWidth/2 );
    TGeoVolume *richPhDetWindowLower = new TGeoVolume("RichPhDetWindowLower",lRichPhDetWindowLower, gGeoManager->GetMedium("RichPDWindow"));
    richPhDetWindowLower->SetLineColor(kGreen);
    richPhDetWindowLower->SetTransparency(40); 
    richPhDetLower->AddNode(richPhDetWindowLower, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );
    // Si
    y_shift += dir*thickness/2;
    thickness = siThickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetSiLower = new TGeoBBox("richPhDetSiLower",
                                               phDetLength/2,
                                               thickness/2,
                                               phDetWidth/2 );
    TGeoVolume *richPhDetSiLower = new TGeoVolume("RichPhDetSiLowerSensor",lRichPhDetSiLower, gGeoManager->GetMedium("silicon"));
    richPhDetSiLower->SetLineColor(kRed);
    richPhDetSiLower->SetTransparency(40); 
    richPhDetLower->AddNode(richPhDetSiLower, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );
    // STEF
    y_shift += dir*thickness/2;
    thickness = stefThickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetSTEFLower = new TGeoBBox("richPhDetSTEFLower",
                                                 phDetLength/2,
                                                 thickness/2,
                                                 phDetWidth/2 );
    TGeoVolume *richPhDetSTEFLower = new TGeoVolume("RichPhDetSTEFLower",lRichPhDetSTEFLower, gGeoManager->GetMedium("pcbmvd"));
    richPhDetSTEFLower->SetLineColor(kYellow);
    richPhDetSTEFLower->SetTransparency(40); 
    richPhDetLower->AddNode(richPhDetSTEFLower, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );
    // electronics STEF
    y_shift += dir*(thickness/2+gapThickness);
    thickness = stef2Thickness;
    y_shift += dir*thickness/2;
    TGeoBBox* lRichPhDetSTEF2Lower = new TGeoBBox("richPhDetSTEF2Lower",
                                                  phDetLength/2,
                                                  thickness/2,
                                                  phDetWidth/2 );
    TGeoVolume *richPhDetSTEF2Lower = new TGeoVolume("RichPhDetSTEF2Lower",lRichPhDetSTEF2Lower, gGeoManager->GetMedium("pcbmvd"));
    richPhDetSTEF2Lower->SetLineColor(kYellow);
    richPhDetSTEF2Lower->SetTransparency(40); 
    richPhDetLower->AddNode(richPhDetSTEF2Lower, 0, new TGeoCombiTrans( 0, y_shift, 0, new TGeoRotation(0) ) );

   //--------------------------------------------------------------------------------------------- 
 

  // create top volume:
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoBBox*   lTop = new TGeoBBox(500,500,richOffset.Z() + alBoxSize.Z());
  TGeoVolume* top = new TGeoVolume("RICH", lTop, gGeoManager->GetMedium("RichAir"));
  gGeoManager->SetTopVolume(top);
  
  top->AddNode(alRichBox, 0,
               new TGeoCombiTrans( richOffset.X(), 
                                   richOffset.Y(), 
                                   richOffset.Z() + alBoxSize.Z()/2,
                                   new TGeoRotation(0) ) );
      
  gGeoManager->CloseGeometry();

  top->CheckOverlaps(0.0001, "");
  gGeoManager->CheckOverlaps(0.00001,""); // [cm]
  gGeoManager->SetVisLevel(4);
  //gGeoManager->CheckGeometryFull();

  top->Write();
  fi->Close(); 
  top->Draw("ogl");
  //pdbase->Draw("ogl"); 
  
  TObjArray *listOfOverlaps = gGeoManager->GetListOfOverlaps();
  cout<<listOfOverlaps->GetEntries()<<endl;
  listOfOverlaps->Print();


}
