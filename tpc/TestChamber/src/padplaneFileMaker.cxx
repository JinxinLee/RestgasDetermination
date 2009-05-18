#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<cmath>
#include<sstream>
#include<algorithm>

using namespace std;



class pad_t{
public:
  int id;
  int orderid;
  double x;
  double y;
  std::vector<int> neighbors;
  pad_t() : id(-10),orderid(-10),x(-10.),y(-10.){
  }

  void print() {
	cout << "*************" << endl;
	cout << "id: " << id << endl;
	cout << "orderid: " << orderid << endl;
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
class sortPadbyID{
public:
  bool operator()(pad_t a1, pad_t a2){
    return a1.id<a2.id;
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
	char ch;
	short type;
	istr >> ch>>ch>>ch>>type>>ch>>aPad.id >> aPad.x >> aPad.y;
	if(type==2){
	  aPad.id+=800;
	}
	pads.push_back(aPad);
  }

  
  std::sort(pads.begin(),pads.end(),sortPadbyID());

  for(vector<pad_t>::iterator it =pads.begin();it!=pads.end();++it) {
    for(vector<pad_t>::iterator it2 =pads.begin();it2!=pads.end();++it2) {
      double dist = sqrt(pow((*it).x -(*it2).x,2)+pow((*it).y -(*it2).y,2));
      if(dist<3.1&&(*it2).id!=(*it).id){
	(*it).neighbors.push_back((*it2).id);
      }
    } 
  }


  //  cout << orderedpads.size() << endl;
  for(vector<pad_t>::iterator it =pads.begin();it!=pads.end();++it) {
    (*it).printForFile();
  }


   

}
 
 

