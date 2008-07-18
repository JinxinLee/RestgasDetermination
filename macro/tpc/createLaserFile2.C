//Macro designed to create the input file for the TpcLaserTask
//Felix Boehmer, initial creation 19.7.07
{

#include <fstream>

//TPC geometry

double _rMin = 15.5;
double _rMax = 41.5;
double _zMin = -39.5;
double _zMax = 109.5;

int ngrids=15;
double dz=10;

  double _iondens = 45;
 double _beamwidth = 0.02; 
 double _firetime = 0;     //...
 double z=-40;
 double deltaZ = 2;        //total shift of x-y-plane for windschief beams

 double spread = 24;       //spreading angle between two beams

 TVector3 zaxis(0,0,z);

 char* _filename="LaserGridTest.dat";
 
 std::ofstream outfile(_filename);
 
 outfile<<"#This file was created using macro createLaserFile.C"<<std::endl;
 outfile<<"\n#the next line specifies that we are using directions for the beams"
 	<<std::endl;
 outfile<<"\n!direction"<<std::endl<<std::endl;


 TVector3 start(_rMax,0,0);

 int nmi=6;
 int check=0;
 double phimi[6]={-85.,-60.,-30.,30.,60.,85.};
 double phimi2[12]={-150.,-120.,-95.,-85.,-60.,-30.,30.,60.,85.,95.,120.,150.};

 for(int i=0;i<nmi;++i)phimi[i]*=TMath::Pi()/180.;

 double alpha=TMath::ASin((_rMin+0.5)/_rMax);

 double tilt[6]={-60.,-45.,-30.,30.,45.,60.}
 double tilt3[12]={-60.,-45.,-30.,30.,45.,60.,-30.,-60.,-45.,45.,60.,30.};
 double tilt2[8]={-80.,-60.,-40.,-20.,20.,40.,60.,80.};

 for(int i=0;i<nmi;++i)tilt[i]=TMath::Tan(tilt[i]*TMath::Pi()/180.);
 
 std::cout<<"Beginning building!"<<std::endl;

 for(int iz=0;iz<ngrids;++iz){
   z+=dz;
   start.SetZ(z);
   zaxis.SetZ(z);
   for(int imi=0; imi<nmi; ++imi){// loop over mirror-points
     start.SetPhi(phimi[imi]);

     TVector3 beam=zaxis-start;
     beam*= (1./beam.Mag());

     double sign=-1;
     if(start.Y()>0)sign=1;
     beam.RotateZ(sign*alpha);

     beam.SetZ(tilt[imi]);
     beam*= (1./beam.Mag());

     start.Print();
     beam.Print();

     TPolyLine3D* ray=new TPolyLine3D(2);
     ray->SetPoint(0,start.X(),start.Y(),start.Z());
     TVector3 l=start+100*beam;
     ray->SetPoint(1,l.X(),l.Y(),l.Z());
     ray->Draw();

     //if(iz>(0.6*ngrids) && check==0) {   //try to compensate bad edges
     // _iondens*=5;
     //  check++;
     //}
           
     outfile<<start.X()<<"  "<<start.Y()<<"  "<<start.Z()<<"  "
	    <<beam.X()<<"   "<<beam.Y()<<"  "<<beam.Z()<<"  "
	    <<_iondens<<"  "<<_beamwidth<<"  "<<_firetime<<std::endl;
   }// end loop over mirror-points
 }// end loop over grids

 outfile.close();
 
 std::cout<<"\nAll done. Wrote Laser-File to "<<_filename
 	  <<". Use it with TpcLaserTask."<<std::endl; 
 std::cout<<"Beware that TPC geometry is hardcoded in this macro!"<<std::endl;
 
}
