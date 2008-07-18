#include "PndTpcDEDXDiagnostics.h"
#include <iostream>
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "TPolyLine.h"

using namespace std;

ClassImp(PndTpcDEDXDiagnostics);

void PndTpcDEDXDiagnostics::SetPoint(PndTpcDEDXDiagnosticPoint point)
{
	fPoints.push_back(point);
}


TPolyLine3D* PndTpcDEDXDiagnostics::GetConnectedLine() const
{
	TPolyLine3D* line=new TPolyLine3D(fPoints.size());
	std::vector<PndTpcDEDXDiagnosticPoint>::const_iterator cit;
	std::vector<PndTpcDEDXDiagnosticPoint>::const_iterator end=fPoints.end();
	int i=0;
	for(cit=fPoints.begin();cit!=end;++cit)
	{
		line->SetPoint(i,cit->GetX(),cit->GetY(),cit->GetZ());
		++i;
	}	
	return line;
}

TPolyMarker3D* PndTpcDEDXDiagnostics::GetMarker() const
{
	TPolyMarker3D* line=new TPolyMarker3D(fPoints.size());
	std::vector<PndTpcDEDXDiagnosticPoint>::const_iterator cit;
	std::vector<PndTpcDEDXDiagnosticPoint>::const_iterator end=fPoints.end();
	int i=0;
	for(cit=fPoints.begin();cit!=end;++cit)
	{
		line->SetPoint(i,cit->GetX(),cit->GetY(),cit->GetZ());
		++i;
	}	
	return line;
}

TPolyLine* PndTpcDEDXDiagnostics::GetLine2D() const
{
	TPolyLine* line=new TPolyLine(fPoints.size());
	std::vector<PndTpcDEDXDiagnosticPoint>::const_iterator cit;
	std::vector<PndTpcDEDXDiagnosticPoint>::const_iterator end=fPoints.end();
	int i=0;
	for(cit=fPoints.begin();cit!=end;++cit)
	{
		line->SetPoint(i,cit->GetX(),cit->GetY());
		++i;
	}	
	return line;
}

