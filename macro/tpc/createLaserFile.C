//Macro designed to create the input file for the TpcLaserTask
//Felix Boehmer, initial creation 19.7.07
{

#include <fstream>

//TPC geometry

double _rMin = 15.5;
double _rMax = 41.5;
double _zMin = -39.5;
double _zMax = 109.5;

int ngrids=14;
double dz=10;

 TVector3 beam, beamtemp;

 double _iondens = 20;
 double _beamwidth = 0.04; 
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
 
 
 TVector3 start = TVector3(-41.5,0,-40); //vector pointing to first origin of beams

 for(int iz=0;iz<ngrids;++iz){
   z+=dz;
   start.SetZ(z);
   zaxis.SetZ(z);
   for (int i=1; i<=10; i++)
     {
       int trackCount = 0;
       TVector3 dZ;
       for (double angle=0; angle<=60; angle+=24)
	 {
	   double temp = angle;
	   dZ = TVector3(0,0,trackCount*deltaZ);
	   if(temp==0) {
	     beam = zaxis-start;
	     beam *= (1/beam.Mag());
	     outfile<<start.X()<<"  "<<start.Y()<<"  "<<start.Z()<<"  "
		    <<beam.X()<<"   "<<beam.Y()<<"  "<<beam.Z()<<"  "
		    <<_iondens<<"  "<<_beamwidth<<"  "<<_firetime<<std::endl;
	     continue;
	   }
	   for(int j=0;j<=1;j++) {
	     beamtemp=zaxis-start;
	     beam=beamtemp+dZ;       //create windschief vectors
	     beam.RotateZ((temp/180)*TMath::Pi());
	     beam *= (1/beam.Mag());
	     outfile<<start.X()<<"  "<<start.Y()<<"  "<<start.Z()<<"  "
		    <<beam.X()<<"   "<<beam.Y()<<"  "<<beam.Z()<<"  "
		    <<_iondens<<"  "<<_beamwidth<<"  "<<_firetime<<std::endl;
	     temp=-temp; //create symmetrical beams in this way
	     beam = beamtemp;
	     dZ = -dZ;
	   }
	   trackCount++;
	 }
       start.RotateZ(TMath::Pi()/5);
     }
 }
 outfile.close();
 
 std::cout<<"\nAll done. Wrote Laser-File to "<<_filename
	  <<". Use it with TpcLaserTask."<<std::endl; 
 std::cout<<"Beware that TPC geometry is hardcoded in this macro!"<<std::endl;
 
}
