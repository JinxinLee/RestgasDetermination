//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPadShapePool
//      see PndTpcPadShapePool.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcPadShapePool.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <fstream>
// Collaborating Class Headers --------
#include "PndTpcAbsPadShape.h"
#include "PndTpcPadShapeSquare.h"
#include "PndTpcPadShapePolygon.h"
#include "PndTpcPRLookupTable.h"
#include "TError.h"
// Class Member definitions -----------

PndTpcPadShapePool::PndTpcPadShapePool()
{;}

PndTpcPadShapePool::PndTpcPadShapePool(const char* const filename,
				 const PndTpcGem& gem,
				 const double _range,      
				 const double _step,
				 const double _intStep)
{
  ReadFromFile(filename);
  BuildLookupTable(gem, _range, _step, _intStep);
}

PndTpcPadShapePool::~PndTpcPadShapePool()
{
  for(int i=0; i<references.size(); i++)
  {
    if (references[i] != 0)
      delete references[i];
  }
}

PndTpcAbsPadShape*
PndTpcPadShapePool::GetPadShape(const unsigned int _ID) const
{
  if (references.size() <= _ID)
    return(0);
  return(references[_ID]);
}

void
PndTpcPadShapePool::AddPadShape(PndTpcAbsPadShape* padshape)
{
  if (TryAddingPadShape(padshape) == false)
    Fatal("PndTpcPadShapePool::AddPadShape","The padshape could not be added, because the ID is already occupied.");
}

void
PndTpcPadShapePool::RemovePadShape(const unsigned int _ID)
{
  if (_ID >= references.size() || references[_ID] == 0)
  {
    Warning("PndTpcPadShapePool::RemovePadShape","There is no PndTpcAbsPadShape with the ID=%i",_ID);
    return;
  }
  delete references[_ID];
  references[_ID] = 0;
  if (_ID == references.size()-1)
  {
    int i=_ID;
    while (i>=0 && references[i] == 0)
      i--;
    references.resize(i+1,0);
  }    
}

void
PndTpcPadShapePool::ReadFromFile(const char* const filename)
{
  //Reading the file
  bool noproblem = true;
  std::ifstream infile(filename, std::fstream::in);
  Warning("PndTpcPadShapePool::ReadFromFile","Reading file %s",filename);
  if (!infile.good())
    Fatal("PndTpcPadShapePool::ReadFromFile","The file %s can not be found.",filename);
  while(infile.good() && noproblem)
    noproblem = ReadLine(infile);
  if (!infile.eof())
    Fatal("PndTpcPadShapePool::ReadFromFile","The file %s can not be read.",filename);
  infile.close();
  if (noproblem == false)
    Fatal("PndTpcPadShapePool::ReadFromFile","The file %s does not correspond to the required format.",filename);
}

void
PndTpcPadShapePool::BuildLookupTable(const PndTpcGem& gem,
				  const double _range,      
				  const double _step,
				  const double _intStep){
  for (int i=0; i<references.size(); i++)
    if (references[i] != 0 && !references[i]->GetLookupTable()->IsBuilt())
      references[i]->GetLookupTable()->BuildTable(gem, *references[i],
						  _range, _step, _intStep);
}

bool
PndTpcPadShapePool::TryAddingPadShape(PndTpcAbsPadShape* padshape)
{
  unsigned int id = padshape->GetID();
  if (references.size() > id && references[id] != 0)
    return(false);
  else if (references.size() <= id)
    references.resize(id+1, 0);
  references[id]=padshape;
  return(true);
}

bool
PndTpcPadShapePool::ReadLine(std::ifstream& infile)
{
  PndTpcAbsPadShape* padshape=0;
  char line[256];
  int begin=0;
  infile.getline(line, 256);
  //remove blanks at begin of line
  while (isspace((int) line[begin]))
  {
    if (line[begin] == '\0') return(true);
    begin++;
  }
  //remove comments at end of line
  char* com=strchr(line, '*');//comment
  if (com != 0)
    *com = '\0';
  //decide which PadShape
  if (line[begin] == 's')
    padshape = ReadShape(&line[++begin]);
  else if (line[begin] == 'p')
    padshape = ReadPolygon(&line[++begin]);
  else if (line[begin] == '\0')
    return(true);
  else
  {
    Warning("PndTpcPadShapePool::ReadLine","In line: %i \nFirst character in line has no meaning.",line);
    return(false);
  }
  //if it was not possible to create a padshape with th data
  if (padshape==0)
  {
    Warning("PndTpcPadShapePool::ReadLine","In line: %i\nIt was not possible to create a PndTpcPadShape.",line);
    return(false);
  }
  if (TryAddingPadShape(padshape)== true)
    return(true);
  //if the ID is already used
  Warning("PndTpcPadShapePool::ReadLine","In line: %i\nThe padshape could not be added, because the ID %i is already occupied.",line,padshape->GetID());
  delete padshape;
  return(false);
}


PndTpcAbsPadShape*
PndTpcPadShapePool::ReadShape(const char* const line)
{
  char* rest;
  unsigned int id;
  double d;
  id = (unsigned int) strtoul(line,&rest,10);
  d = strtod(rest,&rest);
  if (d==0)
    Warning("PndTpcPadShapePool::ReadShape","The size of the padshape wit ID=%i is 0.",id);
  PndTpcPadShapeSquare* pads = new PndTpcPadShapeSquare(d, id);
  return(pads);
}

PndTpcAbsPadShape*
PndTpcPadShapePool::ReadPolygon(const char* const line)
{
  char* rest;
  unsigned int id;
  std::vector<double> xCoord(0);
  std::vector<double> yCoord(0);
  id = (unsigned int) strtoul(line,&rest,10);
  for (int i=0; ;i++)
  {
    char* end;
    xCoord.push_back(strtod(rest,&rest));
    yCoord.push_back(strtod(rest,&end));
    if (rest == end)
    {
      xCoord.pop_back();
      yCoord.pop_back();
      break;
    }
    rest = end;
  }
  PndTpcPadShapePolygon* padp = new PndTpcPadShapePolygon(xCoord, yCoord, id);
  return(padp);
}

std::ostream& operator<< (std::ostream& s, const PndTpcPadShapePool& me)
{
  return s <<"PndTpcPadShapePool:\n"
           <<"max. ID of a PadShape="<<me.references.size()-1<<"\n";
}
