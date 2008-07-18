//some ploting macros
//they use a stupid class PndTpcDEDXDiagnostics which has to be stored
//in a tree that has been previously loaded in root
//the plotWholeEntry macro uses the PndTpcCluster class as well

//plotTrack: plots the clusters of a track that are stored in PndTpcDEDXDiagnostics
//that means for example if there are two tracks the clusters of the tracks
//can be plotted independently 
//(if  PndTpcDEDXDiagnostics is filled in that way)
//for example PndTpcDEDXDiagnosticTask fills the PndTpcDEDXDiag in a suiting way
//a line between the clusters trys to visualize sorting
void plotTrack(int entry_to_show=0,int track_to_show=0, bool ShowOrder=false)
{
	TClonesArray *pTrackArray=NULL;
	cbmsim->SetBranchAddress("PndTpcDEDXDiag", &pTrackArray);
	Long64_t nTreeEntries=cbmsim->GetEntriesFast();
	
	if(nTreeEntries<=entry_to_show)	{
		cout << "only" <<  nTreeEntries << "Tree entries available." << endl;
		return;
	}	
	cbmsim->GetEntry(entry_to_show);
	int nTracks=pTrackArray->GetEntriesFast();
	if(nTracks>track_to_show)	{
		if(!ShowOrder)	{
		cbmsim->SetMarkerSize(1);
		cbmsim->SetMarkerStyle(20);
		cbmsim->Draw("PndTpcDEDXDiag.fZ:PndTpcDEDXDiag.fY:PndTpcDEDXDiag.fX:PndTpcDEDXDiag.fAmp","","",1,entry_to_show);
		}
		else	{
		cbmsim->SetMarkerSize(1);
		cbmsim->SetMarkerStyle(20);
		cbmsim->Draw("PndTpcDEDXDiag.fZ:PndTpcDEDXDiag.fY:PndTpcDEDXDiag.fX:PndTpcDEDXDiag.fNumber*4","","",1,entry_to_show);
		}
		PndTpcDEDXDiagnostics *pDiag=(PndTpcDEDXDiagnostics*)pTrackArray->At(track_to_show);
		TPolyLine3D *line=pDiag->GetConnectedLine();
		line->SetLineWidth(2);
		line->Draw("SAME");
		
	}
	else	{
		cout << "only " <<  nTracks << " Tracks available." << endl;
	}
}


//plotAllTracks: Will plot a whole entry, if there are more tracks than one
//the lines connecting the clusters that belong to different tracks
//are painted in different colors
void plotAllTracks(int entry_to_show=0, bool ShowOrder=false)
{
	TClonesArray *pTrackArray=NULL;
	cbmsim->SetBranchAddress("PndTpcDEDXDiag", &pTrackArray);
	Long64_t nTreeEntries=cbmsim->GetEntriesFast();
	
	if(nTreeEntries<=entry_to_show)	{
		cout << "only" <<  nTreeEntries << "Tree entries available." << endl;
		return;
	}	
	cbmsim->GetEntry(entry_to_show);
	int nTracks=pTrackArray->GetEntriesFast();
	short int color=1;	//black, red, green, blue
	if(!ShowOrder)	{
		cbmsim->SetMarkerSize(1);
		cbmsim->SetMarkerStyle(20);
		cbmsim->Draw("PndTpcDEDXDiag.fZ:PndTpcDEDXDiag.fY:PndTpcDEDXDiag.fX:PndTpcDEDXDiag.fAmp","","",1,entry_to_show);
	}
	else	{
		cbmsim->SetMarkerSize(1);
		cbmsim->SetMarkerStyle(20);
		cbmsim->Draw("PndTpcDEDXDiag.fZ:PndTpcDEDXDiag.fY:PndTpcDEDXDiag.fX:PndTpcDEDXDiag.fNumber","","",1,entry_to_show);
	}
	for(int i=0; i<nTracks; ++i)	{
			PndTpcDEDXDiagnostics *pDiag=(PndTpcDEDXDiagnostics*)pTrackArray->At(i);
			TPolyLine3D *line=pDiag->GetConnectedLine();
			line->SetLineColor(color++);
			line->SetLineWidth(1);
			line->Draw("SAME");
	}
}


//plotWholeEntry: Shows all clusters not just the ones associated with tracks
void plotWholeEntry(int entry_to_show=0)
{
	TClonesArray *pTrackArray=NULL;
	cbmsim->SetBranchAddress("PndTpcDEDXDiag", &pTrackArray);
	Long64_t nTreeEntries=cbmsim->GetEntriesFast();
	
	if(nTreeEntries<=entry_to_show)	{
		cout << "only" <<  nTreeEntries << "Tree entries available." << endl;
		return;
	}	
	cbmsim->GetEntry(entry_to_show);
	int nTracks=pTrackArray->GetEntriesFast();
	short int color=1;	//black, red, green, blue

	cbmsim->SetMarkerSize(1);
	cbmsim->SetMarkerStyle(20);
	cbmsim->Draw("PndTpcCluster._pos.fZ:PndTpcCluster._pos.fY:PndTpcCluster._pos.fX:PndTpcCluster._amp","","",1,entry_to_show);
	
	for(int i=0; i<nTracks; ++i)	{
			PndTpcDEDXDiagnostics *pDiag=(PndTpcDEDXDiagnostics*)pTrackArray->At(i);
			TPolyLine3D *line=pDiag->GetConnectedLine();
			line->SetLineColor(color++);
			line->SetLineWidth(1);
			line->Draw("SAME");
	}
}