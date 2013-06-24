
c ###### 
c  standard routine !!!
c ######
c$$$      FUNCTION PHOCOR(MPASQR,MCHREN,ME)
c$$$C.----------------------------------------------------------------------
c$$$C.
c$$$C.    PHOTOS:   PHOton radiation in decays CORrection weight from
c$$$C.              matrix elements
c$$$C.
c$$$C.    Purpose:  Calculate  photon  angle.  The reshaping functions  will
c$$$C.              have  to  depend  on the spin S of the charged particle.
c$$$C.              We define:  ME = 2 * S + 1 !
c$$$C.
c$$$C.    Input Parameters:  MPASQR:  Parent mass squared,
c$$$C.                       MCHREN:  Renormalised mass of charged system,
c$$$C.                       ME:      2 * spin + 1 determines matrix element
c$$$C.
c$$$C.    Output Parameter:  Function value.
c$$$C.
c$$$C.    Author(s):  Z. Was, B. van Eijk             Created at:  26/11/89
c$$$C.                                                Last Update: 21/03/93
c$$$C.
c$$$C.----------------------------------------------------------------------
c$$$      IMPLICIT NONE
c$$$      DOUBLE PRECISION MPASQR,MCHREN,BETA,XX,YY,DATA
c$$$      INTEGER ME
c$$$      REAL*8 PHOCOR,PHOFAC,WT1,WT2,WT3
c$$$      DOUBLE PRECISION MCHSQR,MNESQR
c$$$      REAL*8 PNEUTR
c$$$      COMMON/PHOMOM/MCHSQR,MNESQR,PNEUTR(5)
c$$$      DOUBLE PRECISION COSTHG,SINTHG
c$$$      REAL*8 XPHMAX,XPHOTO
c$$$      COMMON/PHOPHS/XPHMAX,XPHOTO,COSTHG,SINTHG
c$$$      INTEGER IREP
c$$$      REAL*8 PROBH,CORWT,XF
c$$$      COMMON/PHOPRO/PROBH,CORWT,XF,IREP
c$$$C--
c$$$C--   Shaping (modified by ZW)...
c$$$      XX=4.D0*MCHSQR/MPASQR*(1.D0-XPHOTO)/(1.D0-XPHOTO+(MCHSQR-MNESQR)/
c$$$     &MPASQR)**2
c$$$      IF (ME.EQ.1) THEN
c$$$        YY=1.D0
c$$$        WT3=(1.D0-XPHOTO/XPHMAX)/((1.D0+(1.D0-XPHOTO/XPHMAX)**2)/2.D0)
c$$$      ELSEIF (ME.EQ.2) THEN
c$$$        YY=0.5D0*(1.D0-XPHOTO/XPHMAX+1.D0/(1.D0-XPHOTO/XPHMAX))
c$$$        WT3=1.D0
c$$$      ELSEIF ((ME.EQ.3).OR.(ME.EQ.4).OR.(ME.EQ.5)) THEN
c$$$        YY=1.D0
c$$$        WT3=(1.D0+(1.D0-XPHOTO/XPHMAX)**2-(XPHOTO/XPHMAX)**3)/
c$$$     &  (1.D0+(1.D0-XPHOTO/XPHMAX)** 2)
c$$$      ELSE
c$$$        DATA=(ME-1.D0)/2.D0
c$$$        CALL PHOERR(6,'PHOCOR',DATA)
c$$$        YY=1.D0
c$$$        WT3=1.D0
c$$$      ENDIF
c$$$      BETA=SQRT(1.D0-XX)
c$$$      WT1=(1.D0-COSTHG*SQRT(1.D0-MCHREN))/(1.D0-COSTHG*BETA)
c$$$      WT2=(1.D0-XX/YY/(1.D0-BETA**2*COSTHG**2))*(1.D0+COSTHG*BETA)/2.D0
c$$$      WT2=WT2*PHOFAC(1)
c$$$      PHOCOR=WT1*WT2*WT3
c$$$      CORWT=PHOCOR
c$$$      IF (PHOCOR.GT.1.D0) THEN
c$$$        DATA=PHOCOR
c$$$        CALL PHOERR(3,'PHOCOR',DATA)
c$$$      ENDIF
c$$$      RETURN
c$$$      END
c$$$


      FUNCTION PHOCORN(MPASQR,MCHREN,ME)
c ###### 
c  replace with, 
c ######

C.----------------------------------------------------------------------
C.
C.    PHOTOS:   PHOton radiation in decays CORrection weight from
C.              matrix elements This version for spin 1/2 is verified for
C.              W decay only
C.    Purpose:  Calculate  photon  angle.  The reshaping functions  will
C.              have  to  depend  on the spin S of the charged particle.
C.              We define:  ME = 2 * S + 1 !
C.
C.    Input Parameters:  MPASQR:  Parent mass squared,
C.                       MCHREN:  Renormalised mass of charged system,
C.                       ME:      2 * spin + 1 determines matrix element
C.
C.    Output Parameter:  Function value.
C.
C.    Author(s):  Z. Was, B. van Eijk, G. Nanava  Created at:  26/11/89
C.                                                Last Update: 01/11/12
C.
C.----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION MPASQR,MCHREN,BETA,BETA0,BETA1,XX,YY,DATA
      INTEGER ME
      REAL*8 PHOCOR,PHOFAC,WT1,WT2,WT3,PHOTRI,S1,PHOCORN
      DOUBLE PRECISION MCHSQR,MNESQR
      REAL*8 PNEUTR
      COMMON/PHOMOM/MCHSQR,MNESQR,PNEUTR(5)
      DOUBLE PRECISION COSTHG,SINTHG,phocorWT3,phocorWT2,phocorWT1
      REAL*8 XPHMAX,XPHOTO
      COMMON/PHOPHS/XPHMAX,XPHOTO,COSTHG,SINTHG
      common/phocorWT/phocorWT3,phocorWT2,phocorWT1
      INTEGER IREP
      REAL*8 PROBH,CORWT,XF
      COMMON/PHOPRO/PROBH,CORWT,XF,IREP

C--
C--   Shaping (modified by ZW)...
      XX=4.D0*MCHSQR/MPASQR*(1.D0-XPHOTO)/(1.D0-XPHOTO+(MCHSQR-MNESQR)/
     &MPASQR)**2
      IF (ME.EQ.1) THEN
        S1=MPASQR  * (1.D0-XPHOTO)
        BETA0=2*PHOTRI(1D0,dsqrt(MCHSQR/MPASQR),dsqrt(MNESQR/MPASQR))
        BETA1=2*PHOTRI(1D0,dsqrt(MCHSQR/S1),dsqrt(MNESQR/S1))
        WT1= (1.D0-COSTHG*SQRT(1.D0-MCHREN))
     $      /((1D0+(1D0-XPHOTO/XPHMAX)**2)/2.D0)*XPHOTO          ! de-presampler
     $     
        WT2= beta1/beta0*XPHOTO                                  !phase space jacobians
        WT3=  beta1**2* (1D0-COSTHG**2) * (1D0-XPHOTO)/XPHOTO**2 ! matrix element
     $    /(1D0 +MCHSQR/S1-MNESQR/S1-BETA1*COSTHG)**2/2D0 
      ELSEIF (ME.EQ.2) THEN
        S1=MPASQR  * (1.D0-XPHOTO)
        BETA0=2*PHOTRI(1D0,dsqrt(MCHSQR/MPASQR),dsqrt(MNESQR/MPASQR))
        BETA1=2*PHOTRI(1D0,dsqrt(MCHSQR/S1),dsqrt(MNESQR/S1))
        WT1= (1.D0-COSTHG*SQRT(1.D0-MCHREN))
     $      /((1D0+(1D0-XPHOTO/XPHMAX)**2)/2.D0)*XPHOTO          ! de-presampler
         
        WT2= beta1/beta0*XPHOTO                                  ! phase space jacobians

        WT3= beta1**2* (1D0-COSTHG**2) * (1D0-XPHOTO)/XPHOTO**2  ! matrix element
     $       /(1D0 +MCHSQR/S1-MNESQR/S1-BETA1*COSTHG)**2/2D0 
        WT3=WT3*(1-xphoto/xphmax+0.5*(xphoto/xphmax)**2)/(1-xphoto/xphmax)
c       print*,"WT3=",wt3
        phocorWT3=WT3
        phocorWT2=WT2
        phocorWT1=WT1

c       YY=0.5D0*(1.D0-XPHOTO/XPHMAX+1.D0/(1.D0-XPHOTO/XPHMAX))
c       BETA=SQRT(1.D0-XX)
c       WT1=(1.D0-COSTHG*SQRT(1.D0-MCHREN))/(1.D0-COSTHG*BETA)
c       WT2=(1.D0-XX/YY/(1.D0-BETA**2*COSTHG**2))*(1.D0+COSTHG*BETA)/2.D0
c       WT3=1.D0
      ELSEIF ((ME.EQ.3).OR.(ME.EQ.4).OR.(ME.EQ.5)) THEN
        YY=1.D0
        BETA=SQRT(1.D0-XX)
        WT1=(1.D0-COSTHG*SQRT(1.D0-MCHREN))/(1.D0-COSTHG*BETA)
        WT2=(1.D0-XX/YY/(1.D0-BETA**2*COSTHG**2))*(1.D0+COSTHG*BETA)/2.D0
        WT3=(1.D0+(1.D0-XPHOTO/XPHMAX)**2-(XPHOTO/XPHMAX)**3)/
     &  (1.D0+(1.D0-XPHOTO/XPHMAX)** 2)
      ELSE
        DATA=(ME-1.D0)/2.D0
        CALL PHOERR(6,'PHOCOR',DATA)
        YY=1.D0
        BETA=SQRT(1.D0-XX)
        WT1=(1.D0-COSTHG*SQRT(1.D0-MCHREN))/(1.D0-COSTHG*BETA)
        WT2=(1.D0-XX/YY/(1.D0-BETA**2*COSTHG**2))*(1.D0+COSTHG*BETA)/2.D0
        WT3=1.D0
      ENDIF
      WT2=WT2*PHOFAC(1)
      PHOCOR=WT1*WT2*WT3
      PHOCORN=PHOCOR
      CORWT=PHOCOR
      IF (PHOCOR.GT.1.D0) THEN
        DATA=PHOCOR
        CALL PHOERR(3,'PHOCOR',DATA)
      ENDIF
      RETURN
      END
      SUBROUTINE PHOBWnlo(WT)
C.----------------------------------------------------------------------
C.
C.    PHOTOS:   PHOtos Boson W correction weight
C.
C.    Purpose:  calculates correction weight due to amplitudes of 
C.              emission from W boson. It is ecact, but not verified
C.              for exponentiation yet.
C.              
C.              
C.              
C.
C.    Input Parameters:  Common /PHOEVT/, with photon added.
C.                       wt  to be corrected
C.                       
C.                       
C.                       
C.    Output Parameters: wt
C.
C.    Author(s):  G. Nanava, Z. Was               Created at:  13/03/03
C.                                                Last Update: 01/11/12
C.
C.----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION WT
      INTEGER NMXPHO
      PARAMETER (NMXPHO=10000)
      INTEGER IDPHO,ISTPHO,JDAPHO,JMOPHO,NEVPHO,NPHO
      REAL*8 PPHO,VPHO
      INTEGER PHLUN
      COMMON/PHOLUN/PHLUN
      REAL*8 ALPHA,XPHCUT
      COMMON/PHOCOP/ALPHA,XPHCUT
      COMMON/PHOEVT/NEVPHO,NPHO,ISTPHO(NMXPHO),IDPHO(NMXPHO),JMOPHO(2,NMXPHO),
     &              JDAPHO(2,NMXPHO),PPHO(5,NMXPHO),VPHO(4,NMXPHO)
      INTEGER I,JJ,II,I3,I4,IJ
      DOUBLE PRECISION EMU,MCHREN,BETA,COSTHG,MPASQR,XPH,
     &                 PW(0:3),PMU(0:3),PPHOT(0:3),PNE(0:3),
     &                 B_PW(0:3),B_PNE(0:3),B_PMU(0:3),AMPSQR,SANC_MC_INIT
      DOUBLE PRECISION WDecayAmplitudeSqrKS_1ph,WDecayEikonalSqrKS_1ph,
     &                 WDecayBornAmpSqrKS_1ph,EIKONALFACTOR
      EXTERNAL WDecayAmplitudeSqrKS_1ph,WDecayEikonalSqrKS_1ph,WDecayBornAmpSqrKS_1ph

      INTEGER  d_h_nmxhep         ! maximum number of particles
      PARAMETER (d_h_NMXHEP=10000)
      REAL*8  d_h_phep,  d_h_vhep ! to be real*4/ *8  depending on host
      INTEGER d_h_nevhep,d_h_nhep,d_h_isthep,d_h_idhep,d_h_jmohep,
     $        d_h_jdahep
      COMMON /ph_hepevt/
     $      d_h_nevhep,               ! serial number
     $      d_h_nhep,                 ! number of particles
     $      d_h_isthep(d_h_nmxhep),   ! status code
     $      d_h_idhep(d_h_nmxhep),    ! particle ident KF
     $      d_h_jmohep(2,d_h_nmxhep), ! parent particles
     $      d_h_jdahep(2,d_h_nmxhep), ! childreen particles
     $      d_h_phep(5,d_h_nmxhep),   ! four-momentum, mass [GeV]
     $      d_h_vhep(4,d_h_nmxhep)    ! vertex [mm]

      SAVE SANC_MC_INIT
      DATA SANC_MC_INIT /-123456789D0/
      INTEGER           mcLUN
      DOUBLE PRECISION  spV(0:3),bet(0:3)
      DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
      COMMON /Kleiss_Stirling/spV,bet
      COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN        
!      write(*,*) 'IDPHOs=',IDPHO(1),IDPHO(2),IDPHO(3),IDPHO(4),IDPHO(5)
!      write(*,*) 'IDPHOs=',JDAPHO(1,1),npho
!      write(*,*) 'd_h_IDPHOs=',d_h_IDhep(1),d_h_IDhep(2),d_h_IDhep(3),d_h_IDhep(4),d_h_IDhep(5)

C--
        IF (ABS(IDPHO(1)).EQ.24.AND.
     $     ABS(IDPHO(JDAPHO(1,1)  )).GE.11.AND.
     $     ABS(IDPHO(JDAPHO(1,1)  )).LE.16.AND.
     $     ABS(IDPHO(JDAPHO(1,1)+1)).GE.11.AND.
     $     ABS(IDPHO(JDAPHO(1,1)+1)).LE.16     ) THEN

           IF(
     $      ABS(IDPHO(JDAPHO(1,1)  )).EQ.11.OR.
     $      ABS(IDPHO(JDAPHO(1,1)  )).EQ.13.OR.
     $      ABS(IDPHO(JDAPHO(1,1)  )).EQ.15    ) THEN 
              I=JDAPHO(1,1)
           ELSE
              I=JDAPHO(1,1)+1
           ENDIF
c..        muon energy   
           EMU=PPHO(4,I)
c..        muon mass square
           MCHREN=ABS(PPHO(4,I)**2-PPHO(3,I)**2
     $               -PPHO(2,I)**2-PPHO(1,I)**2)
           BETA=SQRT(1- MCHREN/ PPHO(4,I)**2)
           COSTHG=(PPHO(3,I)*PPHO(3,NPHO)+PPHO(2,I)*PPHO(2,NPHO)
     $                                   +PPHO(1,I)*PPHO(1,NPHO))/
     $            SQRT(PPHO(3,I)**2+PPHO(2,I)**2+PPHO(1,I)**2)   /
     $            SQRT(PPHO(3,NPHO)**2+PPHO(2,NPHO)**2+PPHO(1,NPHO)**2)
           MPASQR=PPHO(4,1)**2
           XPH=PPHO(4,NPHO)

c...       Initialization of the W->l\nu\gamma 
c...       decay Matrix Element parameters 
           IF (SANC_MC_INIT.EQ.-123456789D0) THEN
              SANC_MC_INIT=1D0

              PI=4*datan(1d0)
              QF1=0d0                           ! neutrino charge
              MF1=1d-10                         ! newutrino mass
              VF=1d0                            ! V&A couplings
              AF=1d0
              alphaI=1d0/ALPHA
              CW=0.881731727d0                  ! Weak Weinberg angle
              SW=0.471751166d0
           

c...          An auxilary K&S vectors
              bet(0)= 1d0
              bet(1)= 0.0722794881816159d0
              bet(2)=-0.994200045099866d0
              bet(3)= 0.0796363353729248d0 

              spV(0)= 0d0 
              spV(1)= 7.22794881816159d-002
              spV(2)=-0.994200045099866d0     
              spV(3)= 7.96363353729248d-002

              mcLUN = PHLUN
           ENDIF 


           MB=PPHO(4,1)                      ! W boson mass
           MF2=dsqrt(MCHREN)                 ! muon mass

           DO IJ=1,d_h_nhep
            IF(ABS(d_h_idhep(IJ)).EQ.24) I3=IJ ! position of W 
           ENDDO
           IF(
     $      ABS(d_h_idhep(d_h_jdahep(1,I3)  )).EQ.11.OR.
     $      ABS(d_h_idhep(d_h_jdahep(1,I3)  )).EQ.13.OR.
     $      ABS(d_h_idhep(d_h_jdahep(1,I3)  )).EQ.15    ) THEN 
              I4=d_h_jdahep(1,I3)              ! position of lepton
           ELSE
              I4=d_h_jdahep(1,I3)+1            ! position of lepton
           ENDIF


              IF (d_h_idhep(I3).EQ.-24) QB=-1D0  ! W boson charge
              IF (d_h_idhep(I3).EQ.+24) QB=+1D0    
              IF (d_h_idhep(I4).GT.0d0) QF2=-1D0 ! lepton charge
              IF (d_h_idhep(I4).LT.0d0) QF2=+1D0


c...          Particle momenta before foton radiation; effective Born level
              DO JJ=1,4
                B_PW(mod(JJ,4))=d_h_phep(JJ,I3)  ! W boson
                B_PNE(mod(JJ,4))=d_h_phep(JJ,I3)-d_h_phep(JJ,I4) ! neutrino
                B_PMU(mod(JJ,4))=d_h_phep(JJ,I4) ! muon
              ENDDO

c..        Particle monenta after photon radiation
           DO JJ=1,4
             PW(mod(JJ,4))=PPHO(JJ,1)
             PMU(mod(JJ,4))=PPHO(JJ,I)
             PPHOT(mod(JJ,4))=PPHO(JJ,NPHO)
             PNE(mod(JJ,4))=PPHO(JJ,1)-PPHO(JJ,I)-PPHO(JJ,NPHO)
           ENDDO
C two options of calculating neutrino (spectator) mass
           MF1=SQRT(ABS(B_PNE(0)**2-B_PNE(1)**2-B_PNE(2)**2-B_PNE(3)**2))
           MF1=SQRT(ABS(  PNE(0)**2-  PNE(1)**2-  PNE(2)**2-  PNE(3)**2))
c..        Exact amplitude square      
           AMPSQR=WDecayAmplitudeSqrKS_1ph(PW,PNE,PMU,PPHOT)

           EIKONALFACTOR=WDecayBornAmpSqrKS_1ph(B_PW,B_PNE,B_PMU)
     &                  *WDecayEikonalSqrKS_1ph(PW,PNE,PMU,PPHOT)
      
c..        New weight
!           write(*,*) 'B_pne=',B_PNE
!           write(*,*) 'B_PMU=',B_PMU
!           write(*,*) 'bornie=',WDecayBornAmpSqrKS_1ph(B_PW,B_PNE,B_PMU)

!           write(*,*) ' '
!           write(*,*) '  pne=',pne
!           write(*,*) '  pmu=',pmu
!           write(*,*) 'pphot=',pphot
!           write(*,*) ' '
!           write(*,*) '  b_pw=',B_PW
!           write(*,*) '  b_pne=',B_PNE
!           write(*,*) 'b_pmu=',B_PMU
 
 !          write(*,*) 'cori=',AMPSQR/EIKONALFACTOR,AMPSQR,EIKONALFACTOR
           WT=WT*AMPSQR/EIKONALFACTOR
c           
c          WT=WT*(1-8*EMU*XPH*(1-COSTHG*BETA)*     
c     $           (MCHREN+2*XPH*SQRT(MPASQR))/
c     $            MPASQR**2/(1-MCHREN/MPASQR)/(4-MCHREN/MPASQR)) 
        ENDIF
!      write(*,*)   'AMPSQR/EIKONALFACTOR= ',   AMPSQR/EIKONALFACTOR
      END

c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$
c$$$   WffGammaME.f  follows now: 
c$$$
c$$$
c$$$
c$$$

      FUNCTION WDecayAmplitudeSqrKS_1ph(p3,p1,p2,k)
c========================================================
c        The squared amplitude for W decay              =
c        into fermion pair and one photon               =
c  INPUT :                                              =
c                                                       =
c  OUTPUT:                                              =
c========================================================       
         IMPLICIT NONE    
         INTEGER          l1,l2,l3,s,flag
         DOUBLE PRECISION k(0:3),p1(0:3),p2(0:3),p3(0:3)
         DOUBLE PRECISION spinSumAvrg,WDecayAmplitudeSqrKS_1ph
         DOUBLE COMPLEX   WDecayAmplitudeKS_1ph,wDecAmp
         EXTERNAL         WDecayAmplitudeKS_1ph 
         INTEGER           mcLUN
         DOUBLE PRECISION  spV(0:3),bet(0:3)
         DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
         COMMON /Kleiss_Stirling/spV,bet
         COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN        

         spinSumAvrg = 0d0
         DO l3 = -1,1        
           DO l1 = -1,1,2  
             DO l2 = -1,1,2  
               DO s = -1,1,2  
                 wDecAmp     = WDecayAmplitudeKS_1ph(p3,l3,p1,l1,p2,l2,k,s)
                 spinSumAvrg = spinSumAvrg + wDecAmp*CONJG(wDecAmp) 
               ENDDO
             ENDDO
           ENDDO
         ENDDO        
         WDecayAmplitudeSqrKS_1ph = spinSumAvrg
      
      RETURN
      END    

      FUNCTION WDecayBornAmpSqrKS_1ph(p3,p1,p2)
c========================================================
c        The squared eikonal factor for W decay         =
c        into fermion pair and one photon               =
c  INPUT :                                              =
c                                                       =
c  OUTPUT:                                              =
c========================================================       
         IMPLICIT NONE    
         INTEGER          l1,l2,l3
         DOUBLE PRECISION k(0:3),p1(0:3),p2(0:3),p3(0:3)
         DOUBLE PRECISION spinSumAvrg,WDecayBornAmpSqrKS_1ph
         DOUBLE COMPLEX   WDecayBornAmpKS_1ph,wDecAmp
         EXTERNAL         WDecayBornAmpKS_1ph
         INTEGER           mcLUN
         DOUBLE PRECISION  spV(0:3),bet(0:3)
         DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
         COMMON /Kleiss_Stirling/spV,bet
         COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN        

         spinSumAvrg = 0d0
         DO l3 = -1,1      
           DO l1 = -1,1,2  
             DO l2 = -1,1,2  
                wDecAmp     = WDecayBornAmpKS_1ph(p3,l3,p1,l1,p2,l2)
                spinSumAvrg = spinSumAvrg + wDecAmp*CONJG(wDecAmp) 
             ENDDO
           ENDDO
         ENDDO        
         WDecayBornAmpSqrKS_1ph = spinSumAvrg

       RETURN
       END   

      FUNCTION WDecayEikonalSqrKS_1ph(p3,p1,p2,k)
c========================================================
c        The squared eikonal factor for W decay         =
c        into fermion pair and one photon               =
c  INPUT :                                              =
c                                                       =
c  OUTPUT:                                              =
c========================================================       
         IMPLICIT NONE    
         INTEGER          s
         DOUBLE PRECISION k(0:3),p1(0:3),p2(0:3),p3(0:3)
         DOUBLE PRECISION spinSumAvrg,WDecayEikonalSqrKS_1ph
         DOUBLE COMPLEX   WDecayEikonalKS_1ph,wDecAmp
         EXTERNAL         WDecayEikonalKS_1ph
         INTEGER           mcLUN
         DOUBLE PRECISION  spV(0:3),bet(0:3)
         DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
         COMMON /Kleiss_Stirling/spV,bet
         COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN         

         spinSumAvrg = 0d0
         DO s = -1,1,2  
           wDecAmp     = WDecayEikonalKS_1ph(p3,p1,p2,k,s)
           spinSumAvrg = spinSumAvrg + wDecAmp*CONJG(wDecAmp) 
         ENDDO
         WDecayEikonalSqrKS_1ph = spinSumAvrg

       RETURN
       END   
              

       FUNCTION WDecayEikonalKS_1ph(p3,p1,p2,k,s)
c======================================================================                 
c     
c Eikonal factor of decay W->l_1+l_2+\gamma in terms of K&S objects !
c 
c   EikFactor = q1*eps.p1/k.p1 + q2*eps.p2/k.p2 - q3*eps.p3/k.p3
c
c   indices 1,2 are for charged decay products
c   index 3 is for W
c   
c   q - charge
c    
c======================================================================
         IMPLICIT NONE    
         INTEGER          s
         DOUBLE PRECISION k(0:3),p1(0:3),p2(0:3),p3(0:3)
         DOUBLE PRECISION scalProd1,scalProd2,scalProd3
         DOUBLE COMPLEX   WDecayEikonalKS_1ph,BsFactor,BSoft1,BSoft2  
         EXTERNAL         BsFactor
         INTEGER           mcLUN
         DOUBLE PRECISION  spV(0:3),bet(0:3)
         DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
         COMMON /Kleiss_Stirling/spV,bet
         COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN    

         scalProd1 = p1(0)*k(0)-p1(1)*k(1)-p1(2)*k(2)-p1(3)*k(3)
         scalProd2 = p2(0)*k(0)-p2(1)*k(1)-p2(2)*k(2)-p2(3)*k(3)
         scalProd3 = p3(0)*k(0)-p3(1)*k(1)-p3(2)*k(2)-p3(3)*k(3)

         BSoft1  = BsFactor(s,k,p1,mf1)
         BSoft2  = BsFactor(s,k,p2,mf2)
 
        WDecayEikonalKS_1ph = 
     &       sqrt(pi/alphaI)*(-(qf1/scalProd1+qb/scalProd3)*BSoft1   
     &                        +(qf2/scalProd2-qb/scalProd3)*BSoft2)

       RETURN
       END

       FUNCTION WDecayBornAmpKS_1ph(p3,l3,p1,l1,p2,l2)
c======================================================================                 
c                                                                     =
c                            p1,mf1,l1                                =
c                           /                                         =
c                         \/_                                         = 
c                         /                                           =
c        p3,mb,l3        /                                            =
c              \/\/\/\/\/\      ------> g_(mu,1)*(1+g5_(1))           =
c                         \                                           =
c                         _\/                                         = 
c                           \                                         =
c                            p2,mf2,l2                                =
c INPUT : p1,m1,l1; p2,m2,l2; p3,m3,l3  -- momenta,mass and helicity  =
c                                                                     =
c OUTPUT: value of functions            -- decay amplitude            =
c                                                                     =
c======================================================================
         IMPLICIT NONE    
         INTEGER          l1,l2,l3
         DOUBLE PRECISION p1(0:3),p2(0:3),p3(0:3),coeff
         DOUBLE COMPLEX   WDecayBornAmpKS_1ph,TrMatrix_mass
         EXTERNAL         TrMatrix_mass
         INTEGER           mcLUN
         DOUBLE PRECISION  spV(0:3),bet(0:3)
         DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
         COMMON /Kleiss_Stirling/spV,bet
         COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN 

         coeff = Dsqrt(pi/alphaI/2d0)/sw      ! vertex: g/2/sqrt(2)

         WDecayBornAmpKS_1ph = coeff*TrMatrix_mass(p2,mf2,l2,p3,mb,l3,p1,-mf1,-l1)
         

       RETURN
       END

       FUNCTION WDecayAmplitudeKS_1ph(p3,l3,p1,l1,p2,l2,k,s)
c======================================================================                 
c                k,0,l                                                =
c                   \        p1,mf1,l1                                =
c                   /       /                                         =
c                   \     \/_                                         = 
c                   /     /                                           =
c        p3,mb,l3   \    /                                            =
c              \/\/\/\/\/\      ------> g_(mu,1)*(1+g5_(1))           =
c                         \                                           =
c                         _\/                                         = 
c                           \                                         =
c                            p2,mf2,l2                                =
c           { + }                                                     =
c                            p1,mf1,l1                                =
c                           /                                         = 
c                         \/_~~~~~~~ k,0,s                            = 
c                         /                                           =
c        p3,mb,l3        /                                            =
c              \/\/\/\/\/\      ------> g_(mu,1)*(1+g5_(1))           =
c                         \                                           =
c                         _\/                                         = 
c                           \                                         =
c                            p2,mf2,l2                                =
c           { + }                                                     =
c                            p1,mf1,l1                                =
c                           /                                         =
c                         \/_                                         =
c                         /                                           =
c        p3,mb,l3        /                                            =
c              \/\/\/\/\/\      ------> g_(mu,1)*(1+g5_(1))           =
c                         \                                           =
c                         _\/ ~~~~~~~ k,0,s                           =
c                           \                                         =
c                             p2,mf2,l2                               =
c                                                                     =
c                   all momentas, exept k are incoming !!!            =
c                                                                     =
c This function culculates The W-ff\gamma decay amplitude into permion=
c pair and one photon using Kleisse&Stirling method for helicity      =
c amplitudes, which includes three above feynman diagramms..          = 
c                                                                     =
c INPUT : p1,m1,l1; p2,m2,l2; p3,m3,l3  -- momenta,mass and helicity  =
c                                                                     =
c OUTPUT: value of functions            -- decay amplitude            =
c                                                                     =
c======================================================================
         IMPLICIT NONE    
         INTEGER          s,l1,l2,l3,la1
         DOUBLE PRECISION k(0:3),p1(0:3),p2(0:3),p3(0:3)
         DOUBLE PRECISION scalProd1,scalProd2,scalProd3,coeff,theta3,ph3
         DOUBLE COMPLEX   bornAmp,TrMx1,TrMx2,WDecayAmplitudeKS_1ph,eikAmp
         DOUBLE COMPLEX   BsFactor,TrMatrix_zero,TrMatrix_mass,BSoft1,BSoft2  
         EXTERNAL         BsFactor,TrMatrix_zero,TrMatrix_mass
         INTEGER           mcLUN
         DOUBLE PRECISION  spV(0:3),bet(0:3)
         DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
         COMMON /Kleiss_Stirling/spV,bet
         COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN

         coeff = Dsqrt(2d0)*pi/sw/alphaI      ! vertex: g/2/sqrt(2) * e

         scalProd1 = p1(0)*k(0)-p1(1)*k(1)-p1(2)*k(2)-p1(3)*k(3)
         scalProd2 = p2(0)*k(0)-p2(1)*k(1)-p2(2)*k(2)-p2(3)*k(3)
         scalProd3 = p3(0)*k(0)-p3(1)*k(1)-p3(2)*k(2)-p3(3)*k(3)

         BSoft1  = BsFactor(s,k,p1,mf1)
         BSoft2  = BsFactor(s,k,p2,mf2)
         bornAmp = TrMatrix_mass(p2,mf2,l2,p3,mb,l3,p1,-mf1,-l1)
         TrMx1   = Dcmplx(0d0,0d0)  
         TrMx2   = Dcmplx(0d0,0d0)  
 
         DO la1=-1,1,2            
           TrMx1 = TrMx1 + TrMatrix_zero(k,0d0,-la1,k,s,p1,-mf1,-l1)*
     &                     TrMatrix_mass(p2,mf2,l2,p3,mb,l3,k,0d0,-la1)
           TrMx2 = TrMx2 + TrMatrix_zero(p2,mf2,l2,k,s,k,0d0,la1)*
     &                     TrMatrix_mass(k,0d0,la1,p3,mb,l3,p1,-mf1,-l1)
         ENDDO

         WDecayAmplitudeKS_1ph = coeff * (        
     &        + (-(qf1/scalProd1+qb/scalProd3)*BSoft1              ! IR-divergent part of amplitude      
     &           +(qf2/scalProd2-qb/scalProd3)*BSoft2)/2d0*bornAmp

     &        - (qf1/ScalProd1+qb/scalProd3)*TrMx1/2d0             ! IR-finite part of amplitude            
     &        + (qf2/ScalProd2-qb/scalProd3)*TrMx2/2d0
     &        ) 

       RETURN
       END
 
       FUNCTION TrMatrix_mass(p1,m1,l1,k,m,s,p2,m2,l2)
c//////////////////////////////////////////////////////////////
c          transition matrix for massive boson               //
c                                                            // 
c                                                            //
c                         \ eps(k,m,s)                       //
c                         /                                  // 
c                        _\                                  //
c                         /\ k                               // 
c                         \                                  //
c             <-- p1      /         <-- p2                   //                       
c           ---<----------\----------<---                    //
c       Ub(p1,m1,l1)                  U(p2,m2,l2)            //
c                                                            // 
c//////////////////////////////////////////////////////////////                         
           IMPLICIT NONE
           INTEGER          l1,l2,s,i
           DOUBLE PRECISION forSqrt1,forSqrt2
           DOUBLE PRECISION m,m1,m2
           DOUBLE PRECISION k_1(0:3),k_2(0:3),p1(0:3),p2(0:3),k(0:3)
           DOUBLE PRECISION forSqrt3,forSqrt4,sqrt3,sqrt1,sqrt2,sqrt4
           DOUBLE COMPLEX   InProd_zero,inPr1,inPr2,inPr3,inPr4
           DOUBLE COMPLEX   TrMatrix_mass
           EXTERNAL         InProd_zero
           INTEGER           mcLUN
           DOUBLE PRECISION  spV(0:3),bet(0:3)
           DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
           COMMON /Kleiss_Stirling/spV,bet
           COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN      

           DO i=0,3
             k_1(i) = 1d0/2d0*(k(i) - m*spV(i))
             k_2(i) = 1d0/2d0*(k(i) + m*spV(i))                                
           ENDDO

           IF ( (l1.EQ.+1).AND.(l2.EQ.+1).AND.(s.EQ.0) ) THEN 
                
              inPr1 = InProd_zero(p1,+1,k_2,-1)
              inPr2 = InProd_zero(p2,-1,k_2,+1)
              inPr3 = InProd_zero(p1,+1,k_1,-1)
              inPr4 = InProd_zero(p2,-1,k_1,+1)
              sqrt1 = Dsqrt(p1(0)-p1(1))
              sqrt2 = Dsqrt(p2(0)-p2(1))
              sqrt3 = m1*m2/sqrt1/sqrt2

              TrMatrix_mass =                 
     &                      (inPr1*inPr2-inPr3*inPr4)*(vf+af)/m 
     &          + (k_1(0)-k_2(0)-k_1(1)+k_2(1))*sqrt3*(vf-af)/m         
                 
           ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.-1).AND.(s.EQ.0) ) THEN 

              inPr1 = InProd_zero(p1,+1,k_1,-1)
              inPr2 = InProd_zero(p1,+1,k_2,-1)
              inPr3 = InProd_zero(p2,+1,k_2,-1)
              inPr4 = InProd_zero(p2,+1,k_1,-1)

              forSqrt1 = (k_1(0)-k_1(1))/(p2(0)-p2(1))
              forSqrt2 = (k_2(0)-k_2(1))/(p2(0)-p2(1))
              forSqrt3 = (k_2(0)-k_2(1))/(p1(0)-p1(1))
              forSqrt4 = (k_1(0)-k_1(1))/(p1(0)-p1(1))
              sqrt1 = Dsqrt(forSqrt1)
              sqrt2 = Dsqrt(forSqrt2)
              sqrt3 = Dsqrt(forSqrt3)
              sqrt4 = Dsqrt(forSqrt4)     

              TrMatrix_mass = 
     &                 (inPr1*sqrt1 - inPr2*sqrt2)*(vf+af)*m2/m
     &               + (inPr3*sqrt3 - inPr4*sqrt4)*(vf-af)*m1/m
                        
           ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.+1).AND.(s.EQ.0) ) THEN 

              inPr1 = InProd_zero(p1,-1,k_1,+1)
              inPr2 = InProd_zero(p1,-1,k_2,+1)
              inPr3 = InProd_zero(p2,-1,k_2,+1)
              inPr4 = InProd_zero(p2,-1,k_1,+1)

              forSqrt1 = (k_1(0)-k_1(1))/(p2(0)-p2(1))
              forSqrt2 = (k_2(0)-k_2(1))/(p2(0)-p2(1))
              forSqrt3 = (k_2(0)-k_2(1))/(p1(0)-p1(1))
              forSqrt4 = (k_1(0)-k_1(1))/(p1(0)-p1(1))
              sqrt1 = Dsqrt(forSqrt1)
              sqrt2 = Dsqrt(forSqrt2)
              sqrt3 = Dsqrt(forSqrt3)
              sqrt4 = Dsqrt(forSqrt4)     
        
              TrMatrix_mass = 
     &                 (inPr1*sqrt1 - inPr2*sqrt2)*(vf-af)*m2/m
     &               + (inPr3*sqrt3 - inPr4*sqrt4)*(vf+af)*m1/m

           ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.-1).AND.(s.EQ.0) ) THEN 

              inPr1 = InProd_zero(p2,+1,k_2,-1)
              inPr2 = InProd_zero(p1,-1,k_2,+1)
              inPr3 = InProd_zero(p2,+1,k_1,-1)
              inPr4 = InProd_zero(p1,-1,k_1,+1)
              sqrt1 = Dsqrt(p1(0)-p1(1))
              sqrt2 = Dsqrt(p2(0)-p2(1))
              sqrt3 = m1*m2/sqrt1/sqrt2

              TrMatrix_mass =                    
     &                      (inPr1*inPr2 - inPr3*inPr4)*(vf-af)/m  
     &            + (k_1(0)-k_2(0)-k_1(1)+k_2(1))*sqrt3*(vf+af)/m
        
           ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.+1).AND.(s.EQ.+1) ) THEN 

              inPr1 = InProd_zero(p1,+1,k_1,-1)
              inPr2 = InProd_zero(k_2,-1,p2,+1)
              inPr3 = inPr1*inPr2

              forSqrt1 = (k_1(0)-k_1(1))/(p1(0)-p1(1))                       
              forSqrt2 = (k_2(0)-k_2(1))/(p2(0)-p2(1))  
              sqrt1 = Dsqrt(forSqrt1)                   
              sqrt2 = Dsqrt(forSqrt2)                   
              sqrt3 = m1*m2*sqrt1*sqrt2

              TrMatrix_mass =
     &                 Dsqrt(2d0)/m*(inPr3*(vf+af)+sqrt3*(vf-af))

           ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.-1).AND.(s.EQ.+1) ) THEN 

              inPr1 = InProd_zero(p1,+1,k_1,-1)
              inPr2 = InProd_zero(p2,+1,k_1,-1) 

              forSqrt1 = (k_2(0)-k_2(1))/(p2(0)-p2(1))                       
              forSqrt2 = (k_2(0)-k_2(1))/(p1(0)-p1(1))                       
              sqrt1 = m2*Dsqrt(forSqrt1)                   
              sqrt2 = m1*Dsqrt(forSqrt2)                                     
                     
              TrMatrix_mass =
     &                 Dsqrt(2d0)/m*( + inPr1*sqrt1*(vf+af)
     &                                - inPr2*sqrt2*(vf-af)
     &                                )

           ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.+1).AND.(s.EQ.+1) ) THEN 

              inPr1 = InProd_zero(k_2,-1,p2,+1)
              inPr2 = InProd_zero(k_2,-1,p1,+1)

              forSqrt1 = (k_1(0)-k_1(1))/(p1(0)-p1(1))                       
              forSqrt2 = (k_1(0)-k_1(1))/(p2(0)-p2(1))                       
              sqrt1 = m1*Dsqrt(forSqrt1)                   
              sqrt2 = m2*Dsqrt(forSqrt2)                                     
                     
              TrMatrix_mass =
     &                 Dsqrt(2d0)/m*( + inPr1*sqrt1*(vf+af)
     &                                - inPr2*sqrt2*(vf-af)
     &                                )

           ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.-1).AND.(s.EQ.+1) ) THEN 

              inPr1 = InProd_zero(p2,+1,k_1,-1)
              inPr2 = InProd_zero(k_2,-1,p1,+1)
              inPr3 = inPr1*inPr2

              forSqrt1 = (k_1(0)-k_1(1))/(p1(0)-p1(1))                       
              forSqrt2 = (k_2(0)-k_2(1))/(p2(0)-p2(1))  
              sqrt1 = Dsqrt(forSqrt1)                   
              sqrt2 = Dsqrt(forSqrt2)                   
              sqrt3 = m1*m2*sqrt1*sqrt2

              TrMatrix_mass = 
     &                 Dsqrt(2d0)/m*(inPr3*(vf-af)+sqrt3*(vf+af))

           ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.+1).AND.(s.EQ.-1) ) THEN 

              inPr1 = InProd_zero(p2,-1,k_1,+1)
              inPr2 = InProd_zero(k_2,+1,p1,-1)
              inPr3 = inPr1*inPr2

              forSqrt1 = (k_1(0)-k_1(1))/(p1(0)-p1(1))                       
              forSqrt2 = (k_2(0)-k_2(1))/(p2(0)-p2(1))  
              sqrt1 = Dsqrt(forSqrt1)                   
              sqrt2 = Dsqrt(forSqrt2)                   
              sqrt3 = m1*m2*sqrt1*sqrt2

              TrMatrix_mass =               
     &                 Dsqrt(2d0)/m*(inPr3*(vf+af)+sqrt3*(vf-af))

           ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.-1).AND.(s.EQ.-1) ) THEN 

              inPr1 = InProd_zero(k_2,+1,p2,-1)
              inPr2 = InProd_zero(k_2,+1,p1,-1)

              forSqrt1 = (k_1(0)-k_1(1))/(p1(0)-p1(1))                       
              forSqrt2 = (k_1(0)-k_1(1))/(p2(0)-p2(1))                       
              sqrt1 = m1*Dsqrt(forSqrt1)                   
              sqrt2 = m2*Dsqrt(forSqrt2)                                     
                     
              TrMatrix_mass =
     &                 Dsqrt(2d0)/m*(+ inPr1*sqrt1*(vf-af)
     &                               - inPr2*sqrt2*(vf+af)
     &                               )

           ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.+1).AND.(s.EQ.-1) ) THEN 

              inPr1 = InProd_zero(p1,-1,k_1,+1)
              inPr2 = InProd_zero(p2,-1,k_1,+1)

              forSqrt1 = (k_2(0)-k_2(1))/(p2(0)-p2(1))                       
              forSqrt2 = (k_2(0)-k_2(1))/(p1(0)-p1(1))                       
              sqrt1 = m2*Dsqrt(forSqrt1)                   
              sqrt2 = m1*Dsqrt(forSqrt2)                                     
                     
              TrMatrix_mass =
     &                 Dsqrt(2d0)/m*(+ inPr1*sqrt1*(vf-af)
     &                               - inPr2*sqrt2*(vf+af) 
     &                               )
    
           ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.-1).AND.(s.EQ.-1) ) THEN 

              inPr1 = InProd_zero(p1,-1,k_1,+1)
              inPr2 = InProd_zero(k_2,+1,p2,-1)
              inPr3 = inPr1*inPr2

              forSqrt1 = (k_1(0)-k_1(1))/(p1(0)-p1(1))                       
              forSqrt2 = (k_2(0)-k_2(1))/(p2(0)-p2(1))  
              sqrt1 = Dsqrt(forSqrt1)                   
              sqrt2 = Dsqrt(forSqrt2)                   
              sqrt3 = m1*m2*sqrt1*sqrt2

              TrMatrix_mass =  
     &                 Dsqrt(2d0)/m*(inPr3*(vf-af)+sqrt3*(vf+af))

           ELSE

              WRITE(MCLUN,*) " "             
              WRITE(MCLUN,*) " TrMatrix_mass: Wrong values for l1,l2,s:"
              WRITE(MCLUN,*) "          l1,l2 = -1,+1; s = -1,0,1 "
              WRITE(MCLUN,*) " "             
              STOP

           ENDIF 
         
        RETURN
        END

        FUNCTION TrMatrix_zero(p1,m1,l1,k,s,p2,m2,l2)
c############################################################################# 
c                                                                            #
c                         \ eps(k,0,s)                                       # 
c                         /                                                  #   
c                        _\                                                  # 
c                         /\                                                 #
c                         \                                                  #
c                         /                                                  #
c           ---<----------\-------------<---                                 #
c       Ub(p1,m1,l1)                  U(p2,m2,l2)                            #
c                                                                            #
c                                                                            #
c             definition of arbitrary light-like vector beta!!               #
c                                                                            #
c              bet(0) = 1.d0                                                 #
c              bet(1) = 1.d0                                                 #
c              bet(2) = 0.d0      <==> bet == k0  expression becomes easy!!  #
c              bet(3) = 0.d0                                                 #
c#############################################################################
              IMPLICIT NONE
              INTEGER          l1,l2,s,i
              DOUBLE PRECISION m1,m2,forSqrt1,forSqrt2,p1(0:3)
              DOUBLE PRECISION p1_1(0:3),p2_1(0:3),k(0:3),p2(0:3)
              DOUBLE PRECISION sqrt1,sqrt2,scalProd1,scalProd2
              DOUBLE COMPLEX   InProd_zero,inPr1,inPr2,inPr3,TrMatrix_zero
              LOGICAL          equal
              EXTERNAL         InProd_zero             
              INTEGER           mcLUN
              DOUBLE PRECISION  spV(0:3),bet(0:3)
              DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
              COMMON /Kleiss_Stirling/spV,bet
              COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN               
              equal = .TRUE.    
              DO i=0,3
                IF (p1(i).NE.p2(i))  equal = equal.AND..FALSE.
              ENDDO
                    

              IF ( (m1.EQ.m2).AND.(equal) ) THEN
c..          
c..             when:  p1=p2=p <=> m1=m2 TrMatrix_zero is diagonal
c..               
                 IF ( (l1.EQ.+1).AND.(l2.EQ.+1) ) THEN 

                    inPr1    = InProd_zero(k,+s,p1,-s)
                    forSqrt1 = (p1(0)-p1(1))/(k(0)-k(1)) 
                    sqrt1    = Dsqrt(2d0*forSqrt1)

                    TrMatrix_zero = sqrt1*inPr1
                    GOTO 111  
 
                 ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.-1) ) THEN                

                    TrMatrix_zero = Dcmplx(0d0,0d0)
                    GOTO 111  

                 ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.+1) ) THEN                

                    TrMatrix_zero = Dcmplx(0d0,0d0)
                    GOTO 111  

                 ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.-1) ) THEN                

                    inPr1    = InProd_zero(k,+s,p1,-s)
                    forSqrt1 = (p1(0)-p1(1))/(k(0)-k(1)) 
                    sqrt1    = Dsqrt(2d0*forSqrt1)

                    TrMatrix_zero = sqrt1*inPr1
                    GOTO 111  
          
                 ELSE 
        
                    WRITE(MCLUN,*) ""             
                    WRITE(MCLUN,*) " ERROR IN  TrMatrix_zero: "
                    WRITE(MCLUN,*) "       WRONG VALUES FOR l1,l2,s" 
                    WRITE(MCLUN,*) ""             
                    STOP

                 ENDIF       

              ENDIF

              IF ( (l1.EQ.+1).AND.(l2.EQ.+1).AND.(s.EQ.+1) ) THEN 

                 inPr1    = InProd_zero(k,+1,p1,-1)
                 forSqrt1 = (p2(0)-p2(1))/(k(0)-k(1))
                 sqrt1    = Dsqrt(2d0*forSqrt1)                   
 
                 TrMatrix_zero = sqrt1*inPr1

              ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.-1).AND.(s.EQ.+1) ) THEN 
 
                 TrMatrix_zero = Dcmplx(0d0,0d0)

              ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.+1).AND.(s.EQ.+1) ) THEN 
  
                 forSqrt1 = (p1(0)-p1(1))/(p2(0)-p2(1))             
                 forSqrt2 = 1d0/forSqrt1
                 sqrt1    = Dsqrt(2d0*forSqrt1)                   
                 sqrt2    = Dsqrt(2d0*forSqrt2)                   
                     
                 TrMatrix_zero = Dcmplx(m2*sqrt1-m1*sqrt2,0d0)

              ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.-1).AND.(s.EQ.+1) ) THEN 

                 inPr1    = InProd_zero(k,+1,p2,-1)
                 forSqrt1 = (p1(0)-p1(1))/(k(0)-k(1))
                 sqrt1    = Dsqrt(2d0*forSqrt1)                   
  
                 TrMatrix_zero = inPr1*sqrt1

              ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.+1).AND.(s.EQ.-1) ) THEN 
 
                 inPr1    = -InProd_zero(k,-1,p2,+1)
                 forSqrt1 = (p1(0)-p1(1))/(k(0)-k(1))
                 sqrt1    = Dsqrt(2d0*forSqrt1)                   
 
                 TrMatrix_zero = -sqrt1*inPr1


              ELSEIF ( (l1.EQ.+1).AND.(l2.EQ.-1).AND.(s.EQ.-1) ) THEN 
           
                 forSqrt1 = (p1(0)-p1(1))/(p2(0)-p2(1))     
                 forSqrt2 = 1d0/forSqrt1
                 sqrt1    = Dsqrt(2d0*forSqrt1)                   
                 sqrt2    = Dsqrt(2d0*forSqrt2)                   
                     
                 TrMatrix_zero = Dcmplx(m2*sqrt1-m1*sqrt2,0d0)

              ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.+1).AND.(s.EQ.-1) ) THEN 

                 TrMatrix_zero = Dcmplx(0d0,0d0)

              ELSEIF ( (l1.EQ.-1).AND.(l2.EQ.-1).AND.(s.EQ.-1) ) THEN 

                 inPr1    = -InProd_zero(k,-1,p1,+1)
                 forSqrt1 = (p2(0)-p2(1))/(k(0)-k(1))
                 sqrt1    = Dsqrt(2d0*forSqrt1)                   
  
                 TrMatrix_zero = -inPr1*sqrt1

              ELSE

                 WRITE(MCLUN,*) ""
                 WRITE(MCLUN,*) " ERROR IN TrMatrix_zero: "
                 WRITE(MCLUN,*) "    WRONG VALUES FOR l1,l2,s"
                 WRITE(MCLUN,*) ""             
                 STOP

              ENDIF 

 111          CONTINUE           

       RETURN
       END



       DOUBLE PRECISION FUNCTION InSqrt(p,q)

           DOUBLE PRECISION q(0:3),p(0:3)
            
           InSqrt = Dsqrt( (p(0)-p(1)) / (q(0)-q(1)) )

       RETURN
       END    


       DOUBLE COMPLEX FUNCTION BsFactor(s,k,p,m)
c///////////////////////////////////////////////////////////////////
c                                                                 //
c  this is small B_{s}(k,p) function when TrMartix is diaagonal!! //
c                                                                 //
c///////////////////////////////////////////////////////////////////
          IMPLICIT NONE
          INTEGER          s
          DOUBLE PRECISION p_1(0:3),p(0:3),k(0:3),m
          DOUBLE PRECISION forSqrt1,sqrt1
          DOUBLE COMPLEX   InProd_zero,inPr1
          EXTERNAL         InProd_zero             
          INTEGER           mcLUN
          DOUBLE PRECISION  spV(0:3),bet(0:3)
          DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
          COMMON /Kleiss_Stirling/spV,bet
          COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN            

          IF ( s.EQ.+1 ) THEN 

             inPr1    = InProd_zero(k,+1,p,-1)
             forSqrt1 = (p(0)-p(1))/(k(0)-k(1))
             sqrt1    = Dsqrt(2d0*forSqrt1)  
             BsFactor = inPr1*sqrt1

          ELSEIF ( s.EQ.-1 ) THEN 

             inPr1    = InProd_zero(k,-1,p,+1)
             forSqrt1 = (p(0)-p(1))/(k(0)-k(1))
             sqrt1    = Dsqrt(2d0*forSqrt1)  
             BsFactor = inPr1*sqrt1

         ELSE

            WRITE(MCLUN,*) " "             
            WRITE(MCLUN,*) " ERROR IN BsFactor: "
            WRITE(MCLUN,*) "       WRONG VALUES FOR s : s = -1,+1"
            WRITE(MCLUN,*) " "             
            STOP

         ENDIF 

       RETURN
       END   

       DOUBLE COMPLEX FUNCTION SoftFactor(s,k,p1,m1,p2,m2,Gmass2)

c
c       Gauge invariant soft factor for decay!!
c       Gmass2 -- photon mass square       
c 
          IMPLICIT NONE
          INTEGER          s
          DOUBLE PRECISION p1(0:3),p2(0:3),k(0:3),Gmass2
          DOUBLE PRECISION m1,m2,ScalProd1,ScalProd2
          DOUBLE COMPLEX   BsFactor2,BsFactor1,BsFactor
          EXTERNAL         BsFactor             

          ScalProd1 = k(0)*p1(0)-k(1)*p1(1)-k(2)*p1(2)-k(3)*p1(3)
          ScalProd2 = k(0)*p2(0)-k(1)*p2(1)-k(2)*p2(2)-k(3)*p2(3)
          
          BsFactor1 = BsFactor(s,k,p1,m1)
          BsFactor2 = BsFactor(s,k,p2,m2)

          SoftFactor= + BsFactor2/2.d0/(ScalProd2-Gmass2)
     &                - BsFactor1/2.d0/(ScalProd1-Gmass2)

       RETURN
       END
       DOUBLE COMPLEX FUNCTION InProd_mass(p1,m1,l1,p2,m2,l2)
c////////////////////////////////////////////////////////////////
c                                                              //
c  Inner product for massive spinors: Ub(p1,m1,l1)*U(p2,m2,l2) //
c                                                              //
c////////////////////////////////////////////////////////////////
              
          IMPLICIT NONE
          INTEGER          l1,l2
          DOUBLE PRECISION p1(0:3),p2(0:3),m1,m2
          DOUBLE PRECISION sqrt1,sqrt2,forSqrt1
          DOUBLE COMPLEX   InProd_zero
          EXTERNAL         InProd_zero
          INTEGER           mcLUN
          DOUBLE PRECISION  spV(0:3),bet(0:3)
          DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
          COMMON /Kleiss_Stirling/spV,bet
          COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN        

          IF ((l1.EQ.+1).AND.(l2.EQ.+1)) THEN                
             forSqrt1    = (p1(0)-p1(1))/(p2(0)-p2(1))
             sqrt1       = dsqrt(forSqrt1)
             sqrt2       = 1d0/sqrt1
             InProd_mass = Dcmplx(m1*sqrt2+m2*sqrt1,0d0)

          ELSEIF ((l1.EQ.+1).AND.(l2.EQ.-1)) THEN                             
             InProd_mass = InProd_zero(p1,+1,p2,-1)

          ELSEIF ((l1.EQ.-1).AND.(l2.EQ.+1)) THEN                        
             InProd_mass = InProd_zero(p1,-1,p2,+1)               

          ELSEIF ((l1.EQ.-1).AND.(l2.EQ.-1)) THEN                             
             forSqrt1    = (p1(0)-p1(1))/(p2(0)-p2(1))
             sqrt1       = dsqrt(forSqrt1)
             sqrt2       = 1d0/sqrt1
             InProd_mass = Dcmplx(m1*sqrt2+m2*sqrt1,0d0)

          ELSE        
             WRITE(MCLUN,*) " "             
             WRITE(MCLUN,*) " ERROR IN InProd_mass.."
             WRITE(MCLUN,*) "       WRONG VALUES FOR l1,l2"
             WRITE(MCLUN,*) " "             
             STOP
          ENDIF       

       RETURN
       END

       DOUBLE COMPLEX FUNCTION InProd_zero(p1,l1,p2,l2)
c////////////////////////////////////////////////////////////////
c         small s_{+,-}(p1,p2) for massless case:              //
c                 p1^2 = p2^2 = 0                              // 
c                                                              //
c     k0(0) = 1.d0                                             //
c     k0(1) = 1.d0                                             //
c     k0(2) = 0.d0  Kleisse_Stirling k0 points to X-axis       // 
c     k0(3) = 0.d0                                             //
c                                                              //
c////////////////////////////////////////////////////////////////
           IMPLICIT NONE
           INTEGER           l1,l2,i    
           DOUBLE PRECISION  p1(0:3),p2(0:3)
           DOUBLE PRECISION  forSqrt1,forSqrt2,sqrt1,sqrt2
           DOUBLE COMPLEX    i_,Dcmplx
           LOGICAL           equal
           PARAMETER         ( i_=(0.d0,1.d0) )
           INTEGER           mcLUN
           DOUBLE PRECISION  spV(0:3),bet(0:3)
           DOUBLE PRECISION  pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af
           COMMON /Kleiss_Stirling/spV,bet
           COMMON /mc_parameters/pi,sw,cw,alphaI,qb,mb,mf1,mf2,qf1,qf2,vf,af,mcLUN                          

           equal = .TRUE.    
           DO i=0,3
             IF (p1(i).NE.p2(i))  equal = equal.AND..FALSE.                
           ENDDO

           IF ( (l1.EQ.l2) .OR. equal ) THEN

              InProd_zero = Dcmplx(0d0,0d0)

           ELSEIF ( (l1.EQ.+1) .AND. (l2.EQ.-1) ) THEN

              forSqrt1 = (p1(0)-p1(1))/(p2(0)-p2(1))
              forSqrt2 = 1.0d0/forSqrt1
              sqrt1    = Dsqrt(forSqrt2)
              sqrt2    = Dsqrt(forSqrt1)

              InProd_zero = (p1(2)+i_*p1(3))*sqrt1 -
     &                      (p2(2)+i_*p2(3))*sqrt2 

           ELSEIF ( (l1.EQ.-1) .AND. (l2.EQ.+1) ) THEN

              forSqrt1 = (p1(0)-p1(1))/(p2(0)-p2(1))
              forSqrt2 = 1.0d0/forSqrt1
              sqrt1    = Dsqrt(forSqrt2)
              sqrt2    = Dsqrt(forSqrt1)

              InProd_zero = (p2(2)-i_*p2(3))*sqrt2 -
     &                      (p1(2)-i_*p1(3))*sqrt1 
                                    
           ELSE
                 
              WRITE(MCLUN,*) " "             
              WRITE(MCLUN,*) " ERROR IN InProd_zero:"
              WRITE(MCLUN,*) "    WRONG VALUES FOR l1,l2: l1,l2 = -1,+1"
              WRITE(MCLUN,*) " "             
              STOP

           ENDIF   

        RETURN
        END
