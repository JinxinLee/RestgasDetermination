C.             2) GENERAL INTERFACE:
C.                                      PHOTOS_GET
C.                                      PHOTOS_MAKE


C.   COMMONS:
C.   NAME     USED IN SECT. # OF OCC.     Comment
C.   PHOQED   1) 2)            3      Flags whether emisson to be gen. 
C.   PHOLUN   1) 4)            6      Output device number
C.   PHOCOP   1) 3)            4      photon coupling & min energy
C.   PHPICO   1) 3) 4)         5      PI & 2*PI
C.   PHSEED   1) 4)            3      RN seed 
C.   PHOSTA   1) 4)            3      Status information
C.   PHOKEY   1) 2) 3)         7      Keys for nonstandard application
C.   PHOVER   1)               1      Version info for outside
C.   HEPEVT   2)               2      PDG common
C.   PH_HEPEVT2)               8      PDG common internal
C.   PHOEVT   2) 3)           10      PDG branch
C.   PHOIF    2) 3)            2      emission flags for PDG branch 
C.   PHOMOM   3)               5      param of char-neutr system
C.   PHOPHS   3)               5      photon momentum parameters
C.   PHOPRO   3)               4      var. for photon rep. (in branch)
C.   PHOCMS   2)               3      parameters of boost to branch CMS
C.   PHNUM    4)               1      event number from outside         
C.----------------------------------------------------------------------

      SUBROUTINE PHLUPAB(IPOINT)
      IMPLICIT NONE
C.----------------------------------------------------------------------
C.
C.    PHLUPA:   debugging tool
C.
C.    Purpose:  NONE, eventually may printout content of the 
C.              /PHOEVT/ common
C.
C.    Input Parameters:   Common /PHOEVT/ and /PHNUM/ 
C.                        latter may have number of the event. 
C.
C.    Output Parameters:  None
C.
C.    Author(s):  Z. Was                          Created at:  30/05/93
C.                                                Last Update: 09/10/05
C.
C.----------------------------------------------------------------------
      INTEGER NMXPHO
      PARAMETER (NMXPHO=10000)
      INTEGER IDPHO,ISTPHO,JDAPHO,JMOPHO,NEVPHO,NPHO,I,J,IPOINT
      INTEGER IPOIN,IPOIN0,IPOINM,IEV
      INTEGER IOUT
      REAL*8 PPHO,VPHO,SUM
      COMMON/PH_HEPEVT/NEVPHO,NPHO,ISTPHO(NMXPHO),IDPHO(NMXPHO),
     &JMOPHO(2,NMXPHO),JDAPHO(2,NMXPHO),PPHO(5,NMXPHO),VPHO(4,NMXPHO)
      COMMON /PHNUM/ IEV
      INTEGER PHLUN
      COMMON/PHOLUN/PHLUN
      DIMENSION SUM(5)
      DATA IPOIN0/ -5/
      COMMON /PHLUPY/ IPOIN,IPOINM
      SAVE IPOIN0
      IF (IPOIN0.LT.0) THEN
        IPOIN0=400 000  ! maximal no-print point
        IPOIN =IPOIN0
        IPOINM=400 001  ! minimal no-print point
      ENDIF
      IF (IPOINT.LE.IPOINM.OR.IPOINT.GE.IPOIN ) RETURN
      IOUT=56
      IF (IEV.LT.1000) THEN
      DO I=1,5
        SUM(I)=0.0D0
      ENDDO 
      WRITE(PHLUN,*) 'EVENT NR=',IEV,
     $            'WE ARE TESTING /PH_HEPEVT/ at IPOINT=',IPOINT
      WRITE(PHLUN,10)
      I=1
      WRITE(PHLUN,20) IDPHO(I),PPHO(1,I),PPHO(2,I),PPHO(3,I),
     $                      PPHO(4,I),PPHO(5,I),JDAPHO(1,I),JDAPHO(2,I)
      I=2
      WRITE(PHLUN,20) IDPHO(I),PPHO(1,I),PPHO(2,I),PPHO(3,I),
     $                      PPHO(4,I),PPHO(5,I),JDAPHO(1,I),JDAPHO(2,I)
      WRITE(PHLUN,*) ' '
      DO I=3,NPHO
      WRITE(PHLUN,20) IDPHO(I),PPHO(1,I),PPHO(2,I),PPHO(3,I),
     $                      PPHO(4,I),PPHO(5,I),JMOPHO(1,I),JMOPHO(2,I)
        DO J=1,4
          SUM(J)=SUM(J)+PPHO(J,I)
        ENDDO
      ENDDO
      SUM(5)=SQRT(ABS(SUM(4)**2-SUM(1)**2-SUM(2)**2-SUM(3)**2))
      WRITE(PHLUN,30) SUM
 10   FORMAT(1X,'  ID      ','p_x      ','p_y      ','p_z      ',
     $                   'E        ','m        ',
     $                   'ID-MO_DA1','ID-MO DA2' )
 20   FORMAT(1X,I4,5(F14.9),2I9)
 30   FORMAT(1X,' SUM',5(F14.9))
      ENDIF
      END



























      SUBROUTINE PHOTOS_MAKE_C(IPARR)
C.----------------------------------------------------------------------
C.
C.    PHOTOS_MAKE:   General search routine
C.
C.    Purpose:  Search through the /PH_HEPEVT/ standard HEP common, sta-
C.              rting from  the IPPAR-th  particle.  Whenevr  branching 
C.              point is found routine PHTYPE(IP) is called.
C.              Finally if calls on PHTYPE(IP) modified entries, common
C               /PH_HEPEVT/ is ordered.
C.
C.    Input Parameter:    IPPAR:  Pointer   to   decaying  particle  in
C.                                /PH_HEPEVT/ and the common itself,
C.
C.    Output Parameters:  Common  /PH_HEPEVT/, either with or without 
C.                                new particles added.
C.
C.    Author(s):  Z. Was, B. van Eijk             Created at:  26/11/89
C.                                                Last Update: 30/08/93
C.
C.----------------------------------------------------------------------
      IMPLICIT NONE
      REAL*8 PHOTON(5)
      INTEGER IP,IPARR,IPPAR,I,J,K,L,NLAST
      DOUBLE PRECISION DATA
      INTEGER MOTHER,POSPHO
      LOGICAL CASCAD
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      INTEGER IDHEP,ISTHEP,JDAHEP,JMOHEP,NEVHEP,NHEP
      REAL*8 PHEP,VHEP
      COMMON/PH_HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      LOGICAL QEDRAD
      COMMON/PH_PHOQED/QEDRAD(NMXHEP)
      INTEGER NMXPHO
      PARAMETER (NMXPHO=10000)
      INTEGER ISTACK(0:NMXPHO),NUMIT,NTRY,KK,LL,II,NA,FIRST,LAST
      INTEGER FIRSTA,LASTA,IPP,IDA1,IDA2,MOTHER2,IDPHO,ISPHO
      REAL*8 PORIG(5,NMXPHO)
C--
      CALL PHLUPAB(3)
C      NEVHEP=EVENT

C      write(*,*) 'at poczatek'
C      CALL PHODMP
      IPPAR=ABS(IPARR)
C--   Store pointers for cascade treatement...
      IP=IPPAR
      NLAST=NHEP
      CASCAD=.FALSE.

C--
C--   Check decay multiplicity and minimum of correctness..
      IF ((JDAHEP(1,IP).EQ.0).OR.(JMOHEP(1,JDAHEP(1,IP)).NE.IP)) RETURN
      CALL PHOtoRF

C      write(*,*) 'at przygotowany'
C      CALL PHODMP
C--
C-- single branch mode 
C-- we start looking for the decay points in the cascade 
C-- IPPAR is original position where the program was called
      ISTACK(0)=IPPAR
C--   NUMIT denotes number of secondary decay branches
      NUMIT=0
C--   NTRY denotes number of secondary branches already checked for 
C--        for existence of further branches 
      NTRY=0
C-- let-s search if IPARR does not prevent searching. 
      IF (IPARR.GT.0)  THEN
 30    CONTINUE
         DO I=JDAHEP(1,IP),JDAHEP(2,IP)
          IF (JDAHEP(1,I).NE.0.AND.JMOHEP(1,JDAHEP(1,I)).EQ.I) THEN
            NUMIT=NUMIT+1
              IF (NUMIT.GT.NMXPHO) THEN
               DATA=NUMIT
               CALL PHOERR(7,'PHOTOS',DATA)
              ENDIF
            ISTACK(NUMIT)=I
          ENDIF
         ENDDO
      IF(NUMIT.GT.NTRY) THEN
       NTRY=NTRY+1
       IP=ISTACK(NTRY)
       GOTO 30
      ENDIF
      ENDIF
C-- let-s do generation

      DO 25 KK=0,NUMIT
        NA=NHEP
        FIRST=JDAHEP(1,ISTACK(KK))
        LAST=JDAHEP(2,ISTACK(KK))
        DO II=1,LAST-FIRST+1
         DO LL=1,5
          PORIG(LL,II)=PHEP(LL,FIRST+II-1) 
         ENDDO
        ENDDO
C--   
        CALL PHTYPE(ISTACK(KK))

C--
C--  Correct energy/momentum of cascade daughters
        IF(NHEP.GT.NA) THEN 
        DO II=1,LAST-FIRST+1
          IPP=FIRST+II-1
          FIRSTA=JDAHEP(1,IPP)
          LASTA=JDAHEP(2,IPP)
          IF(JMOHEP(1,IPP).EQ.ISTACK(KK))
     $      CALL PHOBOS(IPP,PORIG(1,II),PHEP(1,IPP),FIRSTA,LASTA) 
        ENDDO
        ENDIF
 25   CONTINUE
C--
C--   rearrange  /PH_HEPEVT/  to get correct order..
        IF (NHEP.GT.NLAST) THEN
          DO 160 I=NLAST+1,NHEP
C--
C--   Photon mother and position...
            MOTHER=JMOHEP(1,I)
            POSPHO=JDAHEP(2,MOTHER)+1
C--   Intermediate save of photon energy/momentum and pointers
              DO 90 J=1,5
   90         PHOTON(J)=PHEP(J,I)
              ISPHO =ISTHEP(I)
              IDPHO =IDHEP(I)
              MOTHER2 =JMOHEP(2,I)
              IDA1 =JDAHEP(1,I)
              IDA2 =JDAHEP(2,I)
C--
C--   Exclude photon in sequence !
            IF (POSPHO.NE.NHEP) THEN
C--
C--
C--   Order /PH_HEPEVT/
              DO 120 K=I,POSPHO+1,-1
                ISTHEP(K)=ISTHEP(K-1)
                QEDRAD(K)=QEDRAD(K-1)
                IDHEP(K)=IDHEP(K-1)
                DO 100 L=1,2
                JMOHEP(L,K)=JMOHEP(L,K-1)
  100           JDAHEP(L,K)=JDAHEP(L,K-1)
                DO 110 L=1,5
  110           PHEP(L,K)=PHEP(L,K-1)
                DO 120 L=1,4
  120         VHEP(L,K)=VHEP(L,K-1)
C--
C--   Correct pointers assuming most dirty /PH_HEPEVT/...
              DO 130 K=1,NHEP
                DO 130 L=1,2
                  IF ((JMOHEP(L,K).NE.0).AND.(JMOHEP(L,K).GE.
     &            POSPHO)) JMOHEP(L,K)=JMOHEP(L,K)+1
                  IF ((JDAHEP(L,K).NE.0).AND.(JDAHEP(L,K).GE.
     &            POSPHO)) JDAHEP(L,K)=JDAHEP(L,K)+1
  130         CONTINUE
C--
C--   Store photon energy/momentum
              DO 140 J=1,5
  140         PHEP(J,POSPHO)=PHOTON(J)
            ENDIF
C--
C--   Store pointers for the photon...
            JDAHEP(2,MOTHER)=POSPHO
            ISTHEP(POSPHO)=ISPHO
            IDHEP(POSPHO)=IDPHO
            JMOHEP(1,POSPHO)=MOTHER
            JMOHEP(2,POSPHO)=MOTHER2
            JDAHEP(1,POSPHO)=IDA1
            JDAHEP(2,POSPHO)=IDA2
C--
C--   Get photon production vertex position
            DO 150 J=1,4
  150       VHEP(J,POSPHO)=VHEP(J,POSPHO-1)
  160     CONTINUE
        ENDIF
C      write(*,*) 'at po dzialaniu '
C      CALL PHODMP

      CALL PHOtoLAB
C      write(*,*) 'at koniec'
C      CALL PHODMP
      RETURN
      END
      subroutine PHOtoRF
      IMPLICIT NONE
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      INTEGER IDHEP,ISTHEP,JDAHEP,JMOHEP,NEVHEP,NHEP
      REAL*8 PHEP,VHEP
      COMMON/PH_HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      LOGICAL QEDRAD
      COMMON/PH_PHOQED/QEDRAD(NMXHEP)
      REAL*8 QQ(4),XM,th1,fi1
      COMMON /PH_TOFROM/ QQ,XM,th1,fi1
      REAL*8 PP(4),RR(4)
      DOUBLE PRECISION PHOAN1,PHOAN2
      INTEGER K,L
      DO K=1,4
       QQ(k)=0
      ENDDO
      DO L=JDAHEP(1,JMOHEP(1,NHEP)),JDAHEP(2,JMOHEP(1,NHEP))
       DO K=1,4
        QQ(k)=QQ(K)+PHEP(K,L)
       ENDDO
      ENDDO
      XM =QQ(4)**2-QQ(3)**2-QQ(2)**2-QQ(1)**2
      IF (XM.GT.0D0) XM=SQRT(XM)
      IF (XM.LE.0) RETURN
      DO L=1,NHEP
       DO K=1,4
        PP(K)=phep(K,L)
       ENDDO
       call bostdq(1,qq,pp,rr)
       DO K=1,4
        phep(K,L)=RR(K)
       ENDDO
      ENDDO
      FI1=0.D0
      TH1=0.d0
      IF(ABS(PHEP(1,1))+ABS(PHEP(2,1)).GT.0D0) 
     $  FI1=PHOAN1(PHEP(1,1),PHEP(2,1))
      IF(ABS(PHEP(1,1))+ABS(PHEP(2,1))+ABS(PHEP(3,1)).GT.0D0) 
     $  TH1=PHOAN2(PHEP(3,1),SQRT(PHEP(1,1)**2+PHEP(2,1)**2))
      DO L=1,NHEP
        CALL PHORO3(-FI1,PHEP(1,L))
        CALL PHORO2(-TH1,PHEP(1,L))
      ENDDO
      return
      end
      subroutine PHOtoLAB
      IMPLICIT NONE
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      INTEGER IDHEP,ISTHEP,JDAHEP,JMOHEP,NEVHEP,NHEP
      REAL*8 PHEP,VHEP
      COMMON/PH_HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      LOGICAL QEDRAD
      COMMON/PH_PHOQED/QEDRAD(NMXHEP)
      REAL*8 QQ(4),XM,th1,fi1
      COMMON /PH_TOFROM/ QQ,XM,th1,fi1
      REAL*8 PP(4),RR(4)
      INTEGER K,L
  
      IF (XM.LE.0) RETURN

      DO L=1,NHEP
        CALL PHORO2( TH1,PHEP(1,L))
        CALL PHORO3( FI1,PHEP(1,L))
      ENDDO

      DO L=1,NHEP
       DO K=1,4
        PP(K)=phep(K,L)
       ENDDO
       call bostdq(-1,qq,pp,rr)
       DO K=1,4
        phep(K,L)=RR(K)
       ENDDO
      ENDDO
      return
      end

      SUBROUTINE bostdq(mode,qq,pp,r)
*     *******************************
* Boost along arbitrary axis (as implemented by Ronald Kleiss).
* The method is described in book of Bjorken and Drell
* p boosted into r  from actual frame to rest frame of q
* forth (mode = 1) or back (mode = -1).
* q must be a timelike, p may be arbitrary.
      IMPLICIT DOUBLE PRECISION (a-h,o-z)
      DIMENSION qq(4),pp(4),r(4)
      DIMENSION q(4),p(4)
      DO k=1,4
         p(k)=pp(k)
         q(k)=qq(k)
      ENDDO
      amq =dsqrt(q(4)**2-q(1)**2-q(2)**2-q(3)**2)
      IF    (mode .EQ. -1) THEN
         r(4) = (p(1)*q(1)+p(2)*q(2)+p(3)*q(3)+p(4)*q(4))/amq
         fac  = (r(4)+p(4))/(q(4)+amq)
      ELSEIF(mode .EQ.  1) THEN
         r(4) =(-p(1)*q(1)-p(2)*q(2)-p(3)*q(3)+p(4)*q(4))/amq
         fac  =-(r(4)+p(4))/(q(4)+amq)
      ELSE
         WRITE(*,*) ' ++++++++ wrong mode in boostdq '
         STOP
      ENDIF
      r(1)=p(1)+fac*q(1)
      r(2)=p(2)+fac*q(2)
      r(3)=p(3)+fac*q(3)
      END

