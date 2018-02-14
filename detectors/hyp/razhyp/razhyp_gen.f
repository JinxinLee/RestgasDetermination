C-------------------- Authors A. Sanchez ,A. Botvina --------------------
      
      SUBROUTINE RAZHYP_GEN(shell)
      DOUBLE PRECISION shell
      COMMON /frag/ N,IAG(1000),IZG(1000),
     *IHG(1000),PXs(1000),PYs(1000),PZs(1000),
     *EP(1000),GM(1000)
      
     

c      SHE = shell
c ------ loop on the number of events -------
c      DO 400 JJJ=1,ITNUM
      
      CALL razhyp_event(KST1)
      
c ------------------
      IF(KST1.LT.1) GO TO 400
      PRINT 8, KST1
 8    FORMAT(1X,'   number of particles:  KST1=',I3)


      CALL RHYP_FC(KST1)

c      IF(JJJ.LE.3) PRINT 608,AS,ZS,HS,PTX,PTY,PTZ,EEX,ETM,ETK
c 608  FORMAT(2X,'sum. balance: AS,ZS,HS=',3F6.1/
c     *2X,' PTX, PTY, PTZ (MeV/c)=',3F9.2/
c     *2X,' EEX, ETM, ETK (MeV)=',3F10.4)
c      IF(JJJ.LE.3) PRINT 111
c 111  FORMAT(2X,'----------------------------------------',2X/)
c ------ file event by event
c      WRITE(9,33) SHE,YAZH(3,2),(YAZH(4,2)-S1_1H4H),S1_1H4H,
c     *(YAZH(4,3)-S1_1H4HE),S1_1H4HE,YAZH(5,3),YAZH(6,3),YAZH(8,3),
c     *YAZH(6,4),(YAZH(7,4)-S1_1H7LI-S2_1H7LI-S3_1H7LI-S4_1H7LI),
c     *S1_1H7LI,S2_1H7LI,S3_1H7LI,S4_1H7LI,YAZH(8,4),YAZH(9,4),
c     *YAZH(7,5),YAZH(8,5),(YAZH(9,5)-S1_1H9BE-S2_1H9BE),S1_1H9BE,
c     *S2_1H9BE,YAZH(10,5),YAZH(9,6),(YAZH(10,6)-S1_1H10B),S1_1H10B,
c     *(YAZH(11,6)-S1_1H11B-S2_1H11B),S1_1H11B,S2_1H11B,YAZH(12,6)
c 33   FORMAT(F4.1,28(f10.0,2x))
c      WRITE(10,34) SHE,YAZ2H(6,3),YAZ2H(7,3),(YAZ2H(8,4)-S1_2H8LI
c     *-S2_2H8LI),S1_2H8LI,S2_2H8LI,(YAZ2H(9,4)-S1_2H9LI-S2_2H9LI
c     *-S3_2H9LI),S1_2H9LI,S2_2H9LI,S3_2H9LI,(YAZ2H(9,5)-S1_2H9BE
c     *-S2_2H9BE-S3_2H9BE),S1_2H9BE,S2_2H9BE,S3_2H9BE,(YAZ2H(10,5)
c     *-S1_2H10BE),S1_2H10BE,(YAZ2H(11,5)-S1_2H11BE-S2_2H11BE),
c     *S1_2H11BE,S2_2H11BE,(YAZ2H(12,5)-S1_2H12BE),S1_2H12BE,
c     *YAZ2H(11,6),(YAZ2H(12,6)-S1_2H12B),S1_2H12B
c 34   FORMAT(F4.1,23(f10.0,2x))
 
 400  CONTINUE
    
    
c ------ end of the loop on the number of events -------
 
c
    
      PRINT 1100
 1100 FORMAT(2X,'=================================================')
c 110  CONTINUE
c      CLOSE(UNIT=9)
c      STOP
      RETURN
      END


      SUBROUTINE razhyp_event(KST1)
      COMMON /BLIEX/IEX /BLOKC/SPTU(10,500)
      COMMON /ILEVRA/ILEVRA /FKAP/FKAP /BINMAX/INMAX
      COMMON /WWW/WK(20),WA(20),WN(300) /SUMN/NK(20),NF
c      DIMENSION PN(3),PP(3),YAZT(25,11),HISTA(500),HISTZ(200),
c     #YAZH(25,11),HYPA(500),YAZ2H(25,11),HYP2A(500)
      COMMON /ALI/PN(3),PP(3),YAZT(25,11),HISTA(500),HISTZ(200),
     #YAZH(25,11),HYPA(500),YAZ2H(25,11),HYP2A(500)
      COMMON /bind/SHE,IA0,IZ0,IH0,IAS,IZS,IHS

     
      JJJ=4

c ------ puting initial arrays of events to zero ------
      DO 200 I=1,10
      DO 200 J=1,500
 200  SPTU(I,J)=0.
      KSTART=1
c --- for checking mom. and energ. balance --------
      AS=0.
      ZS=0.
      HS=0.
      PTX=0.
      PTY=0.
      PTZ=0.
      ETM=0.
      ETK=0.
      EEX=0.
c parameters of the source
      AA=IA0
      ZZ=IZ0
      HH=IH0
cc      EE=ETOT*AA
c total available energy for excitation
      GMASKS=1.32132
      PRINT 2,IAS,IZS,IHS
 2    FORMAT(2X,'initial: IAS,IZS,IHS=',3I3)
      GMAS00=GMASHY(IAS,IZS,IHS)
      EMAS=GMAS00+GMASKS
c -------
      PN(1)=0.
      PN(2)=0.
      PN(3)=0.
C      IF(JJJ.GT.3) GO TO 157
      PRINT 12,IAS,IZS,GMAS00,GMASKS
 12   FORMAT(2X,'initial: IAS,IZS=',2I3,' GMAS00=',F10.6,
     *' GMASKS=',F10.6)
c      CALL DELAM(AA,ZZ,DLMAA,DSHAA,BARAA)
cc      DLMHY=1000.*GMASHY(IA0,IZ0,IH0)-931.494*IA0
      PRINT 154,AA,ZZ,EMAS,SHE
 154  FORMAT(2X,'RAZHYS: AA,ZZ =',2F4.0,
     *'  EMAS(GeV), SHE(MeV)=',F10.6,1X,F4.1)
C 157  CONTINUE
c ------------------
      EMAS=EMAS-0.001*SHE
      CALL RAZHYS(EMAS,AA,ZZ,HH,PN,KSTART)
      KST1=KSTART-1
c ------------------------------------------------------
     
      RETURN
      END
      


      SUBROUTINE RHYP_FC(KST1)

c --- fill the fragments information
      COMMON /frag/ N,IAG(1000),IZG(1000),
     *IHG(1000),PXs(1000),PYs(1000),PZs(1000),
     *EP(1000),GM(1000)

      COMMON /BLIEX/IEX /BLOKC/SPTU(10,500)
      COMMON /ILEVRA/ILEVRA /FKAP/FKAP /BINMAX/INMAX
      COMMON /WWW/WK(20),WA(20),WN(300) /SUMN/NK(20),NF
c      DIMENSION PN(3),PP(3),YAZT(25,11),HISTA(500),HISTZ(200),
c     #YAZH(25,11),HYPA(500),YAZ2H(25,11),HYP2A(500)
      COMMON /ALI/PN(3),PP(3),YAZT(25,11),HISTA(500),HISTZ(200),
     #YAZH(25,11),HYPA(500),YAZ2H(25,11),HYP2A(500)
      COMMON /bind/SHE,IA0,IZ0,IH0,IAS,IZS,IHS

c --- for correlations of twin hypernuclei in events ---
      N0_4H=0
      N0_4HE=0
      N0_7LI=0
      N0_9BE=0
      N1_4H=0
      N1_4HE=0
      N1_7LI=0
      N2_7LI=0
      N3_7LI=0
      N4_7LI=0
      N1_9BE=0
      N2_9BE=0
      DO 105 J=1,KST1
      IF(JJJ.LE.3) PRINT 7,SPTU(2,J),(SPTU(JPER,J),JPER=4,10)
 7    FORMAT(1X,'SPT=',F10.2,1X,3F6.3,1X,F8.2,1X,3F5.0)
      IF(SPTU(2,J).LT.99.) GO TO 71
      PRINT 72,SPTU(2,J),(SPTU(JPER,J),JPER=4,10)
 72   FORMAT(1X,'no break-up: SPT=',F10.2,1X,3F6.3,1X,F8.2,1X,3F5.0)
c      GO TO 110
 71   CONTINUE
      IZ=INT(SPTU(8,J)+0.5)
      IA=INT(SPTU(9,J)+0.5)
      IH=INT(SPTU(10,J)+0.5)
      IF(SPTU(4,J).GT.1.) SPTU(4,J)=1.
      IF(SPTU(4,J).LT.-1.) SPTU(4,J)=-1.
      IF(SPTU(5,J).GT.1.) SPTU(5,J)=1.
      IF(SPTU(5,J).LT.-1.) SPTU(5,J)=-1.
      IF(SPTU(6,J).GT.1.) SPTU(6,J)=1.
      IF(SPTU(6,J).LT.-1.) SPTU(6,J)=-1.
      COT=SPTU(4,J)
      IF(IA.LT.1.OR.IA.GT.500) GO TO 304
      HISTA(IA)=HISTA(IA)+1.
      IF(IH.EQ.1) HYPA(IA)=HYPA(IA)+1.
      IF(IH.EQ.2) HYP2A(IA)=HYP2A(IA)+1.
 304  CONTINUE
      IF(IZ.LT.0.OR.IZ.GT.199) GO TO 305
      HISTZ(IZ+1)=HISTZ(IZ+1)+1.
 305  CONTINUE
      IF(IA.GT.25) GO TO 203
      IF(IZ.GT.10) GO TO 203
      YAZT(IA,IZ+1)=YAZT(IA,IZ+1)+1.
      IF(IH.EQ.1) YAZH(IA,IZ+1)=YAZH(IA,IZ+1)+1.
      IF(IH.EQ.2) YAZ2H(IA,IZ+1)=YAZ2H(IA,IZ+1)+1.
 203  CONTINUE
      EX=SPTU(2,J)

c
      IF(IA.EQ.4.AND.IZ.EQ.1.AND.IH.EQ.1.AND.EX.GT.1.04.AND.EX.LT.1.06) 
     *S1_1H4H=S1_1H4H+1.
      IF(IA.EQ.4.AND.IZ.EQ.1.AND.IH.EQ.1.AND.EX.GT.1.04.AND.EX.LT.1.06) 
     *N1_4H=N1_4H+1
      IF(IA.EQ.4.AND.IZ.EQ.1.AND.IH.EQ.1.AND.EX.EQ.0.0) 
     *N0_4H=N0_4H+1
c
      IF(IA.EQ.4.AND.IZ.EQ.2.AND.IH.EQ.1.AND.EX.GT.1.14.AND.EX.LT.1.16) 
     *S1_1H4HE=S1_1H4HE+1.
      IF(IA.EQ.4.AND.IZ.EQ.2.AND.IH.EQ.1.AND.EX.GT.1.14.AND.EX.LT.1.16) 
     *N1_4HE=N1_4HE+1
      IF(IA.EQ.4.AND.IZ.EQ.2.AND.IH.EQ.1.AND.EX.EQ.0.0) 
     *N0_4HE=N0_4HE+1
c
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.0.68.AND.EX.LT.0.70) 
     *S1_1H7LI=S1_1H7LI+1.
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.0.68.AND.EX.LT.0.70) 
     *N1_7LI=N1_7LI+1
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.EQ.0.0) 
     *N0_7LI=N0_7LI+1
c
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.2.04.AND.EX.LT.2.06) 
     *S2_1H7LI=S2_1H7LI+1.
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.2.04.AND.EX.LT.2.06) 
     *N2_7LI=N2_7LI+1
     
c
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.2.51.AND.EX.LT.2.53) 
     *S3_1H7LI=S3_1H7LI+1.
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.2.51.AND.EX.LT.2.53) 
     *N3_7LI=N3_7LI+1
      
c
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.3.87.AND.EX.LT.3.89) 
     *S4_1H7LI=S4_1H7LI+1.
      IF(IA.EQ.7.AND.IZ.EQ.3.AND.IH.EQ.1.AND.EX.GT.3.87.AND.EX.LT.3.89) 
     *N4_7LI=N4_7LI+1
     
c
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.1.AND.EX.GT.3.01.AND.EX.LT.3.03) 
     *S1_1H9BE=S1_1H9BE+1.
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.1.AND.EX.GT.3.01.AND.EX.LT.3.03) 
     *N1_9BE=N1_9BE+1
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.1.AND.EX.EQ.0.0) 
     *N0_9BE=N0_9BE+1
c
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.1.AND.EX.GT.3.06.AND.EX.LT.3.08) 
     *S2_1H9BE=S2_1H9BE+1.
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.1.AND.EX.GT.3.06.AND.EX.LT.3.08) 
     *N2_9BE=N2_9BE+1
     
c
      IF(IA.EQ.10.AND.IZ.EQ.5.AND.IH.EQ.1.AND.EX.GT.0.09.AND.EX.LT.0.11) 
     *S1_1H10B=S1_1H10B+1.
      IF(IA.EQ.11.AND.IZ.EQ.5.AND.IH.EQ.1.AND.EX.GT.0.99.AND.EX.LT.1.01) 
     *S1_1H11B=S1_1H11B+1.
      IF(IA.EQ.11.AND.IZ.EQ.5.AND.IH.EQ.1.AND.EX.GT.1.47.AND.EX.LT.1.49) 
     *S2_1H11B=S2_1H11B+1.
c
      IF(IA.EQ.8.AND.IZ.EQ.3.AND.IH.EQ.2.AND.EX.GT.1.35.AND.EX.LT.1.37) 
     *S1_2H8LI=S1_2H8LI+1.
      IF(IA.EQ.8.AND.IZ.EQ.3.AND.IH.EQ.2.AND.EX.GT.5.62.AND.EX.LT.5.64) 
     *S2_2H8LI=S2_2H8LI+1.
      IF(IA.EQ.9.AND.IZ.EQ.3.AND.IH.EQ.2.AND.EX.GT.0.72.AND.EX.LT.0.74) 
     *S1_2H9LI=S1_2H9LI+1.
      IF(IA.EQ.9.AND.IZ.EQ.3.AND.IH.EQ.2.AND.EX.GT.4.54.AND.EX.LT.4.56) 
     *S2_2H9LI=S2_2H9LI+1.
      IF(IA.EQ.9.AND.IZ.EQ.3.AND.IH.EQ.2.AND.EX.GT.5.95.AND.EX.LT.5.97) 
     *S3_2H9LI=S3_2H9LI+1.
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.2.AND.EX.GT.0.70.AND.EX.LT.0.72) 
     *S1_2H9BE=S1_2H9BE+1.
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.2.AND.EX.GT.4.53.AND.EX.LT.4.55) 
     *S2_2H9BE=S2_2H9BE+1.
      IF(IA.EQ.9.AND.IZ.EQ.4.AND.IH.EQ.2.AND.EX.GT.5.91.AND.EX.LT.5.93) 
     *S3_2H9BE=S3_2H9BE+1.
      IF(IA.EQ.10.AND.IZ.EQ.4.AND.IH.EQ.2.AND.EX.GT.2.85.AND.EX.LT.2.87) 
     *S1_2H10BE=S1_2H10BE+1.
      IF(IA.EQ.11.AND.IZ.EQ.4.AND.IH.EQ.2.AND.EX.GT.1.77.AND.EX.LT.1.79) 
     *S1_2H11BE=S1_2H11BE+1.
      IF(IA.EQ.11.AND.IZ.EQ.4.AND.IH.EQ.2.AND.EX.GT.6.09.AND.EX.LT.6.11) 
     *S2_2H11BE=S2_2H11BE+1.
      IF(IA.EQ.12.AND.IZ.EQ.4.AND.IH.EQ.2.AND.EX.GT.3.46.AND.EX.LT.3.48) 
     *S1_2H12BE=S1_2H12BE+1.
      IF(IA.EQ.12.AND.IZ.EQ.5.AND.IH.EQ.2.AND.EX.GT.3.28.AND.EX.LT.3.30) 
     *S1_2H12B=S1_2H12B+1.

 105  CONTINUE
c ----------------------
c      WRITE(9,910)JJJ
c 910  FORMAT(2X,'event',I8)
c       WRITE(9,33)KST1
c 33   FORMAT(2x,'mult',I3) 

c ------- fragment multiplicity ------
      N = KST1
c ------------------------
    

c ----------------------
      IF(N1_4H.EQ.2) C11_4H_4H=C11_4H_4H+1.
      IF(N1_4H.EQ.1.AND.N1_4HE.EQ.1) C11_4H_4HE=C11_4H_4HE+1.
      IF(N1_4H.EQ.1.AND.N1_7LI.EQ.1) C11_4H_7LI=C11_4H_7LI+1.
      IF(N1_4H.EQ.1.AND.N2_7LI.EQ.1) C12_4H_7LI=C12_4H_7LI+1.
      IF(N1_4H.EQ.1.AND.N3_7LI.EQ.1) C13_4H_7LI=C13_4H_7LI+1.
      IF(N1_4H.EQ.1.AND.N4_7LI.EQ.1) C14_4H_7LI=C14_4H_7LI+1.
      IF(N1_4H.EQ.1.AND.N1_9BE.EQ.1) C11_4H_9BE=C11_4H_9BE+1.
      IF(N1_4H.EQ.1.AND.N2_9BE.EQ.1) C12_4H_9BE=C12_4H_9BE+1.
      IF(N0_4H.EQ.2) C11gg_4H_4H=C11gg_4H_4H+1.
      IF(N0_4H.EQ.1.AND.N0_4HE.EQ.1) C11gg_4H_4HE=C11gg_4H_4HE+1.
      IF(N0_4H.EQ.1.AND.N0_7LI.EQ.1) C11gg_4H_7LI=C11gg_4H_7LI+1.
      IF(N0_4H.EQ.1.AND.N0_9BE.EQ.1) C11gg_4H_9BE=C11gg_4H_9BE+1.
      IF((N0_4H.EQ.1.AND.N1_4H.EQ.1).OR.(N1_4H.EQ.1.AND.N0_4H.EQ.1)) 
     *C11ge_4H_4H=C11ge_4H_4H+1.
      IF((N0_4H.EQ.1.AND.N1_4HE.EQ.1).OR.(N1_4H.EQ.1.AND.N0_4HE.EQ.1)) 
     *C11ge_4H_4HE=C11ge_4H_4HE+1.
      IF(N0_4H.EQ.1.AND.N1_7LI.EQ.1) C11ge_4H_7LI=C11ge_4H_7LI+1.
      IF(N0_4H.EQ.1.AND.N2_7LI.EQ.1) C12ge_4H_7LI=C12ge_4H_7LI+1.
      IF(N0_4H.EQ.1.AND.N3_7LI.EQ.1) C13ge_4H_7LI=C13ge_4H_7LI+1.
      IF(N0_4H.EQ.1.AND.N4_7LI.EQ.1) C14ge_4H_7LI=C14ge_4H_7LI+1.
      IF((N0_4H.EQ.1.AND.N1_9BE.EQ.1).OR.(N1_4H.EQ.1.AND.N0_9BE.EQ.1)) 
     *C11ge_4H_9BE=C11ge_4H_9BE+1.
      IF(N0_4H.EQ.1.AND.N2_9BE.EQ.1) C12ge_4H_9BE=C12_4H_9BE+1.
      IF(N1_4HE.EQ.2) C11_4HE_4HE=C11_4HE_4HE+1.
      IF(N1_4HE.EQ.1.AND.N1_7LI.EQ.1) C11_4HE_7LI=C11_4HE_7LI+1.
      IF(N1_4HE.EQ.1.AND.N2_7LI.EQ.1) C12_4HE_7LI=C12_4HE_7LI+1.
      IF(N1_4HE.EQ.1.AND.N3_7LI.EQ.1) C13_4HE_7LI=C13_4HE_7LI+1.
      IF(N1_4HE.EQ.1.AND.N4_7LI.EQ.1) C14_4HE_7LI=C14_4HE_7LI+1.
      IF(N0_4HE.EQ.2) C11gg_4HE_4HE=C11gg_4HE_4HE+1.
      IF(N0_4HE.EQ.1.AND.N0_7LI.EQ.1) C11gg_4HE_7LI=C11gg_4HE_7LI+1.
      IF(N0_4HE.EQ.1.AND.N1_4HE.EQ.1) C11ge_4HE_4HE=C11gg_4HE_4HE+1.
      IF((N0_4HE.EQ.1.AND.N1_7LI.EQ.1).OR.(N1_4HE.EQ.1.AND.N0_7LI.EQ.1)) 
     *C11ge_4HE_7LI=C11ge_4HE_7LI+1.
      IF(N0_4HE.EQ.1.AND.N2_7LI.EQ.1) C12ge_4HE_7LI=C12_4HE_7LI+1.
      IF(N0_4HE.EQ.1.AND.N3_7LI.EQ.1) C13ge_4HE_7LI=C13_4HE_7LI+1.
      IF(N0_4HE.EQ.1.AND.N4_7LI.EQ.1) C14ge_4HE_7LI=C14_4HE_7LI+1.

c ------------------
     
    
      DO 135 J=1,KST1
      IZ=INT(SPTU(8,J)+0.5)
      IA=INT(SPTU(9,J)+0.5)
      IH=INT(SPTU(10,J)+0.5)
      EX=SPTU(2,J)
      IF(IA.LE.0) GO TO 135
      AS=AS+IA
      ZS=ZS+IZ
      HS=HS+IH
      EEX=EEX+EX
      DLMH=1000.*GMASHY(IA,IZ,IH)-931.494*IA
      ETM=ETM+DLMH
      COT=SPTU(4,J)
      SIT=SQRT(1.-COT*COT)
c      CMN=938.*SPTU(9,J)
      CMN=DLMH+931.494*IA
      CALL TINP(PP,COT,SIT,SPTU(6,J),SPTU(5,J),SPTU(7,J),CMN)
      PTX=PTX+PP(1)
      PTY=PTY+PP(2)
      PTZ=PTZ+PP(3)
      ETK=ETK+SPTU(7,J)
c     ------------- filling the output variables------
      IAG(J)= IA
      IZG(J)= IZ
      IHG(J)= IH
      PXs(J)= PP(1)
      PYs(J)= PP(2)
      PZs(J)= PP(3)
      EP(J)= EX
      GM(J)= GMASHY(IA,IZ,IH)
      
      PRINT 22,IA,IZ,IH
 22   FORMAT(2X,'loop : IA,IZS,IHS=',3I3)
      PRINT 23,PP(1),PP(2),PP(3)
 23   FORMAT(2X,'loop : Px,Py,Pz=',3F9.2)

c      WRITE(9,320)IA,IZ,IH,GMASHY(IA,IZ,IH),EX,PP(1),PP(2),PP(3)

c 320  FORMAT(1X,3I6,1X,F9.4,
c     *1X,F9.4,3F9.2)
c     -----------------------      
 135  CONTINUE
      RETURN
      END
c ----------------------------------------------------------------------
c GSI 2006, RAZHYP - fermi break-up of hyper-nuclei, 
c without secondary POVRAZ
c -----------  previously: ----------------------------------------
c GSI 2006, RAZVAL extended to A.ge.18 @ILEVRA=5; arrays (WN...) up to 
c INMAX<=300.
c ----------------------------------------------------------------------
c *** RAZVAL modernized as for Milazzo paper, if label MILAZ=1. (=0 -old)
c ----------------------------------------------------------------------
c ...
      SUBROUTINE RAZHYS(EMAS,AP,ZP,HP,PN,KST)
C break-up of hyper-nuclei (lambda)
c UP - excitation energy (GeV), AP - mass number, ZP - charge, 
c HP - lambda number (0 or 1), PN(3) - momenta GeV/c, KST - output array index
      COMMON /ILEVRA/ILEVRA
      COMMON /SPDM/ MS(12,11),DM(12,11) /FKAP/FKAP /BLOKC/SPTU(10,500)
      COMMON /WWW/WK(20),WA(20),WN(300) /BLENN/ENN(300) /SUMN/NK(20),NF
      COMMON /BLJ/J(20),IEND /BLIAN/IAN(300) /BLIZN/IZN(300)
      COMMON /BMILAZ/MILAZ /BLIEX/IEX /BINMAX/INMAX /BLIHN/IHN(300)
      DIMENSION PN(3),VN(3),PNC(3),PNL(3),PN0(3,500),AMK(500)
      DIMENSION IPA(20),IPZ(20),WCON(4000),JCON(24000)
      DIMENSION IPH(20),EPE(20)
      KSTOLD=KST
      MILAZ=1
      ILEVRA=6
      IF(ILEVRA.EQ.6) INMAX=165+14+10
      FKAP=2.
      IA0=INT(AP+0.5)
      IZ0=INT(ZP+0.5)
      IH0=INT(HP+0.5)
      NF=0
      DO 69 I=1,INMAX
 69   WN(I)=0.
      DO 70 I=1,20
      NK(I)=0
      WA(I)=0.
 70   WK(I)=0.
      IF(AP.LT.1.9) GO TO 37
      PQ=PN(1)**2+PN(2)**2+PN(3)**2
      EAP=SQRT(PQ+(0.938*AP)**2)
      DO 40 I=1,3
 40   VN(I)=PN(I)/EAP
c ------- exit for hot fragments (into SPT) -----
      IF(IEX.EQ.1) GO TO 37
c -----------------------------------------------
      CALL HELPHY
cc      DAZ=UP+GMASHY(IA0,IZ0,IH0)
c total available energy
      DAZ=EMAS
c ------ print -----
c      PRINT 103,IA0,IZ0,DAZ
c 103  FORMAT(2X,'RAZHYP: IA0,IZ0=',2I5,' DAZ=',F12.6)
c ------
      SRQ=0.
      K2=0
      K00=IA0
      IF(K00.GT.6) K00=6
      DO 71 K=2,K00
      J(1)=0
      DO 72 I=2,K
 72   J(I)=1
 78   CONTINUE
      CALL DISNHY(IA0,IZ0,IH0,K)
c ------ print -----
c      IF(IEND.EQ.1) PRINT 7,(J(I),I=1,K)
c 7    FORMAT(2X,'**** IEND=1 ****  J=',10I5)
c ------
      IF(IEND.EQ.1) GO TO 71
      IF(NF.GE.4000.OR.K2.GT.23980) GO TO 71
      NF=NF+1
      NK(K)=NK(K)+1
      TQ=BCUL(K)
      RMQ=WEIGHY(IA0,K,DAZ,TQ)
c ------ print -----
c      PRINT 300,NF,TQ,RMQ
c 300  FORMAT(1X,'NF=',I5,' TQ=',F10.6,' RMQ=',E12.4)
c      PRINT 3,(J(I),I=1,K)
c 3    FORMAT(1X,'selec.config.J=',15I4)
c ------
      SRQ=SRQ+RMQ
      WCON(NF)=RMQ
      DO 73 I=1,K
      JI=J(I)
      JCON(K2+I)=JI
      IA=IAN(JI)
      IZ=IZN(JI)
      WN(JI)=WN(JI)+RMQ
 73   WA(IA)=WA(IA)+RMQ
      K2=K2+K
      WK(K)=WK(K)+RMQ
      GO TO 78
 71   CONTINUE
c ------ print -----
c      PRINT 106,NF,K2,SRQ
c 106  FORMAT(2X,' #channels NF=',I4,'#particles K2=',I5,' SRQ=',E12.4)
c ------
      IF(SRQ.LE.0.) GO TO 37
      DO 33 I=1,IA0
      WK(I)=WK(I)/SRQ
 33   WA(I)=WA(I)/SRQ
      DO 34 I=1,INMAX
 34   WN(I)=WN(I)/SRQ
      DO 32 I=1,NF
 32   WCON(I)= WCON(I)/SRQ
 210                 BR=RNDM(-1)
      SR=0.
      DO 13 N=1,NF
      NB=N
      SR=SR+WCON(N)
      IF(BR.LT.SR) GO TO 14
 13   CONTINUE
   14 CONTINUE
      NNP=0
      NN=0
      DO 61 I=2,K00
      NNP=NNP+NK(I)*I
      NN=NN+NK(I)
      IF(NN.GE.NB) GO TO 62
 61   CONTINUE
 62   K=I
      NPK1=(NB-(NN-NK(K))-1)*K
      NPK0=(NNP-NK(K)*K)+NPK1
c ------ print -----
c      PRINT 101,NN,NB,NPK1,NPK0,K
c 101  FORMAT(2X,'NN,NB,NPK1,NPK0=',4I5,' select particls: K=',I5)
c ------
      TN=DAZ
      DO 63 I=1,K
      JI=JCON(NPK0+I)
      IPA(I)=IAN(JI)
      IPZ(I)=IZN(JI)
      IPH(I)=IHN(JI)
      EPE(I)=ENN(JI)
      AMK(I)=GMASHY(IPA(I),IPZ(I),IPH(I))+0.001*ENN(JI)
      TN=TN-AMK(I)
c ------ print -----
c      PRINT 105,JI
c 105  FORMAT(5X,'JI=',I5)
c      PRINT 102,IPA(I),IPZ(I),ENN(JI),AMK(I),TN
c 102  FORMAT(2X,'IPA,IPZ=',2I4,' ENN=',F5.2,' AMK=',F11.6,'TN=',F11.6)
c ------
 63   CONTINUE
      IF(TN.LT.0.) GO TO 210
      CALL DISIMP(K,AMK,PN0,TN)
      DO 35 I=1,K
      DO 36 II=1,3
 36   PNC(II)=PN0(II,I)
      EN=SQRT(PNC(1)**2+PNC(2)**2+PNC(3)**2+AMK(I)**2)
      CALL CLPV(PNC,VN,PNL,EN)
      CALL PINT(PNL,CT,ST,CF,SF,TK,AMK(I))
c ------ print -----
c      PRINT 104,I,TK,(PNL(II),II=1,3)
c 104  FORMAT(2X,'Nparticl=',I3,' TK=',F11.6,' PNL=',3F8.4)
c ------
      SPTU(4,KST)=CT
      SPTU(5,KST)=SF
      SPTU(6,KST)=CF
      SPTU(7,KST)=1000.*TK
      SPTU(8,KST)=IPZ(I)
c      SPTU(9,KST)=0.94*INT(AMK(I)/0.94+0.5)
      SPTU(9,KST)=IPA(I)
      SPTU(10,KST)=IPH(I)
      SPTU(2,KST)=EPE(I)
      KST=KST+1
 35   CONTINUE
      GO TO 200
C      RETURN
 37   CONTINUE
      IF(AP.LT.0.99) RETURN
      WK(1)=1.
      WA(IA0)=1.
      NF=1
      NK(1)=1
      DO 38 II=1,INMAX
      IA1=IAN(II)
      IZ1=IZN(II)
      IF(IA0.EQ.IA1.AND.IZ0.EQ.IZ1) WN(II)=WN(II)+1.
      IF(IA0.EQ.IA1.AND.IZ0.EQ.IZ1) GO TO 39
 38   CONTINUE
 39   CONTINUE
      CMN=0.938*AP
      CALL PINT(PN,CT,ST,CF,SF,TMN,CMN)
      SPTU(4,KST)=CT
      SPTU(5,KST)=SF
      SPTU(6,KST)=CF
      SPTU(7,KST)=1000.*TMN
      SPTU(8,KST)=ZP
c      SPTU(9,KST)=CMN
      SPTU(9,KST)=AP
      SPTU(10,KST)=HP
c      SPTU(2,KST)=1000.*UP
      SPTU(2,KST)=1000.*DAZ
c      IF(IEX.EQ.1) SPTU(2,KST)=1000.*UP
      IF(IEX.EQ.1) SPTU(2,KST)=1000.*DAZ
      KST=KST+1
c      GO TO 200
      RETURN
 200  CONTINUE
      RETURN
      END
      FUNCTION WEIGHY(MA,K,DAZ,TQ)
C O[PE[E[EH[E [P[BE[EHHO[ BEPO[THOCT[ PAC[A[A MA HA K [PA[MEHTOB. HOMEP
C [PA[M. [AH[ B MACC[BE J(I). DAZ-[HEP[[[ MA,BK[[[A[ [HEP[[[ [OKO[. TQ-
C K[[OHOBCK[[ [AP[EP. AKV-[P[BE[EHH[[ O[[EM [[[ O[HO[O [PA[MEHTA. AKM-[P
C BE[EH[E C[[HOB[X [ MACCOB[X [AKTOPOB. RPM-[AKTOP[A[ TO[[ECTBEHH[X [[E-
C HOB. GAM-O[PATHA[ [AMMA-[[HK[[[. TN-K[HET[[ECKA[ [HEP[[[ OCKO[KOB.
      COMMON /SPDM/ MS(12,11),DM(12,11) /VAK/VAK /GAF/GAF(20)
      COMMON /BLJ/J(20),IEND /BLIAN/IAN(300) /BLIZN/IZN(300)
      COMMON /BLISP/ISP(300) /BLENN/ENN(300) /FKAP/FKAP /BLIHN/IHN(300)
      COMMON /BMILAZ/MILAZ
      BNQ=DAZ
      RMQ=0.
      SPM=1.
      AKM=1.
      AKV=MA*VAK
c --- correction on multiple-dependent freez-out (translat.) volume ---
      IF(MILAZ.EQ.1) GO TO 50
      AMA03=(FLOAT(MA))**0.333333
      AKV=(AKV/FKAP)*((1.+1.4*(K**0.333333-1)/(1.17*AMA03))**3-1.)
 50   CONTINUE
c ---
      DO 40 I=1,K
      JI=J(I)
      IA=IAN(JI)
      IZ=IZN(JI)
      AKM=AKM*IA
      SPM=SPM*ISP(JI)
      IH=IHN(JI)
      BNQ=BNQ-GMASHY(IA,IZ,IH)-0.001*ENN(JI)
 40   CONTINUE
      TN=BNQ
      IF(TN.LE.TQ) GO TO 20
      TN=TN-TQ
      AKM=AKM/MA
      AKM=AKM*SQRT(AKM)*SPM
      IF(K.GT.2) GO TO 29
      RMQ=1.1283792*AKV*AKM*SQRT(TN)
      IF(J(1).EQ.J(2)) RMQ=0.5*RMQ
      GO TO 20
 29   CONTINUE
      KI=K-1
      RPM=1.
      VMK=1.
      TEQ=2.71828183*TN/(1.5*K-2.5)
      VTK=AKV*TEQ*SQRT(TEQ)
      DO 21 I=1,KI
      MRS=1
      IK=I+1
      VMK=VMK*VTK
      DO 22 I1=IK,K
      IF(J(I).EQ.J(I1)) MRS=MRS+1
 22   CONTINUE
      RPM=RPM*MRS
 21   CONTINUE
      GAM=GAF(K)
      RMQ=VMK*GAM*AKM/(TEQ*RPM)
 20   WEIGHY=RMQ
      RETURN
      END
      FUNCTION GMASHY(MA,MZ,IH)
C O[PE[E[EH[E MACC[ [E[K[X [[EP B [[B.([C[O[[[[[TC[ [[[EPO[H[E E[[H[[[)
C ( OCHOBHOE COCTO[H[E) - (GeV)
      COMMON /SPDM/ MS(12,11),DM(12,11)
      IF(IH.GT.0) GO TO 13
      I2=MZ
      I=MA-I2
      IF(I.LT.0.OR.I.GT.11) GO TO 10
      IF(I2.LT.0.OR.I2.GT.10) GO TO 10
      DMP=DM(I+1,I2+1)
      IF(DMP.GT.90) GO TO 10
c      GMASHY=0.931494*(I+I2)+0.001*DMP
c correction for electrons in neutral atoms
      GMASHY=0.931494*(I+I2)+0.001*DMP-0.000511*I2
      GO TO 11
c ---- mass formula ----
 10   CONTINUE
      AA=MA
      ZZ=MZ
      CALL DELAM(AA,ZZ,DLMPP,DSH,BAR)
c      GMASHY=0.931494*AA+0.001*DLMPP
c correction for electrons in neutral atoms
      GMASHY=0.931494*AA+0.001*DLMPP-0.000511*ZZ
c ----
 11   CONTINUE
      RETURN
 13   GMASHY=99999.
      IF(IH.GT.1) GO TO 12
c ground masses (GeV) of one-lambda hypernuclei:
c experimental data from H.Bando, Int.J.Mod.Phys. 5 (1990) 4021
      IF(MA.EQ.1.AND.MZ.EQ.0) GMASHY=1.115684
      IF(MA.EQ.3.AND.MZ.EQ.1) GMASHY=2.99114
      IF(MA.EQ.4.AND.MZ.EQ.1) GMASHY=3.9225
      IF(MA.EQ.4.AND.MZ.EQ.2) GMASHY=3.92168
      IF(MA.EQ.5.AND.MZ.EQ.2) GMASHY=4.8399
      IF(MA.EQ.6.AND.MZ.EQ.2) GMASHY=5.77934
      IF(MA.EQ.8.AND.MZ.EQ.2) GMASHY=7.65407
      IF(MA.EQ.6.AND.MZ.EQ.3) GMASHY=5.7788
      IF(MA.EQ.7.AND.MZ.EQ.3) GMASHY=6.7116
      IF(MA.EQ.8.AND.MZ.EQ.3) GMASHY=7.6427
      IF(MA.EQ.9.AND.MZ.EQ.3) GMASHY=8.5786
      IF(MA.EQ.7.AND.MZ.EQ.4) GMASHY=6.7159
      IF(MA.EQ.8.AND.MZ.EQ.4) GMASHY=7.64303
      IF(MA.EQ.9.AND.MZ.EQ.4) GMASHY=8.56382
      IF(MA.EQ.10.AND.MZ.EQ.4) GMASHY=9.49932
      IF(MA.EQ.9.AND.MZ.EQ.5) GMASHY=8.57971
      IF(MA.EQ.10.AND.MZ.EQ.5) GMASHY=9.5001
      IF(MA.EQ.11.AND.MZ.EQ.5) GMASHY=10.4299
      IF(MA.EQ.12.AND.MZ.EQ.5) GMASHY=11.35686
      IF(MA.EQ.12.AND.MZ.EQ.6) GMASHY=11.35893
      IF(MA.EQ.13.AND.MZ.EQ.6) GMASHY=12.27885
      IF(MA.EQ.14.AND.MZ.EQ.6) GMASHY=13.21299
      IF(MA.EQ.14.AND.MZ.EQ.7) GMASHY=13.2147
      IF(MA.EQ.15.AND.MZ.EQ.7) GMASHY=14.14229
      IF(MA.EQ.16.AND.MZ.EQ.8) GMASHY=15.07436
      IF(GMASHY.GT.99990.) GO TO 15
      RETURN
 12   CONTINUE
      IF(IH.GT.2) GO TO 15
c theoretically predicted masses (and levels) for double hypernuclei
c from E.Hiyama PRC 66 (2002) 024117 and T.Yamada PRC 56 (1997) 3216
      IF(MA.EQ.6.AND.MZ.EQ.2) GMASHY=5.95137
      IF(MA.EQ.7.AND.MZ.EQ.2) GMASHY=6.88983
      IF(MA.EQ.7.AND.MZ.EQ.3) GMASHY=6.88955
      IF(MA.EQ.8.AND.MZ.EQ.3) GMASHY=7.82144
      IF(MA.EQ.9.AND.MZ.EQ.3) GMASHY=8.75061
      IF(MA.EQ.9.AND.MZ.EQ.4) GMASHY=8.75104
      IF(MA.EQ.10.AND.MZ.EQ.4) GMASHY=9.67108
      IF(MA.EQ.11.AND.MZ.EQ.4) GMASHY=10.60335
      IF(MA.EQ.11.AND.MZ.EQ.5) GMASHY=10.60371
      IF(MA.EQ.12.AND.MZ.EQ.4) GMASHY=11.53405
      IF(MA.EQ.12.AND.MZ.EQ.5) GMASHY=11.53252
c      IF(MA.EQ.13.AND.MZ.EQ.5) GMASHY=12.45659
      IF(GMASHY.GT.99990.) GO TO 15
      RETURN
 15   PRINT 14,MA,MZ,IH,GMASHY
 14   FORMAT(2X,'no mass for hypernucleus A,Z,H=',I4,I4,I3,
     #' GMASHY=',F8.1)
      STOP
      END
      FUNCTION BCUL(K)
C  O[PE[E[EH[E K[[OHOBCKO[O [AP[EPA [[[ PA[BA[A HA KOH[[[[PA[[[ J(1-K)
C  [[ K [ACT[[.
      COMMON /FKAP/FKAP /BLJ/J(20),IEND /BLIAN/IAN(300) /BLIZN/IZN(300)
      DIMENSION IA(20),IZ(20)
      A=0.
      Z=0.
      COEF=(3.*1.44/(5.*1.3))*(1./(1.+FKAP)**0.333333)
      DO 1 I=1,K
      JI=J(I)
      IA(I)=IAN(JI)
      A=A+IA(I)
      IZ(I)=IZN(JI)
      Z=Z+IZ(I)
 1    CONTINUE
      EC=0.
      DO 4 I=1,K
      AP=IA(I)
      ZP=IZ(I)
      EC=EC+ZP*ZP/(AP**0.333333)
 4    CONTINUE
      EC=Z*Z/(A**0.333333)-EC
      BCUL=0.001*COEF*EC
      IF(BCUL.LT.0.) BCUL=0.
      RETURN
      END
      
      SUBROUTINE DISNHY(IA0,IZ0,IH0,K)
C  HAXO[[EH[E K [ACT[[ B KOH[[[[PA[[[ B B[[E [X HOMEPOB J(1-K) B MAC-
C  C[BAX IAN (B [OP[[.BO[POC.A) [ IZN (B [OP[[.BO[POC.Z [P[ [AHHOM A)
C  BO[MO[HA[ KOH[[[[PA[[[ IEND=0, KOHE[ IEND=1.
C  HA[.KOH[[[.J(2-K)=1, J(1)=0 [A[AETC[ [EPE[ DISNUM.
      COMMON /BLIAN/IAN(300) /BLIZN/IZN(300) /BLJ/J(20),IEND
      COMMON /BINMAX/INMAX /BLIHN/IHN(300)
 5    L=0
 1    L=L+1
      IF(L.LT.K) GO TO 10
      IF(L.GE.K) GO TO 11
 10   J(L)=J(L)+1
      IF(J(L).GT.J(L+1)) GO TO 2
      ISA=0
      DO 12 I=1,K
      JI=J(I)
 12   ISA=ISA+IAN(JI)
      IF(ISA.GT.IA0) GO TO 2
      IF(ISA.LT.IA0) GO TO 5
 4    ISZ=0
      ISH=0
      DO 13 I=1,K
      JI=J(I)
      ISH=ISH+IHN(JI)
 13   ISZ=ISZ+IZN(JI)
      IF(ISZ.NE.IZ0) GO TO 5
      IF(ISH.NE.IH0) GO TO 5
      IEND=0
      RETURN
 2    J(L)=1
      GO TO 1
 11   J(L)=J(L)+1
      IF(J(L).GT.INMAX) GO TO 15
      ISA=0
      DO 14 I=1,K
      JI=J(I)
 14   ISA=ISA+IAN(JI)
      IF(ISA.LT.IA0) GO TO 5
      IF(ISA.EQ.IA0) GO TO 4
 15   IEND=1
      RETURN
      END
      BLOCK DATA RAZHY
C  tables for allowed hypernuclei: masses, charges, spin factors, 
c  hyper-number (number of lambdas), energies (MeV) above ground states.
      COMMON /BLIAN6/IAN6(300) /BLIZN6/IZN6(300) /BLENN6/ENN6(300)
      COMMON /BLISP6/ISP6(300) /BLIHN6/IHN6(300)
c previously:
c --  3:  ground + excited states (~<5MeV) allowed by liquid-drop masses  
c     made from 2, just put ISP3=0 for some nuclei (for 0 in weights) --
c --  5: made by from 3 (extension above 100) , untill A=18, INMAX=133 --
c --  6: made by from 5, deleting former nuclei with ISP=0, adding 8He, 9Li, 
c --  11Be, 13B, more 13N, 15C, hypernuclei, 
c --  total number of nuclei/states INMAX=165   --
c --  22 June: increas. table for IH=2 nuclei/states predicted theoretically
c INMAX=165+14, IH=2 from 6He to 9Be.
c --  23 Jan 07: increas table for IH=2 from 10Be to 12B INMAX=165+14+10
      DATA IAN6/
     *    1,    1,    1,    2,    3,    3,    3,    4,    4,    4, 
     *    4,    4,    5,    5,    5,    6,    6,    6,    6,    6,
     *    6,    7,    7,    7,    7,    7,    7,    7,    7,    7,
     *    7,    7,    7,    8,    8,    8,    8,    8,    8,    8,
     *    8,    8,    8,    9,    9,    9,    9,    9,    9,    9, 
     *    9,    9,    9,    9,    9,    9,    9,    9,    9,   10,
     *   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,
     *   10,   10,   10,   11,   11,   11,   11,   11,   11,   11,
     *   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,
     *   11,   11,   11,   12,   12,   12,   12,   12,   12,   12,
     *   12,   12,   12,   12,
     *   12,   12,   12,   12,   13,   13,   13,   13,   13,   13, 
     *   13,   13,   14,   14,   14,   14,   14,   14,   14,   14,
     *   14,   14,   14,   14,   14,   14,   14,   15,   15,   15,
     *   15,   15,   15,   15,   15,   15,   15,   15,   15,   15,
     *   15,   16,   16,   16,   16,   16,   16,   16,   16,   16,
     *   16,   16,
     *16,16,17,17,17,17,17,17,17,17,  17,17,17,17,18,18,18,18,18,18,
     *18,18,18,18,18,18,18,18,18,18,  18,18,18,111*0/
      DATA IZN6/
     *    0,    0,    1,    1,    1,    1,    2,    1,    1,    2,    
     *    2,    2,    2,    2,    3,    2,    2,    2,    3,    3,
     *    3,    2,    3,    3,    3,    3,    3,    3,    3,    3,
     *    4,    4,    4,    2,    2,    3,    3,    3,    3,    3,
     *    3,    4,    4,    3,    3,    3,    3,    3,    3,    4, 
     *    4,    4,    4,    4,    4,    4,    4,    5,    5,    4,
     *    4,    4,    4,    4,    5,    5,    5,    5,    5,    5,
     *    5,    6,    6,    4,    4,    4,    4,    4,    5,    5,
     *    5,    5,    5,    5,    5,    5,    5,    5,    5,    6, 
     *    6,    6,    6,    4,    4,    5,    5,    5,    5,    5,
     *    5,    5,    6,    6, 
     *    6,    6,    6,    6,    5,    6,    6,    6,    6,    6, 
     *    7,    7,    6,    6,    6,    6,    6,    7,    7,    7, 
     *    7,    7,    7,    7,    7,    7,    7,    6,    6,    7, 
     *    7,    7,    7,    7,    7,    7,    7,    7,    7,    8, 
     *    8,    7,    7,    7,    7,    8,    8,    8,    8,    8,
     *    8,    8,
     * 9, 9, 7, 7, 7, 8, 8, 8, 8, 8,   9, 9, 9, 9, 7, 7, 7, 8, 8, 8,
     * 8, 8, 9, 9, 9, 9, 9, 9, 9,10,  10,10,10,111*0/
      DATA IHN6/
     *    0,    1,    0,    0,    0,    1,    0,    1,    1,    0, 
     *    1,    1,    0,    1,    0,    0,    1,    2,    0,    0,
     *    1,    2,    0,    0,    1,    1,    1,    1,    1,    2,
     *    0,    0,    1,    0,    1,    0,    0,    1,    2,    2,
     *    2,    0,    1,    0,    1,    2,    2,    2,    2,    0,
     *    1,    1,    1,    2,    2,    2,    2,    0,    1,    0,
     *    0,    1,    2,    2,    0,    0,    0,    0,    0,    1,
     *    1,    0,    0,    0,    0,    2,    2,    2,    0,    0,
     *    0,    0,    0,    0,    0,    1,    1,    1,    2,    0,
     *    0,    0,    0,    2,    2,    0,    0,    0,    0,    1,
     *    2,    2,    0,    0,
     *    1,    1,    1,    1,    0,    0,    0,    0,    0,    1,
     *    0,    0,    0,    0,    0,    0,    1,    0,    0,    0,
     *    0,    0,    0,    0,    0,    0,    1,    0,    0,    0,
     *    0,    0,    0,    0,    0,    0,    0,    1,    1,    0,
     *    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,
     *    1,    1, 144*0/
      DATA ISP6/
     *    2,    2,    2,    3,    2,    2,    2,    1,    3,    1, 
     *    1,    3,    4,    2,    4,    1,    3,    1,    3,    1,
     *    3,    4,    4,    2,    2,    4,    6,    8,    2,    4,
     *    4,    2,    2,    1,    1,    5,    3,    3,    3,    7,
     *    5,    1,    1,    4,    2,    4,    2,    8,    6,    4, 
     *    2,    6,    4,    4,    2,    8,    6,    4,    4,    1,
     *    5,    1,    1,    5,    7,    3,    1,    3,    5,    3,
     *    5,    1,    5,    2,    2,    4,    5,    7,    4,    2,
     *    6,    4,   10,    6,    4,    6,    8,    2,    4,    4, 
     *    2,    6,    4,    1,    5,    3,    5,    5,    4,    3,
     *    1,    5,    1,    5, 
     *    3,    3,    3,    5,    4,    2,    2,    4,    6,    2, 
     *    2,   10,    1,    3,    8,    6,    1,    3,    1,    3, 
     *    1,    5,    3,    7,    3,    7,    3,    2,    6,    2, 
     *    8,    4,   10,    8,    2,    4,   14,    4,    2,    2, 
     *    8,    5,    1,    7,    3,    1,    8,    5,    3,    1, 
     *    3,    3,
     * 3, 7, 2, 4, 7, 6, 2, 2, 6, 4,   6, 2, 2, 6, 8, 5, 5, 1, 5,10,
     * 5, 3, 3,22, 3, 5, 5,20,15, 1,   5, 9, 6,111*0/
      DATA ENN6/
     * 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 1.05, 0.00, 
     * 0.00, 1.15,16.76, 0.00,16.66, 0.00, 0.00, 0.00, 0.00, 3.56, 
     * 0.00, 0.00, 0.00, 0.48, 0.00, 0.69, 2.05, 2.52, 3.88, 0.00,
     * 0.00, 0.43, 0.00, 0.00, 0.00, 0.00, 0.98, 0.00, 0.00, 1.36,
     * 5.63, 0.00, 0.00, 0.00, 0.00, 0.00, 0.73, 4.55, 5.96, 0.00, 
     * 0.00, 3.02, 3.07, 0.00, 0.71, 4.54, 5.92, 0.00, 0.00, 0.00,
     * 3.37, 0.00, 0.00, 2.86, 0.00, 0.72, 1.74, 2.15, 3.59, 0.00,
     * 0.10, 0.00, 3.35, 0.00, 0.32, 0.00, 1.78, 6.10, 0.00, 2.13,
     * 4.44, 5.02, 6.76, 7.29, 7.98, 0.00, 1.00, 1.48, 0.00, 0.00, 
     * 2.00, 4.32, 4.80, 0.00, 3.47, 0.00, 0.95, 1.67, 2.65, 0.00,
     * 0.00, 3.29, 0.00, 4.44, 
     * 0.00, 2.58, 6.89,10.68, 0.00, 0.00, 3.09, 3.68, 3.85, 0.00,
     * 0.00, 3.53, 0.00, 6.09, 6.69, 6.96, 0.00, 0.00, 2.31, 3.95, 
     * 4.92, 5.11, 5.69, 5.83, 6.20, 6.44, 0.00, 0.00, 0.74, 0.00, 
     * 5.28, 6.32, 7.22, 7.57, 8.31, 8.57, 9.15, 0.00, 2.27, 0.00, 
     * 5.22, 0.00, 0.12, 0.30, 0.40, 0.00, 6.10, 6.92, 7.12, 0.00,
     * 0.03, 6.56, 
     * 0.19, 0.72, 0.00, 1.37, 1.88, 0.00, 0.87, 3.06, 3.84, 4.55,
     * 0.00, 0.50, 3.10, 3.86, 0.06, 0.59, 1.74, 0.00, 1.98, 3.59,
     * 3.92, 4.46, 0.00, 1.04, 1.70, 2.10, 2.52, 3.10, 3.72, 0.00,
     * 1.89, 3.38, 3.60,111*0./
      END
c
      SUBROUTINE DISIMP(K,AMK,PN0,TN)
C O[PE[E[EH[E [M[[[[COB K [PA[MEHTOB- PN0(3,1-K) ([[B/C) B C[CTEME [X
C [.[.  AMK(1-K) ([[B) -[X MACC[. TN ([[B) - [X [O[HA[ K[H. [HEP[[[.
C (AHA[O[[[HO [.[.  KO[[[OB[-1970, C.229-239)
      DIMENSION ANL(3),PNC(3),VRS(3),PN0(3,500),AMK(500)
      SMK=0.
      DO 18 I=1,K
 18   SMK=SMK+AMK(I)
      DO 11 I=1,3
 11   VRS(I)=0.
      TKN=TN
      KL=K-1
      DO 10 L=1,KL
      LK=K-L+1
      AMP=AMK(LK)
      SMK=SMK-AMP
      AMR=SMK
      TPR=TKN
      IF(LK.LT.3) GO TO 20
      TKM=TKN*RNKSI(LK-1)
      TPR=TKN-TKM
 20   PMC=SQRT(2.*((AMP*AMR)/(AMP+AMR))*TPR)
      CALL ISOTR(ANL)
      DO 12 I=1,3
      PNC(I)=PMC*ANL(I)
      PN0(I,LK)=VRS(I)*AMP+PNC(I)
      VRS(I)=VRS(I)-PNC(I)/AMR
 12   CONTINUE
      TKN=TKM
 10   CONTINUE
      DO 21 I=1,3
 21   PN0(I,1)=VRS(I)*AMR
      RETURN
      END
      FUNCTION RNKSI(K)
C HAXO[[EH[E RNKSI(K) O[PE[E[[[[EE K[H.[HEP[[[ K [ACT[[ -T(K) B C[CTEME
C [X [.[. [EPE[ K[H.[HEP[[[ K+1 [ACT[[ -T(K+1) B C[CTEMME [X [.[.
C           (   T(K)=RNKSI(K)*T(K+1)    )
      CSIM=(3.*K-5.)/(3.*K-4.)
      PEX=1.5*K-2.5
      FCSIM=SQRT(1.-CSIM)*CSIM**PEX
 20   CSI=RNDM(-1)
      FCSI =SQRT(1.-CSI)*CSI**PEX
      RF=FCSIM*RNDM(-1)
      IF(RF.GT.FCSI) GO TO 20
      RNKSI=CSI
      RETURN
      END
      SUBROUTINE PINT(P,CT,ST,CF,SF,T,CM)
C O[PE[E[EH[E [O MACCE CM [ [M[[[[C[ P(3) [ACT[[[,EE K[H. [HEP[[[ T
C [ [[[OB - CT,ST,CF,SF ,O[PE[E[[[[[X HA[PAB[EH[E [O[ETA.
      DIMENSION P(3)
      PZ=P(3)**2
      PQ=P(1)**2+P(2)**2+PZ
      IF(PQ.LE.0.) GO TO 11
      CTQ=PZ/PQ
      T=SQRT(PQ+CM**2)-CM
      IF(CTQ.GE.1.) GO TO 10
      CT=SQRT(CTQ)
      IF(P(3).LE.0.) CT=-CT
      ST=SQRT(1.-CTQ)
      PMH=ST*SQRT(PQ)
      CF=P(1)/PMH
      SF=P(2)/PMH
      RETURN
 10   ST=0.
      CT=1.
      SF=0.
      CF=1.
      RETURN
 11   ST=0.
      CT=1.
      SF=0.
      CF=1.
      T=0.
      RETURN
      ENTRY TINP(P,CT,ST,CF,SF,T,CM)
C O[PE[E[EH[E [O MACCE CM,K[H.[HEP[[[ T [ [[[AM [ACT[[[,EE [M[[[[CA P(3)
      PM=SQRT(T*(T+2.*CM))
      P(3)=PM*CT
      PM=PM*ST
      P(1)=PM*CF
      P(2)=PM*SF
      RETURN
      END
      SUBROUTINE CLPV(P1,V,P2,E1)
C O[PE[E[EH[E [M[[[[CA [ACT[[[ P2(3) B [.C.K ,C[[TA[,[TO CTAPA[ C.K.([DE
C [M[[[[C [ACT[[[ P1(3)) [B[[ETC[ CO CKOP.V(3).E1-[O[H.[HEP[.[ACT[[[ B C
      DIMENSION P1(3),V(3),P2(3)
      SPV=0.
      V2=0.
      DO 10 I=1,3
      V2=V2+V(I)**2
      SPV=SPV+P1(I)*V(I)
 10   CONTINUE
      GAM=1./SQRT(1.-V2)
      TEMP=GAM*(SPV*GAM/(GAM+1.)+E1)
      DO 20 K=1,3
 20   P2(K)=P1(K)+V(K)*TEMP
      RETURN
      END
      SUBROUTINE ISOTR(ANL)
      DIMENSION ANL(3)
      CT=1.-2.*RNDM(-1)
      ST=SQRT(ABS(1.-CT*CT))
      ANL(3)=CT
      FI=6.2831843*RNDM(-1)
      ANL(1)=COS(FI)*ST
      ANL(2)=SIN(FI)*ST
      RETURN
      END
c ------------ end of Fermi-Break-up for hypernuclei -----------
      SUBROUTINE DELAM(ANUCL,ZNUCL,DM,SH,BARIER)
      COMMON /FUSR/ BARR,SMASS,SHLL /BLIDR/ILIDR
      IA=INT(ANUCL+0.5)
      IZ=INT(ZNUCL+0.5)
      IN=IA-IZ
      IF(IZ.LT.0.OR.IN.LT.0) GO TO 8
      ODD=-11.*(1+2*(IN/2)-IN+2*(IZ/2)-IZ)/SQRT(ANUCL)
      IF(IA.LT.65) GO TO 7
      IF(IN.GT.240.OR.IZ.GT.240) GO TO 7
      CM=0.
      SHLL=0.
      CBAR=0.
      NBAR=0
      CALL LYMASM(IZ,IA,CM,CBAR,NBAR)
      IF(NBAR.EQ.1) CBAR=BARR
      DM=CM
      SH=SHLL+ODD
      BARIER=CBAR
c ************* inclusion DELA8
      CALL DELA8(ANUCL,ZNUCL,DM8,SH8)
      DM=DM8
c ************* cold masses
      IF(ILIDR.GE.0) RETURN
      CALL DELA1(ANUCL,ZNUCL,DM1,SH1)
      DM=DM1
c 
      RETURN
 7    CONTINUE
      CALL DELA1(ANUCL,ZNUCL,DM1,SH1)
      BAR1=1001.
      IF(IA.GE.65) BAR1=BARIE1(ANUCL,ZNUCL)
      BARIER=BAR1
      DM=DM1
      SH=SH1
      IF(ILIDR.EQ.-1) RETURN
c ************* inclusion DELA8
      IF(ANUCL.GT.55.) CALL DELA8(ANUCL,ZNUCL,DM8,SH8)
      TCOEF=1.-(ANUCL-55.)/10.
      IF(ANUCL.GT.55.AND.ANUCL.LT.65.) DM=DM1*TCOEF+DM8*(1.-TCOEF)
c *************
      RETURN
 8    PRINT 9,IZ,IN
 9    FORMAT(2X,'ERROR IN DELAM: IZ=',I5,' IN=',I5)
      RETURN
      END
      SUBROUTINE DELA8(X,Y,DELTA8,DSHEL8)
      DSHEL8=0.
      I=INT(X+0.5)
      J=INT(Y+0.5)
      L=I-J
      DELTA8=1001.
      IF(I.LE.1.OR.J.LE.0.OR.L.LE.0) RETURN
      X03=X**0.3333333
      X06=X03*X03
      X13=X06*X06
      Y13=Y**1.3333333
      D=(X-2.*Y)/X
      DR=1-0.62025/X06
      ES=(25.8357-44.2355*D*D)*DR*DR*X06
      EC=0.779*(Y*(Y-1.)/X03)*(1.-1.5849/X06+1.2273/X+1.5772/X13)
      EEX=-0.4323*(Y13/X03)*(1.-0.57811/X03-0.14518/X06+0.49597/X)
      EV=X*(-17.0354+31.4506*D*D)
      DELTA8=8.071*X-0.783*Y+EV+ES+EC+EEX
      RETURN
      END
      SUBROUTINE DELA1(X,Y,DELTAM,DSHEL)
C  B[[[C[EH[E (M-A) CO[[ACTHO KAMEPOH[-57 (B [[[EPO[H[X E[[H[[AX)
C XOPO[EE COOTBETCTB[E [P[ Z.LE.8 [ CO[[ACOBAH[E [P[ [O[[[[X Z
C      ([[[ [E[E[ [C[APEH[[ [[ [E[K[X [[EP [ [X CAM[X)
      COMMON /BL1001/T1Y(130) /BL1002/T2XY(200)
      I=INT(X+0.5)
      J=INT(Y+0.5)
      L=I-J
      DELTAM=1001.
      IF(J.EQ.0) DELTAM=8.071*I
      IF(L.EQ.0) DELTAM=7.289*J
      IF(J.GT.10)  GO TO 11
      IF(I.EQ.1.AND.J.EQ.0) DELTAM=8.071
      IF(J.EQ.0)   GO TO 11
      GO TO (1,2,3,4,5,6,7,8,9,10),J
 1    IF(I.EQ.1.AND.J.EQ.1) DELTAM=7.289
      IF(I.EQ.2.AND.J.EQ.1) DELTAM=13.136
      IF(I.EQ.3.AND.J.EQ.1) DELTAM=14.950
      IF(I.EQ.4.AND.J.EQ.1) DELTAM=25.920
      IF(I.EQ.5.AND.J.EQ.1) DELTAM=33.790
      GO TO 11
 2    IF(I.EQ.3.AND.J.EQ.2) DELTAM=14.931
      IF(I.EQ.4.AND.J.EQ.2) DELTAM=2.425
      IF(I.EQ.5.AND.J.EQ.2) DELTAM=28.150
      IF(I.EQ.6.AND.J.EQ.2) DELTAM=17.593
      IF(I.EQ.7.AND.J.EQ.2) DELTAM=26.111
      IF(I.EQ.8.AND.J.EQ.2) DELTAM=31.599
      GO TO 11
 3    IF(I.EQ.5.AND.J.EQ.3) DELTAM=28.340
      IF(I.EQ.6.AND.J.EQ.3) DELTAM=14.087
      IF(I.EQ.7.AND.J.EQ.3) DELTAM=14.908
      IF(I.EQ.8.AND.J.EQ.3) DELTAM=20.947
      GO TO 11
 4    IF(I.EQ.6.AND.J.EQ.4) DELTAM=18.375
      IF(I.EQ.7.AND.J.EQ.4) DELTAM=15.769
      IF(I.EQ.8.AND.J.EQ.4) DELTAM=4.942
      IF(I.EQ.9.AND.J.EQ.4) DELTAM=11.348
      IF(I.EQ.10.AND.J.EQ.4) DELTAM=12.607
      IF(I.EQ.11.AND.J.EQ.4) DELTAM=20.176
      IF(I.EQ.12.AND.J.EQ.4) DELTAM=25.072
      GO TO 11
 5    IF(I.EQ.9.AND.J.EQ.5) DELTAM=12.415
      IF(I.EQ.10.AND.J.EQ.5) DELTAM=12.052
      IF(I.EQ.11.AND.J.EQ.5) DELTAM=8.668
      IF(I.EQ.12.AND.J.EQ.5) DELTAM=13.370
      IF(I.EQ.13.AND.J.EQ.5) DELTAM=16.562
      IF(I.EQ.14.AND.J.EQ.5) DELTAM=24.230
      GO TO 11
 6    IF(I.EQ.10.AND.J.EQ.6) DELTAM=15.702
      IF(I.EQ.11.AND.J.EQ.6) DELTAM=10.650
      IF(I.EQ.12.AND.J.EQ.6) DELTAM=0.
      IF(I.EQ.13.AND.J.EQ.6) DELTAM=3.125
      IF(I.EQ.14.AND.J.EQ.6) DELTAM=3.020
      GO TO 11
 7    IF(I.EQ.13.AND.J.EQ.7) DELTAM=5.346
      IF(I.EQ.14.AND.J.EQ.7) DELTAM=2.864
      IF(I.EQ.15.AND.J.EQ.7) DELTAM=0.102
      IF(I.EQ.16.AND.J.EQ.7) DELTAM=5.683
      IF(I.EQ.17.AND.J.EQ.7) DELTAM=7.871
      IF(I.EQ.18.AND.J.EQ.7) DELTAM=13.274
      IF(I.EQ.19.AND.J.EQ.7) DELTAM=15.790
      GO TO 11
 8    IF(I.EQ.14.AND.J.EQ.8) DELTAM=8.007
      IF(I.EQ.15.AND.J.EQ.8) DELTAM=2.863
      IF(I.EQ.16.AND.J.EQ.8) DELTAM=-4.737
      IF(I.EQ.17.AND.J.EQ.8) DELTAM=-0.809
      IF(I.EQ.18.AND.J.EQ.8) DELTAM=-0.783
      IF(I.EQ.19.AND.J.EQ.8) DELTAM=3.332
      IF(I.EQ.20.AND.J.EQ.8) DELTAM=3.799
      GO TO 11
 9    IF(I.EQ.17.AND.J.EQ.9) DELTAM=1.952
      IF(I.EQ.18.AND.J.EQ.9) DELTAM=0.873
      IF(I.EQ.19.AND.J.EQ.9) DELTAM=-1.486
      IF(I.EQ.20.AND.J.EQ.9) DELTAM=-0.016
      IF(I.EQ.21.AND.J.EQ.9) DELTAM=-0.046
      GO TO 11
 10   IF(I.EQ.19.AND.J.EQ.10) DELTAM=1.752
      IF(I.EQ.20.AND.J.EQ.10) DELTAM=-7.041
 11   DSHEL=0.
      IF(I.LE.1.OR.J.LE.0.OR.L.LE.0) RETURN
      X03=X**0.3333333
      X06=X03*X03
      X13=X06*X06
      Y13=Y**1.3333333
      D=(X-2.*Y)/X
      DR=1-0.62025/X06
      ES=(25.8357-44.2355*D*D)*DR*DR*X06
      EC=0.779*(Y*(Y-1.)/X03)*(1.-1.5849/X06+1.2273/X+1.5772/X13)
      EEX=-0.4323*(Y13/X03)*(1.-0.57811/X03-0.14518/X06+0.49597/X)
      EV=X*(-17.0354+31.4506*D*D)
      IF(J.LE.0.OR.J.GT.130.OR.L.LE.0.OR.L.GT.200) GO TO 22
      T1=T1Y(J)
      T2=T2XY(L)
      GO TO 23
 22   T1=0.
      T2=0.
 23   CONTINUE
      DELTA0=8.071*X-0.783*Y+EV+ES+EC+EEX+T1+T2
      IF(DELTAM.LT.1000.) GO TO 26
      DSHEL=T1+T2
      DELTAM=DELTA0
      RETURN
 26   DSHEL=DELTAM-DELTA0+T1+T2
      RETURN
      END
      BLOCK DATA EVAP
c ** corrected tables for GAM3(): 6He,... - sum over stable excited states
c (+ allowed by liquid-drop approx.) weighted with exp(-E/4.5MeV).
c October 99, Bologna. **
C [A[A[TC[ [APAMETP[ OCTAT.[[PA,[C[AP[[[[XC[ [PA[MEHTOB,O[PATHO[O
C CE[EH[[([OCTPOBCK[[-59),O[O[O[.[O[PABK[(KAMEPOH-57)
      COMMON /BL1001/T1Y(130)/BL1002/T2XY(200)/BL1014/GAM(35)
      COMMON /BL0999/RNCL  /BL1100/AMS,AMFS /BLGAM/GAM1(35),GAM2(35)
      COMMON /BL1005/AJ(35) /BL1006/ZJ(35) /BLCAN/ICAN /BLSEC/ISEC
      COMMON /EXILEV/ ENLEV(20,35),SPLEV(20,35)
      COMMON /BL1020/GAM3(35)
      DIMENSION TT11(65),TT12(65)
      DIMENSION TT21(50),TT22(50),TT23(50),TT24(50)
      EQUIVALENCE (T1Y(1),TT11(1)),(T1Y(66),TT12(1))
      EQUIVALENCE (T2XY(1),TT21(1)),(T2XY(51),TT22(1)),(T2XY(101),
     *TT23(1)),(T2XY(151),TT24(1))
      DATA GAM3/2.,2.,6.,6.,6.,4.,20., 6.,20.,22.,41.,59.,41.,8.,
     * 36., 34., 36.,144.,141.,153., 98.,34., 95., 25., 26.,187.,171.,
     *245., 68., 74.,190.,422.,0.,994.,216./
      DATA GAM2/2.,2.,6.,6.,6.,4.,20.,30.,20.,54.,73.,101.,73.,8.,
     *146.,100.,100.,343.,174.,393.,186.,61.,202.,113.,213.,233.,180.,
     *696.,194.,120.,458.,590.,0.,2790.,216./
      DATA GAM1/2.,2.,6.,6.,6.,4.,20., 6.,20.,18.,28., 40.,28.,8.,
     * 36., 10., 36., 70., 44., 36., 44.,12., 26., 14., 26., 42., 30.,
     * 80., 30., 16.,102., 18.,0.,54.,216./
      DATA GAM/2.,2.,6.,6.,6.,4.,20., 6.,20.,18.,28., 40.,28.,8.,
     * 36., 10., 36., 70., 44., 36., 44.,12., 26., 14., 26., 42., 30.,
     * 80., 30., 16.,102., 18.,0.,54.,216./
      DATA AJ/1.,1.,2.,3.,3.,4.,5.,6.,5.,6.,7.,8.,7.,8.,9.,10.,9.,
     *10.,11.,12.,11.,12.,13.,14.,13.,14.,15.,16.,15.,16.,17.,18.,0.,
     *18.,24./
      DATA ZJ/0.,1.,1.,1.,2.,2.,2.,2.,3.,3.,3.,3.,4.,4.,4.,4.,5.,5.,
     *5.,5.,6.,6.,6.,6.,7.,7.,7.,7.,8.,8.,8.,8.,0.,9.,11./
      DATA RNCL/1.5/,AMS/0.125/,AMFS/0.125/,ICAN/1/,ISEC/1/
      DATA ENLEV/700*0./,SPLEV/700*0./
      DATA TT11/
     *  20.80,  15.80,  21.00,  16.80,  19.80,
     *  16.50,  18.80,  16.50,  18.50,  17.20,
     *  18.26,  15.05,  16.01,  12.04,  13.27,
     *  11.09,  12.17,  10.26,  11.04,   8.41,
     *   9.79,   7.36,   8.15,   5.63,   5.88,
     *   3.17,   3.32,   0.82,   1.83,   0.97,
     *   2.33,   1.27,   2.92,   1.61,   2.91,
     *   1.35,   2.40,   0.89,   1.74,   0.36,
     *   0.95,  -0.65,  -0.04,  -1.73,  -0.96,
     *  -2.87,  -2.05,  -4.05,  -3.40,  -5.72,
     *  -3.75,  -4.13,  -2.42,  -2.85,  -1.01,
     *  -1.33,   0.54,  -0.02,   1.74,   0.75,
     *   2.24,   1.00,   1.98,   0.79,   1.54/
      DATA TT12/
     *   0.39,   1.08,   0.00,   0.78,  -0.35,
     *   0.58,  -0.55,   0.59,  -0.61,   0.59,
     *  -0.35,   0.32,  -0.96,  -0.52,  -2.08,
     *  -2.46,  -3.64,  -1.55,  -0.96,   0.97,
     *   0.88,   2.37,   1.75,   2.72,   1.90,
     *   2.55,   1.46,   1.93,   0.86,   1.17,
     *   0.08,   0.39,  -0.76,  -0.39,  -1.51,
     *  -1.17,  -2.36,  -1.95,  -3.06,  -2.62,
     *  -3.55,  -2.95,  -3.75,  -3.07,  -3.79,
     *  -3.06,  -3.77,  -3.05,  -3.78,  -3.12,
     *  -3.90,  -3.35,  -4.24,  -3.86,  -4.92,
     *  -5.06,  -6.77,  -7.41,  -9.18, -10.16,
     * -11.12,  -9.76,  -9.23,  -7.96,  -7.65/
      DATA TT21/
     *  -8.40, -12.90,  -8.00, -11.90,  -9.20,
     * -12.50, -10.80, -13.60, -11.20, -12.20,
     * -12.81, -15.40, -13.07, -15.80, -13.81,
     * -14.98, -12.63, -13.76, -11.37, -12.38,
     *  -9.23,  -9.65,  -7.64,  -9.17,  -8.05,
     *  -9.72,  -8.87, -10.76,  -8.64,  -8.89,
     *  -6.60,  -7.13,  -4.77,  -5.33,  -3.06,
     *  -3.79,  -1.72,  -2.79,  -0.93,  -2.19,
     *  -0.52,  -1.90,  -0.45,  -2.20,  -1.22,
     *  -3.07,  -2.42,  -4.37,  -3.94,  -6.08/
      DATA TT22/
     *  -4.49,  -4.50,  -3.14,  -2.93,  -1.04,
     *  -1.36,   0.69,   0.21,   2.11,   1.33,
     *   3.29,   2.46,   4.30,   3.32,   4.79,
     *   3.62,   4.97,   3.64,   4.63,   3.07,
     *   4.06,   2.49,   3.30,   1.46,   2.06,
     *   0.51,   0.74,  -1.18,  -1.26,  -3.54,
     *  -3.97,  -5.26,  -4.18,  -3.71,  -2.10,
     *  -1.70,  -0.08,  -0.18,   0.94,   0.27,
     *   1.13,   0.08,   0.91,  -0.31,   0.49,
     *  -0.78,   0.08,  -1.15,  -0.23,  -1.41/
      DATA TT23/
     *  -0.42,  -1.55,  -0.55,  -1.66,  -0.66,
     *  -1.73,  -0.75,  -1.74,  -0.78,  -1.69,
     *  -0.78,  -1.60,  -0.75,  -1.46,  -0.67,
     *  -1.26,  -0.51,  -1.04,  -0.53,  -1.84,
     *  -2.42,  -4.52,  -4.76,  -6.33,  -6.76,
     *  -7.81,  -5.80,  -5.37,  -3.63,  -3.35,
     *  -1.75,  -1.88,  -0.61,  -0.90,   0.09,
     *  -0.32,   0.55,  -0.13,   0.70,  -0.06,
     *   0.49,  -0.20,   0.40,  -0.22,   0.36,
     *  -0.09,   0.58,   0.12,   0.75,   0.15/
      DATA TT24/
     *   0.70,   0.17,   1.11,   0.89,   1.85,
     *   1.62,   2.54,   2.29,   3.20,   2.91,
     *   3.84,   3.53,   4.48,   4.15,   5.12,
     *   4.78,   5.75,   5.39,   6.31,   5.91,
     *   6.87,   6.33,   7.13,   6.61,   7.30,
     *   6.31,   6.27,   4.83,   4.49,   2.85,
     *   2.32,   0.58,  -0.11,  -0.98,   0.81,
     *   1.77,   3.37,   4.13,   5.60,   6.15,
     *   7.29,   7.35,   7.95,   7.67,   8.16,
     *   7.83,   8.31,   8.01,   8.53,   8.27/
       END
      FUNCTION BARIE1(A,Z)
C  B[[[C[EH[E [AP[EPA [E[EH[[ [[PA A,Z (M[B)
C  ( BARASHENKOV,ILJINOV,TONEEV -1972)
      COMMON /BL1001/T1Y(130) /BL1002/T2XY(200)
      I=INT(A+0.1)
      J=INT(Z+0.1)
      L=I-J
      X=Z*Z/A
      IF(X.LE.33.5) BF0=12.5+4.7*(33.5-X)**0.75
      IF(X.GT.33.5) BF0=12.5-2.7*(X-33.5)**0.666667
      IF((2*(J/2)).LT.J) D=0.
      IF((2*(J/2)).EQ.J) D=-0.5
      IF((2*(L/2)).EQ.L) D=D
      IF((2*(L/2)).LT.L) D=D+1.
      IF(J.LE.0.OR.J.GT.130.OR.L.LE.0.OR.L.GT.200) GO TO 22
      T1=T1Y(J)
      T2=T2XY(L)
      GO TO 23
 22   T1=0.
      T2=0.
 23   CONTINUE
      BARIE1=BF0+D-T1-T2
      IF(BARIE1.LE.0.) BARIE1=0.
      RETURN
      END
      SUBROUTINE LYMASM(IZ,IA,CMASS,CBARR,NOBARR)
C
C     WILLIAM D. MYERS - 6 JULY 1970
C
      COMMON /BLOCX/ IPARQ
      COMMON /FFSS/  ENEX
      COMMON /FUSR/   BARR,SMASS,SHLL
C     COMMON /BLOCXX/ ARQ,F1Q,F1MQ,SUFNUC
C     COMMON /CCC/    WOTNUC,VOLNUC,COULMB,
C    *                A,Z,UN,A1,A2,A3,GGMMA,A3RT2,A3RT,ZSQ,
C    *                ODDEV,SYM,PARMAS,ACOR
      DIMENSION EM(10),EMP(10),XK(10),Y(2),F(2)
      DATA ZVT/1.6666666666/, ZT/.3333333333/,
     1 ZTT/.6666666667/, SR5/2.2360679775/
      DATA EM/0.00,2.00,8.00,14.00,28.00,50.00,82.00,126.00,
     1        184.00,258.00/
      DATA CAY1/0.0/, CAY2/0.0/, CAY3/2.0/, CAY4/11.0/, CAY5/8.07144/,
     * CAY6/7.28899/,
     *            D/.444/, C/5.8/, SMALC/.325/
C
C     DMASS = REMAINDER AFTER CM - NO SHELL EFFECTS SUBTRACTED
C     SHLL = CALCULATED SHELL EFFECT
C     DIFMAS= DMASS - SHLL
C
C------------------------------
      A1 = 15.4941
      IPARQ=0
C..... IPARQ=0  PARAMETRS MYERS-SWIATECKI
      IF(IPARQ.NE.0) GO TO 121
      A2 = 17.9439
      A3 = 0.7053
      GAMMA = 1.7826
      GO TO 126
 121  CONTINUE
C..... IPARQ=1  PARAMETRS KRAPPE-NIX
      IF(IPARQ.NE.1) GO TO 123
      A2 = 24.70
      A3 = 0.74476032
      GAMMA = 4.0
      GO TO 126
 123  CONTINUE
C..... IPARQ=2  PARAMETRS PAULI-LEDERGERBER
      IF(IPARQ.NE.2) GO TO 124
      A2 = 19.008
      A3 = 0.720
      GAMMA = 2.840
      GO TO 126
 124  CONTINUE
C..... IPARQ=3  BF(T)   PARAMETRS MYERS-SWIATECKI
      ALEVEL=0.1
      AMPAR=ALEVEL*FLOAT(IA)
      TSQ=ENEX/AMPAR
      A2 = 17.9439*(1.-0.0063157*TSQ)
      A3 = 0.7053*(1.-0.001*TSQ)
      GAMMA = 1.7826
 126  CONTINUE
C------------------------------
      IF(IZ.NE.0) GO TO 15
      CMASS=0.0
      RETURN
   15 NOBARR=0
      DO 1 I=1,10
      EMP(I)=EM(I)**ZVT
    1 CONTINUE
      DO 2 I=1,9
      XK(I)=.600*(EMP(I+1)-EMP(I)) /(EM(I+1)-EM(I))
    2 CONTINUE
C
C     FOR DEFINITIONS OF CAY1 AND RZ,SEE UCRL-11980
C
      CAY1=3.28637900*A3**3
      RZ=.86398700/A3
    5 Z= FLOAT(IZ)
      ZSQ=Z**2
      N=IA-IZ
      UN= FLOAT(N)
      A= FLOAT(IA)
      A3RT=A**ZT
      A3RT2=A3RT**2
      A2RT= SQRT(A)
      SYM=((UN-Z)/A)**2
      ACOR=1.00-GAMMA*SYM
      PARMAS=CAY5*UN+CAY6*Z
      VOLNUC=-A1*ACOR*A
      SUFNUC=A2*ACOR*A3RT2
      COULMB=A3*ZSQ/A3RT
      FUZSUR=-CAY1*ZSQ/A
      ODDEV=-(1.00+2.00*(N/2)-UN+2.00*(IZ/2)-Z)/A2RT*CAY4
      WTERM=-CAY2*A3RT2* EXP(-CAY3*SYM)
      WOTNUC=PARMAS+COULMB+FUZSUR+ODDEV+WTERM
      SMASS=WOTNUC+VOLNUC+SUFNUC
      SPW=SUFNUC+WTERM
      C2=SPW/A3RT2
      X=.5*COULMB/SPW
      IF(X.GE.1.00) GO TO 4
C------------------------------
      ARQ=X
      IF(IPARQ.EQ.0) BARR=SUFNUC*XI(X)
      IF(IPARQ.EQ.2) BARR=SUFNUC*XI(X)
      IF(IPARQ.EQ.3) BARR=SUFNUC*XI(X)
      IF(IPARQ.EQ.1) BARR=SUFNUC*XIMOD(X)
      IF(IPARQ.EQ.0) F1Q=XI(X)
      IF(IPARQ.EQ.1) F1MQ=XIMOD(X)
C------------------------------
      GO TO 6
    4 BARR=0.0
    6 Y(1)=UN
      Y(2)=Z
      DO 31 J=1,2
      DO 32 I=1,9
      IF(Y(J)-EM(I+1)) 3,3,32
   32 CONTINUE
      PRINT 332,J
  332 FORMAT('1FAILURE IN LYMASS - Y(',I1,') EXCEEDS LAST MAGIC NO.')
      STOP
    3 F(J)=XK(I)*(Y(J)-EM(I))-.600*(Y(J)**ZVT-EMP(I))
   31 CONTINUE
      S=(2.00/A)**ZTT*(F(1)+F(2))-SMALC*A3RT
      C2D2=C2*D**2
      EE=(C2D2+C2D2)*(1.00-X)
      FF=.425917710*C2D2*D*(1.00+X+X)/A3RT
      SSHELL=C*S
      V=SSHELL/EE
      EPS=1.500*FF/EE
      IF(EE*(1.00-3.00*V).LE.0.00) GO TO 51
      QCALC=0.00
      THETA=0.00
      SHLL=SSHELL
      GO TO 52
C
C       ESTIMATE THETA
C
   51 TO=1.00
C
C       ITERATE TO FIND EQUILIBRIUM THETA
C
  101 DO 725 IPQ=1,10
      TO2=TO**2
C----------------------------------------
C     IF (TO2.GT.170.) PRINT 500, IZ,IA
C 500 FORMAT(1X,'LYMASM',2X,2I5)
C----------------------------------------
C     EXMT2= EXP(-TO2)
      EXMT2= 1.E-20
      IF((ABS(TO2)).LT.30.) EXMT2= EXP(-TO2)
C
      T=TO-(1.00-EPS*TO-V*(3.00-TO2-TO2)*EXMT2) /
     1(-EPS+V*TO*(10.00-4.00*TO2)*EXMT2)
      IF(T.LE.0.00) GO TO 728
      IF( ABS(T-TO) .LT.1.E-4) GO TO 732
      TO=T
  725 CONTINUE
      GO TO 729
  732 T2=T**2
C     EXT2= EXP(-T2)
      EXT2= 1.E-20
      IF((ABS(T2)).LT.30.) EXT2= EXP(-T2)
C
      TEST=EE*(1.00-EPS*(T+T)-V*((4.00*T2-12.00)*T2+3.00)* EXT2)
      IF(TEST.GT.0.00) GO TO 81
  728 TO=.100
      DO 100 I=1,20
      TO2=TO**2
      GL=EE*(1.00-EPS*TO-V*(3.00-TO2-TO2)* EXP(-TO2))
      IF(GL.GT.0.00) GO TO 101
  100 CONTINUE
  729 CMASS=SMASS
      CBARR=0.00
      NOBARR=1
      RETURN
   81 THETA=T
      ALPHA0=D*SR5/A3RT
      ALPHA=ALPHA0*THETA
      SIGMA=ALPHA*(1.00+ALPHA/14.00)
      EXS= EXP(SIGMA+SIGMA)- EXP(-SIGMA)
      QCALC=4.E-3*Z*(RZ*A3RT)**2*EXS
      T2=T**2
      SHLL=T2*(EE-FF*T) + SSHELL*(1.00-T2-T2)* EXP(-T2)
   52 CMASS=SMASS+SHLL
      CBARR=BARR-SHLL
      RETURN
      END
      FUNCTION XI(Z)
C
C     6-POINT LAGRANGE INTERPOLATION
C
      DIMENSION Y(51)
      DATA Y/.25900,.255200,.250700,.245100,.2400,.23400,.228500,
     1      .22200,.21600,.2100,.20300,.196800,.1900,.18300,.175800,
     2      .1692400,.1620300,.1547800,.147500,.1401900,.1328400,
     3      .1254500,.1180100,.1105200,.1029600,.0953500,.0876800,
     4      .0799900,.0722900,.064600,.0569500,.0493700,.0419300,
     5      .0347600,.0281100,.0223600,.0176200,.0137300,.0105600,
     6      .0079800,.0059100,.0042500,.0029600,.0019700,.0012300,
     7      7.1E-4,3.6E-4,1.5E-4,4.E-5,1.E-5,0.00/
C
C     THE X VALUES ARE EVENLY SPACED - X = 0(.02)1
C
      ZBH=Z*50.00
      M=IFIX(ZBH)
      DEL=ZBH- FLOAT(M)
      M=M+1
      IF(M.LE.51) GO TO 105
      M=51
  100 XI=Y(M)
      RETURN
  105 IF (DEL.LT.1.E-4) GO TO 100
      IF(M.GE.3) GO TO 110
      DEL=DEL- FLOAT(3-M)
      M=3
      GO TO 115
  110 IF(M.LE.48) GO TO 115
      DEL=DEL+ FLOAT(M-48)
      M=48
  115 DM3=DEL-3.00
      PROD=DM3*DEL
      W6=1.00/(1.2E2*DM3)
      DM2=DM3+1.00
      PROD=DM2*PROD
       W5=-1.00/(24.00*DM2)
      DM1=DM2+1.00
      PROD=DM1*PROD
      W4=1.00/(12.00*DM1)
      DP1=DM1+2.00
      PROD=DP1*PROD
      W2=1.00/(24.00*DP1)
      DP2=DP1+1.00
      PROD=DP2*PROD
      W1=-1.00/(1.2E2*DP2)
      W3=-1.00/(12.00*DEL)
      XI=PROD*(W1*Y(M-2)+W2*Y(M-1)+W3*Y(M)+W4*Y(M+1)+W5*Y(M+2)
     1 +W6*Y(M+3))
      RETURN
      END
      FUNCTION XIMOD(Z)
C
C     6-POINT LAGRANGE INTERPOLATION
C      IN MODIFIED LIQUID-DROP FORMULA
C         ( KRAPPE [ NIX --  IAEA-SM-174/12 )
C
      DIMENSION Y(51)
      DATA  Y/
     1    0.12200, 0.12100, 0.11980, 0.11830, 0.11690, 0.11520, 0.1133,
     2    0.11130, 0.10900, 0.10670, 0.10420, 0.10150, 0.09850, 0.09540,
     3    0.09180, 0.08780, 0.08350, 0.07900, 0.07460, 0.06960, 0.06470,
     4    0.05960, 0.05420, 0.04880, 0.04350, 0.03880, 0.03400, 0.02920,
     5    0.02460, 0.02020, 0.01580, 0.01220, 0.00900, 0.00660, 0.00490,
     6    0.00360, 0.00280, 0.00220, 0.00180, 0.00140, 0.00100, 0.00090,
     7    0.00060, 0.00040, 0.00020, 0.00010, 0.00000, 0.00000, 0.00000,
     8    0.00000, 0.00000/
C
C     THE X VALUES ARE EVENLY SPACED - X = 0(.02)1
C
      ZBH=Z*50.00
      M=IFIX(ZBH)
      DEL=ZBH- FLOAT(M)
      M=M+1
      IF(M.LE.51) GO TO 105
      M=51
  100 XIMOD=Y(M)
      RETURN
  105 IF (DEL.LT.1.E-4) GO TO 100
      IF(M.GE.3) GO TO 110
      DEL=DEL- FLOAT(3-M)
      M=3
      GO TO 115
  110 IF(M.LE.48) GO TO 115
      DEL=DEL+ FLOAT(M-48)
      M=48
  115 DM3=DEL-3.00
      PROD=DM3*DEL
      W6=1.00/(1.2E2*DM3)
      DM2=DM3+1.00
      PROD=DM2*PROD
       W5=-1.00/(24.00*DM2)
      DM1=DM2+1.00
      PROD=DM1*PROD
      W4=1.00/(12.00*DM1)
      DP1=DM1+2.00
      PROD=DP1*PROD
      W2=1.00/(24.00*DP1)
      DP2=DP1+1.00
      PROD=DP2*PROD
      W1=-1.00/(1.2E2*DP2)
      W3=-1.00/(12.00*DEL)
      XIMOD=PROD*(W1*Y(M-2)+W2*Y(M-1)+W3*Y(M)+W4*Y(M+1)+W5*Y(M+2)
     1 +W6*Y(M+3))
      RETURN
      END
c
