mAp(TString Base)
{
	TString Pfad="macro/tpc/dedx/de/";
	
	TString Line1=".x " + Pfad + "mAp1.C(\"" + Base + "\")";
	cout << Line1.Data() << endl;
	gROOT->ProcessLine(Line1.Data());
	
	TString Line2=".x " + Pfad + "mAp2.C(\"" + Base + "\")";
	gROOT->ProcessLine(Line2.Data());	
	
	TString Line3=".x " + Pfad + "mAp3.C(\"" + Base + "\")";
	gROOT->ProcessLine(Line3.Data());		
	
}