/*
 * PastaTBRawToRoot.C
 *
 *  Created on: 20.09.2017
 *      Author: Stockmanns
 */


int PastaTBRawToRootTorino(TString fileName)
{
	PndFileNameCreator creator(fileName.Data());
	TString outputFileName = creator.GetDigiFileName();
	TFile f(outputFileName, "RECREATE");
	TTree t("PastaTB","Pasta Digis from TestBeam");

	TClonesArray* pastadata = new TClonesArray("PndMvdPastaDigi");
	t.Branch("data", &pastadata);


	std::ifstream inputFile(fileName.Data());
	PndMvdPastaTorino dataReader;

	int nHits = 1;

	while (nHits > 0){
		std::vector<PndMvdPastaDigi> pastavec = dataReader.ReadTorinoDaqFrame(inputFile);
		nHits = pastavec.size();
		for (int i = 0; i < pastavec.size(); i++){
			//std::cout << pastavec[i];
			new((*pastadata)[pastadata->GetEntries()]) PndMvdPastaDigi(pastavec[i]);
		}
		t.Fill();
		pastadata->Delete();
	}
	std::cout << "*************** Finish conversion **************" << std::endl;

	f.Write();
	return 0;
}
