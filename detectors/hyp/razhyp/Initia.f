      SUBROUTINE INITIA(s,IAN,IZN,IHN)
      DOUBLE PRECISION  s
c-----------------------------------------------------
      COMMON /BLIEX/IEX /BLOKC/SPTU(10,500)
      COMMON /ILEVRA/ILEVRA /FKAP/FKAP /BINMAX/INMAX
      COMMON /WWW/WK(20),WA(20),WN(300) /SUMN/NK(20),NF
c      DIMENSION PN(3),PP(3),YAZT(25,11),HISTA(500),HISTZ(200),
c     #YAZH(25,11),HYPA(500),YAZ2H(25,11),HYP2A(500)
      COMMON /ALI/PN(3),PP(3),YAZT(25,11),HISTA(500),HISTZ(200),
     #YAZH(25,11),HYPA(500),YAZ2H(25,11),HYP2A(500)
      COMMON /bind/SHE,IA0,IZ0,IH0,IAS,IZS,IHS
c --------------------------------------------
c ---------------- light source ----------------------
c ----C13-->B14--IA0=13--IZ0=5--IH0=2
c      IA0=13
c      IZ0=5
c      IH0=2
c      IA0=10
c      IZ0=3
c      IH0=2
       IA0=IAN
       IZ0=IZN
       IH0=IHN 
c -- output file hypernuclei SHP12C.dat
c      OPEN(UNIT=9,FILE='SHP12C_gen.dat')

c -- output file hypernuclei DHP12C_gen.dat
c      OPEN(UNIT=10,FILE='DHP12C_gen.dat')

c -- possible exits in RAZVAL+EVANUC for writing hot fragments in SPT-array,
c    IEX=0 -as usual (normal second.deexcit.includ.), IEX=1 - for hot fragm.
      IEX=0
cc energy
cc      IF(IITT.EQ.1) ETOT=0.003
c initial nucleus (before ksi-minus absorption)
      SHE=S
      IAS=IA0-1
      IZS=IZ0+1
      IHS=0
    
      
c   number of events : ITNUM
c      ITNUM=10
c      ITNUM=3
c   now it will be given by the user
      PRINT 10
 10   FORMAT(5X,'***********  Fermi-break-up: ***************')
      PRINT 11,IA0,IZ0,IH0,SHE,ITNUM
 11   FORMAT(2X,'IA0,IZ0,IH0=',3I3,
     *' reduc.excit.ener. SHE=',F4.1,'MeV  ITNUM=',I7)
c installation of initial random number
      CALL RNDM2(0)
c ------ puting the accumulated arrays to zero
      DO 201 I=1,25
      DO 201 J=1,11
      YAZH(I,J)=0.
      YAZ2H(I,J)=0.
 201  YAZT(I,J)=0.
      DO 120 I=1,500
      HYPA(I)=0.
      HYP2A(I)=0.
 120  HISTA(I)=0.
      DO 121 I=1,200
 121  HISTZ(I)=0.
c separate levels of hypernuclei
      S1_1H4H=0.
      S1_1H4HE=0.
      S1_1H7LI=0.
      S2_1H7LI=0.
      S3_1H7LI=0.
      S4_1H7LI=0.
      S1_1H9BE=0.
      S2_1H9BE=0.
      S1_1H10B=0.
      S1_1H11B=0.
      S2_1H11B=0.
      S1_2H8LI=0.
      S2_2H8LI=0.
      S1_2H9LI=0.
      S2_2H9LI=0.
      S3_2H9LI=0.
      S1_2H9BE=0.
      S2_2H9BE=0.
      S3_2H9BE=0.
      S1_2H10BE=0.
      S1_2H11BE=0.
      S2_2H11BE=0.
      S1_2H12BE=0.
      S1_2H12B=0.

c initialization of twins, both at g.s
      C11gg_4H_4H=0.
      C11gg_4H_4HE=0.
      C11gg_4H_7LI=0.
      C12gg_4H_7LI=0.
      C13gg_4H_7LI=0.
      C14gg_4H_7LI=0.
      C11gg_4H_9BE=0.
      C12gg_4H_9BE=0.
      C11gg_4HE_4HE=0.
      C11gg_4HE_7LI=0.
      C12gg_4HE_7LI=0.
      C13gg_4HE_7LI=0.
      C14gg_4HE_7LI=0.
c initialization of twins, both at ex.s
      C11_4H_4H=0.
      C11_4H_4HE=0.
      C11_4H_7LI=0.
      C12_4H_7LI=0.
      C13_4H_7LI=0.
      C14_4H_7LI=0.
      C11_4H_9BE=0.
      C12_4H_9BE=0.
      C11_4HE_4HE=0.
      C11_4HE_7LI=0.
      C12_4HE_7LI=0.
      C13_4HE_7LI=0.
      C14_4HE_7LI=0.
c initialization of twins, one at g.s,other at ex.s
      C11ge_4H_4H=0.
      C11ge_4H_4HE=0.
      C11ge_4H_7LI=0.
      C12ge_4H_7LI=0.
      C13ge_4H_7LI=0.
      C14ge_4H_7LI=0.
      C11ge_4H_9BE=0.
      C12ge_4H_9BE=0.
      C11ge_4HE_4HE=0.
      C11ge_4HE_7LI=0.
      C12ge_4HE_7LI=0.
      C13ge_4HE_7LI=0.
      C14ge_4HE_7LI=0.
c extra inizialization of parameters for GMASHY
      ILEVRA=6
      IF(ILEVRA.EQ.6) INMAX=165+14+10
      FKAP=2.
      CALL HELPHY
      RETURN
      END


      SUBROUTINE HELPHY
C MS(I,J)-MACC[B C[[HOB[X [AKTOPOB [E[K[X [[EP =(2*S+1),[[E I=A-Z+1,
C J=Z+1. DM(I,J)-array mass defects (M-A) in carbon units (MeV). [[[T[BA[TC[
C [[PA C 0.LE.Z.LE.10 [ Z.LE.A.LE.(11+Z); MO[HO C[[TAT[ PA[BA[ [E[K[X
C [[EP C Z0.LE.10 [ (A0-Z0).LE.11.
C ([[[T[BA[TC[ C[[H[             OCHOBH[X      COCTO[H[[,   [EPETC[ [HEP
C [[[ OCHOBHO[O COCTO[H[[ - [[[ [[EP HA[[HA[ C [[T[[).
C KPOME TO[O,[POBO[[TC[ BC[OMO[ATE[[H[E B[[[C[EH[[ [[[ O[PE[E[EH[[ [AMMA
C [[HK[[[ ([O CT[P[[H[[) [ [P[BE[EHHO[O O[[EMA.
      COMMON /VAK/VAK /GAF/GAF(20) /SPDM/MS(12,11),DM(12,11)
      COMMON /FKAP/FKAP /ILEVRA/ILEVRA
      COMMON /BLIAN/IAN(300) /BLIZN/IZN(300) /BLENN/ENN(300)
      COMMON /BLISP/ISP(300) /BLIHN/IHN(300)
      COMMON /BINMAX/INMAX
      COMMON /BLIAN6/IAN6(300) /BLIZN6/IZN6(300) /BLENN6/ENN6(300)
      COMMON /BLISP6/ISP6(300) /BLIHN6/IHN6(300)
      DIMENSION IPA(12),JPZ(12),DMP(12),MSP(12),AMP(12),MD(12,11),
     *MS1(12,11)
      DATA MS1/
     * 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     * 2, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     * 0, 2, 1, 4, 1, 0, 1, 0, 0, 0, 0, 0,
     * 0, 0, 4, 3, 4, 5, 4, 0, 0, 0, 0, 0,
     * 0, 0, 1, 4, 1, 4, 1, 2, 1, 0, 0, 0,
     * 0, 0, 0, 5, 4, 7, 4, 3, 4, 0, 0, 0,
     * 0, 0, 0, 4, 1, 4, 1, 2, 1, 2, 1, 0,
     * 0, 0, 0, 0, 0, 3, 2, 3, 2, 5, 2, 0,
     * 0, 0, 0, 0, 0, 4, 1, 2, 1, 6, 1, 6,
     * 0, 0, 0, 0, 0, 0, 0, 1, 6, 3, 2, 5,
     * 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 4/
      DATA MD/
     *99000,
     * 8071,99999,99999,99999,99999,99999,99999,99999,99999,99999,99999,
     * 7289,
     *13136,14950,25920,33790,99999,99999,99999,99999,99999,99999,99999,
     *99000,
     *14931, 2425,11390,17597,26110,31603,42030,50130,65000,75240,89260,
     *99000,
     *25130,11680,14087,14907,20947,24955,33250,40940,52940,61570,72280,
     *99000,
     *31700,18375,15769, 4942,11348,12607,20179,25072,35720,40720,51210,
     *99000,
     *99999,27940,22920,12416,12052, 8668,13370,16560,24230,29410,37960,
     *99000,
     *99999,35093,28912,15702,10650,00000, 3125, 3020, 9873,13693,17560,
     *99000,99999,99999,99999,25450,17338, 5346, 2864,  102, 5683, 7871,
     *13274,99000,99999,99999,42700,32070,23110, 8010, 2860,-4737, -810,
     * -783, 3332,99000,99999,99999,99999,39700,33380,17610,10693, 1952,
     *  873,-1486,  -16,99000,99999,99999,99999,49400,36400,24110,16470,
     * 5320, 1750,-7040,-5730/
      DO 22 I=1,12
      DO 22 I2=1,11
      MS(I,I2)=MS1(I,I2)
 22   DM(I,I2)=0.001*MD(I,I2)
      VL=1.3/(0.21*SQRT(0.938))
      VOL=FKAP*VL*VL*VL
      VAK=VOL*SQRT(2./3.14159265)*0.333333
      GAF(1)=0.
      GAF(2)=1./SQRT(3.1416)
      DO 10 K=3,20
      QK=1./(1.5*K-2.5)
      GQ=1.+QK*(1./12.+QK*(1./288.-QK*(139./51840.)))
 10   GAF(K)=SQRT(QK*0.1591549)/GQ
      IF(ILEVRA-6) 301,30,301
 30   DO 32 I=1,INMAX
      IAN(I)=IAN6(I)
      IZN(I)=IZN6(I)
      ISP(I)=ISP6(I)
      IHN(I)=IHN6(I)
 32   ENN(I)=ENN6(I)
      RETURN
 301  PRINT 302,ILEVRA
 302  FORMAT(2X,'error of inconsistency in HELPHY: ILEVRA=',I2)
      STOP
      END

c
c
c **** random number generator for kiae-computer *****
c
C***************** GGUBFS     *******************************
C   IMSL ROUTINE NAME   - GGUBFS
C
C-----------------------------------------------------------------------
C
C   COMPUTER            - DG7/SINGLE
C
C   LATEST REVISION     - JUNE 1, 1980
C
C   PURPOSE             - BASIC UNIFORM (0,1) RANDOM NUMBER GENERATOR
C                           FUNCTION FORM OF GGUBS
C
C   USAGE               - FUNCTION GGUBFS()
C
C   ARGUMENTS    GGUBFS - RESULTANT DEVIATE.
C                DSEED  - INPUT/OUTPUT DOUBLE PRECISION VARIABLE
C                           ASSIGNED AN INTEGER VALUE IN THE
C                           EXCLUSIVE RANGE (1.D0, 2147483647.D0).
C                           DSEED IS REPLACED BY A NEW VALUE TO BE
C                           USED IN A SUBSEQUENT CALL.
C
C   PRECISION/HARDWARE  - SINGLE/ALL
C
C   REQD. IMSL ROUTINES - NONE REQUIRED
C
C   NOTATION            - INFORMATION ON SPECIAL NOTATION AND
C                           CONVENTIONS IS AVAILABLE IN THE MANUAL
C                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP
C
C   COPYRIGHT           - 1978 BY IMSL, INC. ALL RIGHTS RESERVED.
C
C   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN
C                           APPLIED TO THIS CODE. NO OTHER WARRANTY,
C                           EXPRESSED OR IMPLIED, IS APPLICABLE.
C
C-----------------------------------------------------------------------
C
      REAL FUNCTION GGUBFS()
C                                  SPECIFICATIONS FOR ARGUMENTS
      DOUBLE PRECISION   DSEED
      COMMON /EE/   DSEED,ISEED,RANCL
C                                  SPECIFICATIONS FOR LOCAL VARIABLES
      DOUBLE PRECISION   D2P31M,D2P31
C                                  D2P31M=(2**31) - 1
C                                  D2P31 =(2**31)(OR AN ADJUSTED VALUE)
      DATA               D2P31M/2147483647.D0/
      DATA               D2P31 /2147483711.D0/
      SAVE D2P31M,D2P31
C                                  FIRST EXECUTABLE STATEMENT
      DSEED = DMOD(16807.D0*DSEED,D2P31M)
      GGUBFS = DSEED / D2P31
      RANCL = RANCL + 1.
      RETURN
      END
c 
      FUNCTION RNDM(IX)
      DOUBLE PRECISION   DSEED
      COMMON /EE/   DSEED,ISEED,RANCL
      RNDM=GGUBFS()
      RETURN
      END
c
      SUBROUTINE RNDM2(I)
      DOUBLE PRECISION   DSEED
      COMMON /EE/   DSEED,ISEED,RANCL
      ISEED=I
      IF(ISEED.EQ.0) ISEED=711
      DSEED=DBLE(ISEED)
      RETURN
      END
