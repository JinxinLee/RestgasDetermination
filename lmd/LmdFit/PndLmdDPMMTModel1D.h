/*
 * PndLmdDPMMTModel1D.h
 *
 *  Created on: Dec 19, 2012
 *      Author: steve
 */

#ifndef PNDLMDDPMMTMODEL1D_H_
#define PNDLMDDPMMTMODEL1D_H_

#include "Model1D.h"

#include <memory>

/**
 * \brief This class is the base of lumi models such as #PndLmdLumiModelROOT and #PndLmdLumiModelRooFit which can be fitted to #PndLmdData objects.
 *
 * The reason for a model base class is a common model description in all different fit implementations (ROOT and RooFit etc).
 * The DPM model description can be found in the #evaluate() function.
 *
 * Also this class contains all the necessary handles for the only degree of freedom, the acceptance description (see #PndLmdAcceptance),
 * which is required for applying an acceptance correction. The acceptance description can of course differ depending on its binning, data size, etc.
 * Hence usually more than one model instance can be active.
 */

class PndLmdDPMMTModel1D: public Model1D {
protected:
  // constants
  double pi;
  double hbarc2;
  double alpha;
  double M;

  /**
   * References to the values of the parameters used in the DPM description
   * of the cross section that are constants or are actually parameters in
   * the fit
   */
  //strictly fixed parameters
  std::shared_ptr<ModelPar> p_lab;
  std::shared_ptr<ModelPar> E_lab;
  std::shared_ptr<ModelPar> S;
  std::shared_ptr<ModelPar> pcm2;
  std::shared_ptr<ModelPar> gamma;
  std::shared_ptr<ModelPar> beta;
  std::shared_ptr<ModelPar> beta_lab_cms;
  //possibly free fit parameters
  std::shared_ptr<ModelPar> sigma_tot;
  std::shared_ptr<ModelPar> b;
  std::shared_ptr<ModelPar> rho;
  std::shared_ptr<ModelPar> A1;
  std::shared_ptr<ModelPar> A2;
  std::shared_ptr<ModelPar> A3;
  std::shared_ptr<ModelPar> T1;
  std::shared_ptr<ModelPar> T2;

  /**
   *  initializes the above parameters of the DPM cross section that are absolutely fixed
   * (so not dependent on the beam momentum for example)
   */
  void init();

  void updateDomainFromPars(double *par);

public:
  /**
   * In the constructor that creates a fully defined pure signal cross section model of LMD
   * @param type specifies the model type (see #lmd_signal_model_type)
   */
  PndLmdDPMMTModel1D();

  ~PndLmdDPMMTModel1D();

  virtual void initModelParameters();

  /** @returns parameter b of DPM model cross section */
  double getB() const;
  /** @returns parameter rho of DPM model cross section */
  double getRho() const;
  /** @returns parameter sigma total of DPM model cross section */
  double getSigmaTotal() const;

  double getDelta(const double t) const;

  double getProtonDipoleFormFactor(const double t) const;

  double getRawCoulombPart(double *x) const;

  double getRawInterferencePart(double *x) const;

  double getRawHadronicPart(double *x) const;

  virtual double eval(double *x) const;

  virtual void updateDomain();
};

#endif /* PNDLMDDPMMTMODEL1D_H_ */
