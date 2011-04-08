c----------------------------------------------------------------------
c  STUDY qQ-->mu,mu,G  WITHOUT  qG-->mu,mu,q     
C--------------------------------------------------------------------
      PARAMETER (NEV=200 )   ! total number of events to generate

C...HEPEVT commonblock.
      implicit real*8 (a-h,o-z)
      PARAMETER (NMXHEP=4000)    
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
                               
c      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
     
      INTEGER PYK,PYCHGE,PYCOMP
      PARAMETER (KSUSY1=1000000,KSUSY2=2000000,KEXCIT=4000000)      
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4) 
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)!cros-sectn inf

      COMMON/PYINT6/PROC(0:200)      


      external pydata



c=================  Processes   ====================================

      MSTU(23) = 0 ! put number of errors = 0  

      MSTU(12)=0   ! off title page

c----------  Choice of the nessesary process --------------------
      MSEL=0       
      MSUB(1)=1        ! qQ-->A*/Z0* 
      
c-------------------  Nessesary limits for PANDA ENERGY region ---------

      PARP(2)=0.5d0           ! Lowest inv. Mass (D=10.GeV)


      PARP(111)=0.1d0       ! Minimum invar. mass of the
                            ! remnant hadronic system (D=2.GeV)

   
       CKIN(1)= 1.0d0       ! limits for shat 

C       CKIN(2)= -1.d0      ! no upper limit for S-HAT 

       CKIN(5)= 0.1d0      ! low cut limits for PT_-range
       CKIN(6)= 0.1d0      ! low cut limits for intermediate mass

	
c--------------------Inizialisation----------------------------------

      Ebeam=14.d0             ! 9.              ! PANDA             
      CALL PYINIT ('FIXT','p~-','p+',Ebeam)

      open(91,file="genevent.txt",form="formatted",
     >     status="unknown")
      open(92,file="genevent.dat",form="unformatted",
     >     status="unknown")
  
           
      DO 100 IEV=1,NEV  ! events loop
    
      CALL PYEVNT     ! make an event
      call pyhepc(1)
      

c--------------------------------------------------------------
c      DO 10 I=1,N 
c--------------------------------------------------------------
c      CALL PYLIST(1)
c      analyses
c10    CONTINUE    ! end of listing loop
 
      CALL PYLIST(1)  ! print an event tree

c         call writehepevt_ascii(91,nvers, iev,
         call writehepevt_ascii(91, iev,
     >    nhep, isthep, idhep, jmohep, jdahep,
     >    phep, vhep)

c         call writehepevt(92,iendi,nvers, iev,
        call writehepevt(92, iev,
     >    nhep, isthep, idhep, jmohep, jdahep,
     >    phep, vhep)
      

100   CONTINUE    ! end of events loop   

      close(91)
      close(92)
 
c       CALL PYlist(12)
c       CALL PYSTAT(1)
      
       END 
           
       
c*******************************************************************
c      subroutine writehepevt(iunit, iendi, nvers, nevhep,
      subroutine writehepevt(iunit,  nevhep,
     >  nhep, isthep, idhep, jmohep, jdahep,
     >  phep, vhep)
      
      integer*4  iunit, iendi, nvers, nevhep, nhep
      integer*4  isthep(nhep), idhep(nhep)
      integer*4  jmohep(2,nhep), jdahep(2,nhep)
      real*8     phep(5,nhep), vhep(4,nhep)
     
c      write(iunit) iendi, nvers, 
      write(iunit)
     >  nevhep, nhep, (isthep(k),k=1,nhep),
     > (idhep(k),k=1,nhep),
     > ((jmohep(m,k),m=1,2),k=1,nhep),
     > ((jdahep(m,k),m=1,2),k=1,nhep),
     > ((phep(m,k),m=1,5),k=1,nhep),
     > ((vhep(m,k),m=1,4),k=1,nhep)

      return
      end


c*******************************************************************
c      subroutine writehepevt_ascii(iunit, nvers, nevhep,
      subroutine writehepevt_ascii(iunit, nevhep,
     >  nhep, isthep, idhep, jmohep, jdahep,
     >  phep, vhep)
      
      integer*4  iunit, nvers, nevhep, nhep
      integer*4  isthep(nhep), idhep(nhep)
      integer*4  jmohep(2,nhep), jdahep(2,nhep)
      real*8     phep(5,nhep), vhep(4,nhep)
     
c      write(iunit,*) nhep, nevhep, nvers
      write(iunit,*)  nevhep, nhep
      do 1001 ihep=1, nhep 
         write(iunit,80) 
     >    isthep(ihep), idhep(ihep), 
     >    jmohep(1,ihep), jmohep(2,ihep),
     >    jdahep(1,ihep), jdahep(2,ihep),
     >    (phep(m,ihep),m=1,5),(vhep(m,ihep),m=1,4)
1001   continue
80     format(6i10,9(1x,d15.8))
      return
      end
