#ifndef PNDSIMRECOPARAMSREADER_H_
#define PNDSIMRECOPARAMSREADER_H_

#include "PndSimRecoParams.h"

#include <string>

class PndSimRecoParamsReader {
public:
	PndSimRecoParams readConfigFromFile(const std::string &config_url) const;

	void writeConfigToFile(const PndSimRecoParams &sim_reco_params,
			const std::string &output_file_url) const;
};

#endif /* PNDSIMRECOPARAMSREADER_H_ */
