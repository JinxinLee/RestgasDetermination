/*
 * TransformHisto.C
 *
 *  Created on: Feb 25, 2009
 *      Author: stockman
 */

TH1D TransformHisto(TH2* h2, double min, double max)
{
	TH1D result("h1","h1", 1000, min, max);
	int nbins = h2->GetNbinsX() * h2->GetNbinsY();
	for (int i = 0; i < nbins; i++){
		//std::cout << h2->GetBinContent(i) << std::endl;
		result->Fill(h2->GetBinContent(i));
		if (i == 10)
			cout << h2->GetBinContent(i) << endl;
	}
	return result;
}
