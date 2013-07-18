/*
 * LumiFitStructs.h
 *
 *  Created on: Jul 9, 2013
 *      Author: steve
 */

#ifndef LUMIFITSTRUCTS_H_
#define LUMIFITSTRUCTS_H_

# include "TObject.h"

namespace LumiFit {

class LmdBinaryFitOptions : public TObject {
private:
	/** This set of bits specifies all binary fit options will be used (so on-off type options)
	 * bit 0: 0 = no resolution smearing, 1 = with resolution smearing
	 * bit 1: 0 = no acceptance corr, 1 = with acceptance corr
	 * bit 2: 0 = 1d fit, 1 = 2d fit
	 * bit 3: 0 = use theta-phi data and fit function, 1 = use t (momentum transfer) data and fit function
	 * bit 4: 0 = ROOT, 1 = ROOFIT
	 **/
	unsigned long binary_options;

public:
	/** Constructor
	 * @param bit_flag_options is a integer number, which is represented a set of binary flags:
	 *  bit 0: resolution smearing
	 *  bit 1: acceptance correction
	 *  bit 2: fit dimension
	 *  bit 3: use t instead of theta
	 *  bit 4: fitter type
	 *  The highest number can therefore be 31 and the lowest 0 (higher numbers than 31 are regarded as 31).
	 */
	LmdBinaryFitOptions(unsigned long bit_flag_options) :
			binary_options(bit_flag_options) {
	}

	unsigned long getBinaryOptions() const {
		return binary_options;
	}
	/**
	 * Get method for the fit dimension (1D or 2D)
	 * @returns fit dimension (0 = 1d fit, 1 = 2d fit)
	 */
	bool getFitDimension() const {
		return binary_options & (1 << (2));
	}
	/**
	 * Get method for the fitter type (ROOT or ROOFIT)
	 * @returns fitter type (0 = ROOT, 1 = ROOFIT)
	 */
	bool getFitterType() const {
		return binary_options & (1 << (4));
	}
	/**
	 * Checks if resolution smearing is on.
	 * @returns true if smearing is on, false if off
	 */
	bool isSmearingOn() const {
		return binary_options & 1;
	}
	/**
	 * Checks if acceptance correction is on
	 * @returns true if acceptance correction will be used, false otherwise
	 */
	bool isAcceptanceCorrOn() const {
		return binary_options & (1 << (1));
	}
	/**
	 * Checks if raw fit mode will be used
	 * @returns true if t spectrum and function will be used instead of theta, false otherwise
	 */
	bool isFitRaw() const {
		return binary_options & (1 << (3));
	}

	/**
	 * Setter method for switching between theta and t
	 * @param use_raw_ specifies if raw (momentum transfer) should be used
	 */
	void setFitAsRaw(bool use_raw_) {
		if (use_raw_)
			binary_options |= 1 << 3;
		else
			binary_options &= ~(1 << 3);
	}
	/**
	 * Setter method for the fit dimension (1D or 2D)
	 * @param fit_dimension_ is the new fit dimension that will be used
	 */
	void setFitDimension(bool fit_dimension_) {
		if (fit_dimension_)
			binary_options |= 1 << 2;
		else
			binary_options &= ~(1 << 2);
	}
	/**
	 * Setter method for the fitter type (ROOT or ROOFIT)
	 * @param fitter_type_ is the new fit type that will be used
	 */
	void setFitterType(bool fitter_type_) {
		if (fitter_type_)
			binary_options |= 1 << 4;
		else
			binary_options &= ~(1 << 4);
	}
	/**
	 * Setter method for the smearing mode (0 disabled, 1 enabled)
	 * @param with_smearing_ is the new fit dimension that will be used
	 */
	void setSmearingMode(bool with_smearing_) {
		if (with_smearing_)
			binary_options |= 1;
		else
			binary_options &= ~1;
	}
	/**
	 * Setter method for the acceptance correction mode (0 disabled, 1 enabled)
	 * @param with_acceptance_corr_ is the new fit dimension that will be used
	 */
	void setAcceptanceCorrMode(bool with_acceptance_corr_) {
		if (with_acceptance_corr_)
			binary_options |= 1 << 1;
		else
			binary_options &= ~(1 << 1);
	}

	ClassDef(LmdBinaryFitOptions ,1);
};

}

#endif /* LUMIFITSTRUCTS_H_ */
