###################
##  Generator level filter for B->Ds(K-pi+)l
##
##  C.Voena  Feb 2, 2002
##
##################



mod talk GefSelectFilter 
BooObjects GefKinematic lump   MyKp = e+ mu+ pi+ K+ p+
BooObjects GefKinematic lump   MyKm = e- mu- pi- K- anti-p-
BooObjects GefKinematic lump   MyPp = e+ mu+ pi+ K+ p+
BooObjects GefKinematic lump   MyPm = e- mu- pi- K- anti-p-

BooObjects GefKinematic define MyKpt = K+
BooObjects GefKinematic define MyKmt = K-

BooObjects GefKinematic define MyKp = MyKp -100. 100. -100. 100. -100. 100. 0.493
BooObjects GefKinematic define MyKm = MyKm -100. 100. -100. 100. -100. 100. 0.493
BooObjects GefKinematic define MyPp = MyPp -100. 100. -100. 100. -100. 100. 0.139
BooObjects GefKinematic define MyPm = MyPm -100. 100. -100. 100. -100. 100. 0.139

BooObjects GefKinematic combine MyPhi = MyKp MyKm 0.999 1.039
BooObjects GefKinematic combine MyDsPhP+ = MyPhi MyPp 1.944 1.994
BooObjects GefKinematic combine MyDsPhP- = MyPhi MyPm 1.944 1.994


BooObjects GefKinematic combine MyKst = MyKp MyPm 0.821 0.971
BooObjects GefKinematic combine MyKstbar = MyKm MyPp 0.821 0.971
BooObjects GefKinematic combine MyDsKstK+ = MyKstbar MyKpt 1.944 1.994
BooObjects GefKinematic combine MyDsKstK- = MyKst MyKmt 1.944 1.994


BooObjects GefKinematic combine MyKs = MyPp MyPm 0.488 0.512
BooObjects GefKinematic combine MyDsKsK+ = MyKs MyKpt 1.944 1.994
BooObjects GefKinematic combine MyDsKsK- = MyKs MyKmt 1.944 1.994


BooObjects GefKinematic combine  MyB0 = MyDsPhP- MyPp -100. 100.  -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20 
BooObjects GefKinematic combine  MyB0bar = MyDsPhP+ MyPm -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20 
BooObjects GefKinematic combine  MyB02 = MyDsKsK- MyPp  -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100.  5.18 -.20 .20 
BooObjects GefKinematic combine  MyB0bar2 = MyDsKsK+  MyPm -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20
BooObjects GefKinematic combine  MyB03 = MyDsKstK- MyPp -100. 100.  -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20 
BooObjects GefKinematic combine  MyB0bar3 = MyDsKstK+  MyPm -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20

BooObjects GefKinematic combine  MyB04 = MyDsPhP- MyKpt  -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20 
BooObjects GefKinematic combine  MyB0bar4 = MyDsPhP+   MyKmt -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20 
BooObjects GefKinematic combine  MyB05 = MyDsKsK- MyKpt  -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20 
BooObjects GefKinematic combine  MyB0bar5 = MyDsKsK+  MyKmt -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20
BooObjects GefKinematic combine  MyB06 = MyDsKstK- MyKpt  -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20 
BooObjects GefKinematic combine  MyB0bar6 = MyDsKstK+  MyKmt -100. 100. -100. 100. -100. 100. -100. 100. -100. 100. 5.18 -.20 .20

BooObjects GefKinematic  require  MyB0
BooObjects GefKinematic  require  MyB0bar
BooObjects GefKinematic  require  MyB02
BooObjects GefKinematic  require  MyB0bar2
BooObjects GefKinematic  require  MyB03
BooObjects GefKinematic  require  MyB0bar3
BooObjects GefKinematic  require  MyB04
BooObjects GefKinematic  require  MyB0bar4
BooObjects GefKinematic  require  MyB05
BooObjects GefKinematic  require  MyB0bar5
BooObjects GefKinematic  require  MyB06
BooObjects GefKinematic  require  MyB0bar6
beforeFilter  set GefKinematic

exit

mod talk  GfiEvtGen 
    maxNumFilterTrials set 1000000
exit







