#include <vector>

void classify_tmva( Int_t nrOfEvents=1000, Int_t nrOfFeatures=4 ) 
{
  Char_t name[256];
  TRandom3 myran(NULL);

   cout << endl;
   cout << "==> Start TMVApplication" << endl;
      //
   // create the Reader object
   //
   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    

   Float_t var[16];

   for (Int_t i=0; i<nrOfFeatures; i++)
     {
       sprintf(name,"p%d",i);
       reader->AddVariable(name, &var[i] );
     }
   //
   // book the MVA methods
   //
   string dir    = "weights/";
   string prefix = "TMVAnalysis";

   reader->BookMVA( "KNN method",           dir + prefix + "_KNN.weights.txt" );

   TStopwatch timer;

   timer.Start();
   for (Int_t i=0; i<nrOfEvents; i++)
     {
       for (Int_t j=0; j<nrOfFeatures; j++)
	 {
	   var[j]=myran.Uniform(0,1);
	   std::cout << var[j] << " ";
	 }
       std::cout << std::endl;
       std::cout << reader->EvaluateMVA( "KNN method" ) << std::endl;
     }
   timer.Stop();
   std::cout << "CPU time=" << timer.CpuTime() << " Real time=" << timer.RealTime() << std::endl;

   delete reader;
    
   cout << "==> TMVApplication is done!" << endl << endl;
} 
