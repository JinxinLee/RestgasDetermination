// ****************************************************************
// invexp.C
// ****************************************************************
// Some helper functions for producing a TTree string expression
// for several kinematic variables out of (4-)vector components
//
// If a TTree contains the branches Part0px, ... , Part0e, Part1px, ... , Part1e
// the function m("Part0+Part1") gives "sqrt((Part0e+Part1e)^2-(Part0px+Part1px)^2-(Part0py+Part1py)^2-(Part0pz+Part1pz)^2)"
// as result. Works with + and - sign and arbitrary number of particles.
//
// USAGE:    .L invexp.C+
//           fnc(TString expr, TString suff)
//
//  - expr : formula without suffixes, e.g. "p0+p2-p3-p4";
//           if 1st character is a '*', suffixes used as prefixes, e.g. p("*p0+p1") results in "sqrt((pxp0+pxp1)^2+(pyp0+pyp1)^2+(pzp0+pzp1)^2)"
//  - suff : list with suffixes; default : "e px py pz"
//
// Implemented functions are:
// - m()      : invariant mass
// - m2()     : invariant mass squared
// - p()      : momentum
// - pt()     : transvers momentum
// - pz()     : z-component of momentum
// - tht()    : polar angle in radians
// - thtd()   : polar angle in degrees
// - ctht()   : cosine of polar angle
// - dalitz() : expr = list of 3 particles, e.g. "p0 p1 p2" -> returns 'm2("p0+p1"):m2("p0+p2")' 
//
// ****************************************************************
//  K.Goetzen, GSI, 10/2015
// ****************************************************************



#include "TObjString.h"
#include "TString.h"
#include "TObjArray.h"

#include <vector>

typedef std::vector<TString> StrVec;

int SplitString(TString s, TString delim, StrVec &toks)
{
	toks.clear();
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
	{
		TString token = (((TObjString*)tok->At(i))->String()).Strip(TString::kBoth);
		toks.push_back(token);
	}
	return toks.size();
}

// ---------------------------------
// prepares the TString arrays
// ---------------------------------

bool prepare(TString pts, TString exts, StrVec &c, StrVec &vars)
{
	bool invord = false;
	
	if (pts.BeginsWith("*")) invord=true;
	pts.ReplaceAll("*","");
	
	SplitString(exts," ",c);
	
	if (invord)
	{
		pts.ReplaceAll("-","- ");
		pts.ReplaceAll("+","+ ");
	}
	else
	{
		pts.ReplaceAll("-"," -");
		pts.ReplaceAll("+"," +");
	}
	
	SplitString(pts," ",vars);	
	
	return invord;
}

// ---------------------------------
// makes the formula out of the vector components
// ---------------------------------

TString makefml(TString pts, TString exts, int ncmp=4, TString last="-")
{
	StrVec c,vars;
	bool invord = prepare(pts, exts, c, vars);
	
	TString r="";
	
	for (int i=0;i<ncmp;++i)
	{
		r += "(";
		for (unsigned int j=0;j<vars.size();++j) invord ? r += c[i]+vars[j] : r += vars[j]+c[i];
		r += ")^2";
		if (i<ncmp-1) r += last;
	}
	return r;
}

// ---------------------------------
// ---------------------------------

TString m(TString pts, TString exts="e px py pz")
{	
	return "sqrt("+makefml(pts,exts)+")";
}

// ---------------------------------
// ---------------------------------

TString m2(TString pts, TString exts="e px py pz")
{
	return "("+makefml(pts,exts)+")";
}

// ---------------------------------
// ---------------------------------

TString p(TString pts, TString exts="px py pz")
{
	return "sqrt("+makefml(pts,exts,3,"+")+")";
}

// ---------------------------------
// ---------------------------------

TString pt(TString pts, TString exts="px py pz")
{
	return "sqrt("+makefml(pts,exts,2,"+")+")";
}

// ---------------------------------
// ---------------------------------

TString pz(TString pts, TString exts="px py pz")
{
	StrVec c,vars;
	bool invord = prepare(pts, exts, c, vars);
	
	TString r = "(";
	
	for (unsigned int j=0;j<vars.size();++j) invord ? r += c[2]+vars[j] :r += vars[j]+c[2];
	r += ")";

	return r;
}

// ---------------------------------
// ---------------------------------

TString thtd(TString pts, TString exts="px py pz")
{
	TString tpt = pt(pts,exts);
	TString tpz = pz(pts,exts);
	
	return "(atan2("+tpt+","+tpz+")*57.3)";
}

// ---------------------------------
// ---------------------------------

TString tht(TString pts, TString exts="px py pz")
{
	TString tpt = pt(pts,exts);
	TString tpz = pz(pts,exts);
	
	return "atan2("+tpt+","+tpz+")";
}

// ---------------------------------
// ---------------------------------

TString ctht(TString pts, TString exts="px py pz")
{
	TString tp  = p(pts,exts);
	TString tpz = pz(pts,exts);
	
	return "("+tpz+"/"+tp+")";
}

// ---------------------------------
// ---------------------------------

TString dalitz(TString pts, TString exts="e px py pz")
{
	bool invord = (pts.BeginsWith("*"));
	pts.ReplaceAll("*","");

	StrVec c,vars;
	SplitString(pts, " ", vars);
	
	if (vars.size()!=3) return "1";
	
	TString pts1=vars[0]+"+"+vars[1];
	TString pts2=vars[0]+"+"+vars[2];
	
	TString m01 = m2((invord?"*":"")+pts1,exts);
	TString m02 = m2((invord?"*":"")+pts2,exts);
	
	return m01+":"+m02;
}




// TString invexp(TString p1, TString p2="", TString p3="", TString p4="")
// {
// 	TString c[4]={"e","px","py","pz"};
// 	TString r = "sqrt(";
// 	
// 	for (int i=0;i<4;++i)
// 	{
// 		if (p2=="")      r += TString::Format("%s%s^2",p1.Data(),c[i].Data());
// 		else if (p3=="") r += TString::Format("(%s%s%s%s)^2",p1.Data(),c[i].Data(),p2.Data(),c[i].Data());
// 		else if (p4=="") r += TString::Format("(%s%s%s%s%s%s)^2",p1.Data(),c[i].Data(),p2.Data(),c[i].Data(),p3.Data(),c[i].Data());
// 		else             r += TString::Format("(%s%s%s%s%s%s%s%s)^2",p1.Data(),c[i].Data(),p2.Data(),c[i].Data(),p3.Data(),c[i].Data(),p4.Data(),c[i].Data());
// 		
// 		if (i<3) r += "-";
// 	}
// 	
// 	r += ")";
// 	
// }

