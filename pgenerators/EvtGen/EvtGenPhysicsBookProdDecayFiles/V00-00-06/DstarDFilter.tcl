mod talk GefSelectFilter 

# 
BooObjects GefKinematic lump   MyPp = e+ mu+ pi+ K+ p+
BooObjects GefKinematic lump   MyPm = e- mu- pi- K- anti-p-
BooObjects GefKinematic lump   MyKp = e+ mu+ pi+ K+ p+
BooObjects GefKinematic lump   MyKm = e- mu- pi- K- anti-p-

BooObjects GefKinematic define MyPp = MyPp -100. 100. 0.1 100. -100. 100. 0.139
BooObjects GefKinematic define MyPm = MyPm -100. 100. 0.1 100. -100. 100. 0.139

BooObjects GefKinematic define MyP0 = pi0 
#BooObjects GefKinematic define MyKp =  K+  -100. 100. 0.1 100. -100. 100. 0.493
#BooObjects GefKinematic define MyKm =  K- -100. 100. 0.1 100. -100. 100. 0.493
BooObjects GefKinematic define MyKp = MyKp -100. 100. 0.1 100. -100. 100. 0.493
BooObjects GefKinematic define MyKm = MyKm -100. 100. 0.1 100. -100. 100. 0.493

BooObjects GefKinematic define MyKS0 = K_S0 
# 
BooObjects GefKinematic define AnotherPip =  pi+
BooObjects GefKinematic define AnotherPim =  pi-
BooObjects GefKinematic define AnotherKp =  K+
BooObjects GefKinematic define AnotherKm =  K-
BooObjects GefKinematic combine MyD0Orig =  AnotherKm AnotherPip 
BooObjects GefKinematic combine MyD0Origbar = AnotherKp AnotherPim 
BooObjects GefKinematic combine MyD01 = AnotherKm AnotherPip  1.8644 1.8646
BooObjects GefKinematic combine MyD01bar = AnotherKp AnotherPim   1.8644 1.8646
BooObjects GefKinematic combine MyD02 = MyD0Orig MyP0  1.8644 1.8646
BooObjects GefKinematic combine MyD02bar = MyD0Origbar MyP0  1.8644 1.8646


BooObjects GefKinematic combine MyD*+1 = MyD01 AnotherPip 2.0095 2.0105
BooObjects GefKinematic combine MyD*-1 = MyD01bar AnotherPim  2.0095 2.0105
BooObjects GefKinematic combine MyD*+2 = MyD02 AnotherPip   2.0095 2.0105
BooObjects GefKinematic combine MyD*-2 = MyD02bar AnotherPim 2.0095 2.0105

#
BooObjects GefKinematic combine MyPpP0 = MyPp MyP0 
BooObjects GefKinematic combine MyPmP0 = MyPm MyP0 
#
BooObjects GefKinematic combine MyPpPp = MyPp MyPp 
BooObjects GefKinematic combine MyPpPpPm = MyPpPp MyPm 
BooObjects GefKinematic combine MyPmPm = MyPm MyPm 
BooObjects GefKinematic combine MyPmPmPp = MyPmPm MyPp 
#
BooObjects GefKinematic combine MyPpPm = MyPp MyPm 
#
BooObjects GefKinematic combine MyDp1 = MyKm MyPpPp 1.849 1.889
BooObjects GefKinematic combine MyDm1 = MyKp MyPmPm 1.849 1.889
#
BooObjects GefKinematic combine MyDp2 = MyKS0 MyPp 1.849 1.889
BooObjects GefKinematic combine MyDm2 = MyKS0 MyPm 1.849 1.889
#
BooObjects GefKinematic combine MyKpPp = MyKp MyPp 
BooObjects GefKinematic combine MyDp3 = MyKm MyKpPp 1.849 1.889
BooObjects GefKinematic combine MyKmPm = MyKm MyPm 
BooObjects GefKinematic combine MyDm3 = MyKp MyKmPm 1.849 1.889
#


BooObjects GefKinematic combine  MyB01 = MyD*+1 MyDm1 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB02 = MyD*-1 MyDp1 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB03 = MyD*+1 MyDm2 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB04 = MyD*-1 MyDp2 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB05 = MyD*+1 MyDm3 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB06 = MyD*-1 MyDp3 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB07 = MyD*+2 MyDm1 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB08 = MyD*-2 MyDp1 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB09 = MyD*+2 MyDm2 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB10 = MyD*-2 MyDp2 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB11 = MyD*+2 MyDm3 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
BooObjects GefKinematic combine  MyB12 = MyD*-2 MyDp3 -100. 100. -100. 100. -100. 100.  -100. 100. -100. 100. 5.20   -0.2 0.2
#

#

BooObjects GefKinematic  require  MyB01 MyB02 MyB03 MyB04 MyB05 MyB06  MyB07 MyB08 MyB09 MyB10 MyB11 MyB12
beforeFilter  set GefKinematic
#


exit
