int createCalibJuelichFeb02()
{
  cout<<"Create a calibration file for BonnTS data from Feb02 Beamtime@COSY"<<endl;

  Int_t nFee = 24;
  // Calibration Factors of 24 Frontends representing the ratio of
  // charge to the detected amplitude in adc-counts
  // these factors were obtained from the internal pulse generators
  // of the APV25 Frontend chips
  // values are in units of ke/adc (kilo-electrons per adc-channel)
  // the calibration is valid for the Feb02 Trackingstation 
  // Beamtime @ COSY (Juelich)
  // for questions and comments about these Measurements stick to
  // zaunick(at)hiskp.uni-bonn.de
  double calibFactors[] = { 0.0468423, 0.0486725, 0.0468459,
                            0.0522444, 0.0482401, 0.0499809,
                            0.0461501, 0.0466381, 0.0462991,
                            0.0447197, 0.0456337, 0.0443006,
                            0.0438225, 0.0442459, 0.0443600,
                            0.0432845, 0.0428239, 0.0417876,
                            0.0987825, 0.0910402, 0.1016470,
                            0.1001060, 0.0998894, 0.0976565
                          };

  Int_t nChFe = 128;

  ofstream out("file.calib");

  for (Int_t f = 0 ; f < nFee ; f++)
    {
      for (Int_t c = 0 ; c < nChFe ; c++)
	{

	  out << "0 " << f << " " << c << " " << calibFactors[f] << endl; 

	} // end loop on channels
    }// end loop on fees

  out.close();

  return 0;

}
