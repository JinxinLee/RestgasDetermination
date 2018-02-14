############################################
### FTS: geometry version #2
### Chamber 5 and 6 with same dimensions
### RICH detector after chamber 6 
############################################
# all dimensions are in mm.
# straw made by mylar
# gas: STTmix9010_2bar 
# tungsteno
###################
# number of straws n the first layer: 132 straws
$nstraws=132;
$pigreco=3.141592653589793;
$alpha=5;
$cosangle=cos($alpha*$pigreco/180);
$sinangle=sin($alpha*$pigreco/180);

# straw length
$len = 640. ;

# straw outer diameter
$diametrostraw = 10.1;  
$Zpos = 2954.-$diametrostraw/2;
#  mylar thick
$spessoremylar = 0.03;

# sense wire diameter, wire material W
$diametrofilo= 0.02;

################################################################

# posizione X,Y del CENTRO della straw piu' a DESTRA guardando
# verso downstream
#$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$Ypos = 0.;

$semilen = $len/2.;
$raggio = $diametrostraw/2.;

# cave
open OUT,">../../geometry/fts.geo";
print OUT "//*********************
fts01assembly#1
cave
ASSEMBLY
air
0.  0.   $Zpos
1.   0.   0.   0.   1.   0.   0.   0.   1.
";
########################

$Zpos=0.;

#######################
print OUT "//*********************
fts01tube#1
fts01assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts01gas#1
fts01tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts01wire#1
fts01gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";

for($i=2; $i<61;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;

print OUT "//*********************
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

 ############short tube#########for the beam pipe hole#########bassi
 $i=61;
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 $shortHalf = 112; #(640-192)/4
 $raggio = $diametrostraw/2.;
 $hhole = 192; # h of the hole for the beam pipe
 $Yshort = $shortHalf+($hhole/2);
 print OUT "//*********************
 fts01tubedown#$i
 fts01assembly#1
 TUBE
 mylar
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 $Xpos -$Yshort. $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.
 ";

 #
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts01gasdown#$i
 fts01tubedown#$i
 TUBE
 STTmix9010_2bar
 0. 0. -$shortHalf 
 0. $raggio
 0. 0. $shortHalf 
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 # ancora all'interno un filo di tungsteno
 $raggio = $diametrofilo/2.;
 print OUT "//*********************
 fts01wiredown#$i
 fts01gasdown#$i
 TUBE
 tungsten
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 #### tubi sotto la beam pipe
 for($i=62; $i<73;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 print OUT "//*********************
 fts01tubedown#$i
 fts01assembly#1
 $Xpos -$Yshort. $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }

 $i=61;
 ####tubi sopra la beam pipe
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 $raggio = $diametrostraw/2.;
 print OUT "//*********************
 fts01tubeup#$i
 fts01assembly#1
 TUBE
 mylar
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 $Xpos $Yshort. $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.
 ";
 #
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts01gasup#$i
 fts01tubeup#$i
 TUBE
 STTmix9010_2bar
 0. 0. -$shortHalf 
 0. $raggio
 0. 0. $shortHalf 
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 $raggio = $diametrofilo/2.;
 print OUT "//*********************
 fts01wireup#$i
 fts01gasup#$i
 TUBE
 tungsten
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 ####tubi sopra la beam pipe
 for($i=62; $i<73;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 print OUT "//*********************
 fts01tubeup#$i
 fts01assembly#1
 $Xpos $Yshort. $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 for($i=73; $i<133;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 print OUT "//*********************
 fts01tube#$i
 fts01assembly#1
 $Xpos $Ypos $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }

 #############first layer completed##################
 #  ora metto le altre $nstraws copie della seconda file di straws,                  
 # sono anche staggerate di mezzo posto verso sinistra                               
 $z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
 for($i=133; $i<193;$i++){
 $Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws + 1) ))-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tube#$i
 fts01assembly#1
 $Xpos $Ypos $z2
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 #### tubi sotto la beam pipe
 for($i=193; $i<205;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws + 1)) )-$diametrostraw/4;
 print OUT "//*********************
 fts01tubedown#$i
 fts01assembly#1
 $Xpos -$Yshort. $z2
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 for($i=193; $i<205;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws + 1)) )-$diametrostraw/4;
 print OUT "//*********************
 fts01tubeup#$i
 fts01assembly#1
 $Xpos $Yshort. $z2
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 for($i=205; $i<265;$i++){
 $Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws + 1)) )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tube#$i
 fts01assembly#1
 $Xpos $Ypos $z2 
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 #########primo double layers completato#########
 ########secondo double layers: inclinazione dei tubi di +5 gradi######### 
 $Zpos = 50-$diametrostraw/2;
 $Xpos = $diametrostraw*( $nstraws/2 -0.5);
 $raggio = $diametrostraw/2.;

 for($i=265; $i<325;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1)) )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tube#$i
 fts01assembly#1
 $Xpos $Ypos $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=325; $i<337;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tubedown#$i
 fts01assembly#1
 $Xpos -$Yshort $Zpos 
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=325; $i<337;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tubeup#$i
 fts01assembly#1
 $Xpos $Yshort $Zpos 
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=337; $i<397;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1)) )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tube#$i
 fts01assembly#1
 $Xpos $Ypos $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 #####secondo strato########
 $z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
 $raggio = $diametrostraw/2.;

 for($i=397; $i<457;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws + 1)) )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tube#$i
 fts01assembly#1
 $Xpos $Ypos $z2
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=457; $i<469;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw  )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tubedown#$i
 fts01assembly#1
 $Xpos -$Yshort $z2 
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=457; $i<469;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw  )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tubeup#$i
 fts01assembly#1
 $Xpos $Yshort $z2
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=469; $i<529;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws + 1)) )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts01tube#$i
 fts01assembly#1
 $Xpos $Ypos $z2
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 #########secondo double layers completato#########
 ########terzo double layers: inclinazione dei tubi di -5 gradi#########
 #$Zpos = 3054-$diametrostraw/2;
  $Zpos = 100-$diametrostraw/2;
  $Xpos = $diametrostraw*( $nstraws/2 -0.5);
  $raggio = $diametrostraw/2.;

  for($i=529; $i<589;$i++){
  $Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5  - ($i - (4*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $Zpos
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  for($i=589; $i<601;$i++){
  $Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubedown#$i
  fts01assembly#1
  $Xpos -$Yshort $Zpos 
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  for($i=589; $i<601;$i++){
  $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubeup#$i
  fts01assembly#1
  $Xpos $Yshort $Zpos 
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  for($i=601; $i<661;$i++){
  $Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $Zpos
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  #########secondo strato###############
  $z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
  $raggio = $diametrostraw/2.;

  for($i=661; $i<721;$i++){
  $Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $z2
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  for($i=721; $i<733;$i++){
  $Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubedown#$i
  fts01assembly#1
  $Xpos -$Yshort $z2 
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  for($i=721; $i<733;$i++){
  $Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubeup#$i
  fts01assembly#1
  $Xpos $Yshort $z2
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  for($i=733; $i<793;$i++){
  $Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $z2
  $cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
  }
  #########terzo double layers completato#########
  ########quarto double layers: inclinazione dei tubi di 0 gradi#########
  #$Zpos = 3104-$diametrostraw/2;
  $Zpos = 150-$diametrostraw/2;
  $Xpos = $diametrostraw*( $nstraws/2 -0.5);
  $raggio = $diametrostraw/2.;

  for($i=793; $i<853;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $Zpos
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
  for($i=853; $i<865;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubedown#$i
  fts01assembly#1
  $Xpos -$Yshort $Zpos 
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
  for($i=853; $i<865;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubeup#$i
  fts01assembly#1
  $Xpos $Yshort $Zpos 
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
  for($i=865; $i<925;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $Zpos
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
  #########secondo strato###############
  $z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
  $raggio = $diametrostraw/2.;

  for($i=925; $i<985;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $z2
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
  for($i=985; $i<997;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubedown#$i
  fts01assembly#1
  $Xpos -$Yshort $z2 
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
  for($i=985; $i<997;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tubeup#$i
  fts01assembly#1
  $Xpos $Yshort $z2
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
  for($i=997; $i<1057;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
  print OUT "//*********************                                                           
  fts01tube#$i
  fts01assembly#1
  $Xpos $Ypos $z2
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }


  ##########################################################
  #################FTS2#####################################
  ##########################################################
 $Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
 $Ypos = 0.;
 $Zpos = 3274.-$diametrostraw/2;
 print OUT "//*********************
 fts02assembly#1
 cave
 ASSEMBLY
 air
 0.  0.   $Zpos
 1.   0.   0.   0.   1.   0.   0.   0.   1.
 ";

########################
 $Zpos=0.;
#######################
 print   OUT "//*********************
 fts02tube#1
 fts02assembly#1
 TUBE
 mylar
 0. 0. -$semilen 
 0. $raggio
 0. 0. $semilen  
 $Xpos $Ypos $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.
 ";

# all'interno un cilindro di STTmix9010_2bar
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts02gas#1
 fts02tube#1
 TUBE
 STTmix9010_2bar
 0. 0. -$semilen  
 0. $raggio
 0. 0. $semilen  
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts02wire#1
fts02gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";

for($i=2; $i<61;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;

print OUT "//*********************
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

############short tube#########for the beam pipe hole#########bassi
 $i=61;
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 $shortHalf = 112; #(640-192)/4
 $raggio = $diametrostraw/2.;
 $hhole = 192; # h of the hole for the beam pipe
 $Yshort = $shortHalf+($hhole/2);
print OUT "//*********************
fts02tubedown#$i
fts02assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts02gasdown#$i
fts02tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts02wiredown#$i
fts02gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
#### tubi sotto la beam pipe
for($i=62; $i<73;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

$i=61;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts02tubeup#$i
fts02assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts02gasup#$i
fts02tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts02wireup#$i
fts02gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";

####tubi sopra la beam pipe
for($i=62; $i<73;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

for($i=73; $i<133;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#############primo layer completto##################


#  ora metto le altre  copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=133; $i<193;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=193; $i<205;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=193; $i<205;$i++){
$Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=205; $i<265;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########primo double layers completato#########
########secondo double layers: inclinazione dei tubi di +5 gradi######### 
$Zpos = 50-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=265; $i<325;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=325; $i<337;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $Zpos 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=325; $i<337;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $Zpos 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=337; $i<397;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#####secondo strato########
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=397; $i<457;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - (3*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=457; $i<469;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - (3*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw  )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $z2 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=457; $i<469;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - (3*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw  )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=469; $i<529;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#########secondo double layers completato#########
########terzo double layers: inclinazione dei tubi di -5 gradi#########
$Zpos = 100-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;

for($i=529; $i<589;$i++){
$Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5  - ($i - (4*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=589; $i<601;$i++){
$Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $Zpos 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=589; $i<601;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $Zpos 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=601; $i<661;$i++){
$Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=661; $i<721;$i++){
$Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=721; $i<733;$i++){
$Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $z2 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=721; $i<733;$i++){
$Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=733; $i<793;$i++){
$Xpos =  (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#########terzo double layers completato#########
########quarto double layers: inclinazione dei tubi di 0 gradi#########
$Zpos = 150-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;

for($i=793; $i<853;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=853; $i<865;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=853; $i<865;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=865; $i<925;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=925; $i<985;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=985; $i<997;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubedown#$i
fts02assembly#1
$Xpos -$Yshort $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=985; $i<997;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tubeup#$i
fts02assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=997; $i<1057;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws + 1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts02tube#$i
fts02assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

##########################################################
#################FTS3#####################################
##Camere dentro il magnete: ogni DL ha lunghezza diversa##
##########################################################
##########################################################
$nstraws=176;
#  lunghezza di una straw
$len = 690.3;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$Ypos = 0.;
$Zpos = 3945-$diametrostraw/2;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;

print OUT "//*********************
fts03assembly#1
cave
ASSEMBLY
air
0.  0.  $Zpos
1.   0.   0.   0.   1.   0.   0.   0.   1.
";
#
$Zpos=0.;
$Ypos=0.;
print OUT "//*********************
fts31tube#1
fts03assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
# 
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts31gas#1
fts31tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts31wire#1
fts31gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<81;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts31tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
 ############short tube#########for the beam pipe hole#########bassi
 $i=81;
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 $hhole = 204; # h of the hole for the beam pipe
 $shortHalf = ($len - $hhole)/4; #(690.3-204)/4=121.5
 $raggio = $diametrostraw/2.;
 $Yshort = $shortHalf+($hhole/2);
 print OUT "//*********************
 fts31tubedown#$i
 fts03assembly#1
 TUBE
 mylar
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 $Xpos -$Yshort $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.
 ";
 #
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts31gasdown#$i
 fts31tubedown#$i
 TUBE
 STTmix9010_2bar
 0. 0. -$shortHalf 
 0. $raggio
 0. 0. $shortHalf 
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 # ancora all'interno un filo di tungsteno
 $raggio = $diametrofilo/2.;
 print OUT "//*********************
 fts31wiredown#$i
 fts31gasdown#$i
 TUBE
 tungsten
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
  for($i=82; $i<97;$i++){
  $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
  print OUT "//*********************
  fts31tubedown#$i
  fts03assembly#1
  $Xpos -$Yshort $Zpos
  1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
  }
 $i=81;
 ####tubi sopra la beam pipe
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 $raggio = $diametrostraw/2.;
 print OUT "//*********************
 fts31tubeup#$i
 fts03assembly#1
 TUBE
 mylar
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 $Xpos $Yshort $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.
 ";
 #
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts31gasup#$i
 fts31tubeup#$i
 TUBE
 STTmix9010_2bar
 0. 0. -$shortHalf 
 0. $raggio
 0. 0. $shortHalf 
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 # ancora all'interno un filo di tungsteno
 $raggio = $diametrofilo/2.;
 print OUT "//*********************
 fts31wireup#$i
 fts31gasup#$i
 TUBE
 tungsten
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 for($i=82; $i<97;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 print OUT "//*********************
 fts31tubeup#$i
 fts03assembly#1
 $Xpos $Yshort $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 for($i=97; $i<177;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 print OUT "//*********************
 fts31tube#$i
 fts03assembly#1
 $Xpos $Ypos $Zpos
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 #
 #  ora metto le altre 128 copie della seconda file di straws,                  
 # sono anche staggerate di mezzo posto verso sinistra                               
 $z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
 for($i=177; $i<257;$i++){
 $Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts31tube#$i
 fts03assembly#1  
 $Xpos $Ypos $z2
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 #### tubi sotto la beam pipe
 for($i=257; $i<273;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) )-$diametrostraw/4;
 print OUT "//*********************
 fts31tubedown#$i
 fts03assembly#1
 $Xpos -$Yshort $z2
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 for($i=257; $i<273;$i++){
 $Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) )-$diametrostraw/4;
 print OUT "//*********************
 fts31tubeup#$i
 fts03assembly#1
 $Xpos $Yshort $z2
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }
 for($i=273; $i<353;$i++){
 $Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
 print OUT "//*********************                                                           
 fts31tube#$i
 fts03assembly#1  
 $Xpos $Ypos $z2 
 1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
 }

 ########second double layer: inclination of +5 deg#############
 $len = 703.4 ;
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
 $semilen = $len/2.;
 $raggio = $diametrostraw/2.;
 $Zpos=74.75- $diametrostraw/2;
 print OUT "//*********************
 fts32tube#1
 fts03assembly#1
 TUBE
 mylar
 0. 0. -$semilen 
 0. $raggio
 0. 0. $semilen  
 $Xpos $Ypos $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.
 ";
 #
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts32gas#1
 fts32tube#1
 TUBE
 STTmix9010_2bar
 0. 0. -$semilen  
 0. $raggio
 0. 0. $semilen  
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 # ancora all'interno un filo di tungsteno
 $raggio = $diametrofilo/2.;
 print OUT "//*********************
 fts32wire#1
 fts32gas#1
 TUBE
 tungsten
 0. 0. -$semilen 
 0. $raggio
 0. 0. $semilen  
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 for($i=2; $i<81;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 print OUT "//*********************
 fts32tube#$i
 fts03assembly#1
 $Xpos $Ypos $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 ############short tube#########for the beam pipe hole#########
 ###layer32
 #
 $hhole = 204; # h of the hole for the beam pipe
 $shortHalf = ($len - $hhole)/4; #(703.4-204)/4
 $Yshort = $shortHalf+($hhole/2);
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i + ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
 $raggio = $diametrostraw/2.;
 print OUT "//*********************
 fts32tubedown#$i
 fts03assembly#1
 TUBE
 mylar
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 $Xpos -$Yshort $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.
 ";
 #
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts32gasdown#$i
 fts32tubedown#$i
 TUBE
 STTmix9010_2bar
 0. 0. -$shortHalf 
 0. $raggio
 0. 0. $shortHalf 
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 # ancora all'interno un filo di tungsteno
 $raggio = $diametrofilo/2.;
 print OUT "//*********************
 fts32wiredown#$i
 fts32gasdown#$i
 TUBE
 tungsten
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 for($i=82; $i<97;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i +  ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4; #1.7
 print OUT "//*********************
 fts32tubedown#$i
 fts03assembly#1
 $Xpos -$Yshort $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 ############short tube#########for the beam pipe hole#########alti
 $i=81;
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i - ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
 $raggio = $diametrostraw/2.;
 print OUT "//*********************
 fts32tubeup#$i
 fts03assembly#1
 TUBE
 mylar
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 $Xpos $Yshort $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.
 ";
 #
 $raggio = $raggio - $spessoremylar;
 print OUT "//*********************
 fts32gasup#$i
 fts32tubeup#$i
 TUBE
 STTmix9010_2bar
 0. 0. -$shortHalf 
 0. $raggio
 0. 0. $shortHalf 
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 # ancora all'interno un filo di tungsteno
 $raggio = $diametrofilo/2.;
 print OUT "//*********************
 fts32wireup#$i
 fts32gasup#$i
 TUBE
 tungsten
 0. 0. -$shortHalf
 0. $raggio
 0. 0. $shortHalf
 0. 0. 0.
 1. 0. 0. 0. 1. 0. 0. 0. 1.
 ";
 for($i=82; $i<97;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i -  ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
 print OUT "//*********************
 fts32tubeup#$i
 fts03assembly#1
 $Xpos $Yshort $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=97; $i<177;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
 print OUT "//*********************
 fts32tube#$i
 fts03assembly#1
 $Xpos $Ypos $Zpos
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 #  ora metto le altre 192 copie della seconda file di straws,                  
 # sono anche staggerate di mezzo posto verso sinistra                               
 $z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
 for($i=177; $i<257;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
 print OUT "//*********************                                                  
 fts32tube#$i
 fts03assembly#1  
 $Xpos $Ypos $z2
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 #### tubi sotto la beam pipe
 for($i=257; $i<273;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) + ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
 print OUT "//*********************
 fts32tubedown#$i
 fts03assembly#1
 $Xpos -$Yshort $z2
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=257; $i<273;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) - ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
 print OUT "//*********************
 fts32tubeup#$i
 fts03assembly#1
 $Xpos $Yshort $z2
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
 for($i=273; $i<353;$i++){
 $Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
 print OUT "//*********************                            
 fts32tube#$i
 fts03assembly#1  
 $Xpos $Ypos $z2 
 $cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
 }
####second double layer completed########
######third double layers: inclination of -5 deg#####
##layer33
$len = 728.8 ;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;
$Zpos=220-$diametrostraw/2;
print OUT "//*********************
fts33tube#1
fts03assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts33gas#1
fts33tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts33wire#1
fts33gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<81;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts33tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########
#
$shortHalf = ($len - $hhole)/4; #(728.8-204)/4
$Yshort = $shortHalf+($hhole/2);
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i - ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts33tubedown#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts33gasdown#$i
fts33tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts33wiredown#$i
fts33gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=82; $i<97;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i - ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
print OUT "//*********************
fts33tubedown#$i
fts03assembly#1
$Xpos -$Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=81;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i + ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts33tubeup#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts33gasup#$i
fts33tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts33wireup#$i
fts33gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=82; $i<97;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i + ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4; #1.75
print OUT "//*********************
fts33tubeup#$i
fts03assembly#1
$Xpos $Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=97; $i<177;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts33tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#  ora metto le altre 192 copie della seconda file di straws,                  
####################################                            
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=177; $i<257;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts33tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=257; $i<273;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) - ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************
fts33tubedown#$i
fts03assembly#1
$Xpos -$Yshort $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=257; $i<273;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) + ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************
fts33tubeup#$i
fts03assembly#1
$Xpos $Yshort $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=273; $i<353;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts33tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#######third double layer completed##########
#######fourth double layers: inclination of 0 gradi######
$len = 741.9 ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;
$Zpos=294.75-$diametrostraw/2;
print OUT "//*********************
fts34tube#1
fts03assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts34gas#1
fts34tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts34wire#1
fts34gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<81;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts34tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########
$shortHalf = ($len - $hhole)/4; #(741.9-204)/4
$Yshort = $shortHalf+($hhole/2);
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
#
print OUT "//*********************
fts34tubedown#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts34gasdown#$i
fts34tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts34wiredown#$i
fts34gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=82; $i<97;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts34tubedown#$i
fts03assembly#1
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=81;
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts34tubeup#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts34gasup#$i
fts34tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts34wireup#$i
fts34gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=82; $i<97;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts34tubeup#$i
fts03assembly#1
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=97; $i<177;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts34tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=177; $i<257;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts34tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=257; $i<273;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts34tubedown#$i
fts03assembly#1
$Xpos -$Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=257; $i<273;$i++){
$Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts34tubeup#$i
fts03assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=273; $i<353;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts34tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

###########################################################
##################FTS4#####################################
###Camere dentro il magnete: ogni DL ha lunghezza diversa##
###########################################################
###########################################################
$nstraws=208;
#  lunghezza di una straw
$len = 767.3 ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$Ypos = 0.;
$Zpos = 440-$diametrostraw/2;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;

print OUT "//*********************
fts35tube#1
fts03assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts35gas#1
fts35tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts35wire#1
fts35gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<97;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts35tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########bassi
$shortHalf = ($len - $hhole)/4; #(767.3-204)/4
$Yshort = $shortHalf+($hhole/2);
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
#
print OUT "//*********************
fts35tubedown#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts35gasdown#$i
fts35tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts35wiredown#$i
fts35gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts35tubedown#$i
fts03assembly#1
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
$i=97;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts35tubeup#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts35gasup#$i
fts35tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts35wireup#$i
fts35gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts35tubeup#$i
fts03assembly#1
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=113; $i<209;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts35tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws,
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=209; $i<305;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts35tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=305; $i<321;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts35tubedown#$i
fts03assembly#1
$Xpos -$Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=305; $i<321;$i++){
$Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts35tubeup#$i
fts03assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=321; $i<417;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts35tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#######First double layer completed##############################
#######Second double layer: inclination of straw +5 deg#########
#layer36
$len = 780.4;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;
$Zpos=514.75 - $diametrostraw/2;
print OUT "//*********************
fts36tube#1
fts03assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts36gas#1
fts36tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts36wire#1
fts36gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<97;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts36tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########
$shortHalf = ($len - $hhole)/4; #(767.3-204)/4
$Yshort = $shortHalf+($hhole/2);
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i + ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4; 
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts36tubedown#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts36gasdown#$i
fts36tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts36wiredown#$i
fts36gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i + ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
print OUT "//*********************
fts36tubedown#$i
fts03assembly#1
$Xpos -$Yshort $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=97;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i - ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts36tubeup#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts36gasup#$i
fts36tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts36wireup#$i
fts36gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i - ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
print OUT "//*********************
fts36tubeup#$i
fts03assembly#1
$Xpos $Yshort $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=113; $i<209;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts36tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#  ora metto le altre 192 copie della seconda file di straws,
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=209; $i<305;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts36tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=305; $i<321;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) + ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************
fts36tubedown#$i
fts03assembly#1
$Xpos -$Yshort $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=305; $i<321;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) - ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************
fts36tubeup#$i
fts03assembly#1
$Xpos $Yshort $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=321; $i<417;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts36tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
####second double layer completed############################
#######third double layers: inclination of -5 deg######
$len = 805.8;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;
$Zpos=660-$diametrostraw/2;
print OUT "//*********************
fts37tube#1
fts03assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts37gas#1
fts37tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts37wire#1
fts37gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<97;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts37tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########
$shortHalf = ($len - $hhole)/4; #(805.8-204)/4
$Yshort = $shortHalf+($hhole/2);
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i - ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts37tubedown#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts37gasdown#$i
fts37tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts37wiredown#$i
fts37gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i - ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
print OUT "//*********************
fts37tubedown#$i
fts03assembly#1
$Xpos -$Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=97;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i + ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts37tubeup#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts37gasup#$i
fts37tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts37wireup#$i
fts37gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i + ($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4;
print OUT "//*********************
fts37tubeup#$i
fts03assembly#1
$Xpos $Yshort $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=113; $i<209;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts37tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#  ora metto le altre 192 copie della seconda file di straws,                  
####################################                            
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=209; $i<305;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts37tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=305; $i<321;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) - ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************
fts37tubedown#$i
fts03assembly#1
$Xpos -$Yshort $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=305; $i<321;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) + ($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************
fts37tubeup#$i
fts03assembly#1
$Xpos $Yshort $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=321; $i<417;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts37tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#######terzo double layer completo##########
#######quarto double layers: inclinazione straw 0 gradi######
$len = 818.9;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;
$Zpos=734.75-$diametrostraw/2;
print OUT "//*********************
fts38tube#1
fts03assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts38gas#1
fts38tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts38wire#1
fts38gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<97;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts38tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########
$shortHalf = ($len - $hhole)/4; #(818.9-204)/4
$Yshort = $shortHalf+($hhole/2);
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts38tubedown#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts38gasdown#$i
fts38tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts38wiredown#$i
fts38gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts38tubedown#$i
fts03assembly#1
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=97;
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts38tubeup#$i
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts38gasup#$i
fts38tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts38wireup#$i
fts38gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=98; $i<113;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts38tubeup#$i
fts03assembly#1
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=113; $i<209;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts38tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=209; $i<305;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts38tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=305; $i<321;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts38tubedown#$i
fts03assembly#1
$Xpos -$Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=305; $i<321;$i++){
$Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts38tubeup#$i
fts03assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=321; $i<417;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts38tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

######################################################################
##########################FTS5########################################
######################################################################
######################################################################
$nstraws=388;
#  lunghezza di una straw
$len = 1200. ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$Ypos = 0.;
$Zpos = 6075-$diametrostraw/2;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;

print OUT "//*********************
fts05assembly#1
cave
ASSEMBLY
air
0.  0.  $Zpos
1.   0.   0.   0.   1.   0.   0.   0.   1.
";
$Zpos=0.;
$Ypos=0.;
#######################
print OUT "//*********************
fts05tube#1
fts05assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts05gas#1
fts05tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts05wire#1
fts05gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=2; $i<177;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

############short tube#########for the beam pipe hole#########bassi
$hhole = 258; # h of the hole for the beam pipe
$shortHalf = ($len - $hhole)/4; #(1200-258)/4=235.5
$Yshort = $shortHalf+($hhole/2);
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts05tubedown#$i
fts05assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts05gasdown#$i
fts05tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts05wiredown#$i
fts05gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
#### tubi sotto la beam pipe
for($i=178; $i<201;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

$i=177;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts05tubeup#$i
fts05assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts05gasup#$i
fts05tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts05wireup#$i
fts05gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";

####tubi sopra la beam pipe
for($i=178; $i<201;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

for($i=201; $i<389;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
# #############primo layer completto##################
#  ora metto le altre 128 copie della seconda file di straws.
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=389; $i<565;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=565; $i<589;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=565; $i<589;$i++){
$Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=589; $i<777;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########first double layers completed#########
########second double layers: inclination of +5 deg######### 
$Zpos = 50-$diametrostraw/2;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;

for($i=777; $i<953;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=953; $i<977;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4; #2.75
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $Zpos 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=953; $i<977;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $Zpos 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=977; $i<1165;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#####secondo strato########
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=1165; $i<1341;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1341; $i<1365;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $z2 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1341; $i<1365;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1365; $i<1553;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#########second double layers completed#########
########third double layers: inclination of -5 deg#########
$Zpos = 100-$diametrostraw/2;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;

for($i=1553; $i<1729;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5  - ($i - (4*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1729; $i<1753;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $Zpos 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1729; $i<1753;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $Zpos 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1753; $i<1941;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=1941; $i<2117;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=2117; $i<2141;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $z2 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=2117; $i<2141;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=2141; $i<2329;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#########third double layers completed#########
########fouth double layers: inclination of 0 deg#########
$Zpos = 150-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
for($i=2329; $i<2505;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2505; $i<2529;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2505; $i<2529;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2529; $i<2717;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=2717; $i<2893;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2893; $i<2917;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -$Yshort $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2893; $i<2917;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2917; $i<3105;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}


######################################################################
##########################FTS6########################################
######################################################################
######################################################################
$nstraws=388;
#  lunghezza di una straw
$len = 1200. ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$Ypos = 0.;
$Zpos = 6395-$diametrostraw/2;
$semilen = $len/2.;
$raggio = $diametrostraw/2.;

print OUT "//*********************
fts06assembly#1
cave
ASSEMBLY
air
0.  0.  $Zpos
1.   0.   0.   0.   1.   0.   0.   0.   1.
";
$Zpos=0.;
$Ypos=0.;
#######################
print OUT "//*********************
fts06tube#1
fts06assembly#1
TUBE
mylar
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts06gas#1
fts06tube#1
TUBE
STTmix9010_2bar
0. 0. -$semilen  
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts06wire#1
fts06gas#1
TUBE
tungsten
0. 0. -$semilen 
0. $raggio
0. 0. $semilen  
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";

for($i=2; $i<177;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

############short tube#########for the beam pipe hole#########bassi
$hhole = 258; # h of the hole for the beam pipe
$shortHalf = ($len - $hhole)/4; #(1200-258)/4=235.5
$Yshort = $shortHalf+($hhole/2);
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
#
print OUT "//*********************
fts06tubedown#$i
fts06assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts06gasdown#$i
fts06tubedown#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts06wiredown#$i
fts06gasdown#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
#### tubi sotto la beam pipe
for($i=178; $i<201;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

$i=177;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts06tubeup#$i
fts06assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

#
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts06gasup#$i
fts06tubeup#$i
TUBE
STTmix9010_2bar
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts06wireup#$i
fts06gasup#$i
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
####tubi sopra la beam pipe
for($i=178; $i<201;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=201; $i<389;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i)-$diametrostraw/4;
print OUT "//*********************
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws.
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=389; $i<565;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=565; $i<589;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=565; $i<589;$i++){
$Xpos = $diametrostraw*( $nstraws/2 -  ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=589; $i<777;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - ($nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########first double layers completed#########
########second double layers: inclination of +5 deg######### 
$Zpos = 50-$diametrostraw/2;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;

for($i=777; $i<953;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=953; $i<977;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw)-$diametrostraw/4; #3.35
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $Zpos 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=953; $i<977;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $Zpos 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=977; $i<1165;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (2*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#####secondo strato########
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=1165; $i<1341;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1341; $i<1365;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $z2 
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1341; $i<1365;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1365; $i<1553;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (3*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
$cosangle 0. -$sinangle $sinangle 0. $cosangle 0. 1. 0.\n"
}
#########second double layers completed#########
########third double layers: inclination of -5 deg#########
$Zpos = 100-$diametrostraw/2;
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;

for($i=1553; $i<1729;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5  - ($i - (4*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1729; $i<1753;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $Zpos 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1729; $i<1753;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $Zpos 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=1753; $i<1941;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 + 0.5 - ($i - (4*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=1941; $i<2117;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=2117; $i<2141;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1))-($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $z2 
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=2117; $i<2141;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1))+($Yshort*$sinangle)/$diametrostraw )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
for($i=2141; $i<2329;$i++){
$Xpos = (1/$cosangle)*$diametrostraw*( $nstraws/2 - ($i - (5*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
$cosangle 0. $sinangle -$sinangle 0. $cosangle 0. 1. 0.\n"
}
#########third double layers completed#########
########fourth double layers:inclination of 0 deg#########
$Zpos = 150-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5)-$diametrostraw/4;
$raggio = $diametrostraw/2.;
for($i=2329; $i<2505;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2505; $i<2529;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5  - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2505; $i<2529;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2529; $i<2717;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - (6*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
##########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=2717; $i<2893;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2893; $i<2917;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -$Yshort $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2893; $i<2917;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos $Yshort $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2917; $i<3105;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - (7*$nstraws+1)) )-$diametrostraw/4;
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
