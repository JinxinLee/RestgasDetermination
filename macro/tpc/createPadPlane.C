#include <vector>
#include <fstream>

void
createPadPlane(){
// prepare file
std::ofstream outfile("padplane.dat");

// subdivisions:
int nx=84;
int ny=84;
double sizex=1.;
double sizey=1.;
double cornerx=-42.;
double cornery=-42.;

//First Line
outfile<<"PadPlaneSubDivision "
<<nx<<" "<<ny<<" "<<sizex<<" "<<sizey<<" "<<cornerx<<" "<<cornery<<std::endl;

// define pad-layout
double stepx=0.2;
double stepy=0.2;

// define sectors (for now as rectangles)
int sectnx=16;
int sectny=16;

double x=cornerx;
double y=cornery;

int nxsteps=(int)floor(nx*sizex/stepx);
int nysteps=(int)floor(ny*sizey/stepy);

// reduce number of pads to multiple of sectorsize
nxsteps=nxsteps-nxsteps%sectnx;
nysteps=nysteps-nysteps%sectny;

assert(nxsteps%sectnx==0);
assert(nysteps%sectny==0);

int nsectorsx=nxsteps/sectnx;
int nsectorsy=nysteps/sectny;
int nsectors=nsectorsx*nsectorsy;

int shapeId=0;

unsigned int id=0;

// build up padplane from sectors
for(int isy=0;isy<nsectorsy;++isy){
  for(int isx=0;isx<nsectorsx;++isx){
    double sectorx=isx*sectnx*stepx+cornerx;
    double sectory=isy*sectny*stepy+cornery;
    double sectorfarx=sectorx+sectnx*stepx;
    double sectorfary=sectory+sectny*stepy;
    double r1=sqrt(sectorx*sectorx+sectory*sectory);
    double r2=sqrt(sectorfarx*sectorfarx + sectorfary*sectorfary);
    double r3=sqrt(sectorx*sectorx+sectorfary*sectorfary);
    double r4=sqrt(sectorfarx*sectorfarx + sectory*sectory);
    // if sector not inside tpc continue;
    if(max(max(r1,r2),max(r3,r4))<15)continue;
    if(min(min(r1,r2),min(r3,r4))>42)continue;

    x=sectorx;
    y=sectory;
      for(int iy=0;iy<sectnx;++iy){
	for(int ix=0;ix<sectny;++ix){
	  //determine neighbours
	  std::vector<unsigned int> neighb;
	  // check left col
	  bool left=ix>0;
	  // check right col
	  bool right=ix<sectnx-1;
	  // check bottom row
	  bool bottom=iy>0;
	  // check top row
	  bool top=iy<sectny-1;
	  
	  if(left)neighb.push_back(id-1);
	  if(right)neighb.push_back(id+1);
	  if(bottom)neighb.push_back(id-sectnx);
	  if(top)neighb.push_back(id+sectnx);
	  if(left && bottom)neighb.push_back(id-sectnx-1);
	  if(left && top)neighb.push_back(id+sectnx-1);
	  if(right && bottom)neighb.push_back(id-sectnx+1);
	  if(right && top)neighb.push_back(id+sectnx+1);

	  // determine sectorid
	  unsigned int sectorId=isy*nsectorsx+isx;
	  outfile<<id<<" "
		 <<sectorId<<" "
		 <<shapeId<<" "
		 <<x<<" "
		 <<y<<" "
		 <<0; // angle;
	  for(int inei=0;inei<neighb.size();++inei)outfile<<" "<<neighb[inei];
	  outfile<<std::endl;

	  x+=stepx;
	  ++id;
	}
	y+=stepy;
	x=sectorx;
      }
  }
}


    

 


outfile.close();

}
