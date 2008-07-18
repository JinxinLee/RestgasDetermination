#------------------------------------------------------------------#
# GfiEvtGen specifics:
# 
# Generator level filter requires that there be neither 
# 1) Lambda_c+      ->      p+ K- pi+  NOR
# 2) anti-Lambda_c- -> anti-p- K+ pi-  
#
# The Lambda1520 corrects a bug existant at time of SP4. If a Lambda(1520)
# is forced to p K, it will crash EvtGen when it is assigned a mass below
# m_p + M_K. This happens (2.82 +- 0.01)% of the time.
# To avoid the crash, some ISR .dec files give the Lambda(1520)
# a phony   nu_e nu_tau decay mode. Catch this here. 
#
# Author: Joe Izen, University of Texas at Dallas
#
#------------------------------------------------------------------#

module enable GefSelectFilter
mod talk GefSelectFilter
# catch a bad Lambda1520 or anti-Lambda1520
  BooNew BadLambda1520 = GefPdtList
  BooObjects BadLambda1520 or nu_tau anti-nu_tau
  BooNew NoBadLambda1520 = !BadLambda1520

  BooNew Lc_pKpiX = GefPdtList
  BooObjects Lc_pKpiX ancestors Lambda_c+
  BooObjects Lc_pKpiX ancestorsmode EXCLUSIVE
  BooObjects Lc_pKpiX and p+ K- pi+

  BooNew antiLc_pKpiX = GefPdtList
  BooObjects antiLc_pKpiX ancestors anti-Lambda_c-
  BooObjects antiLc_pKpiX ancestorsmode EXCLUSIVE
  BooObjects antiLc_pKpiX and anti-p- K+ pi-

  #Want to distinguish Lc -> p+ K- pi+ from  Lc -> p+ K- pi+ + Any other particle
  #A Lc->pKpi shouldn't have any of the following descendents
  # pi- pi0 K+ K*+ K0 anti-K0 K*0 f_0 rho+ rho- rho0 omega eta eta' K*+ K*- e+ e- mu+ mu- 

  BooNew Lc_pKpi_Xveto = GefPdtList
  BooObjects Lc_pKpi_Xveto ancestors Lambda_c+
  BooObjects Lc_pKpi_Xveto or pi- pi0 K+ K*+ K0 anti-K0 K*0 f_0 rho+ rho- rho0 omega eta eta' K*+ K*- e+ e- mu+ mu- 

  BooNew antiLc_pKpi_Xveto = GefPdtList
  BooObjects antiLc_pKpi_Xveto ancestors anti-Lambda_c-
  BooObjects antiLc_pKpi_Xveto or pi+ pi0 K- K*- K0 anti-K0 anti-K*0 f_0 rho+ rho- rho0 omega eta eta' K*+ K*- e+ e- mu+ mu- 

  #negate
  BooNew not_Lc_pKpi_Xveto     = ! Lc_pKpi_Xveto
  BooNew not_antiLc_pKpi_Xveto = ! antiLc_pKpi_Xveto

  BooCompose Lc_pKpi     = and     Lc_pKpiX not_Lc_pKpi_Xveto
  BooCompose antiLc_pKpi = and antiLc_pKpiX not_antiLc_pKpi_Xveto
  #negate
  BooNew not_Lc_pKpi     = !     Lc_pKpi
  BooNew not_antiLc_pKpi = ! antiLc_pKpi
 
  BooCompose one_Lc_pKpi       = and   Lc_pKpi     not_antiLc_pKpi
  BooCompose one_antiLc_pKpi   = and   not_Lc_pKpi  antiLc_pKpi

  BooCompose zero_pKpi         = and   not_Lc_pKpi not_antiLc_pKpi
  BooCompose  one_pKpi         = or    one_Lc_pKpi one_antiLc_pKpi
  BooCompose  two_pKpi         = and   Lc_pKpi     antiLc_pKpi


  BooCompose theBeforeFilter = and NoBadLambda1520 zero_pKpi
  beforeFilter set theBeforeFilter
exit














