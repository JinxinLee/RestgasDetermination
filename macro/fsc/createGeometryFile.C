// ALL DIMENSIONS IN mm !!!!!!!

{
  TString outputName = "../../geometry/fsc.geo";
  ofstream ofs;
  ofs.open(outputName,ios::out);

  // based on "ConstructEmc3() from the old code by Piotr Hawranek"

  double     beamlineangle =  -TMath::DegToRad()*0.0; //in rad
  double     zpos          =  6900.0; 
  int         numcellsx     =  28;
  int         numcellsy     =  14;
  double     hole_xsize    =  300.0;
  double     hole_ysize    =  200.0;
  double     hole_xpos     = -120.0;
  double     hole_ypos     =  0.0;
  double     crystalspace  =  0.1;
  double     Absorb_xsize  =  110.0;
  double     Absorb_ysize  =  110.0;
  double     Air_thickness =  0.0375;
  double     Pb_thickness  =  0.275; 
  double     Sci_thickness =  1.5;
  double     Sci_xsize     =  110.0;
  double     Sci_ysize     =  110.0;
  int         EMCnumlayers  =  300;   

  //Piotr's temporary for benchmark production params are freezed! 
  //they'll overwrite previous values if uncommented !!!
  /*
  zpos          = 6900;
  numcellsx     = 14;
  numcellsy     = 14;
  Absorb_xsize  = 110;
  Absorb_ysize  = 110;
  Air_thickness = 0.0375;
  Pb_thickness  = 0.275;
  Sci_thickness = 1.5;
  Sci_xsize     = 110;
  Sci_ysize     = 110;
  EMCnumlayers  = 300;
  hole_xsize    = 300;
  hole_ysize    = 200;
  hole_xpos     = -120.0;
  hole_ypos     = 0;
  crystalspace  = 0.1;*/

  // values for simple tests (to see how geo works...)
//  EMCnumlayers = 3;
//  numcellsx     = 4;
//  numcellsy     = 1;
//  Sci_xsize     = 50;
//  Sci_ysize     = 50;
//  Absorb_xsize  = 50;
//  Absorb_ysize  = 50;
//  crystalspace  = 0.5;
//  hole_xsize    = 0;
//  hole_ysize    = 0;
//  Pb_thickness  = 10.;
//  Air_thickness = 5.;
//  Sci_thickness = 25;

  
  double  cellxsize = TMath::Max(Absorb_xsize,Sci_xsize)+crystalspace;
  double  cellysize = TMath::Max(Absorb_ysize,Sci_ysize)+crystalspace;
  double  xsize     = cellxsize*numcellsx;
  double  ysize     = cellysize*numcellsy;
    
  double  cell_thickness = 2*Air_thickness+Pb_thickness+Sci_thickness;
  double  thickness      = cell_thickness*EMCnumlayers;

  // printing out the key values on the screen...
  cout<<"  Constructing geo file for EMC of the FS shashlyk"<<endl;
  cout<<"      cellxsize      = "<<cellxsize/10.<<" cm"<<endl;
  cout<<"      cellysize      = "<<cellysize/10.<<" cm"<<endl;
  cout<<"      crystalspace   = "<<crystalspace/10.<<" cm"<<endl;
  cout<<"      thickness of Air between Pb and Sci = "<<Air_thickness/10.<<" cm"<<endl;
  cout<<"      Pb thickness   = "<<Pb_thickness/10.<<" cm"<<endl;
  cout<<"      Sci thickness  = "<<Sci_thickness/10.<<" cm"<<endl;
  cout<<"      Num of layers  = "<<EMCnumlayers<<endl;
  cout<<"      thickness      = "<<thickness/10.<<" cm"<<endl;
  cout<<"      numcellsx      = "<<numcellsx<<endl;
  cout<<"      numcellsy      = "<<numcellsy<<endl;
  cout<<"      xsize          = "<<xsize/10.<<" cm"<<endl;
  cout<<"      ysize          = "<<ysize/10.<<" cm"<<endl;
  cout<<"      z position     = "<<zpos/10.<<" cm"<<endl;
  cout<<"      hole xsize     = "<<hole_xsize/10.<<" cm"<<endl;
  cout<<"      hole ysize     = "<<hole_ysize/10.<<" cm"<<endl;
  cout<<"      hole xpos      = "<<hole_xpos/10.<<" cm"<<endl;
  cout<<"      hole ypos      = "<<hole_ypos/10.<<" cm"<<endl;
  // and to the geo file
  ofs<<"//  Constructing geo file for EMC of the FS shashlyk"<<endl;
  ofs<<"//      cellxsize      = "<<cellxsize/10.<<" cm"<<endl;
  ofs<<"//      cellysize      = "<<cellysize/10.<<" cm"<<endl;
  ofs<<"//      crystalspace   = "<<crystalspace/10.<<" cm"<<endl;
  ofs<<"//      thickness of Air between Pb and Sci = "<<Air_thickness/10.<<" cm"<<endl;
  ofs<<"//      Pb thickness   = "<<Pb_thickness/10.<<" cm"<<endl;
  ofs<<"//      Sci thickness  = "<<Sci_thickness/10.<<" cm"<<endl;
  ofs<<"//      Num of layers  = "<<EMCnumlayers<<endl;
  ofs<<"//      thickness      = "<<thickness/10.<<" cm"<<endl;
  ofs<<"//      numcellsx      = "<<numcellsx<<endl;
  ofs<<"//      numcellsy      = "<<numcellsy<<endl;
  ofs<<"//      xsize          = "<<xsize/10.<<" cm"<<endl;
  ofs<<"//      ysize          = "<<ysize/10.<<" cm"<<endl;
  ofs<<"//      z position     = "<<zpos/10.<<" cm"<<endl;
  ofs<<"//      hole xsize     = "<<hole_xsize/10.<<" cm"<<endl;
  ofs<<"//      hole ysize     = "<<hole_ysize/10.<<" cm"<<endl;
  ofs<<"//      hole xpos      = "<<hole_xpos/10.<<" cm"<<endl;
  ofs<<"//      hole ypos      = "<<hole_ypos/10.<<" cm\n\n"<<endl;

  TRotation beamlinerot;
  beamlinerot.RotateY(beamlineangle);
  TString beamlinerotmatrix="";
  beamlinerotmatrix+=beamlinerot.XX();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.XY();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.XZ();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.YX();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.YY();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.YZ();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.ZX();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.ZY();
  beamlinerotmatrix+=" \t ";
  beamlinerotmatrix+=beamlinerot.ZZ();

  TVector3 shiftZ(0,0,zpos);
  TVector3 curShift;
  TString separator ="//************************";
  TString tab       = "\t";
  TString nullRot   = "1. 0. 0. 0. 1. 0. 0. 0. 1.";
  TString modName   = "fscCrystal1#";
  TString curName   = "";
  int       modNo     = 1;

  //building main container
   ofs<<separator<<endl;
  ofs<<"fscCrystal1000 \ncave \nBOX \nair"<<endl;
  ofs<<  xsize/2 <<tab<< -ysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  xsize/2 <<tab<<  ysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -xsize/2 <<tab<<  ysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -xsize/2 <<tab<< -ysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  xsize/2 <<tab<< -ysize/2 <<tab<<  thickness<<endl;
  ofs<<  xsize/2 <<tab<<  ysize/2 <<tab<<  thickness<<endl;
  ofs<< -xsize/2 <<tab<<  ysize/2 <<tab<<  thickness<<endl;
  ofs<< -xsize/2 <<tab<< -ysize/2 <<tab<<  thickness<<endl;
  ofs<< shiftZ.X() <<tab<< shiftZ.Y() <<tab<< shiftZ.Z() <<endl;
  ofs<< beamlinerotmatrix <<endl;
  ofs<<separator<<endl;
  //building container of the first complete "crystal"
  curName = modName;
  curName += 101;
  ofs<<curName<<"\nfscCrystal1000 \nBOX \nair"<<endl;
  ofs<<  cellxsize/2 <<tab<< -cellysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  cellxsize/2 <<tab<<  cellysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -cellxsize/2 <<tab<<  cellysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -cellxsize/2 <<tab<< -cellysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  cellxsize/2 <<tab<< -cellysize/2 <<tab<< thickness<<endl;
  ofs<<  cellxsize/2 <<tab<<  cellysize/2 <<tab<< thickness<<endl;
  ofs<< -cellxsize/2 <<tab<<  cellysize/2 <<tab<< thickness<<endl;
  ofs<< -cellxsize/2 <<tab<< -cellysize/2 <<tab<< thickness<<endl;
  curShift.SetXYZ(-xsize/2+cellxsize/2,ysize/2-cellysize/2,0);
  ofs<< curShift.X() <<tab<< curShift.Y() <<tab<< curShift.Z() <<endl;
  ofs<< nullRot <<endl;
  ofs<<separator<<endl;
  // filling in the first "crystal" with lead and scintillator
  TString namePb="fsc01#1";
  ofs<<namePb<<"\n"<<curName<<"\nBOX \nlead"<<endl;
  ofs<<  Absorb_xsize/2 <<tab<< -Absorb_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  Absorb_xsize/2 <<tab<<  Absorb_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -Absorb_xsize/2 <<tab<<  Absorb_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -Absorb_xsize/2 <<tab<< -Absorb_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  Absorb_xsize/2 <<tab<< -Absorb_ysize/2 <<tab<< Pb_thickness<<endl;
  ofs<<  Absorb_xsize/2 <<tab<<  Absorb_ysize/2 <<tab<< Pb_thickness<<endl;
  ofs<< -Absorb_xsize/2 <<tab<<  Absorb_ysize/2 <<tab<< Pb_thickness<<endl;
  ofs<< -Absorb_xsize/2 <<tab<< -Absorb_ysize/2 <<tab<< Pb_thickness<<endl;
  //ofs<< curShift.X() <<tab<< curShift.Y() <<tab<< curShift.Z() <<endl;
  ofs<< 0.0 <<tab<< 0.0 <<tab<< 0.0 <<endl;
  ofs<< nullRot <<endl;
  ofs<<separator<<endl;
  TString nameSci="fsc02#1";
  ofs<<nameSci<<"\n"<<curName<<"\nBOX \nFscScint"<<endl;
  ofs<<  Sci_xsize/2 <<tab<< -Sci_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  Sci_xsize/2 <<tab<<  Sci_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -Sci_xsize/2 <<tab<<  Sci_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<< -Sci_xsize/2 <<tab<< -Sci_ysize/2 <<tab<<  "0.0"<<endl;
  ofs<<  Sci_xsize/2 <<tab<< -Sci_ysize/2 <<tab<< Sci_thickness<<endl;
  ofs<<  Sci_xsize/2 <<tab<<  Sci_ysize/2 <<tab<< Sci_thickness<<endl;
  ofs<< -Sci_xsize/2 <<tab<<  Sci_ysize/2 <<tab<< Sci_thickness<<endl;
  ofs<< -Sci_xsize/2 <<tab<< -Sci_ysize/2 <<tab<< Sci_thickness<<endl;
  ofs<< 0.0 <<tab<< 0.0 <<tab<< 0.0+Pb_thickness+Air_thickness <<endl;
  ofs<< nullRot <<endl;
  ofs<<separator<<endl;
  
  //placing subsequent layers
  for(int lay=2; lay<=EMCnumlayers; lay++){
    namePb  = "fsc01#";
    nameSci = "fsc02#";
    namePb  += lay;
    nameSci += lay;
    ofs<<namePb<<"\n"<<curName<<endl;
    ofs<<0.0 <<tab<< 0.0 <<tab<< cell_thickness*(lay-1) <<endl;
    ofs<< nullRot <<endl;
    ofs<<separator<<endl;
    ofs<<nameSci<<"\n"<<curName<<endl;
    ofs<<0.0 <<tab<< 0.0 <<tab<<0.0 + cell_thickness*(lay-1)+Pb_thickness+Air_thickness <<endl;
    ofs<< nullRot <<endl;
    ofs<<separator<<endl;
  }
  
  cout<<"X min dist = "<<hole_xsize/2+cellxsize/2<<endl;
  cout<<"Y min dist = "<<hole_ysize/2+cellysize/2<<endl;

  // placing other "crystals"
  for(int yNo=1; yNo<=numcellsy; yNo++){
    for(int xNo=1; xNo<=numcellsx; xNo++){
      
      if(xNo==1 && yNo==1) continue;

      modNo++;
      curName = modName;
      Int_t copyNo=100*xNo+yNo;
      curName+=copyNo;
      //curName += modNo;
      curShift.SetXYZ(-xsize/2+(xNo-0.5)*cellxsize, 
		      ysize/2-(yNo-0.5)*cellysize,
		      0.);      
      //      curShift += shiftZ;
      //exclude "crystals" overlaping with where the hole should be
      if(TMath::Abs(curShift.X()-hole_xpos)<hole_xsize/2+cellxsize/2 &&
	 TMath::Abs(curShift.Y()-hole_ypos)<hole_ysize/2+cellysize/2 ){
	modNo--;
	cout<<"Rejected crystal centred in "<<curShift.X()<<"\t"<<curShift.X()<<endl;
	continue;
      }
      ofs<<curName<<"\nfscCrystal1000"<<endl;
      ofs<<curShift.X() <<tab<< curShift.Y() <<tab<< curShift.Z() <<endl;
      ofs<< nullRot <<endl;
      ofs<<separator<<endl;
      

    } // end of loop over columns xNo
  } // end of loop over rows yNo
  cout<<modNo<<" \"crystals\" created"<<endl;
}
