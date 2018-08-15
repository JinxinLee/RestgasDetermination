/*
 * PndMvdPastaTorino.h
 *
 *  Created on: 23.09.2017
 *      Author: Stockmanns
 */

#ifndef MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTATORINO_H_
#define MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTATORINO_H_

#include "PndMvdPastaDigi.h"
#include "TObject.h"

class PndMvdPastaTorino : public TObject {
public:
	PndMvdPastaTorino();
	virtual ~PndMvdPastaTorino();

	std::vector<PndMvdPastaDigi> ReadTorinoDaqFrame(std::ifstream& infile);

protected:
	FrameHeader AnalyzeHeader(std::string& line);
	PndMvdPastaDigi AnalyzeData(std::string& line);

private:
	FrameHeader fCurrentHeader;

	ClassDef(PndMvdPastaTorino,1);
};

#endif /* MVD_MVDTESTBEAM_PASTATB_PNDMVDPASTATORINO_H_ */
