/*
 * PastaTBRawToRoot.C
 *
 *  Created on: 20.09.2017
 *      Author: Stockmanns
 */

	PndCRCCalculator decoder(16, 0x1021, 0, 0 ,0, 0x0F4A);
	PndMvdPasta pastaConv;
	RunSummary summary;

	unsigned int oldFrameCount = 0;
	ULong64_t framesSinceLastData = 0;
	ULong64_t diffAllFrameCount = 0;
	int oldDiffAllFrameCount = 0;

	int verbose = 1;

void PrintFrame(std::vector<ULong64_t>& frame){
	for (auto data : frame)
		std::cout << std::hex << data << std::endl;
	std::cout << std::endl;
}

bool CheckHitCount(std::vector<ULong64_t> frame)
{
	ULong64_t header;
	if (frame.size() > 0)
		header = frame[0];
	else
		return false;

	unsigned int nHits = (header >> 32) & 0xff;
	if (verbose > 1) std::cout << "Hits in Frame: " << nHits << std::endl;
	if (nHits == frame.size() - 2){
		return true;
	}
	else if (nHits == 0xff && frame.size() == 2)
		return true;
	if (verbose > 0){
		 std::cout << "***** Error Hit Count ***** Header: " << header << " nHits: " << nHits << " size frame: " << frame.size() - 2 << std::endl;
		PrintFrame(frame);
	} 
	return false;
}

bool CheckFrameCount(std::vector<ULong64_t> frame)
{
	ULong64_t header;
	if (frame.size() > 0)
		header = frame[0];
	else
		return false;

	unsigned int frameCount = header & 0xffffffff;
	ULong64_t calcFrameCount = oldFrameCount + framesSinceLastData;

	if (oldFrameCount == 0){
		summary.fAllCountedFrames.push_back(frameCount);			//sets the first "allCountedFrames" to the start value
		if (verbose > 0) std::cout << "****** Info Setting allCountedFrames to start frames ***** " << summary.fAllCountedFrames.back() << std::endl;
	}
	else if (calcFrameCount > frameCount) {
		if (verbose > 0) std::cout << "***** Info PartialReset? calcFrameCount " << calcFrameCount << " > frameCount " << frameCount << std::endl; 
		summary.fAllCountedFrames.push_back(frameCount);			//sets the first "allCountedFrames" to the start value
		summary.fAllPartialResets++;														//if a partial reset has happened the allCountedFrames has to be corrected
	}
	if (frameCount - summary.fAllCountedFrames.back() != oldDiffAllFrameCount){
		if (verbose > 0) std::cout << "Error allCountedFrames does not match!" << std::endl;
		if (frameCount - summary.fAllCountedFrames.back() < 1000000)
			oldDiffAllFrameCount = (frameCount - summary.fAllCountedFrames.back());
	}

	if ((oldFrameCount > 0) && (frameCount > calcFrameCount))
		summary.fMissingFrames += frameCount - calcFrameCount;

	if (verbose > 1) std::cout << "oldFrameCount " << oldFrameCount << " framesSince: " << framesSinceLastData << " newFrame " << frameCount <<
				" difference: " << std::dec << frameCount - calcFrameCount <<
				" allFrameCount " << std::hex << summary.fAllCountedFrames.back() << std::dec <<
				" difference " << frameCount - summary.fAllCountedFrames.back() <<
				" total missing Frames: " << summary.fMissingFrames << std::endl;

	if (calcFrameCount > 0x100000000){
		if (verbose > 0) std::cout << "New Super Frame: calcFrame " << calcFrameCount << std::endl;
		summary.fSuperFrameCount ++;
		calcFrameCount = calcFrameCount & 0xffffffff;
	}

	if (calcFrameCount == frameCount){
		oldFrameCount = frameCount;
		return true;
	}
        if (verbose > 0){
                std::cout << "***** Error Frame Count ***** : calcFrameCount " << calcFrameCount << " frameCount " << frameCount << " allCountedFrames " << summary.fAllCountedFrames.back() << std::endl;
                PrintFrame(frame);
        }

	oldFrameCount = frameCount;
	return false;
}
bool CheckCRC(std::vector<ULong64_t> frame)
{
	std::vector<char> frameInChar = pastaConv.ConvertData(frame);

	ULong64_t calculatedCRC = decoder.CalculateCRCTableFast(frameInChar, frameInChar.size());
	if (verbose > 1) std::cout << "CalculatedCRC: " << hex << calculatedCRC << std::endl;
	if (calculatedCRC == frame.back()){
		return true;
	} else {
		if (verbose > 0){
                 	std::cout << "***** Error CRC ***** CalculatedCRC: " << calculatedCRC << std::endl;
                	PrintFrame(frame);
       		}
		return false;
	}
	return false;
}

std::vector<PndMvdPastaDigi> ProcessFrame(std::vector<ULong64_t> frame)
{
	std::vector<PndMvdPastaDigi> digis;
	FrameHeader header = pastaConv.AnalyzeHeader(frame[0]);
	if (header.nEvents == 0x21){
		frame.erase(frame.begin());
		pastaConv.AnalyzeHeader(frame[0]);
	}
	bool CRC_Ok = CheckCRC(frame);

	if (CRC_Ok == true){
		if (verbose > 1) std::cout << "CRC match!" << std::endl;
		summary.fCrcMatchCount++;
	} else {
		if (verbose > 0) std::cout << "*********** CRC error *************" << std::endl;
		summary.fCrcErrorCount++;
		framesSinceLastData++;
		return digis;	// if CRC is corrupt then the analysis of the other data does not make sense!
	}

	bool HitCount_Ok = CheckHitCount(frame);
	bool FrameCount_Ok = CheckFrameCount(frame);

	if (HitCount_Ok == true){
		if (verbose > 1) std::cout << "Correct Hit Count!" << std::endl;
	} else {
		if (verbose > 0) std::cout << "************* Wrong Hit Counts ***************" << std::endl;
		summary.fWrongHitCount++;
	}
	if (FrameCount_Ok == true){
		if (verbose > 1) std::cout << "Correct Frame Count!" << std::endl;
	} else {
		if (verbose > 0) std::cout << "************** Wrong Frame Count *************" << std::endl;
		summary.fWrongFrameCount++;
	}
	if (verbose > 1) std::cout << std::endl;

	if (CRC_Ok && HitCount_Ok && FrameCount_Ok && (frame.size() % 2 == 0)){
		for (int i = 1; i < frame.size() -1; i+=2){
			PndMvdPastaDigi newDigi(header, pastaConv.AnalyzeThresholdWordFull(frame[i]), pastaConv.AnalyzeThresholdWordFull(frame[i+1]));
			if (verbose > 1) std::cout << newDigi;
			digis.push_back(newDigi);
		}
	}
	return digis;
}

int PastaTBRawToRoot(TString fileName)
{
	PndFileNameCreator creator(fileName.Data());
	TString outputFileName = creator.GetDigiFileName();
	TFile f(outputFileName, "RECREATE");
	TTree t("PastaTB","Pasta Digis from TestBeam");

	TClonesArray* pastadata = new TClonesArray("PndMvdPastaDigi");
	t.Branch("data", &pastadata);
//	t.Branch("header", &summary);


	std::ifstream inputFile(fileName.Data());
	ULong64_t data;
	std::vector<ULong64_t> frame;

	while (!(inputFile.eof())){
		inputFile >> std::hex >> data;
		if (verbose > 1) std::cout << std::hex << data << std::endl;
		if (data == 0xffffffffff){
			if (frame.size() > 1){
				std::vector<PndMvdPastaDigi> pastavec = ProcessFrame(frame);
				for (int i = 0; i < pastavec.size(); i++)
					new((*pastadata)[pastadata->GetEntries()]) PndMvdPastaDigi(pastavec[i]);
				if (pastavec.size() > 0){
					t.Fill();
					pastadata->Delete();
				}
				framesSinceLastData = 0;
			}
			else if (frame.size() == 1) {
				summary.fSingleWordFrames++;
			}
			frame.erase(frame.begin(), frame.end());
			framesSinceLastData++;
			if (summary.fAllCountedFrames.size() > 0)
				summary.fAllCountedFrames.back()++;
		} else {
			frame.push_back(data);
		}
	}
	std::cout << "*************** Summary **************" << std::endl;
	std::cout << std::dec << "CRCMatch: " << summary.fCrcMatchCount <<
			" CRCErrors: " << summary.fCrcErrorCount <<
			" SingleWordFrames: " << summary.fSingleWordFrames <<
			" WrongHitCount: " << summary.fWrongHitCount <<
			" WrongFrameCount: " << summary.fWrongFrameCount <<
			" missing Frames: " << summary.fMissingFrames <<
			" SuperFrameCount: " << summary.fSuperFrameCount <<
			std::endl;
	summary.Write();
	f.Write();
	return 0;
}
