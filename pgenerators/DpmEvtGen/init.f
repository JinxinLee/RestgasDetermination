
C-----------------ast edition 05.04.07 V.Uzhinsky-----
       SUBROUTINE INIT1(Plab, seed, Elastic, tetmin)
C-----------------------------------------------------------------------
       COMMON/UZHI/SqrtS,Ecms,Vcms,Gamma,Proc_Prob(7),P_5str,CS_in,
     ,            CS_el,A1,T1,A2,T2,A3,Tmax,Tmin,Weight1
C-----------------------------------------------------------------------
C             PARAMETERS OF QUARK-GLUON STRING MODEL
C-----------------------------------------------------------------------
      COMMON/QGMPAR/ IFL(12), ALPHAR, ALPHAN, ALPHAF, ALPHA(4),
     *               PUDSC(4), BSLOP, VQMASS, DIQMAS
C =========================================================================
C-----------------------------------------------------------------------
C             PARAMETERS OF PARTICLE DECAYS
C-----------------------------------------------------------------------
      COMMON/LIMMAS/IDSTAB(180), SUMKM(533), AML(180), FI0ML(180)
C
      COMMON/PRINT/ ISYS   /IDGB/ IDGB, IDG
      COMMON/AB/aelm,betav, sigma_tot, parB, rho      !aida
      common/prob_el/prob_col, prob_int, prob_had          !aida

C
C-----------------------------------------------------------------------
      double precision seed
      
C-----------------------------------------------------------------------
c aida        print*, plab, seed, Elastic, tetmin

      Elab=sqrt(0.88+Plab**2)
      Etot=Elab+0.938
      S=1.76+2.*0.938*Elab
      SS=sqrt(S)
      SqrtS=SS
      Ecms=SS/2.
      Pcms=sqrt(Ecms**2-0.88)
      Vcms=Plab/(Elab+0.938)
      Gamma=(Elab+0.938)/SqrtS
*      write(6,*)'" ',Plab,SqrtS,'Plab,SqrtS'
*      write(6,*)'" ',Ecms,Pcms ,'Ecms,Pcms '
c-----------------------------------------------------------------------

      call RNDMSET1(seed)

C=========================================================
C FOR U, D, S, C, B, T QUARKS AND FOR ANTI-U, ANTI-D,
C  ANTI-S, ANTI-C, ANTI-B, ANTI-T QUARKS WE USE
C    THE FOLLOWING IDENTIFICATORS
C
      IFL(1)=1     ! u-quark
      IFL(2)=2     ! d-quark
      IFL(3)=3     ! s-quark
      IFL(4)=4     ! c-quark
      IFL(5)=5     ! b-quark
      IFL(6)=6     ! t-quark
      IFL(7)=7     ! anti_u-quark
      IFL(8)=8     ! anti_d-quark
      IFL(9)=9     ! anti_s-quark
      IFL(10)=10   ! anti_c-quark
      IFL(11)=11   ! anti_b-quark
      IFL(12)=12   ! anti_t-quark
C==========================================================
C       PARAMETERS OF QUARK DISTRIBUTIONS
C----------------------------------------------------------
C
      ALPHAR=+0.5  
      ALPHAN=-0.5
      ALPHAF=0.
C
      ALPHA(1)=ALPHAR
      ALPHA(2)=ALPHAR
      ALPHA(3)=0.
      ALPHA(4)=0.
C
C================================================================
C WE PUT THE CREATION PROBABILITIES OF U - ANTI-U, D - ANTI-D,
C   S - ANTI-S, C - ANTI-C SYSTEMS AS 0.43, 0.43, 0.14, 0. , SO
C
      PUDSC(2)=1.-0.18*(1.-3./SQRT(SqrtS))
      PUDSC(1)=PUDSC(2)/2.
      PUDSC(3)=1.
      PUDSC(4)=1.
C
C===============================================================
C   WE DESCRIBE THE QUARK TRANSVERSAL MOMENTUM DISTRIBUTION BY
C      BSLOP*EXP(-BSLOP*PT), BSLOP IN (GEV/C)**(-1)
C
                     BSLOP=4.5           ! now pbar p
C
C===============================================================
C        WE PUT THE FOLLOWING VALUES OF VALENCE QUARKS
C                 AND DIQUARKS MASSES
C -------------------------------------------------------------
      VQMASS=0.   ! 0.350 pbar p
      DIQMAS=0.   ! 0.700 pbar p
C
C
C==============================================================
C         WE MUST PREPARE TO STRINGS FRAGMENTATION
C              AND PARTICLES DECAYS,
C       SO WE PUT THE FOLLOWING PARTICLES ARE STABLE
C              DEPENDING ON ISTAB

      ISTAB=2   ! to put all particle stable write ISTAB=3 ! Uzhi 09.05

      DO 230  I=1,180
 230  IDSTAB(I)=0
C
C   FOR ISTAB=1
C
      DO 240  I=1,7
 240  IDSTAB(I)=1
C
      IDSTAB(26)=1
      IDSTAB(29)=1
      IDSTAB(30)=1
C
      DO 250  I=133,136
 250  IDSTAB(I)=1
C
      IF(ISTAB.EQ.1)  GO TO 280
C
C   FOR ISTAB=2
C
      DO 260  I=8,16
 260  IDSTAB(I)=1
C
      IF(ISTAB.EQ.2) GO TO 280
C
C   FOR ISTAB=3
C
      DO 270  I=17,180
 270  IDSTAB(I)=1
C
 280  CONTINUE
C
C-----------------------------------------------------------------
C    IF IT IS NEEDED TO POINT OUT THE OTHER STABLE
C    PARTICLES, PUT CORRESPONDING ISTAB = 1
C-----------------------------------------------------------------
       IDSTAB(17)=1  ! \Lambda
       IDSTAB(18)=1  ! Anti_Lambda
       IDSTAB(19)=1  ! K^0_s
       IDSTAB(23)=1  ! \pi^0
C      IDSTAB(33)=1  ! \rho_0

      CALL DATAR3
C
C==============================================================
C         FOR SIMULATION OF PARTICLE DECAYS WE PUT
C
      ISYS=6
      IDGB=-1
C-------------------------------------------------------------
c   PDG parametrization of total and elastic Pbar+P X-sections
c                 Phys. Rev. D54 (1996) 125.
C-------------------------------------------------------------
      ALOGp=Alog(Plab)
      Xtotal=38.4+77.6*Plab**(-0.64)+0.260*ALOGp**2-1.20*ALOGp
      Xelast=10.2+52.7*Plab**(-1.16)+0.125*ALOGp**2-1.28*ALOGp
      Welast=Xelast/Xtotal

      if(Elastic.eq.0.) Welast=0.  ! Inelastic interactions only
      if(Elastic.eq.2.) Welast=1.  ! Elastic   interactions only

C-------------------------------------------------------------
c      Parameters of differencial elastic X-section                    
C-------------------------------------------------------------
      CS_el=Xelast

      A1=115.0+650.0*exp(-Plab/4.08) 
      T1=0.0899
      A2=0.0687+0.307*exp(-Plab/2.367)
      T2=-2.979+3.353*exp(-Plab/483.4)
      A3=0.8372+39.53*exp(-Plab/0.765)

      If(Elastic .eq. 0.) then
      Tmin=0.
      Tmax=-4.*Pcms**2
      Weight1=A3*T2*(1.-exp(Tmax/T2))/(
     (A1*T1*(1.-exp(Tmax/T1))+A1*A2**2*T2*(1.-exp(Tmax/T2))-
     -2.*A1*A2*2.*T1*T2/(T1+T2)*(1.-exp(Tmax*(T1+T2)/2./T1/T2))+
     +A3*T2*(1.-exp(Tmax/T2))      )
      endif

      If(Elastic .gt. 0.) then
 
      Tetmin=tetmin*3.1416/180.
      Tantet2=(sin(Tetmin)/cos(Tetmin))**2       !9.06.09
      Plmin=2*0.938*Plab/(2*0.938+Etot*Tantet2)  !9.06.09      
      Ptmin=Plmin*sin(Tetmin)/cos(Tetmin)
      
      sqmin=Plmin**2+Ptmin**2+0.938**2
      Tmin=2*(Plmin*Plab+0.938**2-Elab*sqrt(sqmin))

      Tmax=-4.*Pcms**2
      tsito=2.*(plab**2)*(1.-cos(tetmin))     !T by Tsito formul
      bbb=(Plab*sin(Tetmin))**2               !T by approx formul
caida  bbbmax=(Plab*sin(0.0082))**2              !11.06.09 dlya lumi

      Tmin=-bbb
caida  Tmax=-bbbmax


       Weight1=A3*T2*(exp(Tmin/T2)-exp(Tmax/T2))/(
     & A1*T1*(exp(Tmin/T1)-exp(Tmax/T1))+
     & A1*A2**2*T2*(exp(Tmin/T2)-exp(Tmax/T2))-
     & 2.*A1*A2*2.*T1*T2/(T1+T2)*(exp(Tmin*(T1+T2)/2./T1/T2)
     &  -exp(Tmax*(T1+T2)/2./T1/T2))+
     &  A3*T2*(exp(Tmin/T2) - exp(Tmax/T2)) )

*   aida begin parameters
       betav=Plab/Elab
       rho=-sqrt(A3)/(sqrt(A1)*(1-A2))
       parB=(A1/T1+A1*(A2**2)/T2-2*A1*A2*(1./2./T1+1./2./T2)+
     & A3/T2)/(A1*(1-A2)**2+A3)
       
       sigma_tot=4*sqrt(3.1416*A1*0.1/5.067**2)*(1-A2)*10
caida       print *, 'sigma_tot',sigma_tot,' B',parB,' ro',rho;
       aelm = 1./137.036    !  0- not Colomb and Interf, 
*  aida end parameters    
***  aida calculation of integrals Colomb, 
      Ndiv=100000
      dt=(Tmax-Tmin)/float(Ndiv)
       SIG_col=0.
      do i=1,Ndiv
      T11=Tmin+(i-1)*dt
      T22=T11+dt
      df_col=DSIG_COL(T22)+DSIG_COL(T11)
      
      SIG_COL=sig_col+(0.5*df_col*abs(dt))
      enddo
caida      PRINT *,'sig_col',SIG_COL

* aida calculation of integral interfer
      SIG_INTER=0.
      SIG_IEXACT=0.
      do i=1,Ndiv
      T11=Tmin+(i-1)*dt
      T22=T11+dt
      df_int=DSIG_INTER(T22)+DSIG_INTER(T11)
      SIG_INTER=SIG_INTER+(0.5*df_int*abs(dt))
      df_iex=DSIG_INT_Ex(T22)+DSIG_INT_Ex(T11)
      SIG_IEXACT=SIG_IEXACT+(0.5* df_iex*abs(dt))              
      enddo
caida      PRINT *,'sig_inter',sig_inter
!  numerical calculation of SIG_had using form.(1)
      sig_had=dsig_had(0.)/parB-dsig_had(Tmax)/parB
caida      PRINT *,'sig_had_el', sig_had
!     calculation of sigma_hadron using our parametrization
      sig_had_p=SIG_HADi(Tmin)-SIG_HADi(Tmax)
caida      PRINT *,'sig_had_p',sig_had_p
    
!       sig_col=0              ! kulon ==0
!        sig_inter=0             ! inter ==0 
!       sig_had_p=0             ! hadron ==0  

      SIG_MAG=SIG_COL+SIG_INTER+SIG_HAD_p      

      SIG_NORM=SIG_COL+SIG_IEXACT+SIG_HAD_p      

caida      print*, 'Xtotal', Xtotal, ' Xelast, hadronic part', Xelast
caida      PRINT *,'sigma_tot', sigma_tot, 'sig_mag', SIG_MAG
caida      PRINT *,'sig_iexact ',sig_iexact,'SIGMA_norm ',sig_norm

!calculation of probability hadron, colomb, interf- elastic
      prob_col=1./sig_mag * sig_col   
      prob_int=1./sig_mag * sig_inter
      prob_had=1./sig_mag * sig_had_p         !our parametr
caida      print *,'prob_col   prob_inter   prob_had'
caida      print*, prob_col, prob_int, prob_had
      endif                                    ! aida end of elastic
     
c ---------------------------- Determination of processes prababilities
      CS_a=0.4*( 129./sqrt(S)-147./S+41./S**1.5)   
      CS_b=0.6*( 129./sqrt(S)-147./S+41./S**1.5)   
      CS_c=93./S-106./S**1.5+30./S**2
      CS_e=0.
      CS_pom=18.6*S**0.08-33.5/sqrt(S)+30.8/S
      CS_lmd=0.

      CS_in=CS_a + CS_b + CS_c + CS_e + CS_pom + CS_lmd

*      write(6,*)' ----------------------------------------------------'
*      write(6,*)'--------------- Plab= ',Plab,'-----------------------'
*      write(6,*)' Cross-sections     a      b      c      e      Pom  '
*      write(6,1)                    CS_a, CS_b, CS_c,   CS_e,  CS_pom
*1     format(    17x,              f6.2,1x,f6.2,1x,f6.2,1x,f6.2,1x,f6.2)
*      write(6,*)' ----------------------------------------------------'
*      write(6,*)' Inelastic cross-section = ',CS_in,' (mb)'
*      write(6,*)' ----------------------------------------------------'
*      write(6,*)' Coefficients       a      b      c      e           '
*      write(6,1)                    C_a,   C_b,   C_c,   C_e
*      write(6,*)' ----------------------------------------------------'

      Proc_Prob(1)=CS_a/CS_in               *(1.-Welast)
      Proc_Prob(2)=Proc_Prob(1)+CS_b/CS_in  *(1.-Welast)
      Proc_Prob(3)=Proc_Prob(2)+CS_c/CS_in  *(1.-Welast)
      Proc_Prob(4)=Proc_Prob(3)+CS_e/CS_in  *(1.-Welast)
      Proc_Prob(5)=Proc_Prob(4)+CS_pom/CS_in*(1.-Welast)
      Proc_Prob(6)=Proc_Prob(5)+CS_lmd/CS_in*(1.-Welast)
      Proc_Prob(7)=Proc_Prob(6)+Welast

      P_5str=0.    ! 0.3      Uzhi

*	write(6,*)Proc_Prob

      RETURN
      END

      SUBROUTINE DATAR3
      DIMENSION IV(36),IP(36),IB(126),IBB(126),IA(126),IAA(126)

      COMMON/INPDAT/IMPS(6,6),IMVE(6,6),IB08(6,21),IB10(6,21),
     *IA08(6,21),IA10(6,21),A1,B1,B2,B3,ISU,BET,AS,B8,AME,DIQ
C
      COMMON/PART/ANAME(180),AM(180),GA(180),TAU(180),ICH(180),IBAR(180)
     ,           ,K1(180),K2(180)
      COMMON/DECAYC/ZKNAME(533),NZK(533,3),WT(533)
C
      COMMON/LIMMAS/IDSTAB(180),SUMKM(533),AML(180),FI0ML(180)
      REAL*8 ANAME
      REAL*8 ZKNAME

      SAVE IP, IV, IB, IBB, IA, IAA
C
      DATA IP/
     *23,14,16,116,2*0,13,23,25,117,2*0,15,24,31,120,2*0,119,118,121,
     *122,14*0/

      DATA IV/
     *33,34,38,123,0,0,32,33,39,124,0,0,36,37,96,127,0,0,126,125,128,
     *129,14*0/

      DATA IB/
     *0,1,21,140,0,0,8,22,137,0,0,97,138,0,0,146,5*0,
     *1,8,22,137,0,0,0,20,142,0,0,98,139,0,0,147,5*0,
     *21,22,97,138,0,0,20,98,139,0,0,0,145,0,0,148,5*0,
     *140,137,138,146,0,0,142,139,147,0,0,145,148,50*0/

      DATA IBB/
     *53,54,104,161,0,0,55,105,162,0,0,107,164,0,0,167,5*0,
     *54,55,105,162,0,0,56,106,163,0,0,108,165,0,0,168,5*0,
     *104,105,107,164,0,0,106,108,165,0,0,109,166,0,0,169,5*0,
     *161,162,164,167,0,0,163,165,168,0,0,166,169,0, 0,170,47*0/

      DATA IA/
     *0,2,99,152,0,0,9,100,149,0,0,102,150,0,0,158,5*0,
     *2,9,100,149,0,0,0,101,154,0,0,103,151,0,0,159,5*0,
     *99,100,102,150,0,0,101,103,151,0,0,0,157,0,0,160,5*0,
     *152,149,150,158,0,0,154,151,159,0,0,157,160,50*0/

      DATA IAA/
     *67,68,110,171,0,0,69,111,172,0,0,113,174,0,0,177,5*0,
     *68,69,111,172,0,0,70,112,173,0,0,114,175,0,0,178,5*0,
     *110,111,113,174,0,0,112,114,175,0,0,115,176,0,0,179,5*0,
     *171,172,174,177,0,0,173,175,178,0,0,176,179,0,0,180,47*0/

      L=0
      DO 1 I=1,6
      DO 2  J=1,6
      L=L+1
      IMPS(I,J)=IP(L)
    2 CONTINUE
    1 CONTINUE

      L=0
      DO 3 I=1,6
      DO 4 J=1,6
      L=L+1
      IMVE(I,J)=IV(L)
    4 CONTINUE
    3 CONTINUE

      L=0
      DO 5 I=1,6
      DO 6 J=1,21
      L=L+1
      IB08(I,J)=IB(L)
    6 CONTINUE
    5 CONTINUE

      L=0
      DO 7 I=1,6
      DO 8 J=1,21
      L=L+1
      IB10(I,J)=IBB(L)
    8 CONTINUE
    7 CONTINUE

      L=0
      DO 9 I=1,6
      DO 10 J=1,21
      L=L+1
      IA08(I,J)=IA(L)
   10 CONTINUE
    9 CONTINUE

      L=0
      DO 11 I=1,6
      DO 12 J=1,21
      L=L+1
      IA10(I,J)=IAA(L)
   12 CONTINUE
   11 CONTINUE

      A1=0.88
      B3=4.5
      B1=8.0
      B2=8.0
      ISU=4
      BET=8.0
      AS=0.65    ! Uzhi 0 - only heavy resonances, 1. - light ones
      AME=0.75
      B8=0.5
      DIQ=0.375
C
      DO 13  I=1,180
      AML(I)=-1.
      IF(GA(I).EQ.0.) AML(I)=AM(I)
 13   CONTINUE
C
      DO 20  I=1,533
 20   SUMKM(I)=-1.
C
C
 80   CONTINUE
C
      DO 110  I=1,180
C
C     IF(AML(I).GE.0.) GO TO 110
C
      IK1=K1(I)
      IK2=K2(I)
C
      DO 90  J=IK1, IK2
      ID1=NZK(J,1)
      IF(ID1.EQ.0) ID1=29
      ID2=NZK(J,2)
      IF(ID2.EQ.0) ID2=29
      ID3=NZK(J,3)
      IF(ID3.EQ.0) ID3=29
      IF(AML(ID1).LT.0.) GO TO 105
      IF(AML(ID2).LT.0.) GO TO 105
      IF(AML(ID3).LT.0.) GO TO 105
      SUMKM(J)=AML(ID1)+AML(ID2)+AML(ID3)
 90   CONTINUE
C
      AMIN=100.
      DO 100  J=IK1,IK2
      IF(WT(J).EQ.0.) GO TO 100
      IF(SUMKM(J).LE.AMIN) AMIN=SUMKM(J)
 100  CONTINUE
C
      IF(AML(I).LT.0.) AML(I)=AMIN
C
 105  CONTINUE
 110  CONTINUE
C
      RETFL=1.
      DO 120  I=1,180
      IF(AML(I).LT.0.) RETFL=-1.
 120  CONTINUE
C
      IF(RETFL.LT.0.) GO TO 80
C
      DO 140  I=1,180
      IF(GA(I).NE.0.) GO TO 130
      FI0ML(I)=0.
      GO TO 140
 130  CONTINUE
      TGFI0=AML(I)**2/AM(I)/GA(I)-AM(I)/GA(I)
      FI0ML(I)=ATAN(TGFI0)
 140  CONTINUE
C
      RETURN
      END

      BLOCK DATA
C*****BLOCK DATA
      COMMON/PART/ANAME(115),ANAM1(65),AM(180),GA(180),TAU(180),ICH(180)
     *,IBAR(180),K1(180),K2(180)

C***BLOCK DATA 2
      COMMON/DECAYC/ZKNAM1(87),ZKNAM2(59),ZKNAM3(105),ZKNAM4(79),
     *ZKNAM5(103),ZKNAM6(100),NZK01(195),NZK1(183),NZK11(155),
     *NZK02(195),NZK2(183),
     *NZK22(155),NZK03(195),NZK3(183),NZK33(155),
     *WT(135),WT1(131),WT2(64),WT3(103),WT4(100)

      COMMON/DQSHR/XDQ,XDQ1 /FLPRO/PUDSC(4) /FLID/IFL(12)
      COMMON/REGPAR/ALPHAR,ALPHAN,ALPHAF,ALPHA(4)
      COMMON/FRAG1/DQFRAG,RM,RDI,BSLOP,PTMIN /IDGB/IDGB,IDG
      COMMON/PRINT/ISYS

      REAL*8 ANAME
      REAL*8 ANAM1
      REAL*8 ZKNAM1,ZKNAM2,ZKNAM3,ZKNAM4,ZKNAM5,ZKNAM6
C
C     PARTICLE NAMES
C
      DATA ANAME/1HP,2HAP,2HE-,2HE+,3HNUE,4HANUE,3HGAM,3HNEU,4HANEU,
     *4HMUE+,4HMUE-,3HK0L,3HPI+,3HPI-,2HK+,2HK-,3HLAM,4HALAM,3HK0S,
     *5HSIGM-,5HSIGM+,5HSIGM0,3HPI0,
     *2HK0,3HAK0,5H     ,6HAN*-14,6HAN*014,2*5H     ,
     *6HETA550,6HRHO+77,6HRHO077,6HRHO-77,6HOM0783,6HK*+892,6HK*0892,
     *6HK*-892,6HAK*089,6HKA+125,6HKA0125,6HKA-125,6HAKA012 ,6HK*+142,
     *6HK*0142,6HK*-142,6HAK*014,6HS+1385,6HS01385,6HS-1385,6HL01820,
     *6HL02030,
     *6HN*++12,6HN*+ 12,6HN*012 ,6HN*-12 ,6HN*++16,6HN*+16 ,6HN*016 ,
     *6HN*-16 ,6HN*+14 ,6HN*014 ,6HN*+15 ,6HN*015 ,6HN*+18 ,6HN*018 ,
     *6HAN--12,6HAN*-12,6HAN*012,6HAN*+12,6HAN--16,6HAN*-16,6HAN*016,
     *6HAN*+16,6HAN*-15,6HAN*015,6HDE*-24,
     *6HRPI+49,6HRPI049,6HRPI-49,6HPIN++ ,6HPIN+0 ,6HPIN+- ,6HPIN-0 ,
     *4HPPPI,4HPNPI,5HAPPPI,5HAPNPI,5HK+PPI,5HK-PPI,5HK+NPI,5HK-NPI,
     *6HS+1820,6HS-2030,
     *6HETA*  ,6HPHI   ,6HTETA0 ,6HTETA- ,6HASIG- ,6HASIG0 ,6HASIG+ ,
     *6HATETA0,6HATETA+,6HSIG*+ ,6HSIG*0 ,6HSIG*- ,6HTETA*0,6HTETA* ,
     *6HOMEGA-,6HASIG*-,6HASIG*0,6HASIG*+,6HATET*0,6HATET*+,6HOMEGA+/
      DATA ANAM1/2HD0,2HD+,2HD-,3HAD0,2HF+,2HF-,4HETAC,3HD*0,3HD*+,
     *3HD*-,4HAD*0,3HF*+,3HF*-,3HPSI,4HJPSI,4HTAU+,4HTAU-,4HNUET,
     *5HANUET,4HNUEM,5HANUEM,
     *6HC0+   ,6HA+    ,6HA0    ,6HC1++  ,6HC1+   ,6HC10   ,6HS+    ,
     *6HS0    ,6HT0    ,6HXU++  ,6HXD+   ,6HXS+   ,6HAC0-  ,6HAA-   ,
     *6HAA0   ,6HAC1-- ,6HAC1-  ,6HAC10  ,6HAS-   ,6HAS0   ,6HAT0   ,
     66HAXU-- ,6HAXD-  ,6HAXS   ,6HC1*++ ,6HC1*+  ,6HC1*0  ,6HS*+   ,
     *6HS*0   ,6HT*0   ,6HXU*++  ,6HXD*+  ,6HXS*+  ,6HTETA++,6HAC1*--,
     *6HAC1*- ,6HAC1*0 ,6HAS*-  ,6HAS*0  ,6HAT*0  ,6HAXU*--,6HAXD*- ,
     *6HAXS*- ,6HATET--/
C
C    PARTICLE MASSES IN GEV
C
      DATA AM/2*0.938,2*0.0005,3*0,2*0.94,2*0.106,0.498,2*0.140,2*0.494,
     *2*1.116,0.498,1.197,1.189,1.193,0.134,2*0.498,
     *0.,2*1.43,
     *2*0,0.548,3*0.776,0.783,4*0.896,4*1.25,4*1.421,3*1.382,1.82,2.03, ! Uzhi
     *4*1.232,4*1.675,2*1.43,2*1.515,2*1.775,4*1.232,4*1.675,2*1.515,
     *2.4,3*0.489,4*1.3,4*2.2,4*1.7,1.82,2.03,0.958,1.019,1.315,1.321,
     *1.189,1.193,1.197,1.315,1.321,3*1.385,2*1.534,1.672,3*1.385,
     *2*1.534,1.672,4*1.867,3*2.03,2.006,2*2.008,2.006,2*2.14,3.684,
     *3.097,2*1.807,2*0.6,2*0.003,2.285,2.47,2.47,2.41,2.42,2.41,2.56,
     *2.56,2.73,3.61,3.61,3.79,2.26,2.47,2.47,2.41,2.42,2.41,2.56,2.56,
     *2.73,3.61,3.61,3.79,3*2.49,2*2.61,2.77,2*3.67,3.85,4.89,3*2.49,
     *2*2.61,2.77,2*3.67,3.85,4.89/
C
C       RESONANCE WIDTH GAMMA IN GEV
C
      DATA GA/26*0,2*0.2,2*0.,
     *1.29E-6,3*0.150,0.01,4*0.051,4*0.45,4*0.108,3*0.05,             ! Uzhi
     *0.085,0.18,4*0.120,4*0.2,2*0.2,2*0.1,2*0.2,4*0.120,4*0.2,2*0.1,
     *0.2,3*0.1,4*0.1,4*0.2,4*0.15,0.085,0.18,2.E-4,0.004,7*0,0.036,
     *0.036,0.039,0.009,0.009,0,0.036,0.036,0.039,0.009,0.009,0,7*0,
     *0.005,0.002,0.002,0.005,0.002,0.002,0.0002,0.0007,6*0,44*0/
C
C       MEAN LIFE TIME IN SECONDS
C
      DATA TAU/7*1.E+18,2*918.,2*2.2E-06,5.2E-08,2*2.6E-08,2*1.2E-08,
     *2*2.6E-10,0.9E-10,1.5E-10,8.E-11,7.4E-20,8.E-17,
     *61*0,10*0,2*0,3.E-10,1.7E-10,0.8E-10,1.E-14,1.5E-10,3.E-10,
     *1.7E-10,5*0,1.E-10,5*0,1.E-10,15*0,4*9.E-12,2*1.E+18,44*0/
C
C      CHARGE OF PARTICLES AND RESONANCES
C
      DATA ICH/+1,-1,-1,
     *+1,
     *5*0,1,-1,0,1,-1,1,-1,0,0,0,-1,1,5*0,-1,0,2*0,0,1,0,-1,0,
     *1,0,-1,0,1,0,-1,0,1,0,-1,0,1,0,-1,0,0,2,1,0,-1,2,1,0,-1,1,0,1,0,
     *1,0,-2,-1,0,1,-2,-1,0,1,-1,0,1,1,0,-1,2,1,0,-1,2,1,0,-1,2,0,1,-1,
     *1,-1,3*0,-1,-1,0,1,0,2*1,0,-1,0, 3*-1,0,1,0,1,1,0,1,-1,0,1,-1,0,0,
     *1,-1,0,1,-1,0,0,1,-1,4*0,1,1,0,2,1,0,1,0,0,2,1,1,-1,-1,0,-2,-1,0,
     *-1,2*0,-2,2*-1,2,1,0,1,2*0,2,2*1,2,-2,-1,0,-1,2*0,-2,2*-1,-2/
C
C      BARYONIC CHARGE
C
      DATA IBAR/1,-1,5*0,1,-1,7*0,1,-1,0,3*1,4*0,2*-1,7*0
     *                                           ,12*0,5*1,14*1,10*-1,
     *2,3*0,4*1,2,2,0,0,4*1,2*1,0,0,1,1,5*-1,6*1,6*-1,21*0,12*1,12*-1,
     *10*1,10*-1/
C
C      FIRST NUMBER OF DECAY CHANNELS USED FOR RESONANCES
C      AND DECYING PARTICLES
C
C************K1
      DATA K1/
     *  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 16, 17, 18,
     * 24, 30, 34, 38, 40, 41, 43, 44,136,138,330,310,317,330,330,
     * 46, 51, 52, 54, 55, 58, 60, 62, 64, 66, 68, 70, 72, 74, 82,
     * 90, 98,106,109,112,114,123,140,141,143,145,146,150,157,164,
     *168,175,182,189,196,204,212,213,215,217,218,222,229,236,240,
     *247,254,256,257,258,259,261,264,267,269,271,274,278,281,284,
     *288,292,295,301,333,337,341,342,343,345,346,347,348,349,352,
     *355,358,360,362,365,368,371,374,376,378,381,385,387,389,393,
     *396,399,402,404,407,410,412,414,416,419,422,427,432,433,434,
     *435,436,450,454,459,460,461,462,463,464,468,470,472,474,488,
     *492,497,498,499,500,501,502,506,508,510,512,513,514,515,516,
     *517,518,519,520,521,524,525,526,527,528,529,530,531,532,324/
C************K2
      DATA K2/
     *  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 15, 16, 17, 23,
     * 29, 31, 35, 39, 40, 42, 43, 45,137,139,330,316,323,330,330,
     * 50, 51, 53, 54, 57, 59, 61, 63, 65, 67, 69, 71, 73, 81, 89,
     * 97,105,108,111,113,122,135,140,142,144,145,149,156,163,167,
     *174,181,188,195,203,211,212,214,216,217,221,228,235,239,246,
     *253,255,256,257,258,260,263,266,268,270,273,277,280,283,287,
     *291,294,300,309,336,340,341,342,344,345,346,347,348,351,354,
     *357,359,361,364,367,370,373,375,377,380,384,386,388,392,395,
     *398,401,403,406,409,411,413,415,418,421,426,431,432,433,434,
     *435,449,453,458,459,460,461,462,463,467,469,471,473,487,491,
     *496,497,498,499,500,501,505,507,509,511,512,513,514,515,516,
     *517,518,519,520,523,524,525,526,527,528,529,530,531,532,326/
C*************ZKNAM1
      DATA ZKNAM1/
     *8HP       ,8HAP      ,8HE-      ,8HE+      ,8HNUE     ,8HANUE    ,
     *8HGAM     ,8HPE-NUE  ,8HAPEANU  ,8HEANUNU  ,8HE-NUAN  ,8H3PI0    ,
     *8HPI+-0   ,8HPIMUNU  ,8HPIE-NU  ,8HMU+NUE  ,8HMU-NUE  ,8HMU+NUE  ,
     *8HPI+PI0  ,8HPI++-   ,8HPI+00   ,8HM+P0NU  ,8HE+P0NU  ,8HMU-NU   ,
     *8HPI-0    ,8HPI+--   ,8HPI-00   ,8HM-P0NU  ,8HE-P0NU  ,8HPPI-    ,
     *8HNPI0    ,8HPE-NUE  ,8HPM-NUE  ,8HAPPI+   ,8HANPI0   ,8HPE+NU   ,
     *8HAPM+NU  ,8HPI+PI-  ,8HPI0PI0  ,8HNPI-    ,8HPPI0    ,8HNPI+    ,
     *8HLAGA    ,8HGAGA    ,8HGAE+E-  ,8HGAGA    ,8HGAGAP0  ,8HPI000   ,
     *8HPI+-0   ,8HPI+-GA  ,8HPI+0    ,8HPI+-    ,8HPI00    ,8HPI-0    ,
     *8HPI+-0   ,8HPI+-    ,8HPI0GA   ,8HK+PI0   ,8HK0PI+   ,8HK0PI0   ,
     *8HK+PI-   ,8HK-PI0   ,8HAK0PI-  ,8HAK0PI0  ,8HK-PI+   ,8HK+PI0   ,
     *8HK0PI+   ,8HK0PI0   ,8HK+PI-   ,8HK-PI0   ,8HK0PI-   ,8HAK0PI0  ,
     *8HK-PI+   ,8HK+PI0   ,8HK0PI+   ,8HK+89P0  ,8HK08PI+  ,8HK+RO77  ,
     *8HK0RO+7  ,8HK+OM07  ,8HK+E055  ,8HK0PI0   ,8HK+PI+   ,8HK089P0  ,
     *8HK+8PI-  ,8HK0R077  ,8HK+R-77  /
C**************ZKNAM2
      DATA ZKNAM2/
     *8HK+R-77  ,8HK0OM07  ,8HK0E055  ,8HK-PI0   ,8HK0PI-   ,8HK-89P0  ,
     *8HAK08P-  ,8HK-R077  ,8HAK0R-7  ,8HK-OM07  ,8HK-E055  ,8HAK0PI0  ,
     *8HK-PI+   ,8HAK08P0  ,8HK-8PI+  ,8HAK0R07  ,8HAK0OM7  ,8HAK0E05  ,
     *8HLA0PI+  ,8HSI0PI+  ,8HSI+PI0  ,8HLA0PI0  ,8HSI+PI-  ,8HSI-PI+  ,
     *8HLA0PI-  ,8HSI0PI-  ,8HNEUAK0  ,8HPK-     ,8HSI+PI-  ,8HSI0PI0  ,
     *8HSI-PI+  ,8HLA0ET0  ,8HS+1PI-  ,8HS-1PI+  ,8HSO1PI0  ,8HNEUAK0  ,
     *8HPK-     ,8HLA0PI0  ,8HLA0OM0  ,8HLA0RO0  ,8HSI+RO-  ,8HSI-RO+  ,
     *8HSI0RO0  ,8HLA0ET0  ,8HSI0ET0  ,8HSI+PI-  ,8HSI-PI+  ,8HSI0PI0  ,
     *8HK0S     ,8HK0L     ,8HK0S     ,8HK0L     ,8HP PI+   ,8HP PI0   ,
     *8HN PI+   ,8HP PI-   ,8HN PI0   ,8HN PI-   ,8HP PI+   /
C****************ZKNAM3
      DATA ZKNAM3/
     *8HN*>PI0  ,8HN*+PI+  ,8HPRHO+   ,8HP PI0   ,8HN PI+   ,8HN*>PI-  ,
     *8HN*+PI0  ,8HN*0PI+  ,8HPRHO0   ,8HNRHO+   ,8HP PI-   ,8HN PI0   ,
     *8HN*+PI-  ,8HN*0PI0  ,8HN*-PI+  ,8HPRHO-   ,8HNRHO0   ,8HN PI-   ,
     *8HN*0PI-  ,8HN*-PI0  ,8HNRHO-   ,8HP PI0   ,8HN PI+   ,8HN*>PI-  ,
     *8HN*+PI0  ,8HN*0PI+  ,8HPRHO0   ,8HNRHO+   ,8HN PI0   ,8HP  PI-  ,
     *8HN*+PI-  ,8HN*0PI0  ,8HN*-PI+  ,8HPRHO-   ,8HNRHO0   ,8HP PI0   ,
     *8HN PI+   ,8HN*>PI-  ,8HN*+PI0  ,8HN*0PI+  ,8HPRHO0   ,8HNRHO+   ,
     *8HP PI-   ,8HN PI0   ,8HN*+PI-  ,8HN*0PI0  ,8HN*-PI+  ,8HPRHO-   ,
     *8HNRHO0   ,8HP PI0   ,8HN PI+   ,8HPRHO0   ,8HNRHO+   ,8HLAMK+   ,
     *8HS+ K0   ,8HS0 K+   ,8HPETA0   ,8HP PI-   ,8HN PI0   ,8HPRHO-   ,
     *8HNRHO0   ,8HLAMK0   ,8HS0 K0   ,8HS- K+   ,8HNETA/   ,8HAPPI-   ,
     *8HAPPI0   ,8HANPI-   ,8HAPPI+   ,8HANPI0   ,8HANPI+   ,8HAPPI-   ,
     *8HAN*=P0  ,8HAN*-P-  ,8HAPRHO-  ,8HAPPI0   ,8HANPI-   ,8HAN*=P+  ,
     *8HAN*-P0  ,8HAN*0P-  ,8HAPRHO0  ,8HANRHO-  ,8HAPPI+   ,8HANPI0   ,
     *8HAN*-P+  ,8HAN*0P0  ,8HAN*+P-  ,8HAPRHO+  ,8HANRHO0  ,8HANPI+   ,
     *8HAN*0P+  ,8HAN*+P0  ,8HANRHO+  ,8HAPPI0   ,8HANPI-   ,8HAN*=P+  ,
     *8HAN*-P0  ,8HAN*0P-  ,8HAPRHO0  ,8HANRHO-  ,8HAPPI+,  ,8HANPI0   ,
     *8HAN*-P+  ,8HAN*0P0  ,8HAN*+P-  /
C*****************ZKNAM4
      DATA ZKNAM4/
     *8HAPRHO+  ,8HANRHO0  ,8HPN*014  ,8HNN*=14  ,8HPI+0    ,8HPI+-    ,
     *8HPI-0    ,8HP+0     ,8HN++     ,8HP+-     ,8HP00     ,8HN+0     ,
     *8HN+-     ,8HN00     ,8HP-0     ,8HN-0     ,8HP--     ,8HPPPI0   ,
     *8HPNPI+   ,8HPNPI0   ,8HPPPI-   ,8HNNPI+   ,8HAPPPI0  ,8HAPNPI+  ,
     *8HANNPI0  ,8HANPPI-  ,8HAPNPI0  ,8HAPPPI-  ,8HANNPI-  ,8HK+PPI0  ,
     *8HK+NPI+  ,8HK0PPI0  ,8HK-PPI0  ,8HK-NPI+  ,8HAKPPI-  ,8HAKNPI0  ,
     *8HK+NPI0  ,8HK+PPI-  ,8HK0PPI0  ,8HK0NPI+  ,8HK-NPI0  ,8HK-PPI-  ,
     *8HAKNPI-  ,8HPAK0    ,8HSI+PI0  ,8HSI0PI+  ,8HSI+ETA  ,8HS+1PI0  ,
     *8HS01PI+  ,8HNEUK-   ,8HLA0PI-  ,8HSI-OM0  ,8HLA0RO-  ,8HSI0RO-  ,
     *8HSI-RO0  ,8HSI-ET0  ,8HSI0PI-  ,8HSI-0    ,8HAP PI0  ,8HAN  PI+ ,
     *8HAN*>PI+ ,8HAN*-PI0 ,8HAN*0PI- ,8HAPRHO0  ,8HAN*RHO- ,8HAN PI0  ,
     *8HAP PI+  ,8HAN*-PI+ ,8HAN*0PI0 ,8HAN*+PI- ,8HAPRHO+  ,8HAN*RHO0 ,
     *8HAA-KPI  ,8HAT02PI  ,8HAC1--K  ,8H        ,8H        ,8H        ,
     *8H        /
C*******************ZKNAM5
      DATA ZKNAM5/
     *8H        ,8H        ,8HEPI+-   ,8HEPI00   ,8HGAPI+-  ,8HGAGA*   ,
     *8HK+-     ,8HKLKS    ,8HPI+-0   ,8HEGA     ,8HLPI0    ,8HLPI     ,
     *8HAPPI0   ,8HANPI-   ,8HALAGA   ,8HANPI    ,8HALPI0   ,8HALPI+   ,
     *8HLAPI+   ,8HSI+PI0  ,8HSI0PI+  ,8HLAPI0   ,8HSI+PI-  ,8HSI-PI+  ,
     *8HLAPI-   ,8HSI-PI0  ,8HSI0PI-  ,8HTE0PI0  ,8HTE-PI+  ,8HTE0PI-  ,
     *8HTE-PI0  ,8HTE0PI   ,8HTE-PI   ,8HLAK-    ,8HALPI-   ,8HAS-PI0  ,
     *8HAS0PI-  ,8HALPI0   ,8HAS+PI-  ,8HAS-PI+  ,8HALPI+   ,8HAS+PI0  ,
     *8HAS0PI+  ,8HAT0PI0  ,8HAT+PI-  ,8HAT0PI+  ,8HAT+PI0  ,8HAT0PI   ,
     *8HAT+PI   ,8HALK+    ,8HK-PI+   ,8HK-PI+0  ,8HK0PI+-  ,8HK0PI0   ,
     *8HK-PI++  ,8HAK0PI+  ,8HK+PI--  ,8HK0PI-   ,8HK+PI-   ,8HK+PI-0  ,
     *8HAKPI-+  ,8HAK0PI0  ,8HETAPIF  ,8HK++-    ,8HK+AK0   ,8HETAPI-  ,
     *8HK--+    ,8HK-KO    ,8HPI00    ,8HPI+-    ,8HGAGA    ,8HD0PI0   ,
     *8HD0GA    ,8HD0PI+   ,8HD+PI0   ,8HDFGA    ,8HAD0PI-  ,8HD-PI0   ,
     *8HD-GA    ,8HAD0PI0  ,8HAD0GA   ,8HF+GA    ,8HF+GA    ,8HF-GA    ,
     *8HF-GA    ,8HPSPI+-  ,8HPSPI00  ,8HPSETA   ,8HE+E-    ,8HMUE+-   ,
     *8HPI+-0   ,8HM+NN    ,8HE+NN    ,8HRHO+NT  ,8HPI+ANT  ,8HK*+ANT  ,
     *8HM-NN    ,8HE-NN    ,8HRHO-NT  ,8HPI-NT   ,8HK*-NT   ,8HNUET    ,
     *8HANUET   /
C**************ZKNAM6
      DATA ZKNAM6/
     *8HNUEM    ,8HANUEM   ,8HSI+ETA  ,8HSI+ET*  ,8HPAK0    ,8HTET0K+  ,
     *8HSI*+ET  ,8HN*+AK0  ,8HN*++K-  ,8HLAMRO+  ,8HSI0RO+  ,8HSI+RO0  ,
     *8HSI+OME  ,8HPAK*0   ,8HN*+AK*  ,8HN*++K*  ,8HSI+AK0  ,8HTET0PI  ,
     *8HSI+AK*  ,8HTET0RO  ,8HSI0AK*  ,8HSI+K*-  ,8HTET0OM  ,8HTET-RO  ,
     *8HSI*0AK  ,8HC0+PI+  ,8HC0+PI0  ,8HC0+PI-  ,8HA+GAM   ,8HA0GAM   ,
     *8HTET0AK  ,8HTET0K*  ,8HOM-RO+  ,8HOM-PI+  ,8HC1++AK  ,8HA+PI+   ,
     *8HC0+AK0  ,8HA0PI+   ,8HA+AK0   ,8HT0PI+   ,8HASI-ET  ,8HASI-E*  ,
     *8HAPK0    ,8HATET0K  ,8HASI*-E  ,8HAN*-K0  ,8HAN*--K  ,8HALAMRO  ,
     *8HASI0RO  ,8HASI-RO  ,8HASI-OM  ,8HAPK*0   ,8HAN*-K*  ,8HAN*--K  ,
     *8HASI-K0  ,8HATETPI  ,8HASI-K*  ,8HATETRO  ,8HASI0K*  ,8HASI-K*  ,
     *8HATE0OM  ,8HATE+RO  ,8HASI*0K  ,8HAC-PI-  ,8HAC-PI0  ,8HAC-PI+  ,
     *8HAA-GAM  ,8HAA0GAM  ,8HATET0K  ,8HATE0K*  ,8HAOM+RO  ,8HAOM+PI  ,
     *8HAC1--K  ,8HAA-PI-  ,8HAC0-K0  ,8HAA0PI-  ,8HAA-K0   ,8HAT0PI-  ,
     *8HC1++GA  ,8HC1++GA  ,8HC10GAM  ,8HS+GAM   ,8HS0GAM   ,8HT0GAM   ,
     *8HXU++GA  ,8HXD+GAM  ,8HXS+GAM  ,8HA+AKPI  ,8HT02PI+  ,8HC1++2K  ,
     *8HAC1--G  ,8HAC1-GA  ,8HAC10GA  ,8HAS-GAM  ,8HAS0GAM  ,8HAT0GAM  ,
     *8HAXU--G  ,8HAXD-GA  ,8HAXS-GA  ,8H        /
C*****************WT
      DATA WT/
     *1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,
     *1.0000,1.0000,0.2100,0.1300,0.2700,0.3900,1.0000,1.0000,0.6300,
     *0.2100,0.0200,0.0600,0.0300,0.0500,0.6300,0.2100,0.0200,0.0600,
     *0.0300,0.0500,0.6400,0.3600,0.0   ,0.0   ,0.6400,0.3600,0.0   ,
     *0.0   ,0.6900,0.3100,1.0000,0.5200,0.4800,1.0000,0.9900,0.0100,
     *0.3800,0.0300,0.3000,0.2400,0.0500,1.0000,1.0000,0.0   ,1.0000,
     *0.9000,0.0100,0.0900,0.3300,0.6700,0.3300,0.6700,0.3300,0.6700,
     *0.3300,0.6700,0.3300,0.6700,0.3300,0.6700,0.3300,0.6700,0.3300,
     *0.6700,0.1900,0.3800,0.0900,0.2000,0.0300,0.0400,0.0500,0.0200,
     *0.1900,0.3800,0.0900,0.2000,0.0300,0.0400,0.0500,0.0200,0.1900,
     *0.3800,0.0900,0.2000,0.0300,0.0400,0.0500,0.0200,0.1900,0.3800,
     *0.0900,0.2000,0.0300,0.0400,0.0500,0.0200,0.8800,0.0600,0.0600,
     *0.8800,0.0600,0.0600,0.8800,0.1200,0.1900,0.1900,0.1600,0.1600,
     *0.1700,0.0300,0.0300,0.0300,0.0400,0.1000,0.1000,0.2000,0.1200,
     *0.1000,0.0400,0.0400,0.0500,0.0750,0.0750,0.0300,0.0300,0.0400/
      DATA WT1/
     *0.5000,0.5000,0.5000,0.5000,1.0000,0.6700,0.3300,0.3300,0.6700,
     *1.0000,0.2500,0.2700,0.1800,0.3000,0.1700,0.0800,0.1800,0.0300,
     *0.2400,0.2000,0.1000,0.0800,0.1700,0.2400,0.0300,0.1800,0.1000,
     *0.2000,0.2500,0.1800,0.2700,0.3000,0.225 ,0.375 ,0.15  ,0.0938,
     *0.0562,0.0375,0.0626,0.225 ,0.375 ,0.15  ,0.0938,0.0562,0.0375,
     *0.0625,0.1800,0.3700,0.1300,0.0800,0.0400,0.0700,0.1300,0.3700,
     *0.1800,0.0400,0.0800,0.1300,0.1300,0.0700,0.0700,0.1300,0.2300,
     *0.4700,0.0500,0.0200,0.0100,0.0200,0.1300,0.0700,0.4700,0.2300,
     *0.0500,0.0100,0.0200,0.0200,1.0000,0.6700,0.3300,0.3300,0.6700,
     *1.0000,0.2500,0.2700,0.1800,0.3000,0.1700,0.0800,0.1800,0.0300,
     *0.2400,0.2000,0.1000,0.0800,0.1700,0.2400,0.0300,0.1800,0.1000,
     *0.2000,0.2500,0.1800,0.2700,0.3000,0.1800,0.3700,0.1300,0.0800,
     *0.0400,0.0700,0.1300,0.3700,0.1800,0.0400,0.0800,0.1300,0.1300,
     *0.0700,0.5000,0.5000,1.0000,1.0000,1.0000,0.8000,0.2000,0.6000,
     *0.3000,0.1000,0.6000,0.3000,0.1000/
C************WT2
      DATA WT2/
     *0.8000,0.2000,0.3300,0.6700,0.6600,0.1700,0.1700,0.3200,0.1700,
     *0.3200,0.1900,0.3300,0.3300,0.3400,0.3000,0.0500,0.6500,0.3800,
     *0.1200,0.3800,0.1200,0.3800,0.1200,0.3800,0.1200,0.3000,0.0500,
     *0.6500,0.3800,0.2500,0.2500,0.0200,0.0500,0.0500,0.2000,0.2000,
     *0.1200,0.1000,0.0700,0.0700,0.1400,0.0500,0.0500,0.225 ,0.375 ,
     *0.15  ,0.0938,0.0562,0.0375,0.0625,0.225 ,0.375 ,0.15  ,0.0938,
     *0.0562,0.0375,0.0625,0.3000,0.3000,0.4000,1.0   ,1.0   ,1.0   ,
     *1.0000/
C***************WT4
      DATA WT4/
     *1.0000,1.0000,0.0200,0.0300,0.0700,0.0200,0.0200,0.0400,0.1300,
     *0.0700,0.0600,0.0600,0.2000,0.1400,0.0400,0.1000,0.2500,0.0300,
     *0.3000,0.4200,0.2200,0.3500,0.1900,0.1600,0.0800,1.0000,1.0000,
     *1.0000,1.0000,1.0000,0.3700,0.2000,0.3600,0.0700,0.5000,0.5000,
     *0.5000,0.5000,0.5000,0.5000,0.0200,0.0300,0.0700,0.0200,0.0200,
     *0.0400,0.1300,0.0700,0.0600,0.0600,0.2000,0.1400,0.0400,0.1000,
     *0.2500,0.0300,0.3000,0.4200,0.2200,0.3500,0.1900,0.1600,0.0800,
     *1.0000,1.0000,1.0000,1.0000,1.0000,0.3700,0.2000,0.3600,0.0700,
     *0.5000,0.5000,0.5000,0.5000,0.5000,0.5000,1.0000,1.0000,1.0000,
     *1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,0.3000,0.3000,0.4000,
     *1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,
     *1.    /
C****************WT3
      DATA WT3/
     *1.0   ,1.0   ,0.4800,0.2400,0.2600,0.0200,0.4700,0.3500,0.1500,
     *0.0300,1.0000,1.0000,0.5200,0.4800,1.0000,1.0000,1.0000,1.0000,
     *0.9000,0.0500,0.0500,0.9000,0.0500,0.0500,0.9000,0.0500,0.0500,
     *0.3300,0.6700,0.6700,0.3300,0.2500,0.2500,0.5000,0.9000,0.0500,
     *0.0500,0.9000,0.0500,0.0500,0.9000,0.0500,0.0500,0.3300,0.6700,
     *0.6700,0.3300,0.2500,0.2500,0.5000,0.1000,0.5000,0.1600,0.2400,
     *0.7000,0.3000,0.7000,0.3000,0.1000,0.5000,0.1600,0.2400,0.3000,
     *0.4000,0.3000,0.3000,0.4000,0.3000,0.4900,0.4900,0.0200,0.5500,
     *0.4500,0.6800,0.3000,0.0200,0.6800,0.3000,0.0200,0.5500,0.4500,
     *0.9000,0.1000,0.9000,0.1000,0.6000,0.3000,0.1000,0.1000,0.1000,
     *0.8000,0.2800,0.2800,0.3500,0.0700,0.0200,0.2800,0.2800,0.3500,
     *0.0700,0.0200,1.0000,1.0000/
C************NZK1
      DATA NZK01/
     *  1,  2,  3,  4,  5,  6,  7,  1,  2,  4,  3, 23, 13, 13, 13,
     * 10, 11, 10, 13, 13, 13, 10,  4, 11, 14, 14, 14, 11,  3,  1,
     *  8,  1,  1,  2,  9,  2,  2, 13, 23,  8,  1,  8, 17,  7,  7,
     *  7, 23, 23, 13, 13, 13, 13, 23, 14, 13, 13, 23, 15, 24, 24,
     * 15, 16, 25, 25, 16, 15, 24, 24, 15, 16, 24, 25, 16, 15, 24,
     * 36, 37, 15, 24, 15, 15, 24, 15, 37, 36, 24, 15, 24, 24, 16,
     * 24, 38, 39, 16, 25, 16, 16, 25, 16, 39, 38, 25, 16, 25, 25,
     * 17, 22, 21, 17, 21, 20, 17, 22,  8,  1, 21, 22, 20, 17, 48,
     * 50, 49,  8,  1, 17, 17, 17, 21, 20, 22, 17, 22, 21, 20, 22,
     * 19, 12, 19, 12,  1,  1,  8,  1,  8,  8,  1, 53, 54,  1,  1,
     *  8, 53, 54, 55,  1,  8,  1,  8, 54, 55, 56,  1,  8,  8, 55,
     * 56,  8,  1,  8, 53, 54, 55,  1,  8,  8,  1, 54, 55, 56,  1,
     *  8,  1,  8, 53, 54, 55,  1,  8,  1,  8, 54, 55, 56,  1,  8/
      DATA NZK1/
     *  1,  8,  1,  8, 17, 21, 22,  1,  1,  8,  1,  8, 17, 22, 20,
     *  8,  2,  2,  9,  2,  9,  9,  2, 67, 68,  2,  2,  9, 67, 68,
     * 69,  2,  9,  2,  9, 68, 69, 70,  2,  9,  9, 69, 70,  9,  2,
     *  9, 67, 68, 69,  2,  9,  2,  9, 68, 69, 70,  2,  9,  1,  8,
     * 13, 13, 14,  1,  8,  1,  1,  8,  8,  8,  1,  8,  1,  1,  1,
     *  1,  1,  8,  2,  2,  9,  9,  2,  2,  9, 15, 15, 24, 16, 16,
     * 25, 25, 15, 15, 24, 24, 16, 16, 25,  1, 21, 22, 21, 48, 49,
     *  8, 17, 20, 17, 22, 20, 20, 22, 20,  2,  9, 67, 68, 69,  2,
     *  9,  9,  2, 68, 69, 70,  2,  9,150,157,152,  0,  0,  0,  0,
     *  0,  0, 31, 31, 13,  7, 15, 12, 13, 31, 17, 17,  2,  9, 18,
     *  9, 18, 18, 17, 21, 22, 17, 21, 20, 17, 20, 22, 97, 98, 97,
     * 98, 97, 98, 17, 18, 99,100, 18,101, 99, 18,101,100,102,103,
     *102,103,102/
C************NZK11
      DATA NZK11/
     *103, 18, 16, 16, 24, 24, 16, 25, 15, 24, 15, 15, 25, 25, 31,
     * 15, 15, 31, 16, 16, 23, 13,  7,116,116,116,117,117,119,118,
     *118,119,119,120,120,121,121,130,130,130,  4, 10, 13, 10,  4,
     * 32, 13, 36, 11,  3, 34, 14, 38,133,134,135,136, 21, 21,  1,
     * 97,104, 54, 53, 17, 22, 21, 21,  1, 54, 53, 21, 97, 21, 97,
     * 22, 21, 97, 98,105,137,137,137,138,139, 97, 97,109,109,140,
     *138,137,139,138,145, 99, 99,  2,102,110, 68, 67, 18,100, 99,
     * 99,  2, 68, 67, 99,102, 99,102,100, 99,102,103,111,149,149,
     *149,150,151,113,113,115,115,152,150,149,151,150,157,140,141,
     *142,143,144,145,146,147,148,138,145,140,152,153,154,155,156,
     *157,158,159,160,  0/
C************NZK2
      DATA NZK02/
     *  0,  0,  0,  0,  0,  0,  0,  3,  4,  6,  5, 23, 14, 11,  3,
     *  5,  5,  5, 23, 13, 23, 23, 23,  5, 23, 13, 23, 23, 23, 14,
     * 23,  3, 11, 13, 23,  4, 10, 14, 23, 14, 23, 13,  7,  7,  4,
     *  7,  7, 23, 14, 14, 23, 14, 23, 23, 14, 14,  7, 23, 13, 23,
     * 14, 23, 14, 23, 13, 23, 13, 23, 14, 23, 14, 23, 13, 23, 13,
     * 23, 13, 33, 32, 35, 31, 23, 14, 23, 14, 33, 34, 35, 31, 23,
     * 14, 23, 14, 33, 34, 35, 31, 23, 13, 23, 13, 33, 32, 35, 31,
     * 13, 13, 23, 23, 14, 13, 14, 14, 25, 16, 14, 23, 13, 31, 14,
     * 13, 23, 25, 16, 23, 35, 33, 34, 32, 33, 31, 31, 14, 13, 23,
     *  0,  0,  0,  0, 13, 23, 13, 14, 23, 14, 13, 23, 13, 78, 23,
     * 13, 14, 23, 13, 79, 78, 14, 23, 14, 23, 13, 80, 79, 14, 14,
     * 23, 80, 23, 13, 14, 23, 13, 79, 78, 23, 14, 14, 23, 13, 80,
     * 79, 23, 13, 14, 23, 13, 79, 78, 14, 23, 14, 23, 13, 80, 79/
      DATA NZK2/
     * 23, 13, 33, 32, 15, 24, 15, 31, 14, 23, 34, 33, 24, 24, 15,
     * 31, 14, 23, 14, 13, 23, 13, 14, 23, 14, 80, 23, 14, 13, 23,
     * 14, 79, 80, 13, 23, 13, 23, 14, 78, 79, 13, 13, 23, 78, 23,
     * 14, 13, 23, 14, 79, 80, 13, 23, 13, 23, 14, 78, 79, 62, 61,
     * 23, 14, 23, 13, 13, 13, 23, 13, 13, 23, 14, 14, 14,  1,  8,
     *  8,  1,  8,  1,  8,  8,  1,  8,  1,  8,  1,  8,  1,  1,  8,
     *  1,  8,  8,  1,  1,  8,  8,  1,  8, 25, 23, 13, 31, 23, 13,
     * 16, 14, 35, 34, 34, 33, 31, 14, 23, 23, 14, 13, 23, 14, 79,
     * 80, 23, 13, 13, 23, 14, 78, 79, 24, 14, 24,  0,  0,  0,  0,
     *  0,  0, 13, 23, 14,  7, 16, 19, 14,  7, 23, 14, 23, 14,  7,
     * 13, 23, 13, 13, 23, 13, 23, 14, 13, 14, 23, 14, 23, 13, 14,
     * 23, 14, 23, 16, 14, 23, 14, 23, 14, 13, 13, 23, 13, 23, 14,
     * 13, 23, 13/
C************NZK22
      DATA NZK22/
     * 23, 15, 13, 13, 13, 23, 13, 13, 14, 14, 14, 14, 14, 23, 13,
     * 16, 25, 14, 15, 24, 23, 14,  7, 23,  7, 13, 23,  7, 14, 23,
     *  7, 23,  7,  7,  7,  7,  7, 13, 23, 31,  3, 11, 14,135,  5,
     *134,134,134,136,  6,133,133,133,  0,  0,  0,  0, 31, 95, 25,
     * 15, 31, 95, 16, 32, 32, 33, 35, 39, 39, 38, 25, 13, 39, 32,
     * 39, 38, 35, 32, 39, 13, 23, 14,  7,  7, 25, 37, 32, 13, 25,
     * 13, 25, 13, 25, 13, 31, 95, 24, 16, 31, 24, 15, 34, 34, 33,
     * 35, 37, 37, 36, 24, 14, 37, 34, 37, 36, 35, 34, 37, 14, 23,
     * 13,  7,  7, 24, 39, 34, 14, 24, 14, 24, 14, 24, 14,  7,  7,
     *  7,  7,  7,  7,  7,  7,  7, 25, 13, 25,  7,  7,  7,  7,  7,
     *  7,  7,  7,  7,  0/
C************NZK3
      DATA NZK03/
     *  0,  0,  0,  0,  0,  0,  0,  5,  6,  5,  6, 23, 23,  5,  5,
     *  0,  0,  0,  0, 14, 23,  5,  5,  0,  0, 14, 23,  5,  5,  0,
     *  0,  5,  5,  0,  0,  5,  5,  0,  0,  0,  0,  0,  0,  0,  3,
     *  0,  7, 23, 23,  7,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0/
      DATA NZK3/
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0, 23, 13, 14, 23, 23, 14, 23, 23, 23, 14, 23, 13,
     * 23, 14, 13, 23, 13, 23, 14, 23, 14, 14, 23, 13, 13, 23, 13,
     * 14, 23, 23, 14, 23, 13, 23, 14, 14,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0, 14, 14, 24,  0,  0,  0,  0,
     *  0,  0, 14, 23,  7,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0/
      DATA NZK33/
     *  0,  0,  0, 23, 14,  0, 13,  0, 14,  0,  0, 23, 13,  0,  0,
     * 15,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0, 14, 23,  0,  0,  0, 23,134,134,
     *  0,  0,  0,133,133,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0,  0,  0, 13, 13, 25,  0,  0,  0,  0,  0,
     *  0,  0,  0,  0,  0/
      DATA XDQ/0./
      DATA XDQ1/0./
      DATA IFL/1,2,3,4,5,6,7,8,9,10,11,12/
      DATA PUDSC/0.43,0.86,1.,1./
      DATA ALPHAR/0.5/
      DATA ALPHAN/-0.5/
      DATA ALPHAF/0./
      DATA ALPHA/0.5,0.5,0.,0./
      DATA ISYS,IDGB,DQFRAG,RM/6,-1,0.5,0.764/
      DATA RDI,BSLOP,PTMIN/1.2,8.0,0./
      END

      SUBROUTINE DATESS
      COMMON/DECAYC/ZKNAME(533),NZK(533,3),WT(533)
      COMMON/PART/ANAME(180),AM(180),GA(180),TAU(180),ICH(180),IBAR(180)
     *,K1(180),K2(180)
      COMMON/METLSP/ IS,ITS(100000),CXS(100000),CYS(100000),CZS(100000),
     *ELS(100000),PLS(100000)
      COMMON/FINLSP/IR,ITR(100),CXR(100),CYR(100),CZR(100),ELR(100),
     *PLR(100)
      DIMENSION ICHAR(180)
      EQUIVALENCE (ICH(1),ICHAR(1))
      DIMENSION Z(3)
*      DIMENSION IREACT(24),HSI(31)
      REAL*8 ZKNAME
      REAL*8 ANAME
      REAL*8 Z
*      DATA IREACT/13,1,13,8,14,1,14,8,15,1,15,8,16,1,16,8,1,1,1,8,2,1,2,
*     *8/
      I12=12
*      PRINT 102
  102 FORMAT(///,' TABLE OF USED PARTICLES AND RESONANCES (I)',//
     *' I = NUMBER OF PARTICLE OR RESONANCE',/
     *' ANAME = NAME OF I'/,
     *' AM = MASS OF I  (GEV)',/
     *' GA = WIDTH OF I (GEV)',/
     *' TAU = LIFE TIME OF I  (SEC.)',/
     *' ICH = ELECTRIC CHARGE OF I, IBAR = BARYONIC CHARGE OF I',/' ',
     *' K1 = FIRST DECAY CHANNEL NUMBER, K2 = LAST DECAY CHANNEL NUMBER'
     *,' OF I')
      JOO=180
      DO 41 I=1,JOO
   41 CONTINUE
*      PRINT 111
*      PRINT 92
   92 FORMAT(///' DECAY CHANNELS OF PARTICLES  AND RESONANCES',//)
*      PRINT 93
   93 FORMAT(' ANAME = PARTICLE AND RESONANCE NAME'/,
     *' DNAME = DECAY CHANNEL NAME'/,
     *' J = DECAY CHANNEL NUMBER'/,
     *' I = NUMBER OF DECAYING PARTICLE'/,
     *' WT = SUM OF DECAY CHANNEL WEIGHTS FROM K1(I) UP TO J'/,
     *' NZK = PROGRAM INTERNKAL NUMBERS OF DECAY PRODUCTS')
      DO 2 I=1,JOO
      IK1=K1(I)
      IK2=K2(I)
      IF(IK1.LE.0) GO TO 2
      DO 3 IK=IK1,IK2
      I1=NZK(IK,1)
      I2=NZK(IK,2)
      I3=NZK(IK,3)
      IF(I1.LE.0) I1=29
      IF(I2.LE.0) I2=29
      IF(I3.LE.0) I3=29
      J1=I1
      J2=I2
      J3=I3
      Z(1)=ANAME(I1)
      Z(2)=ANAME(I2)
      Z(3)=ANAME(I3)
      AMTEST= AM(I)-AM(J1)-AM(J2)-AM(J3)
      IBTEST=IBAR(I)-IBAR(J1)-IBAR(J2)-IBAR(J3)
      ICTEST=ICHAR(I)-ICHAR(J1)-ICHAR(J2)-ICHAR(J3)
      IF(AMTEST) 51,52,52
   51 MTEST=1
      GO TO 53
   52 MTEST=0
   53 CONTINUE
    3 CONTINUE
    2 CONTINUE
      RETURN
      END

      FUNCTION DSIG_COL(T)
      COMMON/ab/ aelm, betav, sigma_tot, parB, rho
      G4=((1+abs(T)/0.71)**(-2))**4 
      pkoef= 10./(5.0677**2)        !/10.  
      DSIG_COL=4*3.1416*aelm**2*G4*pkoef/((betav * T)**2)
      RETURN 
      END

      Function  DSIG_INTER(T)
      COMMON/ab/ aelm, betav, sigma_tot, parB, rho
      G2=((1+abs(T)/0.71)**(-2))**2
      DSIG_INTER=aelm*sigma_tot*G2*exp(0.5*parB*T)*
     & sqrt(1+rho**2)/ betav/abs(T)
      RETURN
      END

      Function  DSIG_INT_Ex(T)
      COMMON/ab/ aelm, betav, sigma_tot, parB, rho
      G2=((1+abs(T)/0.71)**(-2))**2
      delT=aelm*(0.577+log(parB*abs(T)/2.+ 4*abs(T)/0.71)+
     &  4.*abs(T)/0.71*log(4.*abs(T)/0.71)+2.*abs(T)/0.71)
      DSIG_INT_Ex=aelm*sigma_tot*G2*exp(0.5*parB*T)*
     & (rho*cos(delT) + sin(delT))/ betav/abs(T)
      RETURN
      END


      FUNCTION SIG_HADi(T)
      COMMON/UZHI/SqrtS,Ecms,Vcms,Gamma,Proc_Prob(7),P_5str,CS_in,
     ,            CS_el,A1,T1,A2,T2,A3,Tmax,Tmin,Weight1
      
      pk=0.1/(5.0677**2)     !normir 
      SIG_HADi= A1*T1*exp(T/T1)+A1*(A2**2)*T2*exp(T/T2)-
     - 4.*A1*A2*T1*T2/(T1+T2)*exp(T*(T1+T2)/(2.*T1*T2))+
     + A3*T2*exp(T/T2)                                     

       RETURN 
       END


      FUNCTION DSIG_HAD(T)
      COMMON/UZHI/SqrtS,Ecms,Vcms,Gamma,Proc_Prob(7),P_5str,CS_in,
     ,            CS_el,A1,T1,A2,T2,A3,Tmax,Tmin,Weight1
      COMMON/ab/ aelm, betav, sigma_tot, parB, rho
       pk=(5.0677**2)/10.     !normirovka  
      DSIG_HAD=sigma_tot**2*(1+rho**2)*exp(parB*T)/16./3.1416*pk
      RETURN
      END
