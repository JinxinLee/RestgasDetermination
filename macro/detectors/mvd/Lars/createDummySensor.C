{

  TString outfile= "dummysensor.root";

  // materials & media
  TGeoMaterial *matVacuum = new TGeoMaterial("vacuum",0,0,0);
  TGeoMaterial *matSilicon = new TGeoMaterial("silicon", 28.0855, 14, 2.33);
  TGeoMedium *medVacuum  = new TGeoMedium("vacuum", 1, matVacuum);
  TGeoMedium *medSilicon = new TGeoMedium("silicon", 2, matSilicon);

  //surrounding box
  TGeoShape* topShape= new TGeoBBox(100,100,100);
  TGeoVolume* topNode = new TGeoVolume("top", topShape, medVacuum);
  topNode->SetAsTopVolume();

  TGeoVolumeAssembly* assamb = new TGeoVolumeAssembly("DummysensorAss");
  
  TGeoShape* shape = new TGeoBBox(0.96, 0.96, 0.015);
  TGeoRotation* rot = new TGeoRotation("rot",0.,85.,0.);//2.Stelle in 10er
  TGeoCombiTrans *trafo = new TGeoCombiTrans(0., 0., 4., rot);
  TGeoVolume* sensor = new TGeoVolume("DtsStripSensorRect",shape,medSilicon);

  assamb->AddNode(sensor,0,trafo); 
  topNode->AddNode(assamb,0,new TGeoHMatrix());

  topNode->Export(outfile.Data(),"topNode","recreate");

//  topNode->Draw("ogl");
}

