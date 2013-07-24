#include "PndLmdResultPlotter.h"

#include <stdlib.h>
#include <utility>

const double theta_min = 4.0;
const double theta_max = 10.0;

const double log_scale = false;

int main(int argc, char* argv[]) {
	if (argc == 2) {
		PndLmdResultPlotter plotter;
		plotter.plotDPMModelParts(atof(argv[1]), std::make_pair(theta_min, theta_max), log_scale);
	}
	return 0;
}
