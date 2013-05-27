/*
 * PndLmdDataBaseBase.cxx
 *
 *  Created on: Jul 5, 2012
 *      Author: steve
 */

#include "PndLmdDataBase.h"

#include "PndLmdDPMAngModel1D.h"
#include "PndLmdDPMModelParametrization.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"

ClassImp(PndLmdDataBase)

PndLmdDataBase::PndLmdDataBase() {
}

PndLmdDataBase::PndLmdDataBase(TFile *f_, int num_events_, double plab_,
		PndLmdFit::lmd_dimension th_dimension_,
		PndLmdFit::lmd_dimension phi_dimension_) :
		PndLmdDataInterface(plab_, th_dimension_, phi_dimension_) {
	f = f_;
	setLabMomentum(plab_);
	setNumEvents(num_events_);

	if (th_dimension_.range_low < 0.0)
		th_dimension.range_low = 0.0;

	makeName();

	//
	// 1d histograms
	mc_1d = new TH1D("htheta_mc", " #Theta mc truth", th_dimension.bins,
			th_dimension.range_low, th_dimension.range_high);
	mc_acc_1d = new TH1D("htheta_mc_acc", " #Theta mc truth acc",
			th_dimension.bins, th_dimension.range_low, th_dimension.range_high);
	reco_1d = new TH1D("htheta_reco", " #Theta reco", th_dimension.bins,
			th_dimension.range_low, th_dimension.range_high);

	t_mc_1d = new TH1D("ht_mc", " #t mc truth", t_dimension.bins,
			t_dimension.range_low, t_dimension.range_high);
	t_mc_acc_1d = new TH1D("ht_mc_acc", " #t mc truth acc", t_dimension.bins,
			t_dimension.range_low, t_dimension.range_high);
	t_reco_1d = new TH1D("ht_reco", " #t reco", t_dimension.bins,
			t_dimension.range_low, t_dimension.range_high);

	// 2d histograms
	mc_2d = new TH2D("htheta_vs_phi_mc", " #Theta vs #phi mc truth",
			th_dimension.bins, th_dimension.range_low, th_dimension.range_high,
			phi_dimension.bins, phi_dimension.range_low, phi_dimension.range_high);
	mc_acc_2d = new TH2D("htheta_vs_phi_mc_acc", " #Theta vs #phi mc truth acc",
			th_dimension.bins, th_dimension.range_low, th_dimension.range_high,
			phi_dimension.bins, phi_dimension.range_low, phi_dimension.range_high);
	reco_2d = new TH2D("htheta_vs_phi_reco", " #Theta vs #phi reco",
			th_dimension.bins, th_dimension.range_low, th_dimension.range_high,
			phi_dimension.bins, phi_dimension.range_low, phi_dimension.range_high);

	mc_1d->Sumw2();
	mc_acc_1d->Sumw2();
	reco_1d->Sumw2();

	t_mc_1d->Sumw2();
	t_mc_acc_1d->Sumw2();
	t_reco_1d->Sumw2();

	mc_2d->Sumw2();
	mc_acc_2d->Sumw2();
	reco_2d->Sumw2();

	reco_unbinned = NULL;
}

PndLmdDataBase::~PndLmdDataBase() {

}

bool PndLmdDataBase::operator==(const PndLmdDataBase &lmd_data_base) const {

	if (getLabMomentum() != lmd_data_base.getLabMomentum())
		return false;
	if (th_dimension.bins != lmd_data_base.th_dimension.bins)
		return false;
	if (phi_dimension.bins != lmd_data_base.phi_dimension.bins)
		return false;
	if (th_dimension.range_low != lmd_data_base.th_dimension.range_low)
		return false;
	if (th_dimension.range_high != lmd_data_base.th_dimension.range_high)
		return false;
	if (phi_dimension.range_low != lmd_data_base.phi_dimension.range_low)
		return false;
	if (phi_dimension.range_high != lmd_data_base.phi_dimension.range_high)
		return false;

	return true;
}

bool PndLmdDataBase::operator!=(const PndLmdDataBase &lmd_data_base) const {
	return !(*this == lmd_data_base);
}

void PndLmdDataBase::makeName() {
	char cname[100];
	sprintf(cname, "%i_%i_%f_%f_%i_%f_%f", getNumEvents(), th_dimension.bins,
			th_dimension.range_low, th_dimension.range_high, phi_dimension.bins,
			phi_dimension.range_low, phi_dimension.range_high);

	getName() += cname;
}

void PndLmdDataBase::makeDir() {
	TDirectory *dir = f->GetDirectory(getName());
	if (dir) {
		dir->cd();
	} else {
		f->cd();
		f->mkdir(getName());
		f->cd(getName());
	}
}

void PndLmdDataBase::saveToRootFile() {
}

void PndLmdDataBase::setUnbinnedRecoData(TTree* reco_tree) {
	reco_unbinned = reco_tree;
}
TTree* PndLmdDataBase::getUnbinnedRecoData() const {
	return reco_unbinned;
}

void PndLmdDataBase::fillHistograms(
		std::vector<std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> > &event_data) {
	/*std::cout << "---- MC info ----" << std::endl;
	 std::cout << "t: " << data.first.t << std::endl;
	 std::cout << "theta: " << data.first.theta << std::endl;
	 std::cout << "phi: " << data.first.phi << std::endl;
	 std::cout << "reconstructed: " << data.first.reconstructed << std::endl;
	 std::cout << "-----------------" << std::endl;
	 std::cout << "--- Reco info ---" << std::endl;
	 std::cout << "t: " << data.second.t << std::endl;
	 std::cout << "theta: " << data.second.theta << std::endl;
	 std::cout << "phi: " << data.second.phi << std::endl;
	 std::cout << "reconstructed: " << data.second.reconstructed << std::endl;
	 std::cout << "-----------------" << std::endl;*/

	for (unsigned int i = 0; i < event_data.size(); i++) {
		std::pair<PndLmdFit::lmd_values, PndLmdFit::lmd_values> data = event_data[i];
		if (data.first.reconstructed) {
			t_mc_1d->Fill(data.first.t);

			mc_1d->Fill(1000. * data.first.theta);
			mc_2d->Fill(1000. * data.first.theta, data.first.phi);

			if (data.second.reconstructed) {
				t_mc_acc_1d->Fill(data.first.t);

				mc_acc_1d->Fill(1000. * data.first.theta);
				mc_acc_2d->Fill(1000. * data.first.theta, data.first.phi);
			}
		}
		if (data.second.reconstructed) {
			t_reco_1d->Fill(data.second.t);

			reco_1d->Fill(1000. * data.second.theta);
			reco_2d->Fill(1000. * data.second.theta, data.second.phi);
		}
	}
}
