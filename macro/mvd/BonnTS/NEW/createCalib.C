{

  // to reproduce the calib file of Juelich Feb 10 runs

  const Int_t nBoxes = 4;
  
  const Int_t nFEBox = 6;

  const Int_t nChFE = 128;

  Double_t val[nBoxes][nFEBox];

  val[0][0]=0.0468423;
  val[0][1]=0.0486725;
  val[0][2]=0.0468459;
  val[0][3]=0.0522444;
  val[0][4]=0.0482401;
  val[0][5]=0.0499809;
  val[1][0]=0.0461501;
  val[1][1]=0.0466381;
  val[1][2]=0.0462991;
  val[1][3]=0.0447197;
  val[1][4]=0.0456337;
  val[1][5]=0.0443006;
  val[2][0]=0.0438225;
  val[2][1]=0.0442459;
  val[2][2]=0.04436;
  val[2][3]=0.0432845;
  val[2][4]=0.0428239;
  val[2][5]=0.0417876;
  val[3][0]=0.0987825;
  val[3][1]=0.0910402;
  val[3][2]=0.101647;
  val[3][3]=0.100106;
  val[3][4]=0.0998894;
  val[3][5]=0.0976565;

  

  ofstream out("ExampleBox.calib");

  for (Int_t b = 0 ; b < nBoxes ; b++)
    {

      for (Int_t f = 0 ; f < nFEBox ; f++)
	{

	  for (Int_t c = 0 ; c < nChFE ; c++)
	    {

	      out << b << " " << (nChFE*f)+c << " " << val[b][f] << endl;

	    }
	}

    }

  out.close();

}
