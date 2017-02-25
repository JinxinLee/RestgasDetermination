int createFCalib()
{
  cout<<"Create a fake calibration for BonnTS data"<<endl;

  Int_t nFee = 24;

  Int_t nChFe = 128;

  Float_t value = 10.;

  ofstream out("file.calib");

  for (Int_t f = 0 ; f < nFee ; f++)
    {
      for (Int_t c = 0 ; c < nChFe ; c++)
	{

	  out << "0 " << f << " " << c << " " << value << endl; 

	} // end loop on channels
    }// end loop on fees

  out.close();

  return 0;

}
