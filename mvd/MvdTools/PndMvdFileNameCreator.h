/// PndMvdFileNameCreator.h
/// @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
/// @brief A simple class which adds the corresponding file extensions to a given base class
///

#ifndef PNDMVDFILENAMECREATOR_H
#define PNDMVDFILENAMECREATOR_H

#include <vector>
#include <string>

#include "TObject.h"

class PndMvdFileNameCreator : public TObject
{
	public :
			PndMvdFileNameCreator();
			PndMvdFileNameCreator(std::string fileName);

			void SetFileName(std::string fileName){fFileName = fileName;};
      void SetVerbose(Int_t v) {fVerbose = v;};
			std::string GetFileName() const {return fFileName;};

			std::string GetSimFileName(bool cut = false);
			std::string GetDigiFileName(bool cut = false);
			std::string GetRecoFileName(bool cut = false);
			std::string GetTrackFindingFileName(bool cut = false);
			std::string GetIdealTrackFindingFileName(bool cut = false);
			std::string GetKalmanFileName(bool cut = false);

			std::string GetSimFileName(std::string inputFileName, bool cut = false);
			std::string GetDigiFileName(std::string inputFileName, bool cut = false);
			std::string GetRecoFileName(std::string inputFileName, bool cut = false);
			std::string GetTrackFindingFileName(std::string inputFileName, bool cut = false);
			std::string GetIdealTrackFindingFileName(std::string inputFileName, bool cut = false);
			std::string GetKalmanFileName(std::string inputFileName, bool cut = false);
			std::string GetCustomFileName(std::string ext, bool cut = false);
			std::string GetCustomFileName(std::string inputFileName, std::string ext, bool cut = false);

	private :
			std::string fFileName;
			std::string fExtSim;
			std::string fExtDigi;
			std::string fExtReco;
			std::string fExtTrackF;
			std::string fExtIdealTrackF;
			std::string fExtKalman;
			std::string TruncateFileName(bool cut);
      Int_t fVerbose;
	ClassDef(PndMvdFileNameCreator, 1);
};

#endif
