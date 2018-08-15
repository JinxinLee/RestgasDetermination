// *******************************************************************************
// NEW (!!!) MACRO TO PRODUCE THE GEOMETRY ASCII FILE
// *******************************************************************************
// This macro reproduces the file straws_skewed_blocks_35cm_pipe.geo (rev 13273)
// apart from line 26 (starting and ending phi angles of the tube segment
// representing the outer support cylinder). 
// The phi values in the geo file (2.713409, 177.286591) were obtained with
// outerDiam = 84.4 cm. By running this macro line 26 becomes 2.785918, 177.214082
// *******************************************************************************
// if you want to reduce the tube length:
// - change tubeLength
// - change sttCenterZ
// - length and number of the short skewed straws is changed automatically
// *******************************************************************************

#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

static int counter1;
static int counter2;
static int counter3;
static int counter4;
static int counter5;
static int counter6;

//--------------------------------------------geometry definitions
#define outerDiam     82.2000 
#define innerDiam     31.8000 
#define tubeInnerDiam  1.0000
#define tubeOuterDiam  1.0060
#define tubeSeperation 1.0100
#define wireDiam       0.0020
#define tubeLength   150.0000 

#define safety         0.2000
 
#define sttCenterX 0.
#define sttCenterY 0.
// tubeLength/2 - 40
#define sttCenterZ 35.

#define innerCoverThickness 0.1000 // cm
#define outerCoverThickness 0.1000 // cm

#define panelthickness 0.1000 
#define pipeDiam       4.0000
#define noSupport 0

#define skewangle 3.

#define pi 3.141592653589793238512808959406186204433

#define cave "cave"
#define sttassembly "stt01assembly"
#define innerCylinder "stt01innerCylinder"
#define outerCylinder "stt01outerCylinder"
#define panel1 "stt01box#1"
#define panel2 "stt01box#2"
#define panel3 "stt01box#3"
#define panel4 "stt01box#4"

// materials
#define air "air"
#define AlBe "STTsupport"
#define mylar "mylar"
#define HeMixture "STTmix9010_2bar"
#define W "tungsten"

static int tubeteller;
static int axialtubeteller;
static int skewedtubeteller;
static int shortskewedtubeteller;
static double maximumradius = 0;
static double minimumradius = 100000;

// fuctions to write part of the declaration
void writename(char const *name, bool support = false, bool leftside = false)
{
  if(support && leftside)  cout << name << "#1" << endl;
  else if(support && !leftside) cout << name << "#2" << endl;
  else cout << name << endl;
  counter1++;
}

void writemother(char const *name, bool original = true, bool support = false)
{
  cout << name << endl;
  if (original && !support)
    cout << "TUBE" << endl;
  if(original && support)
    cout << "TUBS" << endl;
  counter2++;
}

void writecylsupport(char const *name, bool firstone)
{
  cout << name << endl;
  if (firstone)  
    cout << "TUBS" << endl;
  counter2++;
}

void writemedium(char *name)
{
  cout << name << endl;
  counter3++;
}

void writetube(double inner, double outer,  double length)
{
  cout << 0. << "    " << 0. << "    " << -1. * length * 10. << endl;  
  cout << inner * 10. << "   " << outer * 10. << endl;
  cout << 0. << "    " << 0. << "    " << length * 10. << endl;  
  counter4++;
}

void writehalftube(double inner, double outer,  double length)
{
  cout << 0. << "    " << 0. << "    " << -1. * length * 10. << endl;  
  cout << inner * 10. << "   " << outer * 10. << endl;
  cout << 0. << "    " << 0. << "    " << length * 10. << endl;  
  cout << atan((pipeDiam/2.)/outer) * (180. / pi) << "    " << 180. - atan((pipeDiam/2.)/outer) * (180. / pi) << endl;
  counter4++;
}

void writepanel(char const *name, bool firstone, double xthick, double ythick,  double length, int side)
{
  xthick *= 10.;
  ythick *= 10.;
  length *= 10.;

  if(firstone) {
    cout << name << endl;
    cout << sttassembly << endl;
    cout << "BOX" << endl;
    cout << AlBe << endl;
    cout <<  xthick/2. << " " << -ythick/2. << " " << -length / 2. << endl;
    cout <<  xthick/2. << " " <<  ythick/2. << " " << -length / 2. << endl;
    cout << -xthick/2. << " " <<  ythick/2. << " " << -length / 2. << endl;
    cout << -xthick/2. << " " << -ythick/2. << " " << -length / 2. << endl;
    cout <<  xthick/2. << " " << -ythick/2. << " " <<  length / 2. << endl;
    cout <<  xthick/2. << " " <<  ythick/2. << " " <<  length / 2. << endl;
    cout << -xthick/2. << " " <<  ythick/2. << " " <<  length / 2. << endl;
    cout << -xthick/2. << " " << -ythick/2. << " " <<  length / 2. << endl;
  }
  else {
    cout << name << endl;
    cout << sttassembly << endl;
  }

}


void writetrans(double x, double y, double z)
{
  cout << x * 10. << "   " << y * 10. << "   " << z * 10. << endl;
  counter5++;
}

void writerot(double x00, double x01, double x02, double x10, double x11, double x12, double x20, double x21, double x22)
{
  cout << x00 << "   "  << x01 << "   " << x02 << "   "  
       << x10 << "   "  << x11 << "   " << x12 << "   "  
       << x20 << "   "  << x21 << "   " << x22 << endl;
  cout << "//----------------------------------------------------------" << endl;
  counter6++;
}

// write the declaration of a complete straw
bool putStraw(double posX, double posY, double posZ)
{

  // CHECK PIPE
  if(posX > -(pipeDiam/2. + panelthickness) && posX < (pipeDiam/2. + panelthickness)) return false;

  // check if the straws fit wihin the inner and outer diameter specified
  if (
      (sqrt(posX * posX + posY * posY) < ((outerDiam / 2.) - outerCoverThickness - (tubeOuterDiam / 2.))) &&
      (sqrt(posX * posX + posY * posY) > ((innerDiam / 2.) + innerCoverThickness + (tubeOuterDiam / 2.)))
      )
    {
      // convert int to string
      stringstream
	conv;
	
      string
	tubetellerStr;
	
      conv << tubeteller + 1;
      conv >> tubetellerStr;
	
      string
	nameItube = "stt01tube#" + tubetellerStr,
	nameIgas = "stt01gas#" + tubetellerStr,
	nameIwire = "stt01wire#" + tubetellerStr;
	
      // keep track of minimum and maximum extend of the straw package 
      if (sqrt(posX * posX + posY * posY) - (tubeOuterDiam / 2.) < minimumradius)
	{
	  minimumradius = sqrt(posX * posX + posY * posY) - (tubeOuterDiam / 2.);
	}
      if (sqrt(posX * posX + posY * posY) + (tubeOuterDiam / 2.) > maximumradius)
	{
	  maximumradius = sqrt(posX * posX + posY * posY) + (tubeOuterDiam / 2.);
	}

      // place the tube
      if (tubeteller == 0)
	{
	  // original tube

	  // Mylar tubes  
	  writename(nameItube.c_str());
	  writemother(sttassembly);
	  writemedium(mylar);
	  writetube(0, tubeOuterDiam / 2., tubeLength / 2.);
	  writetrans(posX, posY, posZ);
	  writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	    
	  // Gas filling
	  writename(nameIgas.c_str());
	  writemother(nameItube.c_str());
	  writemedium(HeMixture);
	  writetube(0., tubeInnerDiam / 2., tubeLength / 2.);
	  writetrans(0., 0., 0.);
	  writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	    
	  // Anode wire
	  writename(nameIwire.c_str());
	  writemother(nameIgas.c_str());
	  writemedium(W);
	  writetube(0., wireDiam / 2., tubeLength / 2.);
	  writetrans(0., 0., 0.);
	  writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	}
      else 
	{
	  // copy 
	  writename(nameItube.c_str());
	  writemother(sttassembly, false);
	  writetrans(posX, posY, posZ);
	  writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	}
      tubeteller++;
      axialtubeteller++;
    }
  else
    {
      return false;
    }
    
  return true;
}

// write the rotation matrix for a right skewed straw 
void plotrotright(double x, double y, double z)
{
  double
    newskewangle = skewangle * -1.;

  cout << 1 + (1 - cos(newskewangle * (pi / 180.))) * (x * x - 1) << "  " 
       << -z * sin(newskewangle * (pi / 180.)) + (1 - cos(newskewangle * (pi / 180.))) * x * y << "  " 
       << y * sin(newskewangle * (pi / 180.)) + (1 - cos(newskewangle * (pi / 180.))) * x * z << "  " 

       << z * sin(newskewangle * (pi / 180.)) + (1 - cos(newskewangle * (pi / 180.))) * x * y << "  " 
       << 1 + (1 - cos(newskewangle * (pi / 180.))) * (y * y - 1) << "  "
       << -x * sin(newskewangle * (pi / 180.)) + (1 - cos(newskewangle * (pi / 180.))) * y * z << "  " 

       << -y * sin(newskewangle * (pi / 180.)) + (1 - cos(newskewangle * (pi / 180.))) * x * z << "  " 
       << x * sin(newskewangle * (pi / 180.)) + (1 - cos(newskewangle * (pi / 180.))) * y * z << "  " 
       << 1 + (1 - cos(newskewangle * (pi / 180.))) * (z * z - 1) << endl;
  cout << "//----------------------------------------------------------" << endl;
  counter6++;
}

// write the rotation matrix for a left skewed straw
void plotrotleft(double x, double y, double z)
{
  cout << 1 + (1 - cos(skewangle * (pi / 180.))) * (x * x - 1) << "  " 
       << -z * sin(skewangle * (pi / 180.)) + (1 - cos(skewangle * (pi / 180.))) * x * y << "  " 
       << y * sin(skewangle * (pi / 180.)) + (1 - cos(skewangle * (pi / 180.))) * x * z << "  " 

       << z * sin(skewangle * (pi / 180.)) + (1 - cos(skewangle * (pi / 180.))) * x * y << "  " 
       << 1 + (1 - cos(skewangle * (pi / 180.))) * (y * y - 1) << "  "
       << -x * sin(skewangle * (pi / 180.)) + (1 - cos(skewangle * (pi / 180.))) * y * z << "  " 

       << -y * sin(skewangle * (pi / 180.)) + (1 - cos(skewangle * (pi / 180.))) * x * z << "  " 
       << x * sin(skewangle * (pi / 180.)) + (1 - cos(skewangle * (pi / 180.))) * y * z << "  " 
       << 1 + (1 - cos(skewangle * (pi / 180.))) * (z * z - 1) << endl;
  cout << "//----------------------------------------------------------" << endl;
  counter6++;
}

bool putStrawRotatedShortLeft(double posX, double posY, double posZ, double xvector, double yvector, double zvector, double length)
{
 
  double extr1 = posX + (length/2.) * sin(skewangle);
  double extr2 = posX - (length/2.) * sin(skewangle);
  
  if (length > 0.)
    {
      // convert int to string
      stringstream
	conv;
	
      string
	tubetellerStr;
	
      conv << tubeteller + 1;
      conv >> tubetellerStr;
	
      string
	nameItube = "stt01tube" + tubetellerStr,
	nameIgas = "stt01gas" + tubetellerStr,
	nameIwire = "stt01wire" + tubetellerStr;
	
      // N.B. we don't use copies here since the short straws are all different in length
	
      // Mylar tubes  
      writename(nameItube.c_str());
      writemother(sttassembly);
      writemedium(mylar);
      writetube(0, tubeOuterDiam / 2., length / 2.);
      writetrans(posX, posY, posZ);
      plotrotleft(xvector, yvector, zvector);
	
      // gas filling
      writename(nameIgas.c_str());
      writemother(nameItube.c_str());
      writemedium(HeMixture);
      writetube(0., tubeInnerDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	
      // anode wires
      writename(nameIwire.c_str());
      writemother(nameIgas.c_str());
      writemedium(W);
      writetube(0., wireDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	
      tubeteller++;
      shortskewedtubeteller++;

	
      return true;
    }
}

bool putStrawRotatedShortRight(double posX, double posY, double posZ, double xvector, double yvector, double zvector, double length)
{
  double extr1 = posX + (length/2.) * sin(skewangle);
  double extr2 = posX - (length/2.) * sin(skewangle);
  
  if (length > 0.)
    {



      // convert int to string
      stringstream
	conv;
	
      string
	tubetellerStr;
	
      conv << tubeteller + 1;
      conv >> tubetellerStr;
	
      string
	nameItube = "stt01tube" + tubetellerStr,
	nameIgas = "stt01gas" + tubetellerStr,
	nameIwire = "stt01wire" + tubetellerStr;
	
      // N.B. don;t use copies here since all short straws have different length
	
      // Mylar tubes  
      writename(nameItube.c_str());
      writemother(sttassembly);
      writemedium(mylar);
      writetube(0, tubeOuterDiam / 2., length / 2.);
      writetrans(posX, posY, posZ);
      plotrotright(xvector, yvector, zvector);
	
      // gas filling
      writename(nameIgas.c_str());
      writemother(nameItube.c_str());
      writemedium(HeMixture);
      writetube(0., tubeInnerDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	
      // anode wire
      writename(nameIwire.c_str());
      writemother(nameIgas.c_str());
      writemedium(W);
      writetube(0., wireDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	
      tubeteller++;
      shortskewedtubeteller++;
    
      return true;
    }
}

bool putStrawRotatedLeft(double posX, double posY, double posZ, double xvector, double yvector, double zvector, double length)
{
  double extr1 = posX + (length/2.) * sin(skewangle);
  double extr2 = posX - (length/2.) * sin(skewangle);
  

  // convert int to string
  stringstream
    conv;
      
  string
    tubetellerStr;
    
  conv << tubeteller + 1;
  conv >> tubetellerStr;
    
  string
    nameItube = "stt01tube#" + tubetellerStr,
    nameIgas = "stt01gas#" + tubetellerStr,
    nameIwire = "stt01wire#" + tubetellerStr;
    
  if (tubeteller == 0)
    {
      // original straw
      // Mylar tubes  
      writename(nameItube.c_str());
      writemother(sttassembly);
      writemedium(mylar);
      writetube(0, tubeOuterDiam / 2., length / 2.);
      writetrans(posX, posY, posZ);
      plotrotleft(xvector, yvector, zvector);
	
      // Gas filling
      writename(nameIgas.c_str());
      writemother(nameItube.c_str());
      writemedium(HeMixture);
      writetube(0., tubeInnerDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	
      // Anode wire
      writename(nameIwire.c_str());
      writemother(nameIgas.c_str());
      writemedium(W);
      writetube(0., wireDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
    }
  else
    {
      // copy volume

      writename(nameItube.c_str());
      writemother(sttassembly, false);
      writetrans(posX, posY, posZ);
      plotrotleft(xvector, yvector, zvector);
    }

  tubeteller++;
  skewedtubeteller++;
    
  return true;
}

bool putStrawRotatedRight(double posX, double posY, double posZ, double xvector, double yvector, double zvector, double length)
{


  double extr1 = posX + (length/2.) * sin(skewangle);
  double extr2 = posX - (length/2.) * sin(skewangle);
  

  // convert int to string
  stringstream
    conv;
    
  string
    tubetellerStr;
    
  conv << tubeteller + 1;
  conv >> tubetellerStr;
    
  string
    nameItube = "stt01tube#" + tubetellerStr,
    nameIgas = "stt01gas#" + tubetellerStr,
    nameIwire = "stt01wire#" + tubetellerStr;
    
  if (tubeteller == 0)
    {
      // original volume

      // Mylar tubes  
      writename(nameItube.c_str());
      writemother(sttassembly);
      writemedium(mylar);
      writetube(0, tubeOuterDiam / 2., length / 2.);
      writetrans(posX, posY, posZ);
      plotrotright(xvector, yvector, zvector);
	
      // gas filling
      writename(nameIgas.c_str());
      writemother(nameItube.c_str());
      writemedium(HeMixture);
      writetube(0., tubeInnerDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
	
      // anode wire
      writename(nameIwire.c_str());
      writemother(nameIgas.c_str());
      writemedium(W);
      writetube(0., wireDiam / 2., length / 2.);
      writetrans(0., 0., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
    }
  else
    {
      // copy volume 
      writename(nameItube.c_str());
      writemother(sttassembly, false);
      writetrans(posX, posY, posZ);
      plotrotright(xvector, yvector, zvector);
    }

  tubeteller++;
  skewedtubeteller++;
   
  return true;
}



void placeSingleLayerStraightExact(double ringteller)
{  
  double
    radius = tubeSeperation / 2.;

  // place a straight double layer  
  double
    xpos = 0.,  
    ypos = ((ringteller) * 2 * radius), 
    zpos = 0.;

  // 6 loops, one for each side of the hexagon
  //      1  / \  4
  //      2  | |  5
  //      3  \ /  6
  for(int i = 0; i < ringteller; i++) 
    {
      xpos -= sqrt(3.) * radius;
      ypos -= radius;
      putStraw(xpos, ypos, zpos);
    }
  for(int i = 0; i < ringteller; i++) 
    {
      ypos -= 2 * radius;
      putStraw(xpos, ypos, zpos);
    }
  for(int i = 0; i < ringteller; i++) 
    {
      xpos += sqrt(3.) *  radius;
      ypos -= radius;
      putStraw(xpos, ypos, zpos);
    }
  for(int i = 0; i < ringteller; i++) 
    {
      xpos += sqrt(3.) * radius;
      ypos += radius;
      putStraw(xpos, ypos, zpos);
    }
  for(int i = 0; i < ringteller; i++) 
    {
      ypos += 2 * radius;
      putStraw(xpos, ypos, zpos);
    }
  for(int i = 0; i < ringteller; i++) 
    {
      xpos -= sqrt(3.) * radius;
      ypos += radius;
      putStraw(xpos, ypos, zpos);
    }
}

void placeSingleLayerSkewedRight(double ringposition)
{  
  // see void placeSingleLayerSkewedLeft(double ringposition)
    
  double radius = tubeSeperation / 2.;
    
  double
    newskewangle = skewangle * -1.;
    
  double
    newradius = radius / cos((newskewangle / 180.) * pi);
    
  double
    xpos = 0., 
    ypos = ringposition / cos((30. / 180.) * pi),  
    zpos = 0.,
    tmpxpos,
    tmpypos,
    vectorx,
    vectory,
    vectorz;
 
  double
    availableSpace = 2. * ringposition * tan((30. / 180.) * pi);
  
  int
    possibleStraws = int((availableSpace - fabs(tubeLength * sin((newskewangle / 180.) * pi))) / (newradius * 2)),
    extraStraws = -1 * possibleStraws + int(availableSpace / (newradius * 2));

  double
    extraspace = (availableSpace - (possibleStraws + extraStraws) * (newradius * 2)) / 2.;

  cerr << "extra space: " << extraspace * 2 << endl;

  double
    skewanglerad = (newskewangle / 180.) * pi,
    sixtyrad = (60. / 180.) * pi,
    translationToLeft = - 0.5 * tubeLength * tan(skewanglerad);


  vector<double>
    lengthsShort;

  double
    xpos2 = xpos - availableSpace * sin((60. / 180.) * pi),
    ypos2 = ypos - availableSpace * cos((60. / 180.) * pi),
    tmpxpos2 = xpos2,
    tmpypos2 = ypos2  - extraspace - newradius,
    limit = ypos2 - availableSpace + newradius + safety;

  for(int i = 0; i < possibleStraws; i++) 
    {
      tmpypos2 -= 2 * newradius;
    }
    
  for (int i = 0; i < extraStraws; i++)
    {
      double
	lengthShort = (tmpypos2 - limit) / sin(newskewangle * (pi / 180.));

      if (lengthShort > tubeLength)
	lengthShort = tubeLength;

      //      cerr << "length: " << lengthShort << endl;
      lengthsShort.push_back(fabs(lengthShort));

      tmpypos2 -= 2 * newradius;
    }
  

  double
    additionalShift = fabs(tubeLength * sin((newskewangle / 180.) * pi));
  
  // =================================================    
  // planes intersecting pipe ========================
  // pipe
  double pipespace = (panelthickness + pipeDiam/2.) * 1./(sqrt(3)/2.);  

  // the available space for a layer at the radius ringposition
  double
    availableSpacePipe = 2. * ringposition * tan((30. / 180.) * pi) - pipespace;
  
  // the number of skewed full-length straws that fit in the available space
  int
    possibleStrawsPipe = int((availableSpacePipe - fabs(tubeLength * sin((skewangle / 180.) * pi))) / (newradius * 2));
  
  // the number of short straws that fit on either side of the pack of full-length straws 
  int
    extraStrawsPipe = -1 * possibleStrawsPipe + int(availableSpacePipe / (newradius * 2));
  // the amount of space that is left in the layer when considering the sum of all
  // full length and short straws together
  double
    extraspacePipe = (availableSpacePipe - (possibleStrawsPipe + extraStrawsPipe) * (newradius * 2)) / 2.;
    
  //  cerr << "layer with pipe " << possibleStraws << "  " <<  extraStraws << endl;
    
  // calculate the lenghts of the different short straws in this layer
  vector<double>
    lengthsShortPipe;
    
  double
    // start at the right edge of the layer
    xpos2Pipe = xpos - availableSpacePipe * sin((60. / 180.) * pi),
    ypos2Pipe = ypos - availableSpacePipe * cos((60. / 180.) * pi),
    // shift to the center of the rightmost straw
    tmpxpos2Pipe = xpos2Pipe,
    tmpypos2Pipe = ypos2Pipe - extraspacePipe - newradius,
    // the limit on the left side of the plane beyond which no center of the short straws may be
    limitPipe = ypos2Pipe - availableSpacePipe + newradius + safety;


  // shift to the center of of the leftmost full-length straw 
  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
      tmpypos2Pipe -= 2 * newradius;
    }
  
  // calculate the maximum allowable length for this short straw and shift to the next 
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = (tmpypos2Pipe - limitPipe) / sin(skewangle * (pi / 180.));
	
      if (lengthShort > tubeLength)
	lengthShort = tubeLength;
      lengthsShortPipe.push_back(lengthShort);
      tmpypos2Pipe -= 2 * newradius;
    }
  // =================================================

  // plane 1
  vectorx = -cos((60. / 180.) * pi);
  vectory = sin((60. / 180.) * pi);
  vectorz = 0;
  tmpxpos = xpos - (extraspacePipe + newradius + pipespace) * sin((60. / 180.) * pi);
  tmpypos = ypos - (extraspacePipe + newradius + pipespace) * cos((60. / 180.) * pi);
  
  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
      
      putStrawRotatedRight(tmpxpos + (translationToLeft - additionalShift) * sin(sixtyrad),
 			   tmpypos + (translationToLeft - additionalShift) * cos(sixtyrad), 
 			   zpos, vectorx, vectory, vectorz, tubeLength);
      
      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }
  
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];

      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 

      putStrawRotatedShortRight(tmpxpos + (translationToLeft - additionalShift - translationToFrontPer) * sin(sixtyrad), 
 				tmpypos + (translationToLeft - additionalShift - translationToFrontPer) * cos(sixtyrad), 
 				zpos - translationToFrontPar, 
 				vectorx, vectory, vectorz, lengthShort);

      putStrawRotatedShortRight(tmpxpos + (translationToLeft - additionalShift + translationToFrontPer + switchSides) * sin(sixtyrad), 
 				tmpypos + (translationToLeft - additionalShift + translationToFrontPer + switchSides) * cos(sixtyrad), 
 				zpos + translationToFrontPar, 
 				vectorx, vectory, vectorz, lengthShort);
			   

      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }
  

  // plane 2
  vectorx = -1.;
  vectory = 0.;
  vectorz = 0.;
  xpos -= availableSpace * sin((60. / 180.) * pi);
  ypos -= availableSpace * cos((60. / 180.) * pi);
  tmpxpos = xpos;
  tmpypos = ypos - extraspace - newradius;

  for(int i = 0; i < possibleStraws; i++) 
    {
      putStrawRotatedRight(tmpxpos, tmpypos - additionalShift + translationToLeft, zpos, vectorx, vectory, vectorz, tubeLength);
      tmpypos -= 2 * newradius;
    }
  
  for (int i = 0; i < extraStraws; i++)
    {
      double
	lengthShort = lengthsShort[i];
     
      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStraws + 1 + (2 * i)) * cos(skewanglerad); 
     
      putStrawRotatedShortRight(tmpxpos, 
 				tmpypos - additionalShift + (translationToLeft - translationToFrontPer), 
				zpos - translationToFrontPar, 
  				vectorx, vectory, vectorz, lengthShort);
      
      putStrawRotatedShortRight(tmpxpos, 
 				tmpypos - additionalShift + (translationToLeft + translationToFrontPer + switchSides), 
 				zpos + translationToFrontPar, 
  				vectorx, vectory, vectorz, lengthShort);
      
      tmpypos -= 2 * newradius;
    }
  
  // plane 3
  vectorx = -1 * cos((60. / 180.) * pi);
  vectory =  -1 * sin((60. / 180.) * pi);
  vectorz = 0;
  ypos -= availableSpace;
  tmpxpos = xpos + (extraspacePipe + newradius) * sin((60. / 180.) * pi);
  tmpypos = ypos - (extraspacePipe + newradius) * cos((60. / 180.) * pi);

  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
      
      putStrawRotatedRight(tmpxpos - (translationToLeft - additionalShift) * sin(sixtyrad),
			   tmpypos + (translationToLeft - additionalShift) * cos(sixtyrad), 
			   zpos, vectorx, vectory, vectorz, tubeLength);
        
      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }
  
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];

      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 
            
      putStrawRotatedShortRight(tmpxpos - (translationToLeft - translationToFrontPer - additionalShift) * sin(sixtyrad), 
				tmpypos + (translationToLeft - translationToFrontPer - additionalShift) * cos(sixtyrad), 
				zpos - translationToFrontPar, 
				vectorx, vectory, vectorz, lengthShort);
 
      putStrawRotatedShortRight(tmpxpos - (translationToLeft + translationToFrontPer - additionalShift + switchSides) * sin(sixtyrad), 
				tmpypos + (translationToLeft + translationToFrontPer - additionalShift + switchSides) * cos(sixtyrad), 
				zpos + translationToFrontPar, 
				vectorx, vectory, vectorz, lengthShort);
      
      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }
  
  // plane 4
  vectorx = 1 * cos((60. / 180.) * pi);
  vectory = -1 * sin((60. / 180.) * pi);
  vectorz = 0;
  xpos += availableSpace * sin((60. / 180.) * pi);
  ypos -= availableSpace * cos((60. / 180.) * pi);
  tmpxpos = xpos + (extraspacePipe + newradius + pipespace) * sin((60. / 180.) * pi);
  tmpypos = ypos + (extraspacePipe + newradius + pipespace) * cos((60. / 180.) * pi);
    
  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
      
      putStrawRotatedRight(tmpxpos - (translationToLeft - additionalShift) * sin(sixtyrad), 
			   tmpypos - (translationToLeft - additionalShift) * cos(sixtyrad), 
			   zpos, vectorx, vectory, vectorz, tubeLength);
      
      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
  
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];

      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 
      
      putStrawRotatedShortRight(tmpxpos - (translationToLeft - additionalShift - translationToFrontPer) * sin(sixtyrad), 
				tmpypos - (translationToLeft - additionalShift - translationToFrontPer) * cos(sixtyrad), 
				zpos - translationToFrontPar, 
  				vectorx, vectory, vectorz, lengthShort);
 
      putStrawRotatedShortRight(tmpxpos - (translationToLeft - additionalShift + translationToFrontPer + switchSides) * sin(sixtyrad), 
				tmpypos - (translationToLeft - additionalShift + translationToFrontPer + switchSides) * cos(sixtyrad), 
  				zpos + translationToFrontPar, 
  				vectorx, vectory, vectorz, lengthShort);
      
      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
  
  // plane 5
  vectorx = 1.;
  vectory = 0.;
  vectorz = 0.;
  xpos += availableSpace * sin((60. / 180.) * pi);
  ypos += availableSpace * cos((60. / 180.) * pi);
  tmpxpos = xpos;
  tmpypos = ypos + extraspace + newradius;

  for(int i = 0; i < possibleStraws; i++) 
    {
      putStrawRotatedRight(tmpxpos, tmpypos - (translationToLeft - additionalShift), zpos, vectorx, vectory, vectorz, tubeLength);
      tmpypos += 2 * newradius;
    }
  
  for (int i = 0; i < extraStraws; i++)
    {
      double
	lengthShort = lengthsShort[i];

      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStraws + 1 + (2 * i)) * cos(skewanglerad); 
      
      putStrawRotatedShortRight(tmpxpos, tmpypos - (translationToLeft - additionalShift - translationToFrontPer), 
  				zpos - translationToFrontPar, 
  				vectorx, vectory, vectorz, lengthShort);
 
      putStrawRotatedShortRight(tmpxpos, tmpypos - (translationToLeft - additionalShift + translationToFrontPer + switchSides), 
  				zpos + translationToFrontPar, 
  				vectorx, vectory, vectorz, lengthShort);

      tmpypos += 2 * newradius;
    }
  
  // plane 6
  vectorx = cos((60. / 180.) * pi);
  vectory = sin((60. / 180.) * pi);
  vectorz = 0;
  ypos += availableSpace;
  tmpxpos = xpos - (extraspacePipe + newradius) * sin((60. / 180.) * pi);
  tmpypos = ypos + (extraspacePipe + newradius) * cos((60. / 180.) * pi);

  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
      
      putStrawRotatedRight(tmpxpos + (translationToLeft - additionalShift) * sin(sixtyrad), 
			   tmpypos - (translationToLeft - additionalShift) * cos(sixtyrad), 
			   zpos, vectorx, vectory, vectorz, tubeLength);
      
      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
  
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];

      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 

      putStrawRotatedShortRight(tmpxpos + (translationToLeft - additionalShift - translationToFrontPer) * sin(sixtyrad), 
				tmpypos - (translationToLeft - additionalShift - translationToFrontPer) * cos(sixtyrad), 
				zpos - translationToFrontPar, 
 				vectorx, vectory, vectorz, lengthShort);
      
      putStrawRotatedShortRight(tmpxpos + (translationToLeft - additionalShift + translationToFrontPer + switchSides) * sin(sixtyrad), 
				tmpypos - (translationToLeft - additionalShift + translationToFrontPer + switchSides) * cos(sixtyrad), 
				zpos + translationToFrontPar, 
 				vectorx, vectory, vectorz, lengthShort);

      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
  
}




void placeSingleLayerSkewedLeft(double ringposition)
{  
  // calculation of some general properties of the layer layout
  // see layerfront.ps and layertop.ps if description is not clear

  // skewed straws become ellipses in the x-y projection
  // the short ellipse radius is still the straw radius, the
  // long ellipse radius (within the straw plane) is given by:

  double 
    radius = tubeSeperation / 2.;

  double
    newradius = radius / cos((skewangle / 180.) * pi);
    
  
  double
    xpos = 0.,  
    ypos = ringposition / cos((30. / 180.) * pi),
    zpos = 0.,
    tmpxpos,
    tmpypos,
    vectorx,
    vectory,
    vectorz;
    
 
  // the available space for a layer at the radius ringposition
  double
    availableSpace = 2. * ringposition * tan((30. / 180.) * pi);
     
  // the number of skewed full-length straws that fit in the available space
  int
    possibleStraws = int((availableSpace - tubeLength * sin((skewangle / 180.) * pi)) / (newradius * 2));

  // the number of short straws that fit on either side of the pack of full-length straws 
  int
    extraStraws = int(availableSpace / (newradius * 2)) - possibleStraws;

  // the amount of space that is left in the layer when considering the sum of all
  // full length and short straws together
  double
    extraspace = (availableSpace - (possibleStraws + extraStraws) * (newradius * 2)) / 2.;
    
  //  cerr << possibleStraws << "  " <<  extraStraws << endl;
    
  double
    skewanglerad = (skewangle / 180.) * pi,
    sixtyrad = (60. / 180.) * pi,
    // translation between the front end of the straw and the center
    translationToLeft = - 0.5 * tubeLength * tan(skewanglerad);
       
  // calculate the lenghts of the different short straws in this layer
  vector<double>
    lengthsShort;
    
  double
    // start at the right edge of the layer
    xpos2 = xpos - availableSpace * sin((60. / 180.) * pi),
    ypos2 = ypos - availableSpace * cos((60. / 180.) * pi),
    // shift to the center of the rightmost straw
    tmpxpos2 = xpos2,
    tmpypos2 = ypos2  - extraspace - newradius,
    // the limit on the left side of the plane beyond which no center of the short straws may be
    limit = ypos2 - availableSpace + newradius + safety;

  //   cerr << "xpos2/ypos2 " << xpos2 << " " << ypos2 << endl;
  //   cerr << "tmpxpos2/tmpypos2 " << tmpxpos2 << " " << tmpypos2 << endl;
  

  // shift to the center of of the leftmost full-length straw 
  for(int i = 0; i < possibleStraws; i++) 
    {
      tmpypos2 -= 2 * newradius;
    }
  
  // calculate the maximum allowable length for this short straw and shift to the next 
  for (int i = 0; i < extraStraws; i++)
    {
      double
	lengthShort = (tmpypos2 - limit) / sin(skewangle * (pi / 180.));
	
      if (lengthShort > tubeLength)
	lengthShort = tubeLength;
	
      //      cerr << "length: " << lengthShort << endl;
      lengthsShort.push_back(lengthShort);
	
      tmpypos2 -= 2 * newradius;
    }
  // =================================================    
  // planes intersecting pipe ========================
  // pipe
  double pipespace = (panelthickness + pipeDiam/2.) * 1./(sqrt(3)/2.);  

  // the available space for a layer at the radius ringposition
  double
    availableSpacePipe = 2. * ringposition * tan((30. / 180.) * pi) - pipespace;
  
  // the number of skewed full-length straws that fit in the available space
  int
    possibleStrawsPipe = int((availableSpacePipe - tubeLength * sin((skewangle / 180.) * pi)) / (newradius * 2));
  
  // the number of short straws that fit on either side of the pack of full-length straws 
  int
    extraStrawsPipe = int(availableSpacePipe / (newradius * 2)) - possibleStrawsPipe;
  
  // the amount of space that is left in the layer when considering the sum of all
  // full length and short straws together
  double
    extraspacePipe = (availableSpacePipe - (possibleStrawsPipe + extraStrawsPipe) * (newradius * 2)) / 2.;
    
  //  cerr << "layer with pipe " << possibleStraws << "  " <<  extraStraws << endl;
    
  // calculate the lenghts of the different short straws in this layer
  vector<double>
    lengthsShortPipe;
    
  double
    // start at the right edge of the layer
    xpos2Pipe = xpos - availableSpacePipe * sin((60. / 180.) * pi),
    ypos2Pipe = ypos - availableSpacePipe * cos((60. / 180.) * pi),
    // shift to the center of the rightmost straw
    tmpxpos2Pipe = xpos2Pipe, 
    tmpypos2Pipe = ypos2Pipe - extraspacePipe - newradius,
    // the limit on the left side of the plane beyond which no center of the short straws may be
    limitPipe = ypos2Pipe - availableSpacePipe + newradius + safety;

  // shift to the center of of the leftmost full-length straw 
  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
      tmpypos2Pipe -= 2 * newradius;
    }
  
  // calculate the maximum allowable length for this short straw and shift to the next 
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = (tmpypos2Pipe - limitPipe) / sin(skewangle * (pi / 180.));
	
      if (lengthShort > tubeLength)
	lengthShort = tubeLength;
	
      lengthsShortPipe.push_back(lengthShort);
	
      tmpypos2Pipe -= 2 * newradius;
    }
  // =================================================
  // plane 1
  // place full straws:
  vectorx = -cos((60. / 180.) * pi);
  vectory = sin((60. / 180.) * pi);
  vectorz = 0;
  tmpxpos = xpos - (extraspacePipe + newradius + pipespace) * sin((60. / 180.) * pi);
  tmpypos = ypos - (extraspacePipe + newradius + pipespace) * cos((60. / 180.) * pi);
    
  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
      // place a straw
      putStrawRotatedLeft(tmpxpos + translationToLeft * sin(sixtyrad), 
			  tmpypos + translationToLeft * cos(sixtyrad), 
			  zpos, vectorx, vectory, vectorz, tubeLength);
      
      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }
    
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];
	
      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	// translation to the other side of the straw layer
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 
	
	
      putStrawRotatedShortLeft(tmpxpos + (translationToLeft + translationToFrontPer) * sin(sixtyrad), 
			       tmpypos + (translationToLeft + translationToFrontPer) * cos(sixtyrad), 
			       zpos + translationToFrontPar, 
			       vectorx, vectory, vectorz, lengthShort);
	
      putStrawRotatedShortLeft(tmpxpos + (translationToLeft - translationToFrontPer + switchSides) * sin(sixtyrad), 
			       tmpypos + (translationToLeft - translationToFrontPer + switchSides) * cos(sixtyrad),
			       zpos - translationToFrontPar, 
			       vectorx, vectory, vectorz, lengthShort);
    
	
      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }

  // plane 2
  vectorx = -1.;
  vectory = 0.;
  vectorz = 0.;
  xpos -= availableSpace * sin((60. / 180.) * pi);
  ypos -= availableSpace * cos((60. / 180.) * pi);
  tmpxpos = xpos;
  tmpypos = ypos - extraspace - newradius;

  for(int i = 0; i < possibleStraws; i++) 
    {
      putStrawRotatedLeft(tmpxpos, tmpypos + translationToLeft, zpos, vectorx, vectory, vectorz, tubeLength);
      tmpypos -= 2 * newradius;
    }
    
  for (int i = 0; i < extraStraws; i++)
    {
      double
	lengthShort = lengthsShort[i];
	
      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStraws + 1 + (2 * i)) * cos(skewanglerad); 
	
      putStrawRotatedShortLeft(tmpxpos,
  			       tmpypos + (translationToLeft + translationToFrontPer), 
  			       zpos + translationToFrontPar, 
  			       vectorx, vectory, vectorz, lengthShort);
	
      putStrawRotatedShortLeft(tmpxpos,
			       tmpypos + (translationToLeft - translationToFrontPer + switchSides), 
			       zpos - translationToFrontPar, 
			       vectorx, vectory, vectorz, lengthShort);
	
      tmpypos -= 2 * newradius;
    }
    
  // plane 3
  vectorx =  -1 * cos((60. / 180.) * pi);
  vectory =  -1 * sin((60. / 180.) * pi);
  vectorz = 0;
  ypos -= availableSpace;
  tmpxpos = xpos + (extraspacePipe + newradius) * sin((60. / 180.) * pi);
  tmpypos = ypos - (extraspacePipe + newradius) * cos((60. / 180.) * pi);

  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
	
      putStrawRotatedLeft(tmpxpos - translationToLeft * sin(sixtyrad), 
			  tmpypos + translationToLeft * cos(sixtyrad), 
  			  zpos, vectorx, vectory, vectorz, tubeLength);
      
      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }

  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];
	
      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 
	
      putStrawRotatedShortLeft(tmpxpos - (translationToLeft + translationToFrontPer) * sin(sixtyrad), 
			       tmpypos + (translationToLeft + translationToFrontPer) * cos(sixtyrad), 
			       zpos + translationToFrontPar, 
  			       vectorx, vectory, vectorz, lengthShort);

      putStrawRotatedShortLeft(tmpxpos - (translationToLeft - translationToFrontPer + switchSides) * sin(sixtyrad), 
			       tmpypos + (translationToLeft - translationToFrontPer + switchSides) * cos(sixtyrad), 
			       zpos - translationToFrontPar, 
			       vectorx, vectory, vectorz, lengthShort);
	
      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos -= 2 * newradius * cos((60. / 180.) * pi);
    }
    
  // plane 4
  vectorx = 1 * cos((60. / 180.) * pi);
  vectory = -1 * sin((60. / 180.) * pi);
  vectorz = 0;
  xpos += availableSpace * sin((60. / 180.) * pi);
  ypos -= availableSpace * cos((60. / 180.) * pi);
  tmpxpos = xpos + (extraspacePipe + newradius + pipespace) * sin((60. / 180.) * pi);
  tmpypos = ypos + (extraspacePipe + newradius + pipespace) * cos((60. / 180.) * pi);
    
  for(int i = 0; i < possibleStrawsPipe; i++) 
    {
	
      putStrawRotatedLeft(tmpxpos - translationToLeft * sin(sixtyrad), 
			  tmpypos - translationToLeft * cos(sixtyrad), 
			  zpos, vectorx, vectory, vectorz, tubeLength);
	
      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
    
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];
	
      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 

	
      putStrawRotatedShortLeft(tmpxpos - (translationToLeft + translationToFrontPer) * sin(sixtyrad), 
			       tmpypos - (translationToLeft + translationToFrontPer) * cos(sixtyrad), 
			       zpos + translationToFrontPar, 
  			       vectorx, vectory, vectorz, lengthShort);
      
      putStrawRotatedShortLeft(tmpxpos - (translationToLeft - translationToFrontPer + switchSides) * sin(sixtyrad), 
			       tmpypos - (translationToLeft - translationToFrontPer + switchSides) * cos(sixtyrad), 
			       zpos - translationToFrontPar, 
  			       vectorx, vectory, vectorz, lengthShort);

      tmpxpos += 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
    
  // plane 5
  vectorx = 1.;
  vectory = 0.;
  vectorz = 0.;
  xpos += availableSpace * sin((60. / 180.) * pi);
  ypos += availableSpace * cos((60. / 180.) * pi);
  tmpxpos = xpos;
  tmpypos = ypos + extraspace + newradius;

  for(int i = 0; i < possibleStraws; i++) 
    {

      putStrawRotatedLeft(tmpxpos, tmpypos - translationToLeft, zpos, vectorx, vectory, vectorz, tubeLength);

      tmpypos += 2 * newradius;
    }
    
  for (int i = 0; i < extraStraws; i++)
    {
      double
	lengthShort = lengthsShort[i];
	
      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStraws + 1 + (2 * i)) * cos(skewanglerad); 
	
      putStrawRotatedShortLeft(tmpxpos, 
			       tmpypos - (translationToLeft + translationToFrontPer), 
  			       zpos + translationToFrontPar, 
  			       vectorx, vectory, vectorz, lengthShort);
	
      putStrawRotatedShortLeft(tmpxpos,
  			       tmpypos - (translationToLeft - translationToFrontPer + switchSides), 
  			       zpos - translationToFrontPar, 
  			       vectorx, vectory, vectorz, lengthShort);

      tmpypos += 2 * newradius;
    }
    
  // plane 6
  vectorx = cos((60. / 180.) * pi);
  vectory = sin((60. / 180.) * pi);
  vectorz = 0;
  ypos += availableSpace;
  tmpxpos = xpos - (extraspacePipe + newradius) * sin((60. / 180.) * pi);
  tmpypos = ypos + (extraspacePipe + newradius) * cos((60. / 180.) * pi);

  for(int i = 0; i < possibleStrawsPipe; i++) 
    {

      putStrawRotatedLeft(tmpxpos + translationToLeft * sin(sixtyrad), 
			  tmpypos - translationToLeft * cos(sixtyrad), 
			  zpos, vectorx, vectory, vectorz, tubeLength);

      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
    
  for (int i = 0; i < extraStrawsPipe; i++)
    {
      double
	lengthShort = lengthsShortPipe[i];
	
      double
	translationToFrontPar = ((tubeLength - lengthShort) / 2.) * cos(skewanglerad),
	translationToFrontPer = ((tubeLength - lengthShort) / 2.) * sin(skewanglerad),
	switchSides = 2 * newradius * (possibleStrawsPipe + 1 + (2 * i)) * cos(skewanglerad); 
	
      putStrawRotatedShortLeft(tmpxpos + (translationToLeft + translationToFrontPer) * sin(sixtyrad), 
			       tmpypos - (translationToLeft + translationToFrontPer) * cos(sixtyrad), 
			       zpos + translationToFrontPar, 
  			       vectorx, vectory, vectorz, lengthShort);
	
      putStrawRotatedShortLeft(tmpxpos + (translationToLeft - translationToFrontPer + switchSides) * sin(sixtyrad), 
			       tmpypos - (translationToLeft - translationToFrontPer + switchSides) * cos(sixtyrad), 
			       zpos - translationToFrontPar, 
			       vectorx, vectory, vectorz, lengthShort);
	
      tmpxpos -= 2 * newradius * sin((60. / 180.) * pi);
      tmpypos += 2 * newradius * cos((60. / 180.) * pi);
    }
}


void makeDoubleLayerStraightExact(double &startRadius)
{
  // close packed straight layers are only possible at specific positions
  // these are indicated by the integer ringteller. ringteller = 0 would be 
  // a single straw at the origin. ringteller = 1 would be the first layer of
  // 6 straws closepacked around that single straw. And so on ...

  // look for the first ring number which is completely outside the startradius 
  // given:
  double
    radius = tubeSeperation / 2.;

  int
    ringteller = 0;
    
  while (ringteller * sqrt(3.) * radius - radius <= startRadius)
    {
      ringteller++;
    }
  cerr << "startRadius " << startRadius << endl;
  // move the startRadius to the next double layer
  startRadius = (ringteller + 1) * sqrt(3.) * radius;
    
  // place the first layer of this double layer
  cerr << "positioning ring at: " << ringteller * sqrt(3.) * radius << endl;
  placeSingleLayerStraightExact(ringteller);
  // place the second layer
  cerr << "positioning ring at: " << (ringteller + 1) * sqrt(3.) * radius << endl;
  placeSingleLayerStraightExact(ringteller + 1);

  cerr << "ringteller " << ringteller << endl;
}

void makeSingleLayerStraightExact(double &startRadius)
{
  // see comments at makeDoubleLayerStraightExact()

  double 
    radius = tubeSeperation / 2.;
 
  int
    ringteller = 0;
  
  while (ringteller * sqrt(3.) * radius - radius + innerCoverThickness <= startRadius) // CHECK
    {
      ringteller++;
    }

  startRadius = (ringteller + 1) * sqrt(3.) * radius;
  
  cerr << "positioning ring at: " << ringteller * sqrt(3.) * radius << endl;
  placeSingleLayerStraightExact(ringteller);
  cerr << "ringteller " << ringteller << " radius " << radius << endl;
 
}

void makeDoubleLayerSkewedRight(double startRadius)
{
  // see comments at makeDoubleLayerSkewedLeft()
  placeSingleLayerSkewedRight(startRadius);
  placeSingleLayerSkewedRight(startRadius + sqrt(3.) * (tubeSeperation / 2.));
}

void makeDoubleLayerSkewedLeft(double startRadius)
{
  // place first skewed left layer at the specified radius
  placeSingleLayerSkewedLeft(startRadius);

  // the straw separation is the separation between the centers of two straws next to each other within one layer. 
  // Close packing then dictates that separation between the layers is sqrt(3) * (separation / 2)
  placeSingleLayerSkewedLeft(startRadius + sqrt(3.) * (tubeSeperation / 2.));
}

int main()
{
  // reset counters for the numbers of straws
  counter1 = 0;
  counter2 = 0;
  counter3 = 0;
  counter4 = 0;
  counter5 = 0;
  counter6 = 0;
  tubeteller = 0;
  axialtubeteller = 0;
  skewedtubeteller = 0;

  cout << setiosflags(ios::fixed) << setprecision(6);

  //------------------------------------------------------ volumes

  ////////////////////////////////////////
  //                                    //
  //         STT Support:               //
  //                                    //
  ////////////////////////////////////////

  if(!noSupport)
    {
      // container for all tubes!
      // for X > 0     
      cout << sttassembly << endl;
      cout << cave << endl;
      cout << "ASSEMBLY" << endl;
      cout << air  << endl;
      counter4++;
      writetrans(0., 0., sttCenterZ);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
      
      // cylinder inner x > 0
      writename(innerCylinder, 1, 1);
      writecylsupport(sttassembly, true);
      //       writemother(sttassembly, 1, 1);
      writemedium(AlBe);
      writehalftube((innerDiam / 2.), (innerDiam / 2.) + innerCoverThickness, tubeLength / 2.);
      writetrans(0., 0., 0.);
      writerot(0., 1., 0., -1., 0., 0., 0., 0., 1.);
      //      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);

      // cylinder outer x > 0
      writename(outerCylinder, 1, 1);
      writecylsupport(sttassembly, true);
      //       writemother(sttassembly, 1, 1);
      writemedium(AlBe);
      writehalftube((outerDiam / 2.) - outerCoverThickness, (outerDiam / 2.), tubeLength / 2.);
      writetrans(0., 0., 0.);
      writerot(0., 1., 0., -1., 0., 0., 0., 0., 1.);
      //      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);

      // cylinder inner x < 0
      writename(innerCylinder, 1, 0);
      writecylsupport(sttassembly, false);
      //       writemother(sttassembly, 1, 1);
      //       writemedium(AlBe);
      //       writehalftube((innerDiam / 2.), (innerDiam / 2.) + innerCoverThickness, tubeLength / 2.);
      writetrans(0., 0., 0.);
      writerot(0., -1., 0., 1., 0., 0., 0., 0., 1.);
      //      writerot(1., 0., 0., 0., -1., 0., 0., 0., 1.);

      // cylinder outer x < 0
      writename(outerCylinder, 1, 0);
      writecylsupport(sttassembly, false);
      //       writemother(sttassembly, 1, 1);
      //       writemedium(AlBe);
      //       writehalftube((outerDiam / 2.) - outerCoverThickness, (outerDiam / 2.), tubeLength / 2.);
      writetrans(0., 0., 0.);
      writerot(0., -1., 0., 1., 0., 0., 0., 0., 1.);
      //      writerot(1., 0., 0., 0., -1., 0., 0., 0., 1.);

      // around the pipe CHECK why (pipeDiam/2.) "-" panelthickness/2?? -----------------
      // panel up x > 0
      writepanel(panel1, true, panelthickness, (outerDiam/2. - innerDiam/2.),  tubeLength, 1);
      writetrans((pipeDiam/2.) - panelthickness/2.,  (outerDiam/2. + innerDiam/2.)/2., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
      // panel up x < 0
      writepanel(panel2,  false, panelthickness, (outerDiam/2. - innerDiam/2.),  tubeLength, 2);
      writetrans(-((pipeDiam/2.) - panelthickness), (outerDiam/2. + innerDiam/2.)/2., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
      // panel down x > 0
      writepanel(panel3, false, panelthickness, (outerDiam/2. - innerDiam/2.),  tubeLength, 1); 
      writetrans((pipeDiam/2.) - panelthickness, -(outerDiam/2. + innerDiam/2.)/2., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);
      // panel down x < 0
      writepanel(panel4, false, panelthickness, (outerDiam/2. - innerDiam/2.),  tubeLength, 2); 
      writetrans(-((pipeDiam/2.) - panelthickness), -(outerDiam/2. + innerDiam/2.)/2., 0.);
      writerot(1., 0., 0., 0., 1., 0., 0., 0., 1.);

    }

  ////////////////////////////////////////
  //                                    //
  //  Layers Geometry and               //
  //  Placement of the logical tubes :  //
  //                                    //
  ////////////////////////////////////////

  double
    startRadius = innerCoverThickness + innerDiam / 2.; // CHECK added CoverThickness // CHECK

  bool
    wintzdesign = true;

  if (wintzdesign)
    {

      // NEW ONE!
      // 4 double layers of straight straws
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);

      cerr << "start radius before skewed " << startRadius << endl;
      // startRadius = (ringteller + 1) * sqrt(3.) * radius;
  
      makeDoubleLayerSkewedLeft((26 * sqrt(3.) + 2) * (tubeSeperation / 2.));
      makeDoubleLayerSkewedRight((27 * sqrt(3.) + 4) * (tubeSeperation / 2.) + 0.244);
      makeDoubleLayerSkewedLeft((28 * sqrt(3.) + 6) * (tubeSeperation / 2.) + 2*0.244);
      makeDoubleLayerSkewedRight((29 * sqrt(3.) + 8) * (tubeSeperation / 2.) + 3*0.244);

      
      cerr << "start radius after " << startRadius << endl;
      startRadius += 10 * tubeSeperation * sqrt(3.) /2.;

      cerr << "start radius " << startRadius << endl;
    
      // then two double layers of straight straws
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);

    }
  else
    {
      makeDoubleLayerStraightExact(startRadius);
      startRadius += tubeSeperation;
      makeDoubleLayerSkewedLeft(startRadius);
      startRadius += tubeSeperation + sqrt(3.) * (tubeSeperation / 2.);
      makeDoubleLayerSkewedRight(startRadius);
      startRadius += (tubeSeperation / 2.) + sqrt(3.) * (tubeSeperation / 2.);
      makeDoubleLayerStraightExact(startRadius);
      startRadius += tubeSeperation;
      makeDoubleLayerSkewedLeft(startRadius);
      startRadius += tubeSeperation + sqrt(3.) * (tubeSeperation / 2.);
      makeDoubleLayerSkewedRight(startRadius);
      startRadius += (tubeSeperation / 2.) + sqrt(3.) * (tubeSeperation / 2.);
      makeDoubleLayerStraightExact(startRadius);
      startRadius += tubeSeperation;
      makeDoubleLayerSkewedLeft(startRadius);
      startRadius += tubeSeperation + sqrt(3.) * (tubeSeperation / 2.);
      makeDoubleLayerSkewedRight(startRadius);
      startRadius += (tubeSeperation / 2.) + sqrt(3.) * (tubeSeperation / 2.);
      makeDoubleLayerStraightExact(startRadius);
    }

  // fill the rest of the volume with straight straws
  // straws outside the outerDiam will not be placed
  bool
    fillup = true;
   
  if (fillup)
    {
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
      makeDoubleLayerStraightExact(startRadius);
    }

  // output of all counters
  cerr << "tubes: " << tubeteller << endl; 
  cerr << "axial tubes: " << axialtubeteller << endl; 
  cerr << "skewed tubes full length: " << skewedtubeteller << endl; 
  cerr << "skewed tubes short: " << shortskewedtubeteller << endl; 
  cerr << "minimum radius: " << minimumradius << endl;
  cerr << "maximum radius: " << maximumradius << endl;

  return 0;
}



