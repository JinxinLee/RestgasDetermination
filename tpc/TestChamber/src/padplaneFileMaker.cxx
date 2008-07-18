#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<cmath>
#include<sstream>

using namespace std;

class pad_t{
public:
  int id;
  int orderid;
  int xid;
  int yid;
  double x;
  double y;
  std::vector<int> neighbors;
  pad_t() : id(-10),orderid(-10),xid(-10),yid(-10),x(-10.),y(-10.){
  }

  void print() {
	cout << "*************" << endl;
	cout << "id: " << id << endl;
	cout << "orderid: " << orderid << endl;
	cout << "xid: " << xid << endl;
	cout << "yid:" << yid << endl;
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;
	cout << "neighbors: ";
	for(unsigned int i=0;i<neighbors.size();i++) {
	  cout << neighbors[i] << " ";
	} 
	cout << endl;
	cout << "+++++++++++++" << endl;
  }

  void printForFile() {

	cout << id << " 0 0 " << x << " " << y << " 0 ";
	for(unsigned int i=0;i<neighbors.size();i++) {
	  cout << neighbors[i] << " ";
	} 
	cout << endl;

  }

};

#define XP 0.62
#define YP 0.1
#define EPS 1.E-8

int main (int argc, char* argv[]){
  if(argc != 2) {
	cerr << "NumArgs is not 1" << endl;
	throw;
  }

  std::vector<pad_t> pads;

  //  string infilename(argv[1]);
  ifstream infile(argv[1]);

  char line[200];
  int j=0;
  while(infile.getline(line,199)) {
	std::istringstream istr(line);
	pad_t aPad;
	istr >> aPad.id >> aPad.xid >> aPad.yid;
	aPad.x = aPad.xid*XP+XP/2.;
	aPad.y = (aPad.yid-40)*YP+YP/2.;
	pads.push_back(aPad);
  }

  

  
  for(int i=0;i<128;i++) {
	int yrow = i%8;
	int xrow = i/8;
	for(int j=0;j<128;j++) {
	  if( fabs(pads.at(j).x-(xrow*XP+XP/2.))<EPS &&
		  fabs(pads.at(j).y-(yrow*YP+YP/2.))<EPS ) {
		pads.at(j).orderid = i;
	  }
	}
  }

  /*
  for(int i=0;i<128;i++) {
	pads.at(i).print();
  }
  */

  std::vector<pad_t> orderedpads;

  for(int i=0;i<128;i++) {
	for(int j=0;j<128;j++) {
	  if(i==pads.at(j).orderid) {
		orderedpads.push_back(pads.at(j));
	  }
	}
  }


  for(int i=0;i<128;i++) {
	int yrow = i%8;
	int xrow = i/8;
	for(int j=0;j<128;j++) {
	  if(orderedpads.at(j).xid >= xrow-1 &&
		 orderedpads.at(j).xid <= xrow+1 && 
		 orderedpads.at(j).yid-40 >= yrow-1 &&
		 orderedpads.at(j).yid-40 <= yrow+1 &&
		 orderedpads.at(j).id != orderedpads.at(i).id){
		
		orderedpads.at(i).neighbors.push_back(orderedpads.at(j).id);
	  }

	}
  }


  //  cout << orderedpads.size() << endl;
  for(int i=0;i<128;i++) {
	orderedpads.at(i).printForFile();
  }


   

}
 
 
