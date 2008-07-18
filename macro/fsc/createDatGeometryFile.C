// ALL DIMENSIONS IN mm !!!!!!!

{
  TString outputName = "../../geometry/fsc.dat";
  ofstream ofs;
  ofs.open(outputName,ios::out);

  // based on "ConstructEmc3() from the old code by Piotr Hawranek"

  double     beamlineangle =  -TMath::DegToRad()*0.0; //in rad
  double     zpos          =  6900.0; // all positions in mm
  int         numcellsx     =  28;
  int         numcellsy     =  14;
  double     hole_xsize    =  200.0;
  double     hole_ysize    =  200.0;
  double     hole_xpos     = -0.0;
  double     hole_ypos     =  0.0;
  double     crystalspace  =  0.1;
  double     Absorb_xsize  =  110.0;
  double     Absorb_ysize  =  110.0;
  double     Air_thickness =  0.0375;
  double     Pb_thickness  =  0.275;
  double     Sci_thickness =  1.5;
  double     Sci_xsize     =  110.0;
  double     Sci_ysize     =  110.0;
  int         EMCnumlayers  =  378;   

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
 
  int       module     = 5;
  TString tab="\t";
  cout<<"X min dist = "<<hole_xsize/2+cellxsize/2<<endl;
  cout<<"Y min dist = "<<hole_ysize/2+cellysize/2<<endl;
  ofs << "module\trow\tcrystal\ttheta\tphi\ttau\tposX\t\tposY\tposZ\tpDz\tpTheta\tpPhi\tpDy1\tpDx1\tpDx2\tpAlp1\tpDy2\tpDx3\tpDx4\tpAlp2" << endl;
  ofs << "#\t#\t#\t[deg]\t[deg]\t[deg]\t[mm]\t\t[mm]\t[mm]\t[mm]\t[deg]\t[deg]\t[mm]\t[mm]\t[mm]\t[mm]\t[mm]\t[mm]\t[mm]\t[mm]" << endl << endl;
  
  Float_t crX, crY, crZ;
  // placing other "crystals"
  for(int yNo=1; yNo<=numcellsy/2; yNo++){
    for(int xNo=1; xNo<=numcellsx/2; xNo++){
      crX =  -(xNo-0.5)*cellxsize;
      crY = (yNo-0.5)*cellysize;
      crZ = zpos;
      
      if(TMath::Abs(crX-hole_xpos)<hole_xsize/2+cellxsize/2. &&
	 TMath::Abs(crY-hole_ypos)<hole_ysize/2+cellysize/2. ) 
	{
	  cout<<"Rejected crystal centred in "<<crX <<"\t"<<crY <<endl;
	continue;  
	}
	
      ofs << module << tab << xNo << tab << yNo << tab << 0. << tab << 0. << tab << 0. << tab; // module row and crystal
      ofs << crX << "  \t" << crY << tab << crZ << tab; // XYZ positions
      ofs << Air_thickness << tab << 0. << tab << 0. << tab <<  Absorb_ysize/2. <<   tab <<Absorb_xsize/2. << tab << Sci_xsize/2. << tab << Pb_thickness/2. << tab << Sci_ysize/2. << tab <<  EMCnumlayers << tab << 0. << tab << Sci_thickness/2. << endl;
      cout  << module << tab << xNo << tab << yNo << endl;
    } // end of loop over columns xNo
  } // end of loop over rows yNo
  ofs.close();
  
  

}
