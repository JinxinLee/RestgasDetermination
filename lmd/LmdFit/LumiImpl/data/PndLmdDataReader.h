/*
 * PndLmdDataReader.h
 *
 *  Created on: Aug 24, 2013
 *      Author: steve
 */

#ifndef PNDLMDDATAREADER_H_
#define PNDLMDDATAREADER_H_

#include "LumiFitStructs.h"

#include <vector>

#include "TString.h"
#include "TLorentzVector.h"

class PndLmdAbstractData;
class PndLmdData;
class PndLmdAcceptance;
class PndLmdResolution;

class TDatabasePDG;
class PndLmdTrackQ;
class TClonesArray;

class PndLmdDataReader {
private:
	std::vector<PndLmdData*> registered_data;
	std::vector<PndLmdAcceptance*> registered_acceptances;
	std::vector<PndLmdResolution*> registered_resolutions;

	void clearRegisters();

	std::vector<PndLmdAbstractData*> combineAllRegisteredDataObjects();

	void removeFinished(std::vector<PndLmdAbstractData*> &lmd_vec,
			int num_events);
	int getNextMinEventIndex(std::vector<PndLmdAbstractData*> &lmd_vec);

	double getTrackParameterValue(PndLmdTrackQ *track_pars,
			const LumiFit::LmdDimension &lmd_dim) const;
	double getTrackParameterDifference(PndLmdTrackQ *track_pars,
			LumiFit::LmdDimension lmd_dim) const;

	bool skipDataObject(const PndLmdAbstractData* data, bool reconstruced) const;

	void fillData(PndLmdTrackQ *track_pars);

	void cleanup();

	virtual unsigned int getEntries() const =0;
	virtual void initDataStream() =0;
	virtual void clearDataStream() =0;

	virtual TClonesArray* getEntry(unsigned int i) =0;

	TLorentzVector beam;

protected:
	TDatabasePDG *pdg;

	std::vector<TString> data_dirs;

public:
	PndLmdDataReader();
	virtual ~PndLmdDataReader();

	void setBeam(double lab_momentum);

	void addDataPath(TString dir_path);

	int registerData(PndLmdData* data);
	int registerData(std::vector<PndLmdData> &data_vec);

	int registerAcceptance(PndLmdAcceptance* acc);
	int registerAcceptances(std::vector<PndLmdAcceptance> &acc_vec);

	int registerResolution(PndLmdResolution* res);
	int registerResolutions(std::vector<PndLmdResolution> &res_vec);

	void read();
};

#endif /* PNDLMDDATAREADER_H_ */
