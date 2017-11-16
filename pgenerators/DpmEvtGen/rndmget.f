C-----------------------------------------------------------------------
c  Last date of change  25.08.03 V.Uzhinsky

      SUBROUTINE RNDMGET1(DSEED)
      IMPLICIT INTEGER (I-N)
      IMPLICIT REAL *8 (A-H,O-Z)
      common/rndu/DS
      DOUBLE PRECISION    DS(2), DSEED, DX24
      DATA      DX24   /  1677 7216.D0  /
      SAVE DX24
      DSEED  =  DS(2)*DX24 + DS(1)
      RETURN
      END

      SUBROUTINE RNDMSET1(DSEED)
      IMPLICIT INTEGER (I-N)
      IMPLICIT REAL *8 (A-H,O-Z)
      common/rndu/DS
      DOUBLE PRECISION    DS(2), DSEED, DX24
      DATA      DX24   /  1677 7216.D0  /
      SAVE DX24
      DS(2)  =  DINT(DSEED/DX24)
      DS(1)  =  DSEED - DS(2)*DX24
      RETURN
      END


      REAL*8 FUNCTION RNDM1(IX)
      IMPLICIT INTEGER (I-N)
      IMPLICIT REAL *8 (A-H,O-Z)
1     RNDM1  =  RANF1(1)
      if(RNDM1.eq.0.) go to 1
      RETURN
      END

C-----------------------------------------------------------------------
      FUNCTION RANF1(IX)
      IMPLICIT INTEGER (I-N)
      IMPLICIT REAL *8 (A-H,O-Z)
C     UNIFORM RANDOM NUMBER GENERATOR FROM CERN LIBRARY
C-----------------------------------------------------------------------
      DOUBLE PRECISION    DS(2),    DM(2)
      DOUBLE PRECISION    DX24,     DX48
      DOUBLE PRECISION    DL,       DC,       DU,       DR
      common/rndu/DS

      DATA      DM     /  1518 4245.D0, 265 1554.D0  /
      DATA      DX24   /  1677 7216.D0  /
      DATA      DX48   /  281 4749 7671 0656.D0  /
      SAVE DM, DX24, DX48

  10  DL  =  DS(1) * DM(1)
      DC  =  DINT(DL/DX24)
      DL  =  DL - DC*DX24
      DU  =  DS(1)*DM(2) + DS(2)*DM(1) + DC
      DS(2)  =  DU - DINT(DU/DX24)*DX24
      DS(1)  =  DL
      DR     =  (DS(2)*DX24 + DS(1)) / DX48
*      RANF1  =  SNGL(DR)
      RANF1  =  DR

      RETURN
      END

