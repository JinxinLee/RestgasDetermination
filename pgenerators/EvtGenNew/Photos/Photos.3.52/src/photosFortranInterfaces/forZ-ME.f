      function phwtnlo(xdumm) 
C.----------------------------------------------------------------------
C.
C.    PHWTNLO:   PHotosWTatNLO
C.
C.    Purpose:  calculates instead of interference weight
C.              complete NLO weight for vector boson decays
C.              of pure vector (or pseudovector) couplings
C.              Proper orientation of beams required.
C.              This is not standard in PHOTOS.
C.              At NLO more tuning than in standard is needed.
C.              Works with KORALZ and KKMC. 
C.              Note some commented out commons from MUSTAAL, KORALZ
C.
C.    Input Parameters:   Common /PHOEVT/
C.
C.    Output Parameters:  Function value
C.
C.    Author(s):  Z. Was                          Created at:  08/12/05
C.                                                Last Update: 
C.
C.----------------------------------------------------------------------

      IMPLICIT NONE 
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      COMMON/PHOEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      real*8 phep,vhep
      INTEGER NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      SAVE  /PHOEVT/
C      COMMON / UTIL3 / xFIG,xFI,IT,IBREX
      DOUBLE PRECISION COSTHG,SINTHG
      REAL*8 XPHMAX,XPHOTO
      COMMON/PHOPHS/XPHMAX,XPHOTO,COSTHG,SINTHG
C fi3 orientation of photon, fi1,th1 orientation of neutral
      REAL*8 FI3,fi1,th1
      COMMON /PHOREST/ FI3,fi1,th1
      REAL*8 BETA,WT1,WT2,WT3
      COMMON /PHWT/ BETA,WT1,WT2,WT3
      REAL*8 PROBH,CORWT,XF
      COMMON/PHOPRO/PROBH,CORWT,XF,IREP
      REAL*8 PI
      DATA PI /3.141592653589793238462643D0/
      REAL*8  QP(4),QM(4),PH(4),QQ(4),PP(4),PM(4),QQS(4)
      REAL*8 s,c,svar,xkaM,xkaP,xk,phwtnlo,xdumm,PHINT
      REAL*8 ENE,a,t,u,t1,u1,wagan2,waga,PHASYZ,BT,BU,ENEB
      INTEGER IBREM,K,L,IREP,IDUM
      integer icont,ide,idf
      REAL*8 delta
      REAL*8 PNEUTR,MCHSQR,MNESQR
      COMMON/PHOMOM/MCHSQR,MNESQR,PNEUTR(5)

*/////////////////////
!        call phlupa(299 500)


*/////////////////////
!        call phlupa(299 500)

        XK=2*PHEP(4,nhep)/PHEP(4,1)

!        XK=2*PHEP(4,nhep)/PHEP(4,1)/xphmax  ! it is not used becuse here
                                             ! order of emissions is meaningless
      IF(NHEP.LE.4) XK=0.0D0
C the mother must be Z or gamma*  !!!!
      
      IF (XK.GT.1d-10.AND.(IDHEP(1).EQ.22.OR.IDHEP(1).EQ.23)) THEN

!        write(*,*) 'nhep=',nhep
!      DO K=1,3 ENDDO
!      IF (K.EQ.1) IBREM= 1
!      IF (K.EQ.2) IBREM=-1
!      ICONT=ICONT+1
!      IBREM=IBREX        ! that will be input parameter.
!      IBREM=IBREY        ! that IS now   input parameter.

C We initialize twice 4-vectors, here and again later after boost 
C must be the same way. Important is how the reduction procedure will work.
C It seems at present that the beams must be translated to be back to back.
C this may be done after initialising, thus on 4-vectors.

        DO K=1,4
         PP(K)=PHEP(K,1)
         PM(K)=PHEP(K,2)
         QP(K)=PHEP(K,3)
         QM(K)=PHEP(K,4)
         PH(K)=PHEP(K,nhep)
         QQ(K)=0.0
         QQS(K)=QP(K)+QM(K)
        ENDDO


        PP(4)=(PHEP(4,1)+PHEP(4,2))/2
        PM(4)=(PHEP(4,1)+PHEP(4,2))/2
        PP(3)= PP(4)
        PM(3)=-PP(4)
        
         DO L=5,NHEP-1
         DO K=1,4
          QQ(K)=QQ(K)+ PHEP(K,L)
         QQS(K)=QQS(K)+ PHEP(K,L)
         ENDDO
        ENDDO        

C go to the restframe of 3        
        CALL PHOB(1,QQS,qp)
        CALL PHOB(1,QQS,qm)
        CALL PHOB(1,QQS,QQ)
        ENE=(qp(4)+qm(4)+QQ(4))/2

C preserve direction of emitting particle and wipeout QQ 
        IF (IREP.EQ.1) THEN
         a=sqrt(ENE**2-PHEP(5,3)**2)/sqrt(qm(4)**2-PHEP(5,3)**2)
         qm(1)= qm(1)*a
         qm(2)= qm(2)*a
         qm(3)= qm(3)*a
         qp(1)=-qm(1)
         qp(2)=-qm(2)
         qp(3)=-qm(3)
        ELSE
         a=sqrt(ENE**2-PHEP(5,3)**2)/sqrt(qp(4)**2-PHEP(5,3)**2)
         qP(1)= qP(1)*a
         qP(2)= qP(2)*a
         qP(3)= qP(3)*a
         qm(1)=-qP(1)
         qm(2)=-qP(2)
         qm(3)=-qP(3)
        ENDIF
         qp(4)=ENE
         qm(4)=ENE
C go back to reaction frame (QQ eliminated) 
        CALL PHOB(-1,QQS,qp)
        CALL PHOB(-1,QQS,qm)
        CALL PHOB(-1,QQS,QQ)
C IBREM is spurious but it numbers branches of MUSTRAAL

        IBREM=1
       IF (IREP.EQ.1)  IBREM=-1

        svar=PHEP(4,1)**2

C we calculate C and S, note that TH1 exists in MUSTRAAL as well. 

        C=COS(TH1)
C from off line application we had:
        IF(IBREM.EQ.-1) C=-C
C ... we need to check it. 
        s=sqrt(1D0-C**2)

        IF (IBREM.EQ.1) THEN
         xkaM=(qp(4)*PH(4)-qp(3)*PH(3)-qp(2)*PH(2)-qp(1)*PH(1))/XK
         xkaP=(qM(4)*PH(4)-qM(3)*PH(3)-qM(2)*PH(2)-qM(1)*PH(1))/XK
        ELSE
         xkap=(qp(4)*PH(4)-qp(3)*PH(3)-qp(2)*PH(2)-qp(1)*PH(1))/XK
         xkaM=(qM(4)*PH(4)-qM(3)*PH(3)-qM(2)*PH(2)-qM(1)*PH(1))/XK
        ENDIF    

!        XK=2*PHEP(4,nhep)/PHEP(4,1)/xphmax   ! it is not used becuse here
                                              ! order of emissions is meaningless
!
!        DELTA=2*PHEP(5,4)**2/svar/(1+(1-XK)**2)*(xKAP/xKAM+xKAM/xKAP)
!        waga=SVAR/4./xkap
!        waga=waga*(1.D0-COSTHG*BETA) ! sprawdzone 1= svar/xKAp/4   * (1.D0-COSTHG*BETA)
!        waga=waga*(1-delta) /wt2 ! sprawdzone ze to jest =2/(1.D0+COSTHG*BETA)
!                                 ! czyli ubija de-interferencje
 

C this is true only for intermediate resonances with afb=0!
        t =2*(qp(4)*PP(4)-qp(3)*PP(3)-qp(2)*PP(2)-qp(1)*PP(1))
        u =2*(qM(4)*PP(4)-qM(3)*PP(3)-qM(2)*PP(2)-qM(1)*PP(1))
        u1=2*(qp(4)*Pm(4)-qp(3)*Pm(3)-qp(2)*Pm(2)-qp(1)*Pm(1))
        t1=2*(qM(4)*Pm(4)-qM(3)*Pm(3)-qM(2)*Pm(2)-qM(1)*Pm(1))

C basically irrelevant lines  ...
        t =t - (qp(4)**2-qp(3)**2-qp(2)**2-qp(1)**2)
        u =u - (qm(4)**2-qm(3)**2-qm(2)**2-qm(1)**2)
        u1=u1- (qp(4)**2-qp(3)**2-qp(2)**2-qp(1)**2)
        t1=t1- (qm(4)**2-qm(3)**2-qm(2)**2-qm(1)**2)




      call GETIDEIDF(IDE,IDF)   ! we adjust to what is f-st,s-nd beam flavour 
       IF (IDE*IDHEP(3).GT.0) THEN
        BT=1+PHASYZ(SVAR)
        BU=1-PHASYZ(SVAR)
       ELSE
        BT=1-PHASYZ(SVAR)
        BU=1+PHASYZ(SVAR)
       ENDIF  
        wagan2=2*(BT*t**2+BU*u**2+BT*t1**2+BU*u1**2)
     $        /(1+(1-xk)**2)* 2.0/(BT*(1-c)**2+BU*(1+c)**2)/svar**2

!        waga=waga*wagan2
!        waga=waga*(1-delta) /wt2 ! sprawdzone ze to jest =2/(1.D0+COSTHG*BETA)
        waga=2/(1.D0+COSTHG*BETA)*wagan2  
!     %       * SVAR/4./xkap*(1.D0-COSTHG*BETA)*sqrt(1.0-xk)

        phwtnlo=waga
        IF(wagan2.gt.3.8) THEN
!         write(*,*) 'phwtnlo= ',phwtnlo
         write(*,*) 'idhepy= ',IDHEP(1),IDHEP(2),IDHEP(3),IDHEP(4),IDHEP(5)
         write(*,*) 'IDE=',IDE,'  IDF=',IDF
         write(*,*) 'bt,bu,bt+bu= ',bt,bu,bt+bu
         call PHODMP
         write(*,*) ' '
         write (*,*) IREP,IBREM, '<-- IREP,IBREM '
         write(*,*) 'pneutr= ',pneutr
         write(*,*) 'qp    = ',qp
         write(*,*) 'qm    = ',qm
         write(*,*) ' '
         write(*,*) 'ph    = ',ph
         write(*,*) 'p1= ',PHEP(1,1),PHEP(2,1),PHEP(3,1),PHEP(4,1)
         write(*,*) 'p2= ',PHEP(1,2),PHEP(2,2),PHEP(3,2),PHEP(4,2)
         write(*,*) 'p3= ',PHEP(1,3),PHEP(2,3),PHEP(3,3),PHEP(4,3)
         write(*,*) 'p4= ',PHEP(1,4),PHEP(2,4),PHEP(3,4),PHEP(4,4)
         write(*,*) 'p5= ',PHEP(1,5),PHEP(2,5),PHEP(3,5),PHEP(4,5)

         write (*,*) ' c= ',c,' theta= ',th1
!         write(*,*)  'photos waga daje ... IBREM=',IBREM,' waga=',waga
!         write(*,*) 'xk,COSTHG,c',xk,COSTHG,c
!         write(*,*) SVAR/4./xkap*(1.D0-COSTHG*BETA), 
!     $   (1-delta) /wt2 *(1.D0+COSTHG*BETA)/2, wagan2
!         write(*,*) ' delta, wt2,beta',  delta, wt2,beta
         write(*,*) '  -  '
         write(*,*) 't,u       = ',t,u
         write(*,*) 't1,u1     = ',t1,u1
         write(*,*) 'sredniaki = ',svar*(1-c)/2
     $                            ,svar*(1+c)/2
!         write(*,*) 'xk= ',xk,' c= ',c ,' COSTHG= ' ,COSTHG
         write(*,*) 'PHASYZ(SVAR)=',PHASYZ(SVAR),' svar= ',svar,' waga=',waga
         write(*,*) '  -  '
         write(*,*) 'BT-part= ',2*(BT*t**2+BT*t1**2)
     $        /(1+(1-xk)**2)* 2.0/(BT*(1-c)**2)/svar**2,
     $ ' BU-part= ',2*(BU*u**2+BU*u1**2)
     $        /(1+(1-xk)**2)* 2.0/(BU*(1+c)**2)/svar**2
         write(*,*) 'BT-part*BU-part= ',2*(BT*t**2+BT*t1**2)
     $        /(1+(1-xk)**2)* 2.0/(BT*(1-c)**2)/svar**2
     $         *2*(BU*u**2+BU*u1**2)
     $        /(1+(1-xk)**2)* 2.0/(BU*(1+c)**2)/svar**2, 'wagan2=',wagan2

         write(*,*) 'wagan2=',wagan2
         write(*,*) ' ###################  '
         wagan2=3.8  ! overwrite 
         waga=2/(1.D0+COSTHG*BETA)*wagan2  
!     %       * SVAR/4./xkap*(1.D0-COSTHG*BETA)*sqrt(1.0-xk)

        phwtnlo=waga

        ENDIF
      ELSE
C in other cases we just use default setups.
        phwtnlo= PHINT(IDUM)
      ENDIF
      end
      
      FUNCTION PHASYZ(SVAR)
C.----------------------------------------------------------------------
C.
C.    PHWTNLO:   PHotosASYmmetryofZ
C.
C.    Purpose:  Calculates born level asymmetry for Z
C.              between distributions (1-C)**2 and (1+C)**2
C.              At present dummy, requrires effective Z and gamma 
C.              Couplings and also spin polarization states
C.              For initial and final states.
C.              To be correct this function need to be tuned
C.              to host generator. Axes orientation polarisation
C.              conventions etc etc. 
C.              Modularity of PHOTOS would break. 
C.
C.    Input Parameters:   SVAR
C.
C.    Output Parameters:  Function value
C.
C.    Author(s):  Z. Was                          Created at:  10/12/05
C.                                                Last Update: 
C.
C.----------------------------------------------------------------------

      IMPLICIT NONE
      real*8 PHASYZ,SVAR,AFB,AFBCALC
      INTEGER IDE,IDF,IDEE,IDFF,GETIDEE
      call GETIDEIDF(IDE,IDF)
      IDEE=abs(GETIDEE(IDE))
      IDFF=abs(GETIDEE(IDF))
      AFB= -AFBCALC(SVAR,IDEE,IDFF)
C      AFB=0
      PHASYZ=4.D0/3.D0*AFB
C      write(*,*) 'IDE=',IDE,'  IDF=',IDF,'  SVAR=',SVAR,'AFB=',AFB
      END

      FUNCTION GETIDEE(IDE)
      IMPLICIT NONE
      INTEGER IDEE,IDE, GETIDEE
      IF(IDE.EQ.11.OR.IDE.EQ.13.OR.IDE.EQ.15) THEN
        IDEE=2
      ELSEIF(IDE.EQ.-11.OR.IDE.EQ.-13.OR.IDE.EQ.-15) THEN
        IDEE=-2
      ELSEIF(IDE.EQ.12.OR.IDE.EQ.14.OR.IDE.EQ.16) THEN
         IDEE=1
      ELSEIF(IDE.EQ.-12.OR.IDE.EQ.-14.OR.IDE.EQ.-16) THEN
         IDEE=-1
      ELSEIF(IDE.EQ.1.OR.IDE.EQ.3.OR.IDE.EQ.5) THEN
         IDEE=4
      ELSEIF(IDE.EQ.-1.OR.IDE.EQ.-3 .OR.IDE.EQ.-5 ) THEN
         IDEE=-4
      ELSEIF(IDE.EQ. 2.OR.IDE.EQ. 4.OR.IDE.EQ. 6) THEN
         IDEE=3
      ELSEIF(IDE.EQ.- 2.OR.IDE.EQ.- 4.OR.IDE.EQ.- 6) THEN
         IDEE=-3
      ENDIF
      getidee=idee
      END

      SUBROUTINE PHOB(MODE,PBOOS1,VEC)
C.----------------------------------------------------------------------
C.
C.
C.    PHOB:     PHotosBoost
C.
C.    Purpose:  Boosts VEC to (MODE=1)  rest frame of PBOOS1;  
C.              or back (MODE=1)
C.
C.    Input Parameters:   MODE,PBOOS1,VEC
C.
C.    Output Parameters:  VEC
C.
C.    Author(s):                                  Created at:  08/12/05
C.                Z. Was                          Last Update: 
C.
C.----------------------------------------------------------------------
      IMPLICIT NONE
      DOUBLE PRECISION BET1(3),GAM1,PB
      INTEGER I,J,MODE
      REAL*8 PBOOS1(4),vec(4)

      PB=sqrt(PBOOS1(4)**2-PBOOS1(3)**2-PBOOS1(2)**2-PBOOS1(1)**2)
      DO 10 J=1,3
        IF (MODE.EQ.1) THEN
          BET1(J)=-PBOOS1(J)/PB
        ELSE
          BET1(J)= PBOOS1(J)/PB
        ENDIF 
  10  CONTINUE
      GAM1=PBOOS1(4)/PB

C--
C--   Boost vector 

        PB=BET1(1)*vec(1)+BET1(2)*vec(2)+BET1(3)*vec(3)
        
         DO 30 J=1,3
   30    vec(J)=vec(J)+BET1(J)*(vec(4)+PB/(GAM1+1.D0))
         vec(4)=GAM1*vec(4)+PB

C--

      RETURN
      END



      SUBROUTINE GETIDEIDF(IDE,IDF)
      IMPLICIT NONE
c should provide flavour of first incoming beam, and first tau
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      INTEGER IDHEP,ISTHEP,JDAHEP,JMOHEP,NEVHEP,NHEP
      REAL*8 PHEP,VHEP
      COMMON/PH_HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      LOGICAL QEDRAD
      COMMON/PH_PHOQED/QEDRAD(NMXHEP)
      INTEGER   IDE,IDF   

  
      IDE=IDHEP(1)
      IDF=IDHEP(4)
      IF (ABS(IDHEP(4)).EQ.ABS(IDHEP(3))) IDF=IDHEP(3)
      END

      SUBROUTINE GETBM1BM2(IDE,IDF)
      IMPLICIT NONE
c should provide flavour of first incoming beam, and first tau
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      INTEGER IDHEP,ISTHEP,JDAHEP,JMOHEP,NEVHEP,NHEP
      REAL*8 PHEP,VHEP
      COMMON/PH_HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      LOGICAL QEDRAD
      COMMON/PH_PHOQED/QEDRAD(NMXHEP)
      INTEGER   IDE,IDF   
  
      IDE=IDHEP(1)
      IDF=IDHEP(4)
      IF (ABS(IDHEP(4)).EQ.ABS(IDHEP(3))) IDF=IDHEP(3)
      END
      FUNCTION AFBCALC(SVAR,IDEE,IDFF)
C ----------------------------------------------------------------------
C THIS ROUTINE CALCULATES  BORN ASYMMETRY.
C IT EXPLOITS THE FACT THAT BORN X. SECTION = A + B*C + D*C**2
C
C     called by : EVENTM
C ----------------------------------------------------------------------
      IMPLICIT REAL*8 (A-H,O-Z)
C
      CALL GIVIZO(IDEE,-1,T3e,qe,KOLOR)
      CALL GIVIZO(IDFF,-1,T3f,qf,KOLOR1)

      A=PHBORNM(svar,0.5D0,T3e,qe,T3f,qf,KOLOR*KOLOR1)
      B=PHBORNM(svar,-0.5D0,T3e,qe,T3f,qf,KOLOR*KOLOR1)
      AFBCALC= (A-B)/(A+B)*5.0/2.0 *3.0/8.0
      END
      SUBROUTINE GIVIZO(IDFERM,IHELIC,SIZO3,CHARGE,KOLOR)
C ----------------------------------------------------------------------
C PROVIDES ELECTRIC CHARGE AND WEAK IZOSPIN OF A FAMILY FERMION
C IDFERM=1,2,3,4 DENOTES NEUTRINO, LEPTON, UP AND DOWN QUARK
C NEGATIVE IDFERM=-1,-2,-3,-4, DENOTES ANTIPARTICLE
C IHELIC=+1,-1 DENOTES RIGHT AND LEFT HANDEDNES ( CHIRALITY)
C SIZO3 IS THIRD PROJECTION OF WEAK IZOSPIN (PLUS MINUS HALF)
C AND CHARGE IS ELECTRIC CHARGE IN UNITS OF ELECTRON CHARGE
C KOLOR IS A QCD COLOUR, 1 FOR LEPTON, 3 FOR QUARKS
C
C     called by : EVENTE, EVENTM, FUNTIH, .....
C ----------------------------------------------------------------------
      IMPLICIT REAL*8(A-H,O-Z)
C
      IF(IDFERM.EQ.0.OR.IABS(IDFERM).GT.4) GOTO 901
      IF(IABS(IHELIC).NE.1)                GOTO 901
      IH  =IHELIC
      IDTYPE =IABS(IDFERM)
      IC  =IDFERM/IDTYPE
      LEPQUA=INT(IDTYPE*0.4999999D0)
      IUPDOW=IDTYPE-2*LEPQUA-1
      CHARGE  =(-IUPDOW+2D0/3D0*LEPQUA)*IC
      SIZO3   =0.25D0*(IC-IH)*(1-2*IUPDOW)
      KOLOR=1+2*LEPQUA
C** NOTE THAT CONVENTIONALY Z0 COUPLING IS
C** XOUPZ=(SIZO3-CHARGE*SWSQ)/SQRT(SWSQ*(1-SWSQ))
      RETURN
 901    PRINT *,' STOP IN GIVIZO: WRONG PARAMS.'
      STOP
      END
 

      DOUBLE PRECISION FUNCTION PHBORNM(svar,costhe,T3e,qe,T3f,qf,Ncf)
*///////////////////////////////////////////////////////////////////////////
*//                                                                       //
*// This routine provides unsophisticated Born differential cross section //
*// at the crude x-section level, with Z and gamma s-chanel exchange.     //
*///////////////////////////////////////////////////////////////////////////
      IMPLICIT NONE
      DOUBLE PRECISION   svar,costhe
      DOUBLE PRECISION   s,t,Sw2,MZ,MZ2,GammZ,MW,MW2,AlfInv,GFermi
      DOUBLE PRECISION   sum,T3e,t3f,qf,Qe,deno,Ve,Ae,thresh
      DOUBLE PRECISION   xe,yf,xf,ye,ff0,ff1,amx2,amfin,vf,af
      DOUBLE PRECISION   ReChiZ,SqChiZ,RaZ,RaW,ReChiW,SqChiW
      DOUBLE PRECISION   Born, BornS
      INTEGER  KeyZet,HadMin,KFbeam
      INTEGER  i,ke,KFfin,ncf,kf,IsGenerated,iKF
      INTEGER  KeyWidFix
      REAL*8 PI,TWOPI
      COMMON/PHPICO/PI,TWOPI
      AlfInv= 137.0359895d0
      GFermi=1.16639d-5

*--------------------------------------------------------------------
      s = svar
*------------------------------
*     EW paratemetrs taken from BornV
      MZ=91.187D0
      GammZ=2.50072032D0
      Sw2=.22276773D0
*------------------------------
* Z and gamma couplings to beams (electrons)
* Z and gamma couplings to final fermions
* Loop over all flavours defined in m_xpar(400+i)


*------ incoming fermion
            Ve=  2*T3e -4*Qe*Sw2
            Ae=  2*T3e
*------ final fermion couplings
            amfin = 0.000511D0 !  m_xpar(kf+6)
            Vf =  2*T3f -4*Qf*Sw2
            Af =  2*T3f
            IF(abs(costhe) .GT. 1d0) THEN
               WRITE(*,*) '+++++STOP in PHBORN: costhe>0 =',costhe
               STOP
            ENDIF
            MZ2  = MZ**2
            RaZ  = (GFermi *MZ2 *AlfInv  )/( DSQRT(2d0) *8d0 *pi) !
            RaZ  = 1/(16D0*Sw2*(1d0-Sw2))
            KeyWidFix = 1       ! fixed width
            KeyWidFix = 0       ! variable width
            IF( KeyWidFix .EQ. 0 ) THEN
               ReChiZ=(s-MZ2)*s/((s-MZ2)**2+(GammZ*s/MZ)**2) *RaZ    ! variable width
               SqChiZ=     s**2/((s-MZ2)**2+(GammZ*s/MZ)**2) *RaZ**2 ! variable width
            ELSE
               ReChiZ=(s-MZ2)*s/((s-MZ2)**2+(GammZ*MZ)**2) *RaZ    ! fixed width
               SqChiZ=     s**2/((s-MZ2)**2+(GammZ*MZ)**2) *RaZ**2 ! fixed width
            ENDIF
            xe= Ve**2 +Ae**2
            xf= Vf**2 +Af**2
            ye= 2*Ve*Ae
            yf= 2*Vf*Af
            ff0= qe**2*qf**2 +2*ReChiZ*qe*qf*Ve*Vf +SqChiZ*xe*xf
            ff1=             +2*ReChiZ*qe*qf*Ae*Af +SqChiZ*ye*yf
            Born    = (1d0+ costhe**2)*ff0 +2d0*costhe*ff1
* Colour factor
            Born = NCf*Born
* Crude method of correcting threshold, cos(theta) depencence incorrect!!!
            IF(    svar .LE.  4d0*amfin**2) THEN
               thresh=0d0
            ELSEIF(svar .LE. 16d0*amfin**2) THEN
               amx2=4d0*amfin**2/svar
               thresh=sqrt(1d0-amx2)*(1d0+amx2/2d0)
            ELSE
               thresh=1d0
            ENDIF
            Born= Born*thresh
      PHBORNM = Born
      END

