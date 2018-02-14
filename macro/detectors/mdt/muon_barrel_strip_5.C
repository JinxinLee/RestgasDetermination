int muon_barrel_strip_5bis()
{
  TFile *out = TFile::Open("output.root","RECREATE");

  TGeoManager *geom = new TGeoManager("Assembly", "");

  Float_t MDT_barrel_long  = 390.; 
  Float_t MDT_barrel_short = 362.; 
  Float_t MDT_barrel_short_up; 
  Float_t MDT_barrel_short_down; 
  Float_t MDT_width     =  8.34;
  Float_t MDT_thickness =  1.23; 
  Float_t XSize         =  8.06;
  Float_t YSize         =  1.03;
  Float_t XSize1        =  0.94;
  Float_t YSize1        =  0.97;

  TGeoMaterial *mat1 = new TGeoMaterial("vacuum", 1.e-16, 1.e-16, 1.e-16);
  TGeoMaterial *mat2 = new TGeoMaterial("aluminium", 26.98, 13, 2.7);  
  TGeoMixture  *mat3 = new TGeoMixture("air", 3, 0.001205);
  mat3->DefineElement(0, 39.35, 18,  0.01);
  mat3->DefineElement(1, 16.00,  8,  0.21);
  mat3->DefineElement(2, 14.01,  7,  0.78);
  TGeoMixture  *mat4 = new TGeoMixture("MDTMixture", 3, 0.0019);
  mat4->DefineElement(0, 39.948,  18,  0.476);
  mat4->DefineElement(1, 12.010,   6,  0.143);
  mat4->DefineElement(2, 15.9994,  8,  0.381);
  TGeoMixture *mat5 = new TGeoMixture("MDTPlastic", 2, 1.032);
  mat5->DefineElement(0, 12.01,  6,  9);
  mat5->DefineElement(1, 1.008,  1,  10);
  TGeoMaterial *mat6 = new TGeoMaterial("iron", 55.847, 26, 7.87);  
  TGeoMixture *mat7 = new TGeoMixture("G10", 4, 1.7);
  mat7->DefineElement(0, 12.01,  6,  .259);
  mat7->DefineElement(1, 1.008,  1,  .288);
  mat7->DefineElement(2, 16,     8,  .248);
  mat7->DefineElement(3, 28.09, 14,  .205);

    
  TGeoMedium *med1 = new TGeoMedium("vacuum",     1, mat1);
  TGeoMedium *med2 = new TGeoMedium("aluminium",  2, mat2);
  TGeoMedium *med3 = new TGeoMedium("Air",        3, mat3);
  TGeoMedium *med4 = new TGeoMedium("MDTMixture", 4, mat4);
  TGeoMedium *med5 = new TGeoMedium("MDTPlastic", 5, mat5);
  TGeoMedium *med6 = new TGeoMedium("iron",       6, mat6);
  TGeoMedium *med7 = new TGeoMedium("G10",        7, mat7);

  TGeoVolume *top    = new TGeoVolumeAssembly("top");
  TGeoVolume *MdtOct0    = new TGeoVolumeAssembly("MdtBarrelOct0");
  TGeoVolume *MdtOct2    = new TGeoVolumeAssembly("MdtBarrelOct2");
  TGeoVolume *MdtOct6    = new TGeoVolumeAssembly("MdtBarrelOct6");

  TGeoVolume *mnk[14], *mnk1[14], *mnk2[14], *mnk3[6], *mnk4[6], *mnk5[18];
  TGeoVolume *tube[16][3], *tube_en_l0[44][3], *tube_en_l2[68][3], *tube_en_l3[68][3], *tube_en_l4[68][3], 
              *tube_en_l5[68][3], *tube_en_l6[72][3], *tube_fw[3][3];
    
  for(int i=0;i<14;i++) {
   mnk[i]  = new TGeoVolumeAssembly(Form("MdtBarrelOct0Layer%d",i));
   mnk1[i] = new TGeoVolumeAssembly(Form("MdtBarrelOct2Layer%d",i));
   mnk2[i] = new TGeoVolumeAssembly(Form("MdtBarrelOct6Layer%d",i));
  }

  for(int i=0;i<7;i++) {
   mnk3[i]  = new TGeoVolumeAssembly(Form("MdtEndcapLayer%d",i));
  }

  for(int i=0;i<4;i++) {
   mnk4[i]  = new TGeoVolumeAssembly(Form("MdtMFLayer%d",i));
  }

  for(int i=0;i<18;i++) {
   mnk5[i]  = new TGeoVolumeAssembly(Form("MdtForwardLayer%d",i));
  }

  
  TGeoRotation *rot0 = new TGeoRotation();
  rot0->RotateZ(-90);
  rot0->RegisterYourself();
  TGeoRotation *rot1 = new TGeoRotation();
  rot1->RotateZ(-45);
  rot1->RegisterYourself();
  TGeoRotation *rot2 = new TGeoRotation();
  rot2->RotateZ(0);
  rot2->RegisterYourself();
  TGeoRotation *rot3 = new TGeoRotation();
  rot3->RotateZ(45);
  rot3->RegisterYourself();
  TGeoRotation *rot4 = new TGeoRotation();
  rot4->RotateZ(90);
  rot4->RegisterYourself();
  TGeoRotation *rot5 = new TGeoRotation();
  rot5->RotateZ(135);
  rot5->RegisterYourself();
  TGeoRotation *rot6 = new TGeoRotation();
  rot6->RotateZ(180);
  rot6->RegisterYourself();
  TGeoRotation *rot7 = new TGeoRotation();
  rot7->RotateZ(225);
  rot7->RegisterYourself();
  
 
  geom->SetTopVolume(top);
   
   TGeoVolume *pCombi  = new TGeoVolumeAssembly("MdtBarrel");
   TGeoVolume *pCombi1  = new TGeoVolumeAssembly("MdtEndcap");
   TGeoVolume *pCombi2  = new TGeoVolumeAssembly("MdtMF");
   TGeoVolume *pCombi3  = new TGeoVolumeAssembly("MdtForward");
   float pos_x, pos_y, pos_z, zl, slotpos_x;
   float slotpos_y = -0.03;

    TGeoRotation    *rr1 = new TGeoRotation();
    rr1->RotateZ(0);
    rr1->RotateY(0);
    rr1->RotateX(90);
    rr1->SetName("rr1");
    rr1->RegisterYourself();

    TGeoRotation    *rr0 = new TGeoRotation();
    rr0->RotateZ(90);
    rr0->RotateY(90);
    rr0->SetName("rr0");
    rr0->RegisterYourself();

    float mm[14][3] = { 53.05,0,0,-57.95,0,0,-56.965,-31.45,45.05,59.438,33.938,-42.562,-61.92,-36.42,48.58,
                       64.4,38.9,-46.1,-66.89,-41.39,52.11,69.38,43.88,-49.62,-71.86,-46.36,55.64,74.35,48.85,
                       -61.65,-76.83,-51.33,59.17,79.32,53.82,-65.18,-81.8,-56.3,62.7,84.29,58.79,-68.71 };

    int mm1[14][3] = { 14,0,0,14,0,0,14,2,12,14,2,12,15,2,13,16,2,13,16,2,14,17,2,14, 
                       17,2,15,18,2,16,19,2,16,19,2,17,20,2,17,20,2,18};

//////////////////// 0,1 -> Oct#0,1,3,4,5,7,8 //////
//////////////////// 2-7 ->Oct#2 
///////////////////  8-15 ->Oct#6
    float tube_len[16] = { 390, 362, 158.5, 172.5, 68.55, 45.0, 217.5, 127.5, 212.5, 121.5, 165.5, 
                    179.5, 89.5, 75.5, 217.5, 127.5 };
///////////////////////////////////////////////////////////
   for(int i=0; i<16; i++) { 
     tube[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len[i]/2); 
     tube[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len[i]/2);
     for(int j = 0; j < 8; j++){
     float slotpos_x = (-3.5 + 1.*j);
       tube[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",1,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len[i]/2);
       tube[i][1]->AddNode(tube[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));
     }
     tube[i][0]->AddNode(tube[i][1],1); 
   }

///////////// Strip board for Oct#0,1,3-5,7,8 /////////////////
  TGeoTranslation* tr[14][5];
  float limx[14][4] = {0.,0.,0.,0.,0.,0.,0.,0., 50.7, -46.7, 50.7, -46.7, 46.6, -50.8, 46.6, -50.8, 55., -50.7, 55., -50.7, 55.2, -55., 55.2, -55.,
                      59.2, -55.1, 59.2, -55.1, 59.2, -59.2, 59.2, -59.2, 63.5, -59.2, 63.5, -59.2,, 63.6, -67.8, 63.6, -67.8, 68, -67.8, 68, -67.8,
                      68, -72, 68, -72, 72.1, -72, 72.1, -72, 72.3, -76.3, 72.3, -76.3}; 
  float limz[3][4] = {0,0,0,0,0,0,0,0, 188, 188, -188, -188 };
  
    for(int i=0; i<14; i++) {
     for(int j=0; j<4; j++) {
       if(i<2) {tr[i][j] = new TGeoTranslation(limx[i][j], 0, 0); }
       else {tr[i][j] = new TGeoTranslation(limx[i][j], 0, limz[2][j]);}
       tr[i][j]->SetName(Form("tr%dL%d",j,i));
       tr[i][j]->RegisterYourself();
     }
    }

    TGeoBBox *L[8], *al_L[8];    
    for(int i=2; i<9 ; i++){ 
     L[i] = new TGeoBBox(Form("L%d",i),   ((12+i)*8.5-0.6)/2, 0.075,  MDT_barrel_long/2);
     al_L[i] = new TGeoBBox(Form("al_L%d",i),((12+i)*8.5-0.6)/2, 0.0175, MDT_barrel_long/2);
    }
    
    TGeoBBox* L_1 = new TGeoBBox("L_1",8.5, 0.08, 7.05); 
    TGeoBBox* L_2 = new TGeoBBox("L_2",13, 0.08, 7.05);
    TGeoBBox* al_L_1 = new TGeoBBox("al_L_1",8.5, 0.018, 7.05); 
    TGeoBBox* al_L_2 = new TGeoBBox("al_L_2",13, 0.018, 7.05);

    TGeoVolume *G10[16], *ST[16];
    TGeoCompositeShape *CG10[16], *CST[16]; 
    float ppos[14] = {-2.5,-2.5,-1.7,4.3,-2.4,0.6,-3.1,1.5,-3.7,2.2,-0.4,2.9,-1.0,3.4};

  char longb[250];
  char longb1[250];
  int qqq[12][13]={2,1,0,2,2,1,2,1,2,2,2,3,2,
                   2,2,0,3,1,1,3,2,2,3,1,3,3,
                   3,1,0,4,2,1,4,1,2,4,2,3,4,
                   4,2,0,5,2,1,5,2,2,5,2,3,5,
                   4,1,0,6,2,1,6,1,2,6,2,3,6,
                   5,2,0,7,2,1,7,2,2,7,2,3,7,
                   5,1,0,8,2,1,8,1,2,8,2,3,8,
                   6,2,0,9,1,1,9,2,2,9,1,3,9,
                   7,2,0,10,2,1,10,2,2,10,2,3,10,
                   7,2,0,11,1,1,11,2,2,11,1,3,11,
                   8,2,0,12,2,1,12,2,2,12,2,3,12,
                   8,2,0,13,1,1,13,2,2,13,1,3,13}; 

  for(int i=2; i<14; i++){
    sprintf(longb,"L%d-(L_%d:tr%dL%d+L_%d:tr%dL%d+L_%d:tr%dL%d+L_%d:tr%dL%d)",
    qqq[i-2][0],qqq[i-2][1],qqq[i-2][2],qqq[i-2][3],qqq[i-2][4],qqq[i-2][5],qqq[i-2][6],qqq[i-2][7],qqq[i-2][8],
    qqq[i-2][9],qqq[i-2][10],qqq[i-2][11],qqq[i-2][12]);

    sprintf(longb1,"L%d-(L_%d:tr%dL%d+L_%d:tr%dL%d+L_%d:tr%dL%d+L_%d:tr%dL%d)",
    qqq[i-2][0],qqq[i-2][1],qqq[i-2][2],qqq[i-2][3],qqq[i-2][4],qqq[i-2][5],qqq[i-2][6],qqq[i-2][7],qqq[i-2][8],
    qqq[i-2][9],qqq[i-2][10],qqq[i-2][11],qqq[i-2][12]);

    CG10[i] = new TGeoCompositeShape(longb);
    CST[i]  = new TGeoCompositeShape(longb1);
    G10[i] = new TGeoVolume(Form("GP%d_%d_B",i,0), CG10[i], med7);
    ST[i] = new TGeoVolume(Form("ST%d_%d_B",i,0), CST[i], med7);
  }

///////////////////////////// Double sided strips for Layer #0 ////////////////////
    TGeoBBox* L01 = new TGeoBBox("L0",58.9, 0.075, MDT_barrel_short/2);     
    TGeoBBox* al_L0_1 = new TGeoBBox("al_L0_1",58.9, 0.0175, MDT_barrel_short/2);    
    TGeoBBox* al_L0_2 = new TGeoBBox("al_L0_2",58.9, 0.0175, MDT_barrel_short/2);    
    
    G10[1] = new TGeoVolume("GP01_0",L01, gGeoManager->GetMedium("G10"));
    ST[0] = new TGeoVolume("ST0_0",al_L0_1, gGeoManager->GetMedium("aluminium"));
    ST[1] = new TGeoVolume("ST1_0",al_L0_2, gGeoManager->GetMedium("aluminium"));
///////////////////////////////////////////////////////////////

     for(int ii = 0; ii < 14; ii++){
      if(ii == 0) { ST[ii]->SetLineColor(kBlue); } 
      else {
       G10[ii]->SetLineColor(kRed);
       ST[ii]->SetLineColor(kBlue);
      } 
     }    
     int fl;
//  MDT%ds%dl%db%dw%d -> MDT + 1/2/3/4 (Barrel/Endcap/MF/FS) + octant# + layer# + box# + wire#
///////////// Barrel //////////////////////////
//////////// Oct#0,1,3,4,5,7 /////////////////
     fl=1;
     pos_z = 45.;
     pos_y = 155.715;
     for(int ii = 0; ii < 14; ii++){ // Layer#
      int yy = (ii%2);
      for(int i = 0; i < mm1[ii][0]; i++){ //Number of MDT in Layer
       if(ii>1) {
        pos_y = 155.715+(ii-2)*6;

        if(i <= mm1[ii][1])                   { pos_x = mm[ii][0] + (-1)**ii*8.5*i;                
         name(tube[1][0],fl,0,ii,(-1)**ii*(mm1[ii][0]-i-1)+(ii%2)*(mm1[ii][0]-1),0); 
         mnk[ii]->AddNode(tube[1][0], (-1)**ii*(mm1[ii][0]-i-1)+(ii%2)*(mm1[ii][0]-1), new TGeoTranslation(pos_x,pos_y,pos_z));
         }

        if(i >  mm1[ii][1] && i < mm1[ii][2]) { pos_x = mm[ii][1] + (-1)**ii*8.5*(i-mm1[ii][1]-1); 
         name(tube[0][0],fl,0,ii,(-1)**ii*(mm1[ii][0]-i-1)+(ii%2)*(mm1[ii][0]-1),0);  
         mnk[ii]->AddNode(tube[0][0], (-1)**ii*(mm1[ii][0]-i-1)+(ii%2)*(mm1[ii][0]-1), new TGeoTranslation(pos_x,pos_y,pos_z));}

        if(i >= mm1[ii][2])                   { pos_x = mm[ii][2] + (-1)**ii*8.5*(i-mm1[ii][2]);   
         name(tube[1][0],fl,0,ii,(-1)**ii*(mm1[ii][0]-i-1)+(ii%2)*(mm1[ii][0]-1),0);  
         mnk[ii]->AddNode(tube[1][0], (-1)**ii*(mm1[ii][0]-i-1)+(ii%2)*(mm1[ii][0]-1), new TGeoTranslation(pos_x,pos_y,pos_z));} 
       } else {
          pos_y = (147 - MDT_thickness/2 - (MDT_thickness + 0.5)*(ii-1)); 
          pos_x = (mm[ii][0] - (-1)**ii*i*8.5);  
          name(tube[1][0],fl,0,ii,(-1)**(ii+1)*(mm1[ii][0]-i-1)+(mm1[ii][0]-1)*((ii+1)%2),0); 
          mnk[ii]->AddNode(tube[1][0], (-1)**(ii+1)*(mm1[ii][0]-i-1)+(mm1[ii][0]-1)*((ii+1)%2), new TGeoTranslation(pos_x,pos_y,pos_z));
         }
     }
     
     if(ii >= 2) {
      mnk[ii]->AddNode(G10[ii], ii, new TGeoTranslation(ppos[ii], (pos_y+MDT_thickness/2+0.1), pos_z));
      mnk[ii]->AddNode(ST[ii], ii, new TGeoTranslation(ppos[ii], (pos_y+MDT_thickness/2+0.1+0.0930), pos_z));
      MdtOct0->AddNode(mnk[ii], ii);
     } 
      
     if(ii == 1) {
       mnk[ii]->AddNode(ST[ii], ii, new TGeoTranslation(-2.5, (147.25 +0.0930), pos_z));     
       MdtOct0->AddNode(mnk[ii], ii);
     }

     if(ii == 0) {
       mnk[ii]->AddNode(ST[ii], ii, new TGeoTranslation(-2.5, (147.25 -0.0930), pos_z));     
       mnk[ii]->AddNode(G10[ii], ii, new TGeoTranslation(-2.5, 147.25, pos_z));
       MdtOct0->AddNode(mnk[ii], ii);
     }
    }
    
///////////////////////////////////////////////////////////

//////////// Oct#2 ////////////////////////
  for(int i = 0; i < 26; i++){
    pos_y = 155.715;
    if(i>=0  && i<2)  { pos_z = 146.75;    pos_x = (53.55  - 8.5*i);      name(tube[2][0],fl,2,2,i,0); mnk1[2]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<11) { pos_z = 153.75;    pos_x = (36.55  - 8.5*(i-2));  name(tube[3][0],fl,2,2,i,0); mnk1[2]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=11 && i<14) { pos_z = 146.75;    pos_x = (-39.95 - 8.5*(i-11)); name(tube[2][0],fl,2,2,i,0); mnk1[2]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<18) { pos_z = (-101.75); pos_x = (-56.95 + 8.5*(i-14)); name(tube[4][0],fl,2,2,i,0); mnk1[2]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=18 && i<22) { pos_z = (-90);     pos_x = (-14.75 + 8.5*(i-18)); name(tube[5][0],fl,2,2,i,0); mnk1[2]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=22)         { pos_z = (-101.75); pos_x = (31.45  + 8.5*(i-22)); name(tube[4][0],fl,2,2,i,0); mnk1[2]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 24; i++){
    pos_y = 161.715;
    if(i>=0  && i<1)  { pos_z =  45;       pos_x = (59.348  - 8.5*i);      name(tube[1][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=1  && i<3)  { pos_z = 146.75;    pos_x = (50.938  - 8.5*(i-1));  name(tube[2][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<12) { pos_z = 153.75;    pos_x = (33.938  - 8.5*(i-3));  name(tube[3][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=12 && i<13) { pos_z = 146.75;    pos_x = (-42.562 - 8.5*(i-12)); name(tube[2][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=13 && i<14) { pos_z = 45;        pos_x = (-59.438 - 8.5*(i-13)); name(tube[1][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<17) { pos_z = (-101.75); pos_x = (-50.938 + 8.5*(i-14)); name(tube[4][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<21) { pos_z = (-90.);    pos_x = (-10.75  + 8.5*(i-17)); name(tube[5][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=21)         { pos_z = (-101.75); pos_x = (33.938  + 8.5*(i-21)); name(tube[4][0],fl,2,3,i,0); mnk1[3]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 27; i++){
    pos_y = 167.715;
    if(i>=0  && i<1)  { pos_z =  45;       pos_x = (61.92  - 8.5*i);      name(tube[1][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=1  && i<2)  { pos_z = 146.75;   pos_x = (48.58  - 8.5*(i-1));  name(tube[2][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<12) { pos_z = 153.75;    pos_x = (40.08  - 8.5*(i-2));  name(tube[3][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=12 && i<14) { pos_z = 146.75;    pos_x = (-44.92 - 8.5*(i-12)); name(tube[2][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<15) { pos_z = 45;        pos_x = (-61.92 - 8.5*(i-14)); name(tube[1][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15 && i<19) { pos_z = (-101.75); pos_x = (-53.42 + 8.5*(i-15)); name(tube[4][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=19 && i<23) { pos_z = (-90);     pos_x = (-14.75 + 8.5*(i-19)); name(tube[5][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=23)         { pos_z = (-101.75); pos_x = (27.92  + 8.5*(i-23)); name(tube[4][0],fl,2,4,i,0); mnk1[4]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 28; i++){
    pos_y = 173.715;
    if(i>=0  && i<1)  { pos_z =  45;       pos_x = (64.40  - 8.5*i);      name(tube[1][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=1  && i<3)  { pos_z = 146.75;    pos_x = (55.90  - 8.5*(i-1));  name(tube[2][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<13) { pos_z = 153.75;    pos_x = (38.90  - 8.5*(i-3));  name(tube[3][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=13 && i<15) { pos_z = 146.75;    pos_x = (-46.10 - 8.5*(i-13)); name(tube[2][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15 && i<16) { pos_z = 45;        pos_x = (-64.40 - 8.5*(i-15)); name(tube[1][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<20) { pos_z = (-101.75); pos_x = (-55.90 + 8.5*(i-16)); name(tube[4][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=20 && i<24) { pos_z = (-90);     pos_x = (-10.75 + 8.5*(i-20)); name(tube[5][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=24)         { pos_z = (-101.75); pos_x = (30.40  + 8.5*(i-24)); name(tube[4][0],fl,2,5,i,0); mnk1[5]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 26; i++){
    pos_y = 179.715;
    if(i>=0  && i<2)  { pos_z = 45;        pos_x = (66.89  - 8.5*i);      name(tube[1][0],fl,2,6,i,0); mnk1[6]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<13) { pos_z = 153.75;    pos_x = (43.61  - 8.5*(i-2));  name(tube[3][0],fl,2,6,i,0); mnk1[6]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=13  && i<14){ pos_z = (146.75);  pos_x = (-49.89 - 8.5*(i-13)); name(tube[2][0],fl,2,6,i,0); mnk1[6]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}  
    if(i>=14 && i<16) { pos_z = 45;        pos_x = (-58.39 - 8.5*(i-14)); name(tube[1][0],fl,2,6,i,0); mnk1[6]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<19) { pos_z = (-101.75); pos_x = (-49.89 + 8.5*(i-16)); name(tube[4][0],fl,2,6,i,0); mnk1[6]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=19 && i<23) { pos_z = (-90);     pos_x = (-14.75 + 8.5*(i-19)); name(tube[5][0],fl,2,6,i,0); mnk1[6]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=23)         { pos_z = (-101.75); pos_x = (32.89  + 8.5*(i-23)); name(tube[4][0],fl,2,6,i,0); mnk1[6]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 29; i++){
    pos_y = 185.715;
    if(i>=0  && i<2)  { pos_z = 45;        pos_x = (69.38  - 8.5*i);      name(tube[1][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<3)  { pos_z = 146.75;    pos_x = (52.38  - 8.5*(i-2));  name(tube[2][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=13){ pos_z = 153.75;    pos_x = (43.88  - 8.5*(i-3));  name(tube[3][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<15) { pos_z = 146.75;    pos_x = (-49.62 - 8.5*(i-14)); name(tube[2][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15 && i<17) { pos_z = 45;        pos_x = (-60.88 - 8.5*(i-15)); name(tube[1][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<21) { pos_z = (-101.75); pos_x = (-52.38 + 8.5*(i-17)); name(tube[4][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=21 && i<25) { pos_z = (-90);     pos_x = (-10.75 + 8.5*(i-21)); name(tube[5][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=25)         { pos_z = (-101.75); pos_x = (26.88  + 8.5*(i-25)); name(tube[4][0],fl,2,7,i,0); mnk1[7]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 29; i++){
    pos_y = 191.715;
    if(i>=0  && i<2)  { pos_z = 45;        pos_x = (71.86  - 8.5*i);      name(tube[1][0],fl,2,8,i,0); mnk1[8]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<14) { pos_z = 153.75;    pos_x = (47.14  - 8.5*(i-2));  name(tube[3][0],fl,2,8,i,0); mnk1[8]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<15) { pos_z = 146.75;    pos_x = (-54.86 - 8.5*(i-14)); name(tube[2][0],fl,2,8,i,0); mnk1[8]->AddNode(tube[2][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15 && i<17) { pos_z = 45;        pos_x = (-63.36 - 8.5*(i-15)); name(tube[1][0],fl,2,8,i,0); mnk1[8]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<21) { pos_z = (-101.75); pos_x = (-54.86 + 8.5*(i-17)); name(tube[4][0],fl,2,8,i,0); mnk1[8]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=21 && i<25) { pos_z = (-90);     pos_x = (-14.75 + 8.5*(i-21)); name(tube[5][0],fl,2,8,i,0); mnk1[8]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=25)         { pos_z = (-101.75); pos_x = (29.36  + 8.5*(i-25)); name(tube[4][0],fl,2,8,i,0); mnk1[8]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

 for(int i = 0; i < 28; i++){
    pos_y = 197.715;
    if(i>=0  && i<3)  { pos_z = 45;        pos_x = (74.35  - 8.5*i);      name(tube[1][0],fl,2,9,i,0); mnk1[9]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<15) { pos_z = 153.75;    pos_x = (48.85  - 8.5*(i-3));  name(tube[3][0],fl,2,9,i,0); mnk1[9]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15 && i<18) { pos_z = 45;        pos_x = (-57.35 - 8.5*(i-15)); name(tube[1][0],fl,2,9,i,0); mnk1[9]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=18 && i<21) { pos_z = (-101.75); pos_x = (-48.85 + 8.5*(i-18)); name(tube[4][0],fl,2,9,i,0); mnk1[9]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=21 && i<25) { pos_z = (-90);     pos_x = (-10.75 + 8.5*(i-21)); name(tube[5][0],fl,2,9,i,0); mnk1[9]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=25)         { pos_z = (-101.75); pos_x = (31.85  + 8.5*(i-25)); name(tube[4][0],fl,2,9,i,0); mnk1[9]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 29; i++){
    pos_y = 203.715;
    if(i>=0  && i<3)  { pos_z = 45;        pos_x = (76.83  - 8.5*i);      name(tube[1][0],fl,2,10,i,0); mnk1[10]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<16) { pos_z = 153.75;    pos_x = (50.67  - 8.5*(i-3));  name(tube[3][0],fl,2,10,i,0); mnk1[10]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<19) { pos_z = 45;        pos_x = (-59.83 - 8.5*(i-16)); name(tube[1][0],fl,2,10,i,0); mnk1[10]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=19 && i<22) { pos_z = (-101.75); pos_x = (-51.33 + 8.5*(i-19)); name(tube[4][0],fl,2,10,i,0); mnk1[10]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=22 && i<26) { pos_z = (-90);     pos_x = (-14.75 + 8.5*(i-22)); name(tube[5][0],fl,2,10,i,0); mnk1[10]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=26)         { pos_z = (-101.75); pos_x = (34.33  + 8.5*(i-26)); name(tube[4][0],fl,2,10,i,0); mnk1[10]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 29; i++){
    pos_y = 209.715;
    if(i>=0  && i<3)  { pos_z = 45;        pos_x = (79.32  - 8.5*i);      name(tube[1][0],fl,2,11,i,0); mnk1[11]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<16) { pos_z = 153.75;    pos_x = (53.82  - 8.5*(i-3));  name(tube[3][0],fl,2,11,i,0); mnk1[11]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<19) { pos_z = 45;        pos_x = (-62.32 - 8.5*(i-16)); name(tube[1][0],fl,2,11,i,0); mnk1[11]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=19 && i<23) { pos_z = (-101.75); pos_x = (-53.82 + 8.5*(i-19)); name(tube[4][0],fl,2,11,i,0); mnk1[11]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=23 && i<27) { pos_z = (-90);     pos_x = (-10.75 + 8.5*(i-23)); name(tube[5][0],fl,2,11,i,0); mnk1[11]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=27)         { pos_z = (-101.75); pos_x = (28.32  + 8.5*(i-27)); name(tube[4][0],fl,2,11,i,0); mnk1[11]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}  
  } 

  for(int i = 0; i < 32; i++){
    pos_y = 215.715;
    if(i>=0  && i<3)  { pos_z = 45;        pos_x = (81.80  - 8.5*i);      name(tube[1][0],fl,2,12,i,0); mnk1[12]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<17) { pos_z = 153.75;    pos_x = (54.20  - 8.5*(i-3));  name(tube[3][0],fl,2,12,i,0); mnk1[12]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<20) { pos_z = 45;        pos_x = (-64.80 - 8.5*(i-17)); name(tube[1][0],fl,2,12,i,0); mnk1[12]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=20 && i<24) { pos_z = (-101.75); pos_x = (-56.30 + 8.5*(i-20)); name(tube[4][0],fl,2,12,i,0); mnk1[12]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=24 && i<28) { pos_z = (-90);     pos_x = (-14.75 + 8.5*(i-24)); name(tube[5][0],fl,2,12,i,0); mnk1[12]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=28)         { pos_z = (-101.75); pos_x = (30.80  + 8.5*(i-28)); name(tube[4][0],fl,2,12,i,0); mnk1[12]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 32; i++){
    pos_y = 221.715;
    if(i>=0  && i<3)  { pos_z = 45;        pos_x = (84.29  - 8.5*i);      name(tube[1][0],fl,2,13,i,0); mnk1[13]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<17) { pos_z = 153.75;    pos_x = (58.79  - 8.5*(i-3));  name(tube[3][0],fl,2,13,i,0); mnk1[13]->AddNode(tube[3][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<20) { pos_z = 45;        pos_x = (-67.29 - 8.5*(i-17)); name(tube[1][0],fl,2,13,i,0); mnk1[13]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=20 && i<24) { pos_z = (-101.75); pos_x = (-58.79 + 8.5*(i-20)); name(tube[4][0],fl,2,13,i,0); mnk1[13]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=24 && i<28) { pos_z = (-90);     pos_x = (-10.75 + 8.5*(i-24)); name(tube[5][0],fl,2,13,i,0); mnk1[13]->AddNode(tube[5][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=28)         { pos_z = (-101.75); pos_x = (33.29  + 8.5*(i-28)); name(tube[4][0],fl,2,13,i,0); mnk1[13]->AddNode(tube[4][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

/////////////////////////// 85x85mm ////////////////
///////////////////////// "Zero" Layer#1,0 //////////////
 
 for(int ii = 0; ii < 2; ii++){
  for(int i = 0; i < 16; i++){
    pos_y = (149.-2.-MDT_thickness/2-(MDT_thickness + 0.5)*(ii-1)); 
    if(ii == 0) {
     if(i < 14)  { pos_x = (53.05 - i*8.5); }  
     if(i >= 14) { pos_x = (53.05 - (i-8)*8.5); }
    } else {      
       if(i < 14)  { pos_x = (-57.95 + i*8.5); }  
       if(i >= 14) { pos_x = (-57.95 + (i-8)*8.5); }      
      }  
      
    if((i<6 || i>=8) && i<14 ) { pos_z = 45.;     name(tube[1][0],fl,2,ii,i,0); mnk1[ii]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z)); } 
    if(i>=6 && i<8)            { pos_z = 117.55;  name(tube[6][0],fl,2,ii,i,0); mnk1[ii]->AddNode(tube[6][0], i, new TGeoTranslation(pos_x,pos_y,pos_z)); }  
    if(i>=14)                  { pos_z = (-72.6); name(tube[7][0],fl,2,ii,i,0); mnk1[ii]->AddNode(tube[7][0], i, new TGeoTranslation(pos_x,pos_y,pos_z)); } 
  } 
 }   
/////////////////////////////////////////////////////////

///////////// Strip board for Oct#2 /////////////////
  TGeoBBox* L_3 = new TGeoBBox("L_3",18.4, 0.08, 7.05);
  TGeoBBox* al_L_3 = new TGeoBBox("al_L_3",18.4, 0.018, 7.05);

  int qqq1[12][6]={3,0,2,2,1,2,  // L_3:tr0L2+L_2:tr1L2
                  3,0,3,3,1,3,  // L_3:tr0L3+L_3:tr1L3
                  3,0,4,3,1,4,  // L_3:tr0L4+L_3:tr1L4
                  3,0,5,3,1,5,    3,0,6,3,1,6,    3,0,7,3,1,7,
                  3,0,8,3,1,8,    3,0,9,3,1,9,    3,0,10,3,1,10,
                  3,0,11,3,1,11,  3,0,12,3,1,12,  3,0,13,3,1,13}; 

  TGeoTranslation* trs[14][5];
  float limx1[12][2] = {61.0,-46.3,   55.9,-56.2,   61.9,-59.0,  61.4,-59.7, 
                        65.9,-63.5,   66.4,-63.4,   69.4,-68.6,  71.4,-67.2, 
                        72.8,-73.8,   76.2,-70.8,   76.2,-78.8,  81.2, -74.}; 

    for(int i=0; i<12; i++) {
     for(int j=0; j<2; j++) {
       trs[i][j] = new TGeoTranslation(limx1[i][j], 0, 181);
       trs[i][j]->SetName(Form("trs%dL%d",j,i+2));
       trs[i][j]->RegisterYourself();
     }
    }
                   
    TGeoBBox *K[14], *al_K[14], *K_h[14], *al_K_h[14];
    TGeoTranslation* trr[15];    
    trr_h = new TGeoTranslation(0, 0, -52);
    trr_h->SetName(Form("trr_h"));
    trr_h->RegisterYourself();

    float ln[12]  = {14.,15,15.5,16,16.8,17.2,18,18.5,19,19.7,20.2,20.9};
    float ln2[12] = {14.2,13,13.6,14.2,12.8,13.4,13.9,12.6,13.1,13.8,14.2,14.8};
    char llongb[250];
    char llongb1[250];
    float ppos1[12] = {-2.1, 0.2, 0.2, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
    TGeoVolume *G10_2[16], *ST_2[16];
    TGeoCompositeShape *CG10_2[16], *CST_2[16];  

   for(int i=2; i<14; i++){
    K[i] = new TGeoBBox(Form("K%d",i),           (ln[i-2]*8.5-0.6)/2, 0.075,  (390-14)/2);     
    K_h[i] = new TGeoBBox(Form("K_h%d",i),       (ln2[i-2]*8.5/2), 0.125,     15.9*8.5/2);
    al_K[i] = new TGeoBBox(Form("al_K%d",i),     (ln[i-2]*8.5-0.6)/2, 0.0175, (390-14)/2);     
    al_K_h[i] = new TGeoBBox(Form("al_K_h%d",i), (ln2[i-2]*8.5/2), 0.04,      15.9*8.5/2);

    sprintf(llongb,"K%d-K_h%d:trr_h-L_%d:trs%dL%d-L_%d:trs%dL%d",i,i,qqq1[i-2][0],qqq1[i-2][1],qqq1[i-2][2],qqq1[i-2][3],qqq1[i-2][4],qqq1[i-2][5]);
    sprintf(llongb1,"K%d-K_h%d:trr_h-L_%d:trs%dL%d-L_%d:trs%dL%d",i,i,qqq1[i-2][0],qqq1[i-2][1],qqq1[i-2][2],qqq1[i-2][3],qqq1[i-2][4],qqq1[i-2][5]);

    CG10_2[i] = new TGeoCompositeShape(llongb);
    G10_2[i] = new TGeoVolume(Form("GP%d_%d_B",i,2), CG10_2[i], med7);
    G10_2[i]->SetLineColor(kRed);
    CST_2[i] = new TGeoCompositeShape(llongb1);
    ST_2[i] = new TGeoVolume(Form("ST%d_%d_B",i,2), CST_2[i], med7);
    ST_2[i]->SetLineColor(kBlue);
    mnk1[i]->AddNode(G10_2[i], 1, new TGeoTranslation(ppos1[i-2], 156.43+6*(i-2), 52));
    mnk1[i]->AddNode(ST_2[i], 1, new TGeoTranslation(ppos1[i-2], 156.43+6*(i-2)+0.093, 52));
   }

////////////// For zero
    K[0] = new TGeoBBox(Form("K%d",0),       118.4/2, 0.15/2,  MDT_barrel_short/2);
    al_K[0] = new TGeoBBox(Form("al_K%d",0), 118.4/2, 0.035/2,  MDT_barrel_short/2);
    TGeoBBox* K_h0 = new TGeoBBox("K_h0",       8.5,0.125,8.5);
    TGeoBBox* al_K_h0 = new TGeoBBox("al_K_h0", 8.5,0.04, 8.5);
    trrr = new TGeoTranslation(0, 0, -45);
       trrr->SetName("trrr");
       trrr->RegisterYourself();
    CG10_2[0] = new TGeoCompositeShape("K0-K_h0:trrr");
    CST_2[0] = new TGeoCompositeShape("al_K0-K_h0:trrr");
    CST_2[1] = new TGeoCompositeShape("al_K0-al_K_h0:trrr");
    G10_2[0] = new TGeoVolume(Form("GP%d_%d_B",0,2), CG10_2[0], med7);
    ST_2[0] = new TGeoVolume(Form("ST%d_%d_B",0,2), CST_2[0], med7);
    ST_2[1] = new TGeoVolume(Form("ST%d_%d_B",1,2), CST_2[1], med7);
    G10_2[0]->SetLineColor(kRed);
    ST_2[0]->SetLineColor(kBlue);
    ST_2[1]->SetLineColor(kBlue);
     
    mnk1[0]->AddNode(G10_2[0], 1, new TGeoTranslation(-2.5, (149. - 2. - MDT_thickness + (MDT_thickness+0.5/2)), 45.));
    mnk1[0]->AddNode(ST_2[0], 0, new TGeoTranslation(-2.5, (149. - 2. - MDT_thickness + (MDT_thickness+0.5/2) -0.0930), 45));     
    mnk1[1]->AddNode(ST_2[1], 1, new TGeoTranslation(-2.5, (149. - 2. - MDT_thickness + (MDT_thickness+0.5/2) +0.0930), 45));     

  for(int i = 0; i < 14; i++) {MdtOct2->AddNode(mnk1[i], i);}
/////////////////////////////////////////////////////////////


//////////// Oct#6 ////////////////////////
  for(int i = 0; i < 20; i++){
    pos_y = 155.715;
    if(i>=0  && i<=1)  { pos_z = 45;     pos_x = (56.95  - 8.5*i);      name(tube[1][0],fl,6,2,i,0); mnk2[2]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<=3)  { pos_z = 45;     pos_x = (39.95  - 8.5*(i-2));  name(tube[0][0],fl,6,2,i,0); mnk2[2]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=4  && i<=9)  { pos_z = 133.75; pos_x = (19.55  - 8.5*(i-4));  name(tube[8][0],fl,6,2,i,0); mnk2[2]->AddNode(tube[8][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=10 && i<=11) { pos_z = 45;     pos_x = (-31.45 - 8.5*(i-10)); name(tube[0][0],fl,6,2,i,0); mnk2[2]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=12 && i<=13) { pos_z = 45;     pos_x = (-48.45 - 8.5*(i-12)); name(tube[1][0],fl,6,2,i,0); mnk2[2]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14)          { pos_z = -89.25; pos_x = (-22.95 + 8.5*(i-14)); name(tube[9][0],fl,6,2,i,0); mnk2[2]->AddNode(tube[9][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 20; i++){
    pos_y = 161.715;
    if(i>=0  && i<=1)  { pos_z = 45;     pos_x = (59.438  - 8.5*i);      name(tube[1][0],fl,6,3,i,0); mnk2[3]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<=4)  { pos_z = 45;     pos_x = (42.438  - 8.5*(i-2));  name(tube[0][0],fl,6,3,i,0); mnk2[3]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=5  && i<=9)  { pos_z = 133.75; pos_x = (16.938  - 8.5*(i-5));  name(tube[8][0],fl,6,3,i,0); mnk2[3]->AddNode(tube[8][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=10 && i<=12) { pos_z = 45;     pos_x = (-25.438 - 8.5*(i-10)); name(tube[0][0],fl,6,3,i,0); mnk2[3]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=13 && i<=14) { pos_z = 45;     pos_x = (-50.938 - 8.5*(i-13)); name(tube[1][0],fl,6,3,i,0); mnk2[3]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15)          { pos_z = -89.25; pos_x = (-17.062 + 8.5*(i-15)); name(tube[9][0],fl,6,3,i,0); mnk2[3]->AddNode(tube[9][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 20; i++){
    pos_y = 167.715;
    if(i>=0  && i<=1)  { pos_z = 45;     pos_x = (61.92  - 8.5*i);      name(tube[1][0],fl,6,4,i,0); mnk2[4]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<=4)  { pos_z = 45;     pos_x = (44.92  - 8.5*(i-2));  name(tube[0][0],fl,6,4,i,0); mnk2[4]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=5  && i<=9)  { pos_z = 133.75; pos_x = (14.858 - 8.5*(i-5));  name(tube[8][0],fl,6,4,i,0); mnk2[4]->AddNode(tube[8][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=10 && i<=12) { pos_z = 45;     pos_x = (-27.92 - 8.5*(i-10)); name(tube[0][0],fl,6,4,i,0); mnk2[4]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=13 && i<=14) { pos_z = 45;     pos_x = (-53.42 - 8.5*(i-13)); name(tube[1][0],fl,6,4,i,0); mnk2[4]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15)          { pos_z = -89.25; pos_x = (-19.42 + 8.5*(i-15)); name(tube[9][0],fl,6,4,i,0); mnk2[4]->AddNode(tube[9][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 22; i++){
    pos_y = 173.715;
    if(i>=0  && i<=1)  { pos_z = 45;     pos_x = (64.40  - 8.5*i);      name(tube[1][0],fl,6,5,i,0); mnk2[5]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<=4)  { pos_z = 45;     pos_x = (47.40  - 8.5*(i-2));  name(tube[0][0],fl,6,5,i,0); mnk2[5]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=5  && i<=10) { pos_z = 133.75; pos_x = (21.90  - 8.5*(i-5));  name(tube[8][0],fl,6,5,i,0); mnk2[5]->AddNode(tube[8][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=11 && i<=13) { pos_z = 45;     pos_x = (-30.40 - 8.5*(i-11)); name(tube[0][0],fl,6,5,i,0); mnk2[5]->AddNode(tube[0][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<=15) { pos_z = 45;     pos_x = (-55.90 - 8.5*(i-14)); name(tube[1][0],fl,6,5,i,0); mnk2[5]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16)          { pos_z = -89.25; pos_x = (-20.60 + 8.5*(i-16)); name(tube[9][0],fl,6,5,i,0); mnk2[5]->AddNode(tube[9][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 28; i++){
    pos_y = 179.715;
    if(i>=0  && i<=1)  { pos_z = 45;     pos_x = (66.89  - 8.5*i);      name(tube[1][0],fl,6,6,i,0);  mnk2[6]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<3)   { pos_z = 143.25; pos_x = (49.89  - 8.5*(i-2));  name(tube[10][0],fl,6,6,i,0); mnk2[6]->AddNode(tube[10][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=13) { pos_z = 150.25; pos_x = (41.39  - 8.5*(i-3));  name(tube[11][0],fl,6,6,i,0); mnk2[6]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<=15) { pos_z = 45;     pos_x = (-58.39 - 8.5*(i-14)); name(tube[1][0],fl,6,6,i,0);  mnk2[6]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<=26) { pos_z = -105.2; pos_x = (-43.61 + 8.5*(i-16)); name(tube[12][0],fl,6,6,i,0); mnk2[6]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=27)          { pos_z = -98.2;  pos_x = (49.89  + 8.5*(i-27)); name(tube[13][0],fl,6,6,i,0); mnk2[6]->AddNode(tube[13][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 30; i++){
    pos_y = 185.715;
    if(i>=0  && i<=1)  { pos_z = 45;     pos_x = (69.38  - 8.5*i);      name(tube[1][0],fl,6,7,i,0);  mnk2[7]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=2  && i<3)   { pos_z = 143.25; pos_x = (52.38  - 8.5*(i-2));  name(tube[10][0],fl,6,7,i,0); mnk2[7]->AddNode(tube[10][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=13) { pos_z = 150.25; pos_x = (43.88  - 8.5*(i-3));  name(tube[11][0],fl,6,7,i,0); mnk2[7]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14  && i<15) { pos_z = 143.25; pos_x = (-49.62 - 8.5*(i-14)); name(tube[10][0],fl,6,7,i,0); mnk2[7]->AddNode(tube[10][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15  && i<=16){ pos_z = 45.;    pos_x = (-60.88 - 8.5*(i-15)); name(tube[1][0],fl,6,7,i,0);  mnk2[7]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<18)  { pos_z = -98.2;  pos_x = (-49.62 + 8.5*(i-17)); name(tube[13][0],fl,6,7,i,0); mnk2[7]->AddNode(tube[13][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}  
    if(i>=18 && i<=28) { pos_z = -105.2; pos_x = (-41.12 + 8.5*(i-18)); name(tube[12][0],fl,6,7,i,0); mnk2[7]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=29)          { pos_z = -98.2;  pos_x = (52.38  + 8.5*(i-29)); name(tube[13][0],fl,6,7,i,0); mnk2[7]->AddNode(tube[13][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 28; i++){
    pos_y = 191.715;
    if(i>=0  && i<=2)  { pos_z = 45;     pos_x = (71.18  - 8.5*i);      name(tube[1][0],fl,6,8,i,0);  mnk2[8]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=13) { pos_z = 150.25; pos_x = (38.64  - 8.5*(i-3));  name(tube[11][0],fl,6,8,i,0); mnk2[8]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=14 && i<=16) { pos_z = 45.;    pos_x = (-54.86 - 8.5*(i-14)); name(tube[1][0],fl,6,8,i,0);  mnk2[8]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17)          { pos_z = -105.2; pos_x = (-46.36 + 8.5*(i-17)); name(tube[12][0],fl,6,8,i,0); mnk2[8]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 30; i++){
    pos_y = 197.715;
    if(i>=0  && i<=2)  { pos_z = 45;     pos_x = (74.35  - 8.5*i);      name(tube[1][0],fl,6,9,i,0);  mnk2[9]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=14) { pos_z = 150.25; pos_x = (48.85  - 8.5*(i-3));  name(tube[11][0],fl,6,9,i,0); mnk2[9]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=15 && i<=17) { pos_z = 45.;    pos_x = (-57.35 - 8.5*(i-15)); name(tube[1][0],fl,6,9,i,0);  mnk2[9]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=18)          { pos_z = -105.2; pos_x = (-44.65 + 8.5*(i-18)); name(tube[12][0],fl,6,9,i,0); mnk2[9]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 32; i++){
    pos_y = 203.715;
    if(i>=0  && i<=2)  { pos_z = 45;     pos_x = (76.83  - 8.5*i);      name(tube[1][0],fl,6,10,i,0);  mnk2[10]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=15) { pos_z = 150.25; pos_x = (50.67  - 8.5*(i-3));  name(tube[11][0],fl,6,10,i,0); mnk2[10]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<=18) { pos_z = 45.;    pos_x = (-59.83 - 8.5*(i-16)); name(tube[1][0],fl,6,10,i,0);  mnk2[10]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=19)          { pos_z = -105.2; pos_x = (-51.33 + 8.5*(i-19)); name(tube[12][0],fl,6,10,i,0); mnk2[10]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 32; i++){
    pos_y = 209.715;
    if(i>=0  && i<=2)  { pos_z = 45;     pos_x = (79.32  - 8.5*i);      name(tube[1][0],fl,6,11,i,0);  mnk2[11]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=15) { pos_z = 150.25; pos_x = (53.82  - 8.5*(i-3));  name(tube[11][0],fl,6,11,i,0); mnk2[11]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<=18) { pos_z = 45.;    pos_x = (-62.32 - 8.5*(i-16)); name(tube[1][0],fl,6,11,i,0);  mnk2[11]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=19)          { pos_z = -105.2; pos_x = (-48.18 + 8.5*(i-19)); name(tube[12][0],fl,6,11,i,0); mnk2[11]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

  for(int i = 0; i < 34; i++){
    pos_y = 215.715;
    if(i>=0  && i<=2)  { pos_z = 45;     pos_x = (81.80  - 8.5*i);      name(tube[1][0],fl,6,12,i,0);  mnk2[12]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<=16) { pos_z = 150.25; pos_x = (54.20  - 8.5*(i-3));  name(tube[11][0],fl,6,12,i,0); mnk2[12]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<=19) { pos_z = 45.;    pos_x = (-64.80 - 8.5*(i-17)); name(tube[1][0],fl,6,12,i,0);  mnk2[12]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=20)          { pos_z = -105.2; pos_x = (-56.30 + 8.5*(i-20)); name(tube[12][0],fl,6,12,i,0); mnk2[12]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

//  for(int i = 0; i < 34; i++){
  for(int i = 0; i < 32; i++){
    pos_y = 221.715;
    if(i>=0  && i<=2)  { pos_z = 45;     pos_x = (84.29  - 8.5*i);      name(tube[1][0],fl,6,13,i,0);  mnk2[13]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=3  && i<4)   { pos_z = 45;     pos_x = (58.79  - 8.5*(i-3));  name(tube[0][0],fl,6,13,i,0);  mnk2[13]->AddNode(tube[0][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=4  && i<=15) { pos_z = 150.25; pos_x = (50.29  - 8.5*(i-4));  name(tube[11][0],fl,6,13,i,0); mnk2[13]->AddNode(tube[11][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=16 && i<17)  { pos_z = 45.;    pos_x = (-58.79 - 8.5*(i-16)); name(tube[0][0],fl,6,13,i,0);  mnk2[13]->AddNode(tube[0][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=17 && i<=19) { pos_z = 45.;    pos_x = (-67.29 - 8.5*(i-17)); name(tube[1][0],fl,6,13,i,0);  mnk2[13]->AddNode(tube[1][0],  i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
    if(i>=20)          { pos_z = -105.2; pos_x = (-50.29 + 8.5*(i-20)); name(tube[12][0],fl,6,13,i,0); mnk2[13]->AddNode(tube[12][0], i, new TGeoTranslation(pos_x,pos_y,pos_z));}   
  } 

///////////// Strip board for Oct#6 /////////////////
  float lln[12]  = {14.4,15,15.5,16,16.8,17.2,18,18.5,19,19.7,20.2,20.9};
  float lln2[12] = {6.5,5.1,5.7,6.2,12.9,13.4,11.8,12.6,13.1,13.8,14.3,12.7};
  float ppos1[12] = {-0.2, 0.2, 0.2, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 

    TGeoTranslation* trr[15];    
    ttrr_h = new TGeoTranslation(0, 0, -46);
    ttrr_h->SetName(Form("ttrr_h"));
    ttrr_h->RegisterYourself();

  TGeoBBox *KK[14], *al_KK[14], *KK_h[14], *al_KK_h[14];
  TGeoTranslation* trss[14][5];
  float limx2[12][4] = {62.8,-62,62.8,-62,
                       64.2,-64.4,64.2,-64.4,
                       66.9,-67,66.9,-67,
                       69.4,-69.4,69.4,-69.4,
                       64.5,-66.4,64.5,-66.4,
                       65.5,-64,65.5,-64,
                       61.5,-68.4,61.5,-68.4,
                       70.7,-67.1,70.7,-67.1,
                       72.7,-73.6,72.7,-73.6,
                       75.8,-70.8,75.8,-70.8,
                       76.2,-78.5,76.2,-78.5,
                       82.2,-80.9,82.2,-80.9};
                        
  float limz2[1][4] = {188, 188, -188, -188}; 
    for(int i=0; i<12; i++) {
     for(int j=0; j<4; j++) {
       trss[i][j] = new TGeoTranslation(limx2[i][j], 0, limz2[0][j]);
       trss[i][j]->SetName(Form("trss%dL%d",j,i+2));
       trss[i][j]->RegisterYourself();
     }
    }

    TGeoVolume *G10_6[16], *ST_6[16];
    TGeoCompositeShape *CG10_6[16], *CST_6[16];  

   float sll = 0;
   for(int i=2; i<14; i++){
    KK[i] = new TGeoBBox(Form("KK%d",i),         (lln[i-2]*8.5-0.6)/2, 0.075,  390/2);     
    al_KK[i] = new TGeoBBox(Form("al_KK%d",i),    (lln[i-2]*8.5-0.6)/2, 0.0175, 390/2);     
    if(i>1 && i<6) { sll=6.6*8.5/2; }
    else { sll = 14.2*8.5/2; }
    KK_h[i] = new TGeoBBox(Form("KK_h%d",i),       (lln2[i-2]*8.5/2), 0.125,   sll);
    al_KK_h[i] = new TGeoBBox(Form("al_KK_h%d",i), (lln2[i-2]*8.5/2), 0.04,     sll);

    sprintf(llongb,"KK%d-KK_h%d:ttrr_h-L_3:trss0L%d-L_3:trss1L%d-L_3:trss2L%d-L_3:trss3L%d", i,i,i,i,i,i);
    sprintf(llongb1,"al_KK%d-al_KK_h%d:ttrr_h-L_3:trss0L%d-L_3:trss1L%d-L_3:trss2L%d-L_3:trss3L%d", i,i,i,i,i,i);

    CG10_6[i] = new TGeoCompositeShape(llongb);
    G10_6[i] = new TGeoVolume(Form("GP%d_%d_B",i,6), CG10_6[i], med7);
    G10_6[i]->SetLineColor(kRed);
    CST_6[i] = new TGeoCompositeShape(llongb1);
    ST_6[i] = new TGeoVolume(Form("ST%d_%d_B",i,6), CST_6[i], med7);
    ST_6[i]->SetLineColor(kBlue);
    mnk2[i]->AddNode(G10_6[i], i, new TGeoTranslation(ppos1[i-2], 156.43+6*(i-2), 45));
    mnk2[i]->AddNode(ST_6[i], i, new TGeoTranslation(ppos1[i-2],  156.43+6*(i-2)+0.093, 45));
   }

////////////////////////////////////////////////////

/////////////////////////// 85x85mm ////////////////
///////////////////////// "Zero" Layer#1,0 ////////////// 
 for(int ii = 0; ii < 2; ii++){
  for(int i = 0; i < 16; i++){
    pos_y = (149.-2.-MDT_thickness/2-(MDT_thickness + 0.5)*(ii-1)); 
    if(ii == 0) {
     if(i < 14) { pos_x = (53.05 - i*8.5); }  
     if(i >= 14) { pos_x = (53.05 - (i-8)*8.5); }
    } else {      
       if(i < 14) { pos_x = (-61.7 + 8.5/2 +i*8.5 - 0.5); }  
       if(i >= 14) { pos_x = (-61.7 + 8.5/2 +(i-8)*8.5 - 0.5); }      
      }  
      
    if((i<6 || i>=8) && i<14 ) { pos_z = 45.;     name(tube[1][0],fl,6,ii,i,0); mnk2[ii]->AddNode(tube[1][0], i, new TGeoTranslation(pos_x,pos_y,pos_z)); } 
    if(i>=6 && i<8)            { pos_z = 117.55;  name(tube[6][0],fl,6,ii,i,0); mnk2[ii]->AddNode(tube[6][0], i, new TGeoTranslation(pos_x,pos_y,pos_z)); }  
    if(i>=14)                  { pos_z = (-72.6); name(tube[7][0],fl,6,ii,i,0); mnk2[ii]->AddNode(tube[7][0], i, new TGeoTranslation(pos_x,pos_y,pos_z)); } 
  } 
 }   


////////////// Strips for zero layers
    K[0] = new TGeoBBox(Form("K%d",0),       118.4/2, 0.15/2,  MDT_barrel_short/2);
    al_K[0] = new TGeoBBox(Form("al_K%d",0), 118.4/2, 0.035/2,  MDT_barrel_short/2);
    TGeoBBox* K_h0 = new TGeoBBox("K_h0",       8.5,0.125,8.5);
    TGeoBBox* al_K_h0 = new TGeoBBox("al_K_h0", 8.5,0.04, 8.5);
    trrr = new TGeoTranslation(0, 0, -45);
       trrr->SetName("trrr");
       trrr->RegisterYourself();
    CG10_6[0] = new TGeoCompositeShape("K0-K_h0:trrr");
    CST_6[0] = new TGeoCompositeShape("al_K0-K_h0:trrr");
    CST_6[1] = new TGeoCompositeShape("al_K0-al_K_h0:trrr");
    G10_6[0] = new TGeoVolume(Form("GP%d_%d_B",0,6), CG10_6[0], med7);
    ST_6[0] = new TGeoVolume(Form("ST%d_%d_B",0,6), CST_6[0], med7);
    ST_6[1] = new TGeoVolume(Form("ST%d_%d_B",1,6), CST_6[1], med7);
    G10_6[0]->SetLineColor(kRed);
    ST_6[0]->SetLineColor(kBlue);
    ST_6[1]->SetLineColor(kBlue);
     
    mnk2[0]->AddNode(G10_2[0], 1, new TGeoTranslation(-2.5, (149. - 2. - MDT_thickness + (MDT_thickness+0.5/2)), 45.));
    mnk2[0]->AddNode(ST_2[0], 0, new TGeoTranslation(-2.5, (149. - 2. - MDT_thickness + (MDT_thickness+0.5/2) -0.0930), 45));     
    mnk2[1]->AddNode(ST_2[1], 1, new TGeoTranslation(-2.5, (149. - 2. - MDT_thickness + (MDT_thickness+0.5/2) +0.0930), 45));     
///////////////////////////////////////////////////////////

 
  for(int i = 0; i < 14; i++) MdtOct6->AddNode(mnk2[i], i); 

/////////// End of Barrel /////////////////

//////////////////////// Endcap //////////////////////////////////////
/////////           Endcap    /////////
//////// 2 layers "zero" inside yoke + 5 layers in the endcap.
///////  MdtEndcap00, MdtEndcap01 -> "zero" bi-layers.
//////   MdtEndcap02-MdtEndcap06  -> layers inside the endcap. 

/*
 float tube_len_en_l0[44]  = {109,126,143,160,177,194,211,228,245,262,102,102,102,102,102,102,102,102,102,102,102,102,
                              262,245,228,211,194,177,160,143,126,109,102,102,102,102,102,102,102,102,102,102,102,102};

 float tube_len_en_l2[68] = {171.8,232,223.5,215,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,
                             171.8,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,
                             171.8,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,
                             171.8,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5};

 float tube_len_en_l3[68] = {171.8,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,
                             171.8,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,
                             171.8,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,
                             171.8,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5};

 float tube_len_en_l4[68] = {171.8,212,203.5,195,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,
                             171.8,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,
                             171.8,212,203.5,195,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,
                             171.8,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5};
                              
 float tube_len_en_l5[68] = {142,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,
                             142,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,
                             142,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,
                             142,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5};

 float tube_len_en_l6[72] = {142,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,73,
                             142,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,73,
                             142,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,93,
                             142,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,93};
*/

 float tube_len_en_l0[36]  = {121.5,154.6,171.6,188.6,205.6,222.6,74.1,74.1,74.1,74.1,74.1,74.1,74.1,74.1,74.1,74.1,74.1,74.1,
                              222.6,205.6,188.6,171.6,154.6,121.5,89.5,89.5,89.5,89.5,89.5,89.5,89.5,89.5,89.5,89.5,89.5,89.5};

 float tube_len_en_l2[68] = {164.8,225,216.5,205,196.5,188,179.5,171,162.5,154,145.5,137,128.5,120,111.5,103,94.5,
                             164.8,225,216.5,205,196.5,188,179.5,171,162.5,154,145.5,137,128.5,120,111.5,103,94.5,
                             164.8,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5,
                             164.8,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5};

 float tube_len_en_l3[68] = {164.8,225,216.5,205,196.5,188,179.5,171,162.5,154,145.5,137,128.5,120,111.5,103,94.5,
                             164.8,225,216.5,205,196.5,188,179.5,171,162.5,154,145.5,137,128.5,120,111.5,103,94.5,
                             164.8,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5,
                             164.8,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5};

 float tube_len_en_l4[68] = {164.8,205,196.5,188,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5,
                             164.8,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5,
                             164.8,205,196.5,188,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5,
                             164.8,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5};
                              
 float tube_len_en_l5[68] = {135,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5,
                             135,205,196.5,185,176.5,168,159.5,151,142.5,134,125.5,117,108.5,100,91.5,83,74.5,
                             135,225,216.5,205,196.5,188,179.5,171,162.5,154,145.5,134,128.5,120,111.5,103,94.5,
                             135,225,216.5,205,196.5,188,179.5,171,162.5,154,145.5,134,128.5,120,111.5,103,94.5};

 float tube_len_en_l6[72] = {142,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,73,
                             142,212,203.5,192,183.5,175,166.5,158,149.5,141,132.5,124,115.5,107,98.5,90,81.5,73,
                             142,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,93,
                             142,232,223.5,212,203.5,195,186.5,178,169.5,161,152.5,144,135.5,127,118.5,110,101.5,93};

   for(int i=0; i<36; i++) {
     tube_en_l0[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len_en_l0[i]/2); 
     tube_en_l0[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len_en_l0[i]/2);
     for(int j = 0; j < 8; j++){
     float slotpos_x = (-3.5 + 1.*j);
       tube_en_l0[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",2,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len_en_l0[i]/2);
       tube_en_l0[i][1]->AddNode(tube_en_l0[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));
       tube_en_l0[i][1]->SetLineColor(kBlue);
       tube_en_l0[i][2]->SetLineColor(kBlue);
     } 
     tube_en_l0[i][0]->AddNode(tube_en_l0[i][1],1); 
     tube_en_l0[i][0]->SetLineColor(kBlue);
   }

   for(int i=0; i<68; i++) {
     tube_en_l2[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len_en_l2[i]/2); 
     tube_en_l2[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len_en_l2[i]/2);
     tube_en_l3[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len_en_l3[i]/2); 
     tube_en_l3[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len_en_l3[i]/2);
     tube_en_l4[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len_en_l4[i]/2); 
     tube_en_l4[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len_en_l4[i]/2);
     tube_en_l5[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len_en_l5[i]/2); 
     tube_en_l5[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len_en_l5[i]/2);
     for(int j = 0; j < 8; j++){
     float slotpos_x = (-3.5 + 1.*j);
       tube_en_l2[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",2,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len_en_l2[i]/2);
       tube_en_l2[i][1]->AddNode(tube_en_l2[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));
       tube_en_l3[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",2,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len_en_l3[i]/2);
       tube_en_l3[i][1]->AddNode(tube_en_l3[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));
       tube_en_l4[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",2,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len_en_l4[i]/2);
       tube_en_l4[i][1]->AddNode(tube_en_l4[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));
       tube_en_l5[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",2,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len_en_l5[i]/2);
       tube_en_l5[i][1]->AddNode(tube_en_l5[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));

       tube_en_l2[i][1]->SetLineColor(kBlue);
       tube_en_l2[i][2]->SetLineColor(kBlue);
       tube_en_l3[i][1]->SetLineColor(kBlue);
       tube_en_l3[i][2]->SetLineColor(kBlue);
       tube_en_l4[i][1]->SetLineColor(kBlue);
       tube_en_l4[i][2]->SetLineColor(kBlue);
       tube_en_l5[i][1]->SetLineColor(kBlue);
       tube_en_l5[i][2]->SetLineColor(kBlue);
     } 
     tube_en_l2[i][0]->AddNode(tube_en_l2[i][1],1); 
     tube_en_l3[i][0]->AddNode(tube_en_l3[i][1],1); 
     tube_en_l4[i][0]->AddNode(tube_en_l4[i][1],1); 
     tube_en_l5[i][0]->AddNode(tube_en_l5[i][1],1); 
     
      tube_en_l2[i][0]->SetLineColor(kBlue);
      tube_en_l3[i][0]->SetLineColor(kBlue);
      tube_en_l4[i][0]->SetLineColor(kBlue);
      tube_en_l5[i][0]->SetLineColor(kBlue);
   }

   for(int i=0; i<72; i++) {
     tube_en_l6[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len_en_l6[i]/2); 
     tube_en_l6[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len_en_l6[i]/2);
     for(int j = 0; j < 8; j++){
     float slotpos_x = (-3.5 + 1.*j);
       tube_en_l6[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",2,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len_en_l6[i]/2);
       tube_en_l6[i][1]->AddNode(tube_en_l6[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));
     } 
     tube_en_l6[i][0]->AddNode(tube_en_l6[i][1],1); 
   }

    TGeoRotation    *rrr0 = new TGeoRotation();
//    rrr0->RotateZ(90);
//    rrr0->RotateY(-90);
    rrr0->RotateZ(90);
    rrr0->RotateY(90);
    rrr0->RegisterYourself();

    TGeoRotation    *rrr1 = new TGeoRotation();
    rrr1->RotateX(90);
    rrr1->RegisterYourself();

    TGeoRotation    *rrr2 = new TGeoRotation();
    rrr2->RotateX(90);
    rrr2->RotateY(180);
    rrr2->RegisterYourself();

///////////// Layer#2 /////////////////////
  pos_z = 255.215;
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {

     if(i<=12)          { pos_y=(-10.75-i*8.5);}
     if(i>=13 && i<26)  { pos_y=(-13.75-i*8.5.);}
     if(i>=26 && i<39)  { pos_y=(0.75+(i-26)*8.5);}
     if(i>=39)          { pos_y=(3.75+(i-26)*8.5);}

     if(i<3)            { pos_x=(-1)**l*(146.6-1.5-3.5);                    name(tube_en_l2[l*34][0],2,l,2,25-i,0);     
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));}                    
     if(i>=3 && i<11)   { pos_x=(-1)**l*(10*l+116.5-1.5-3.5);               name(tube_en_l2[1+l*34][0],2,l,2,25-i,0);   
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[1+l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));    }    
     if(i==11)          { pos_x=(-1)**l*(10*l+112.25-(i-11)*8.5/2-1.5-3.5); name(tube_en_l2[i-9+l*34][0],2,l,2,25-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[i-9+l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));  }  
     if(i>=12 && i<26)  { pos_x=(-1)**l*(10*l+110.75-(i-11)*8.5/2-1.5-3.5); name(tube_en_l2[i-9+l*34][0],2,l,2,25-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[i-9+l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));  }  
     if(i>=26 && i<30)  { pos_x=(-1)**l*(146.6-1.5-3.5);                    name(tube_en_l2[17+l*34][0],2,l,2,i,0);     
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[17+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));      }   
     if(i>=30 && i<=37) { pos_x=(-1)**l*(10*l+116.5-1.5-3.5);               name(tube_en_l2[18+l*34][0],2,l,2,i,0);     
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[18+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));      }   
     if(i==38)          { pos_x=(-1)**l*(10*l+112.25-(i-38)*8.5/2-1.5-3.5); name(tube_en_l2[19+l*34][0],2,l,2,i,0);     
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[19+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));      }   
     if(i>38)           { pos_x=(-1)**l*(10*l+110.75-(i-38)*8.5/2-1.5-3.5); name(tube_en_l2[i-19+l*34][0],2,l,2,i,0);   
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);  mnk3[2]->AddNode(tube_en_l2[i-19+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }     
   
   } 
  }

///////////// Layer#3 /////////////////////////////
////////////No reflection for L3&L4 from L2! because an open part of aluminum profile has to be look on strips (along Z)
/////////// No translation/rotation for L3&L4 from L2! because of different geometry layout in XY plane  
  pos_z = 255.215+9;
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {
 
     if(i<=12)          { pos_y=(2.25-i*8.5);}
     if(i>=13 && i<27)  { pos_y=(-0.75-i*8.5.);}
     if(i>=27 && i<40)  { pos_y=(13.75+(i-27)*8.5);}
     if(i>=40)          { pos_y=(16.75+(i-27)*8.5);}

     if(i<4)            { pos_x=(-1)**l*(146.6-1.5-3.5);                    name(tube_en_l3[l*34][0],2,l,3,26-i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=4 && i<12)   { pos_x=(-1)**l*(10*l+116.5-1.5-3.5);               name(tube_en_l3[1+l*34][0],2,l,3,26-i,0);      
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[1+l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i==12 || i ==13){ pos_x=(-1)**l*(10*l+112.25-(i-12)*8.5/2-1.5-3.5); name(tube_en_l3[(i-10)+l*34][0],2,l,3,26-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[(i-10)+l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=14 && i<27)  { pos_x=(-1)**l*(10*l+110.75-(i-12)*8.5/2-1.5-3.5); name(tube_en_l3[(i-10)+l*34][0],2,l,3,26-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[(i-10)+l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=27 && i<30)  { pos_x=(-1)**l*(146.6-1.5-3.5);                    name(tube_en_l3[17+l*34][0],2,l,3,i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[17+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=30 && i<38)  { pos_x=(-1)**l*(10*l+116.5-1.5-3.5);               name(tube_en_l3[18+l*34][0],2,l,3,i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[18+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i==38)          { pos_x=(-1)**l*(10*l+112.25-(i-38)*8.5/2-1.5-3.5); name(tube_en_l3[(i-19)+l*34][0],2,l,3,i,0);    
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[(i-19)+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=39 )         { pos_x=(-1)**l*(10*l+110.75-(i-38)*8.5/2-1.5-3.5); name(tube_en_l3[(i-19)+l*34][0],2,l,3,i,0);    
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[3]->AddNode(tube_en_l3[(i-19)+l*34][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }     
   
   } 
  }

///////////// Layer#4 /////////////////////////////
  pos_z = 255.215+18;
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {
   
     if(i<=12)          { pos_y=(-10.75-i*8.5);}
     if(i>=13 && i<26)  { pos_y=(-13.75-i*8.5.);}
     if(i>=26 && i<39)  { pos_y=(0.75+(i-26)*8.5);}
     if(i>= 39)         { pos_y=(3.75+(i-26)*8.5);}

     if(i<3)            { pos_x=(-1)**l*(146.6-1.5-3.5);               name(tube_en_l4[34*l][0],2,l,4,25-i,0);       
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=3 && i<11)   { pos_x=(-1)**l*(126.5-1.5-3.5);               name(tube_en_l4[1+34*l][0],2,l,4,25-i,0);     
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[1+l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i==11 || i==12) { pos_x=(-1)**l*(122.25-(i-11)*8.5/2-1.5-3.5); name(tube_en_l4[(i-9)+34*l][0],2,l,4,25-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[(i-9)+l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=13 && i<26)  { pos_x=(-1)**l*(120.75-(i-11)*8.5/2-1.5-3.5); name(tube_en_l4[(i-9)+34*l][0],2,l,4,25-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[(i-9)+l*34][0],25-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=26 && i<30)  { pos_x=(-1)**l*(146.6-1.5-3.5);               name(tube_en_l4[17+34*l][0],2,l,4,i,0);       
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[17+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=30 && i<38)  { pos_x=(-1)**l*(126.5-1.5-3.5);               name(tube_en_l4[18+34*l][0],2,l,4,i,0);       
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[18+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i==38)          { pos_x=(-1)**l*(122.25-(i-38)*8.5/2-1.5-3.5); name(tube_en_l4[19+34*l][0],2,l,4,i,0);       
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[19+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>38)           { pos_x=(-1)**l*(120.75-(i-38)*8.5/2-1.5-3.5); name(tube_en_l4[(i-19)+34*l][0],2,l,4,i,0);   
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[4]->AddNode(tube_en_l4[(i-19)+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}    
 
   } 
  }

///////////// Layer#5 /////////////////////////////
//////////////  Large central hole for Straw ///////////////////////
  pos_z = (255.215+27);
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {
 
     if(i<=12)          { pos_y=(2.25-i*8.5);}
     if(i>=13 && i<27)  { pos_y=(-0.75-i*8.5.);}
     if(i>=27 && i<40)  { pos_y=(13.75+(i-27)*8.5);}
     if(i>=40)          { pos_y=(16.75+(i-27)*8.5);}

     if(i<8)            { pos_x=(-1)**l*(161.5-1.5-3.5);                     name(tube_en_l5[l*34][0],2,l,5,26-i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=8 && i<12)   { pos_x=(-1)**l*(-10*l+126.5-1.5-3.5);               name(tube_en_l5[1+l*34][0],2,l,5,26-i,0);      
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[1+l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i==12)          { pos_x=(-1)**l*(-10*l+122.25-(i-12)*8.5/2-1.5-3.5); name(tube_en_l5[2+l*34][0],2,l,5,26-i,0);      
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[2+l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=13 && i<27)  { pos_x=(-1)**l*(-10*l+120.75-(i-12)*8.5/2-1.5-3.5); name(tube_en_l5[(i-10)+l*34][0],2,l,5,26-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[(i-10)+l*34][0],26-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=27 && i<34)  { pos_x=(-1)**l*(161.5-1.5-3.5);                     name(tube_en_l5[17+l*34][0],2,l,5,i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[17+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=34 && i<38)  { pos_x=(-1)**l*(-10*l+126.5-1.5-3.5);               name(tube_en_l5[18+l*34][0],2,l,5,i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[18+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i==38)          { pos_x=(-1)**l*(-10*l+122.25-(i-38)*8.5/2-1.5-3.5); name(tube_en_l5[(i-19)+l*34][0],2,l,5,i,0);    
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[(i-19)+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=39)          { pos_x=(-1)**l*(-10*l+120.75-(i-38)*8.5/2-1.5-3.5); name(tube_en_l5[(i-19)+l*34][0],2,l,5,i,0);    
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[5]->AddNode(tube_en_l5[(i-19)+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}         
     
   } 
  }

///////////// Layer#6 /////////////////////////////
//////////////  Large central hole for Straw ///////////////////////
  pos_z = (255.215+36);
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {
 
     if(i<=12)         { pos_y=(-10.75-i*8.5); }
     if(i>=13 && i<26) { pos_y=(-13.75-i*8.5.); } 
     if(i>=26 && i<39) { pos_y=(0.75+(i-26)*8.5); }
     if(i>=39)         { pos_y=(3.75+(i-26)*8.5); }

     if(i<8)            { pos_x=(-1)**l*(161.5-1.5-3.5);                     name(tube_en_l5[l*34][0],2,l,6,25-i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[l*34][0], 25-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=8 && i<12)   { pos_x=(-1)**l*(-10*l+126.5-1.5-3.5);               name(tube_en_l5[1+l*34][0],2,l,6,25-i,0);      
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[1+l*34][0], 25-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i==12)          { pos_x=(-1)**l*(-10*l+122.25-(i-12)*8.5/2-1.5-3.5); name(tube_en_l5[2+l*34][0],2,l,6,25-i,0);      
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[2+l*34][0], 25-i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=13 && i<26)  { pos_x=(-1)**l*(-10*l+120.75-(i-12)*8.5/2-1.5-3.5); name(tube_en_l5[(i-10)+l*34][0],2,l,6,25-i,0); 
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[(i-10)+l*34][0], 25-i,new TGeoCombiTrans(*tr0, *rrr0));}

     if(i>=26 && i<34)  { pos_x=(-1)**l*(161.5-1.5-3.5);                     name(tube_en_l5[17+l*34][0],2,l,6,i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[17+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=34 && i<=37) { pos_x=(-1)**l*(-10*l+126.5-1.5-3.5);               name(tube_en_l5[18+l*34][0],2,l,6,i,0);        
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[18+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i==38)          { pos_x=(-1)**l*(-10*l+122.25-(i-38)*8.5/2-1.5-3.5); name(tube_en_l5[(i-19)+l*34][0],2,l,6,i,0);    
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[(i-19)+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}
     if(i>=39)          { pos_x=(-1)**l*(-10*l+120.75-(i-38)*8.5/2-1.5-3.5); name(tube_en_l5[(i-19)+l*34][0],2,l,6,i,0);    
                        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk3[6]->AddNode(tube_en_l5[(i-19)+l*34][0], i,new TGeoCombiTrans(*tr0, *rrr0));}    

   } 
  }

///////////// Layer#0/1 "Zero" /////////////////////////////

   for(int l = 0; l < 2; l++) {
    for(int i = 0; i < 36; i++) {
     pos_z =243.5+MDT_thickness/2+3*l; 
     if(i<1)           { pos_x=(-12*8.5 + 8.5/2 -0.25*(-1)**l);            pos_y=(-5+10);              name(tube_en_l0[i][0],2,0,l,i,0);}
     if(i>=1 && i<6)   { pos_x=(-12*8.5 + 8.5/2 -0.25*(-1)**l+8.5*i);      pos_y=(-17.8/2+10/2);       name(tube_en_l0[i][0],2,0,l,i,0);}
     if(i>=6 && i<18)  { pos_x=(-12*8.5 + 8.5/2 -0.25*(-1)**l+8.5*i);      pos_y=(26+6+74.1/2);        name(tube_en_l0[i][0],2,0,l,i,0);}
     if(i>=18 && i<23) { pos_x=(-12*8.5 + 8.5/2 -0.25*(-1)**l+8.5*i);      pos_y=(-17.8/2+10/2);       name(tube_en_l0[i][0],2,0,l,i,0);}
     if(i==23)         { pos_x=(-12*8.5 + 8.5/2 -0.25*(-1)**l+8.5*i);      pos_y=(-5+10);              name(tube_en_l0[i][0],2,0,l,i,0);}    
     if(i>23 && i<36)  { pos_x=(-12*8.5 + 8.5/2 -0.25*(-1)**l+8.5*(i-18)); pos_y=(-26-6-(111.5-22)/2); name(tube_en_l0[i][0],2,0,l,i,0);}
   
    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z);
    if(l==0) mnk3[0]->AddNode(tube_en_l0[i][0], i, new TGeoCombiTrans(*tr0, *rrr2));
    if(l==1) mnk3[1]->AddNode(tube_en_l0[i][0], i, new TGeoCombiTrans(*tr0, *rrr1));
   } 
  }
////////////////////  End of "Zero" layer /////////////////////////////////////

////////////////////////////// Strips for Endcap ///////////////////////
  float ttr[22][3] = {0,0,0, -9.8,0,0, 0,-5,0, 0,-118.5,0, 0,108.4,0, 0,5.4,0, 0,-108.1,0, 0,118.8,0, 0,3.7,0, 0,-109.8,0, 0,117.1,0, 10.1,0,0,
                      0,-2,0, 0,4,0, 0,8,0, 0,122,0, 0,-106,0, 10.5,0,0, 0,121.2,0, -122.6,0,0, 122.6,0,0, 0,120,0};

  TGeoRotation *rot = new TGeoRotation();
  rot->RotateZ(22.5);
  rot->SetName("rot");
  rot->RegisterYourself();

  TGeoTranslation *trans[22];
  for(int i=0; i<22; i++) {
       trans[i] = new TGeoTranslation(ttr[i][0], ttr[i][1], ttr[i][2] );
       trans[i]->SetName(Form("trans%d",i));
       trans[i]->RegisterYourself();
  }

  TGeoTranslation *trans_l0;
  trans_l0 = new TGeoTranslation(0, -3.4, 0);
  trans_l0->SetName(Form("trans_l%d",0));
  trans_l0->RegisterYourself();

  TGeoCombiTrans *combl0   = new TGeoCombiTrans(*trans_l0, *rot);
  combl0->SetName("combl0");
  combl0->RegisterYourself();


  TGeoCombiTrans *comb0   = new TGeoCombiTrans(*trans[0], *rot);
  comb0->SetName("comb0");
  comb0->RegisterYourself();

  TGeoCombiTrans *comb1   = new TGeoCombiTrans(*trans[13], *rot);
  comb1->SetName("comb1");
  comb1->RegisterYourself();
    
  TGeoVolume *G10_endcap[6], *ST_endcap[6];

  TGeoBBox* bar1 = new TGeoBBox("bar1", 10.5, 240, 0.175);    
  TGeoBBox* bar2 = new TGeoBBox("bar2", 231,  1.8, 0.175);    
  TGeoBBox* bar3 = new TGeoBBox("bar3", 219,  1.8, 0.175);    
  TGeoBBox* bar4 = new TGeoBBox("bar4", 20.5, 232, 0.175);    
  TGeoBBox* bar5 = new TGeoBBox("bar5", 200., 10,  0.175);    

  TGeoPgon *ppgon_l0 = new TGeoPgon("ppg_l0", 0., 360., 8, 2);
//  ppgon_l0->DefineSection(0, 0, 0, 127.);
//  ppgon_l0->DefineSection(1, 0.15, 0, 127.);
  ppgon_l0->DefineSection(0, 0, 0, 120.);
  ppgon_l0->DefineSection(1, 0.15, 0, 120.);
  TGeoVolume *ppgon1_l0 = new TGeoVolume("ppgon1_l0", ppgon_l0, med7);
//  TGeoVolume *hhole_l0 = geom->MakeBox("hhole_l0", med1, 48, 30, 0.16);
  TGeoVolume *hhole_l0 = geom->MakeBox("hhole_l0", med1, 51, 32, 0.16);

  TGeoPgon *ppgon_al_l0 = new TGeoPgon("ppg_al_l0", 0., 360., 8, 2);
//  ppgon_al_l0->DefineSection(0, 0, 0, 127.);
//  ppgon_al_l0->DefineSection(1, 0.035, 0, 127.);
  ppgon_al_l0->DefineSection(0, 0, 0, 120.);
  ppgon_al_l0->DefineSection(1, 0.035, 0, 120.);
  TGeoVolume *ppgon1_al_l0 = new TGeoVolume("ppgon1_al_l0", ppgon_al_l0, med2);

  TGeoPgon *ppgon_l2 = new TGeoPgon("ppg_l2", 0., 360., 8, 2);
//  ppgon_l2->DefineSection(0, 0, 0, 228.);
//  ppgon_l2->DefineSection(1, 0.15, 0, 228.);
  ppgon_l2->DefineSection(0, 0, 0, 224.);
  ppgon_l2->DefineSection(1, 0.15, 0, 224.);
  TGeoVolume *ppgon1_l2 = new TGeoVolume("ppgon1_l2", ppgon_l2, med7);
  TGeoVolume *hhole_l2 = geom->MakeBox("hhole_l2", med1, 60.75, 31.9, 0.16);

  TGeoPgon *ppgon_al_l2 = new TGeoPgon("ppg_al_l2", 0., 360., 8, 2);
//  ppgon_al_l2->DefineSection(0, 0, 0, 228.);
//  ppgon_al_l2->DefineSection(1, 0.035, 0, 228.);
  ppgon_al_l2->DefineSection(0, 0, 0, 224.);
  ppgon_al_l2->DefineSection(1, 0.035, 0, 224.);
  TGeoVolume *ppgon1_al_l2 = new TGeoVolume("ppgon1_al_l2", ppgon_al_l2, med2);

  TGeoVolume *hhole_l5 = geom->MakeBox("hhole_l5", med1, 90.5, 65.0, 0.16);
  TGeoVolume *hhole_l6 = geom->MakeBox("hhole_l6", med1, 90.5, 70.5, 0.16);
  TGeoVolume *hhole_l7 = geom->MakeBox("hhole_l7", med1, 90.5, 57.5, 0.16);
  TGeoVolume *hhole_l8 = geom->MakeBox("hhole_l8", med1, 90.5, 56.5, 0.16);
  
  TGeoCompositeShape *LEndcap[7], *MEndcap[7]; 

///////////// Strips for "Zero" Layer /////////////////////////////
///////////// Strip board + Double-sided strips////////////////
//  TGeoCompositeShape *L0_endcap = new TGeoCompositeShape("(ppg_l0:comb0 - hhole_l0:trans0)"); 
//  TGeoCompositeShape *L0_endcap_al = new TGeoCompositeShape("(ppg_al_l0:comb0 - hhole_l0:trans0)"); 
  TGeoCompositeShape *L0_endcap = new TGeoCompositeShape("(ppg_l0:combl0 - hhole_l0:trans0 - bar4:trans19 - bar4:trans20 - bar5:trans21)"); 
  TGeoCompositeShape *L0_endcap_al = new TGeoCompositeShape("(ppg_al_l0:combl0 - hhole_l0:trans0 - bar4:trans19 - bar4:trans20 - bar5:trans21)"); 

  G10_endcap[0] = new TGeoVolume("GP0_E", L0_endcap, med7);
  ST_endcap[0] = new TGeoVolume("ST0_E", L0_endcap_al, med2);
  ST_endcap[1] = new TGeoVolume("ST1_E", L0_endcap_al, med2);
  
  
/////////////////////////////////////////////////////////
  char longbuffer[250];
  char longbuffer1[250];
  
  int qq[5][10]={2,0,1,1,2,2,3,3,3,4,
                 2,0,1,1,2,5,3,6,3,7,
                 2,0,4,0,2,2,3,3,3,4,
                 5,0,1,11,2,8,3,9,3,10,
                 6,2,1,11,2,2,3,3,3,4}; 
  for(int i=2; i<7; i++){
    sprintf(longbuffer,"ppg_l2:comb0 - hhole_l%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d",
    qq[i-2][0],qq[i-2][1],qq[i-2][2],qq[i-2][3],qq[i-2][4],qq[i-2][5],qq[i-2][6],qq[i-2][7],qq[i-2][8],qq[i-2][9]);
    sprintf(longbuffer1,"ppg_al_l2:comb0 - hhole_l%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d",
    qq[i-2][0],qq[i-2][1],qq[i-2][2],qq[i-2][3],qq[i-2][4],qq[i-2][5],qq[i-2][6],qq[i-2][7],qq[i-2][8],qq[i-2][9]);

    LEndcap[i] = new TGeoCompositeShape(longbuffer);
    MEndcap[i] = new TGeoCompositeShape(longbuffer1);
    G10_endcap[i] = new TGeoVolume(Form("GP%d_E",i), LEndcap[i], med7);
    ST_endcap[i] = new TGeoVolume(Form("ST%d_E",i), MEndcap[i], med7);
  }
    for(int ii = 0; ii < 7; ii++){
      if(ii == 1) { ST_endcap[ii]->SetLineColor(kGreen); } 
      else {
       G10_endcap[ii]->SetLineColor(kRed);
       ST_endcap[ii]->SetLineColor(kGreen);
      } 
     }    

////////////// Muon Filter ////////////////////
////////////// Layer#3 ////////////////////////
  pos_z = 329.715; // 2960+4*60+3*30+4.5+12.3/2=330.065
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {

    if(i <= 12)            { pos_y=(2.25-i*8.5); }
    if(i >= 13 && i < 27)  { pos_y=(-0.75-i*8.5.); }
    if(i >= 27 && i <= 39) { pos_y=(13.75+(i-27)*8.5); }
    if(i >= 40)            { pos_y=(16.75+(i-27)*8.5); }

     if(i<7)            { pos_x=(-1)**l*161.5;                        name(tube_en_l6[l*36][0],3,l,3,26-i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=7 && i<=10)  { pos_x=(-1)**l*(-10*l+126.5);                name(tube_en_l6[1+l*36][0],3,l,3,26-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[1+l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i==11)          { pos_x=(-1)**l*(-10*l+122.25-(i-11)*8.5/2);  name(tube_en_l6[2+l*36][0],3,l,3,26-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[2++l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=12 && i<=26) { pos_x=(-1)**l*(-10*l+120.75-(i-11)*8.5/2);  name(tube_en_l6[(i-9)+l*36][0],3,l,3,26-i,0);  TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[(i-9)+l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }

     if(i>=27 && i<=32) { pos_x=(-1)**l*161.5;                        name(tube_en_l6[18+l*36][0],3,l,3,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[18+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=33 && i<=36) { pos_x=(-1)**l*(-10.*l+126.5);               name(tube_en_l6[19+l*36][0],3,l,3,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[19+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i==37 || i==38) { pos_x=(-1)**l*(-10.*l+122.25-(i-37)*8.5/2); name(tube_en_l6[(i-17)+l*36][0],3,l,3,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[(i-17)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=39)          { pos_x=(-1)**l*(-10.*l+120.75-(i-37)*8.5/2); name(tube_en_l6[(i-17)+l*36][0],3,l,3,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[3]->AddNode(tube_en_l6[(i-17)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }     
   } 
  }

////////////// Layer#2 ////////////////////////
  pos_z = 320.715;
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {

    if(i<=12)          { pos_y=(-10.75-i*8.5); }
    if(i>=13 && i<26)  { pos_y=(-13.75-i*8.5.); }
    if(i>=26 && i<=38) { pos_y=(0.75+(i-26)*8.5); }
    if(i>=39)          { pos_y=(3.75+(i-26)*8.5); }

     if(i<6)            { pos_x=(-1)**l*161.5;                        name(tube_en_l6[l*36][0],3,l,2,25-i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=6 && i<=9)   { pos_x=(-1)**l*(-10*l+126.5);                name(tube_en_l6[1+l*36][0],3,l,2,25-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[1+l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i==10)          { pos_x=(-1)**l*(-10*l+122.25-(i-10)*8.5/2);  name(tube_en_l6[2+l*36][0],3,l,2,25-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[2+l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=11 && i<26)  { pos_x=(-1)**l*(-10*l+120.75-(i-10)*8.5/2);   name(tube_en_l6[(i-8)+l*36][0],3,l,2,25-i,0);  TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[(i-8)+l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=26 && i<=32) { pos_x=(-1)**l*161.5;                        name(tube_en_l6[18+l*36][0],3,l,2,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[18+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=33 && i<=37) { pos_x=(-1)**l*(-10.*l+126.5);               name(tube_en_l6[19+l*36][0],3,l,2,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[19+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i==38 || i==39) { pos_x=(-1)**l*(-10.*l+122.25-(i-38)*8.5/2); name(tube_en_l6[(i-18)+l*36][0],3,l,2,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[(i-18)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=40)          { pos_x=(-1)**l*(-10.*l+120.75-(i-38)*8.5/2);  name(tube_en_l6[(i-18)+l*36][0],3,l,2,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[2]->AddNode(tube_en_l6[(i-18)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }     
   } 
  }

////////////// Layer#1 ////////////////////////
  pos_z = 311.715;
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {

    if(i<=12)          { pos_y=(2.25-i*8.5); }
    if(i>=13 && i<27)  { pos_y=(-0.75-i*8.5.); }
    if(i>=27 && i<=39) { pos_y=(13.75+(i-27)*8.5); }
    if(i>=40)          { pos_y=(16.75+(i-27)*8.5); }

     if(i<7)            { pos_x=(-1)**(l+1)*161.5;                        name(tube_en_l6[l*36][0],3,(1-l),1,26-i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=7 && i<=10)  { pos_x=(-1)**(l+1)*(-10*l+126.5);                name(tube_en_l6[1+l*36][0],3,(1-l),1,26-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[1+l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i==11)          { pos_x=(-1)**(l+1)*(-10*l+122.25-(i-11)*8.5/2);  name(tube_en_l6[2+l*36][0],3,(1-l),1,26-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[2+l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=12 && i<=26) { pos_x=(-1)**(l+1)*(-10*l+120.75-(i-11)*8.5/2);  name(tube_en_l6[(i-9)+l*36][0],3,(1-l),1,26-i,0);  TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[(i-9)+l*36][0],26-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=27 && i<=32) { pos_x=(-1)**(l+1)*161.5;                        name(tube_en_l6[18+l*36][0],3,(1-l),1,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[18+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=33 && i<=36) { pos_x=(-1)**(l+1)*(-10.*l+126.5);               name(tube_en_l6[19+l*36][0],3,(1-l),1,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[19+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i==37 || i==38) { pos_x=(-1)**(l+1)*(-10.*l+122.25-(i-37)*8.5/2); name(tube_en_l6[(i-17)+l*36][0],3,(1-l),1,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[(i-17)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=39)          { pos_x=(-1)**(l+1)*(-10.*l+120.75-(i-37)*8.5/2); name(tube_en_l6[(i-17)+l*36][0],3,(1-l),1,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[1]->AddNode(tube_en_l6[(i-17)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }     

   } 
  }

////////////// Layer#0 ////////////////////////
  pos_z = 302.715;  
  for(int l = 0; l < 2; l++) {
   for(int i = 0; i < 53; i++) {

    if(i<=12)          { pos_y=(-10.75-i*8.5); }
    if(i>=13 && i<26)  { pos_y=(-13.75-i*8.5.); }
    if(i>=26 && i<=38) { pos_y=(-0.75+(i-26)*8.5); }
    if(i>=39)          { pos_y=(3.75+(i-26)*8.5); }

     if(i<6)            { pos_x=(-1)**(l+1)*161.5;                        name(tube_en_l6[l*36][0],3,(1-l),0,25-i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=6 && i<=9)   { pos_x=(-1)**(l+1)*(-10*l+126.5);                name(tube_en_l6[1+l*36][0],3,(1-l),0,25-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[1+l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i==10)          { pos_x=(-1)**(l+1)*(-10*l+122.25-(i-10)*8.5/2);  name(tube_en_l6[2+l*36][0],3,(1-l),0,25-i,0);      TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[2+l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=11 && i<26)  { pos_x=(-1)**(l+1)*(-10*l+120.75-(i-10)*8.5/2);  name(tube_en_l6[(i-8)+l*36][0],3,(1-l),0,25-i,0);  TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[(i-8)+l*36][0],25-i,new TGeoCombiTrans(*tr0, *rrr0)); }
     if(i>=26 && i<=32) { pos_x=(-1)**(l+1)*161.5;                        name(tube_en_l6[18+l*36][0],3,(1-l),0,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[18+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=33 && i<=37) { pos_x=(-1)**(l+1)*(-10.*l+126.5);               name(tube_en_l6[19+l*36][0],3,(1-l),0,i,0);        TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[19+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i==38 || i==39) { pos_x=(-1)**(l+1)*(-10.*l+122.25-(i-38)*8.5/2); name(tube_en_l6[(i-18)+l*36][0],3,(1-l),0,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[(i-18)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }
     if(i>=40)          { pos_x=(-1)**(l+1)*(-10.*l+120.75-(i-38)*8.5/2); name(tube_en_l6[(i-18)+l*36][0],3,(1-l),0,i,0);    TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); mnk4[0]->AddNode(tube_en_l6[(i-18)+l*36][0],i,new TGeoCombiTrans(*tr0, *rrr0));    }     
   } 
  }

//////////// Absorber for MF ///////////////
///////////////// Octagon & hole //////////////////////////
  TGeoVolume *fmf[4];
  TGeoTranslation *ftr[4]; 

  TGeoPgon *pgon = new TGeoPgon("pg", 0., 360., 8, 2);
  pgon->DefineSection(0, 0, 0, 240);
  pgon->DefineSection(1, 6, 0, 240);

  TGeoVolume *pgon1 = new TGeoVolume("pgon1", pgon, med6);
  TGeoVolume *hole = geom->MakeBox("hole", med1, 77, 52, 7.5);

  TGeoCompositeShape *cs = new TGeoCompositeShape("ccs", "(pg:comb0 - hole:trans0)"); 
  
  for(int i=0; i<4; i++) {
    fmf[i] = new TGeoVolume(Form("FeMFLayer%d",i), cs, med6);
    ftr[i] = new TGeoTranslation(0,0,296+9*i);
    ftr[i]->SetName("ftr%d");
    ftr[i]->RegisterYourself();
  }
////////////////////////////////////////////////////////////////
//////////////////////Strips for MF //////////////
  TGeoVolume *G10_mf[4], *ST_mf[4];
  TGeoCompositeShape *G10mf[4], *STmf[4]; 
  char longbb[250];
  char longbb1[250];
  
  int qq2[4][11]={0,7,12,1,1,2,2,3,4,3,3,
                  1,7,12,1,1,2,2,3,4,3,3,
                  0,8,0,1,17,2,2,3,4,4,3,
                  1,8,8,1,17,2,14,3,18,3,16}; 

  for(int i=0; i<4; i++){
    sprintf(longbb,"ppg_l2:comb%d - hhole_l%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d",
    qq2[i][0],qq2[i][1],qq2[i][2],qq2[i][3],qq2[i][4],qq2[i][5],qq2[i][6],qq2[i][7],qq2[i][8],qq2[i][9],qq2[i][10]);
    sprintf(longbb1,"ppg_al_l2:comb%d - hhole_l%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d - bar%d:trans%d",
    qq2[i][0],qq2[i][1],qq2[i][2],qq2[i][3],qq2[i][4],qq2[i][5],qq2[i][6],qq2[i][7],qq2[i][8],qq2[i][9],qq2[i][10]);

    G10mf[i] = new TGeoCompositeShape(longbb);
    STmf[i] = new TGeoCompositeShape(longbb1);
    G10_mf[i] = new TGeoVolume(Form("GP%d_MF",i), G10mf[i], med7);
    ST_mf[i] = new TGeoVolume(Form("ST%d_MF",i), STmf[i], med7);
  }

    for(int ii = 0; ii <4; ii++){
      G10_mf[ii]->SetLineColor(kRed);
      ST_mf[ii]->SetLineColor(kBlue); 
//      mnk4[ii]->AddNode(G10_mf[ii], ii, new TGeoTranslation(0, 0., (302.715+9*ii+MDT_thickness/2+0.1)));     
//      mnk4[ii]->AddNode(ST_mf[ii], ii, new TGeoTranslation(0, 0, (302.715+9*ii+MDT_thickness/2+0.26)));           
    }    
//////////////////////////////////////////////////////////

///////////// Forward Range System //////////////////////////
////////////// Absorber plates ///////////////////////////
   char fe[12], hl[12], ffe[12];
   TGeoTranslation *frot[16];
   TGeoVolume *FeForward[16];
    
    TGeoVolume *vol =  geom->MakeBox("fe", med6, 380/2, 252.5/2, 6/2);
    TGeoVolume *vol1 = geom->MakeBox("hl", med1,  30/2,    34/2, 7.5/2);
    TGeoTranslation *vv = new TGeoTranslation((15+1.8), 0, 0);
    vv->SetName("vv");
    vv->RegisterYourself();    
    TGeoCompositeShape *csq = new TGeoCompositeShape("ffe", "(fe - (hl:vv))"); 

   int yt=0, kk=0;
   for (int i=0; i<16; i++) {
    yt=(i%4);
    if(yt==0) kk++;
    if(i<16) { 
     frot[i] = new TGeoTranslation(kk*1.45, 0, 873+9*i); 
     FeForward[i] = new TGeoVolume(Form("FeForwardLayer%d",i),csq,med6);
    }
    else { frot[i] = new TGeoTranslation(4.35, 0, 866.5-4*(i-16)); }
    frot[i]->SetName(Form("frot%d",i));
    frot[i]->RegisterYourself();
   }

   for(int i=0; i<16; i++) pCombi3->AddNode(FeForward[i], i, frot[i]);     
//////////////////////////////////////////////////////////////////

  float tube_len_fw[3] = { 350.5, 135.95, 169.55 };

   for(int i=0; i<3; i++) { 
     tube_fw[i][0] = geom->MakeBox(Form("BP%d",i), med5, MDT_width/2, MDT_thickness/2, tube_len_fw[i]/2); 
     tube_fw[i][1] = geom->MakeBox(Form("BA%d",i), med2, XSize/2,     YSize/2,         tube_len_fw[i]/2);
     for(int j = 0; j < 8; j++){
     float slotpos_x = (-3.5 + 1.*j);
       tube_fw[i][2] = geom->MakeBox(Form("MDT%ds%dl%db%dw%d",4,0,0,0,0), med4, XSize1/2, YSize1/2, tube_len_fw[i]/2);
       tube_fw[i][1]->AddNode(tube_fw[i][2], j, new TGeoTranslation(slotpos_x, slotpos_y, 0.));
     }
     tube_fw[i][0]->AddNode(tube_fw[i][1],1); 
   }

  int dd,dd1;
  for(int l = 0; l < 18; l++){      // layer#
   for(int i = 0; i < 32; i++){  //MDT# in layer
    dd = (l/4); dd1=(l%2); 
    if(l<16) { pos_z = (876.1 + MDT_thickness/2 + l*9); }
    else { pos_z = (870 - 0.1 - MDT_thickness/2 - (l-16)*(MDT_thickness+0.45)); dd=0; }
    
    if((i<12 || i>15) && i<28) { pos_x = 1.45*dd;  pos_y = (-114.75+8.5*i+0.5*dd1); name(tube_fw[0][0],4,0,l,i,0); 
                                 TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); 
                                 mnk5[l]->AddNode(tube_fw[0][0], i, new TGeoCombiTrans(*tr0, *rr0));}   
    if(i>=12 && i<=15)         { pos_x = (107.275+1.45*dd); pos_y = (-12.75+8.5*(i-12)+0.5*dd1); name(tube_fw[1][0],4,0,l,i,0); 
                                 TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); 
                                 mnk5[l]->AddNode(tube_fw[1][0], i, new TGeoCombiTrans(*tr0,*rr0));}   
    if(i>=28)                  { pos_x = (-90.475+1.45*dd); pos_y = (-12.75+8.5*(i-28)+0.5*dd1); name(tube_fw[2][0],4,0,l,i,0);
                                 TGeoTranslation *tr0 = new TGeoTranslation(pos_x, pos_y, pos_z); 
                                 mnk5[l]->AddNode(tube_fw[2][0], i, new TGeoCombiTrans(*tr0,*rr0));}                                    
   }
  }
   
//   for(int i=16; i<18; i++) { 
   for(int i=0; i<18; i++) { 
    if(i<16) { pCombi3->AddNode(mnk5[i], i+2); }
    else { pCombi3->AddNode(mnk5[i], 17-i); }     
   }
////////////// Strips for Forward System ///////////////////
  TGeoVolume *G10_fs[18], *ST_fs[18];
  TGeoCompositeShape *G10fs[18], *STfs[18]; 
  TGeoTranslation *fsrot[18], *fsrot1[18];
  char longbbs[250];
  char longbbs1[250];

  TGeoVolume *vols =  geom->MakeBox("shl",  med7,  351.2/2,  238/2, 0.15/2);
  TGeoVolume *vols1 = geom->MakeBox("shl1", med7,  44.8/2,    34/2, 0.25/2);
  TGeoVolume *vols2 = geom->MakeBox("shl2", med2,  351.2/2,  238/2, 0.035/2);
  TGeoVolume *vols3 = geom->MakeBox("shl3", med2,  44.8/2,    34/2, 0.055/2);
  TGeoTranslation *vv1 = new TGeoTranslation(17.7, 0, 0);
  vv1->SetName("vv1");
  vv1->RegisterYourself();    

  kk=0;
  for(int i=0; i<17; i++) {
    yt=(i%4);
    if(yt==0) kk++;
    sprintf(longbbs,"shl - shl1:vv1",i);
    sprintf(longbbs1,"shl2 - shl3:vv1",i);
     G10fs[i] = new TGeoCompositeShape(longbbs);
     STfs[i] = new TGeoCompositeShape(longbbs1);

    if(i<16) {
     fsrot[i] = new TGeoTranslation(kk*1.45-2, 0, 873+3+1.23+0.1+9*i); 
     fsrot1[i] = new TGeoTranslation(kk*1.45-2, 0, 873+3+1.23+0.26+9*i); 
     G10_fs[i] = new TGeoVolume(Form("GP%d_FS",i+2), G10fs[i], med7);
     ST_fs[i] = new TGeoVolume(Form("ST%d_FS",i+2), STfs[i], med7);
     } else {
        fsrot[i] = new TGeoTranslation(-2, 0, 873-3-1.23-0.1-0.26); 
        fsrot1[i] = new TGeoTranslation(-2, 0, 873-3-1.23-0.1-0.16);    
//        fsrot1[i] = new TGeoTranslation(-2, 0, 873-3-1.23-0.1-0.360);  0 ->17   
//        G10_fs[i] = new TGeoVolume(Form("GP%d_FS",17-i), G10fs[i], med7);
        ST_fs[i] = new TGeoVolume(Form("ST%d_FS",17-i), STfs[i], med7);
      } 

    ST_fs[i]->SetLineColor(kBlue);
//    G10_fs[i]->SetLineColor(kRed);
  } 

   for(int i=0; i<17; i++) { 
    if(i<16) { pCombi3->AddNode(G10_fs[i], i+2, fsrot[i]); pCombi3->AddNode(ST_fs[i], i+2, fsrot1[i]); }
    else { pCombi3->AddNode(G10_fs[i], 17-i, fsrot[i]); pCombi3->AddNode(ST_fs[i], 17-i, fsrot1[i]); }
   }

////////////////////////////////////////////////////////////


///////////// Barrel ////////////////////////////
  pCombi->AddNode(MdtOct0, 0, new TGeoCombiTrans(0., 0., 0., rot0));
  pCombi->AddNode(MdtOct0, 1, new TGeoCombiTrans(0., 0., 0., rot1));
  pCombi->AddNode(MdtOct2, 2, new TGeoCombiTrans(0., 0., 0., rot2));
  pCombi->AddNode(MdtOct0, 3, new TGeoCombiTrans(0., 0., 0., rot3));
  pCombi->AddNode(MdtOct0, 4, new TGeoCombiTrans(0., 0., 0., rot4));
  pCombi->AddNode(MdtOct0, 5, new TGeoCombiTrans(0., 0., 0., rot5));
  pCombi->AddNode(MdtOct6, 6, new TGeoCombiTrans(0., 0., 0., rot6));
  pCombi->AddNode(MdtOct0, 7, new TGeoCombiTrans(0., 0., 0., rot7));


//////////////// Endcap ///////////////////
////////////// MDT planes ///////////////
  pCombi1->AddNode(mnk3[0], 1);
  pCombi1->AddNode(mnk3[1], 2);
  pCombi1->AddNode(mnk3[2], 3);
  pCombi1->AddNode(mnk3[3], 4);
  pCombi1->AddNode(mnk3[4], 5);
  pCombi1->AddNode(mnk3[5], 6);
  pCombi1->AddNode(mnk3[6], 7);
////////////////////////////////////////////
//////////// Strips for Endcap: G10+Aluminium///////////
   for(int i = 0; i < 7; i++){
     if(i==0) {
       pCombi1->AddNode(ST_endcap[i], i, new TGeoTranslation(0, 0, 245.657));     
       pCombi1->AddNode(G10_endcap[i], i, new TGeoTranslation(0, 0, 245.695));     
     } 
     if(i==1){pCombi1->AddNode(ST_endcap[i], 0, new TGeoTranslation(0, 0, 245.848)); }    
     if(i>1){ 
       pCombi1->AddNode(G10_endcap[i], i, new TGeoTranslation(0, 0., (255.215 + 9*(i-2) +MDT_thickness/2+0.1)));     
       pCombi1->AddNode(ST_endcap[i], i, new TGeoTranslation(0, 0, (255.215 + 9*(i-2) + MDT_thickness/2+0.26)));          
     } 
    }
//////////////////////////////////////////

//////////////// Muon Filter ///////////////
 /////////// MDT planes ///////////////
  pCombi2->AddNode(mnk4[0], 1);
  pCombi2->AddNode(mnk4[1], 2);
  pCombi2->AddNode(mnk4[2], 3);
  pCombi2->AddNode(mnk4[3], 4);
//////////// Strips for MF: G10+Aluminium///////////
   for(int i = 0; i < 4; i++){
       pCombi2->AddNode(ST_mf[i], i, new TGeoTranslation(0, 0, 302.715+9*i+0.715));     
       pCombi2->AddNode(G10_mf[i], i, new TGeoTranslation(0, 0, 302.715+9*i+0.875));     
    }
//////// Absorber plates//////
  pCombi2->AddNode(fmf[0], 1, ftr[0]);
  pCombi2->AddNode(fmf[1], 2, ftr[1]);
  pCombi2->AddNode(fmf[2], 3, ftr[2]);
  pCombi2->AddNode(fmf[3], 4, ftr[3]);
////////////////////////////////////
////////////////////////////////////////////


//  top->AddNode(pCombi, 0);
  top->AddNode(pCombi1, 0);
//  top->AddNode(pCombi2, 0);
//  top->AddNode(pCombi3, 0);

  top->Write();

  geom->CloseGeometry();
  geom->SetVisLevel(50);
  geom->SetVisOption(1);
  top->Draw();
  
//  gGeoManager->Export("muon_TS_barrel.root");
//  gGeoManager->Export("muon_TS_endcap.root");
//  gGeoManager->Export("muon_MF.root");
//  gGeoManager->Export("muon_FS.root");
  return 0;
}   
   
int name(TGeoVolume* &vol, int n1, int n2, int n3, int n4, int n5)
{

//  MDT%ds%dl%db%dw%d -> MDT + 1/2/3/4 (Barrel/Endcap/MF/FS) + octant# + layer# + box# + wire#  
  TObjArray* nodes = vol->GetNodes();
  vol->SetName(Form("BP%ds%dl%db%dw%d",n1,n2,n3,n4,n5));
  return 0;
}
