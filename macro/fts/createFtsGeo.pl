# tutte le dimensioni devono essere in mm.
#  assumo qui che la straw sia fatta di mylar con all'interno
#  argon ed un filo di tungsteno
###################    dati costruttivi
# numero di straws nel primo strato da 128 straws
$nstraws=128;

# posizione in Z del CENTRO del primo strato (128 straws)
#$Zpos = 2954.;

#  lunghezza di una straw
$len = 640. ;

# diametro esterno di una straw, assumo sia 10. mm
$diametrostraw = 10.1;  
$Zpos = 2954.-$diametrostraw/2;
# spessore del mylar, assumo sia 0.03 mm
$spessoremylar = 0.03;

# diametro filo straw, assumo sia 0.05 mm
$diametrofilo= 0.02;

################################################################

# posizione X,Y del CENTRO della straw piu' a DESTRA guardando
# verso downstream
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$Ypos = 0.;

# costruzione e posizionamento della straw piu' a DESTRA guardando
# verso downstream;  le altre straws poi saranno delle sue copie.

$semilen = $len/2.;
$raggio = $diametrostraw/2.;

#  cilindro di mylar
open OUT,">fts.geo";
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

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts01gas#1
fts01tube#1
TUBE
argon
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

for($i=2; $i<59;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - $i);

print OUT "//*********************
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

############short tube#########for the beam pipe hole#########bassi
$Xpos = $diametrostraw*( 128/2 + 0.5 - $i);
$shortHalf = 117; #(640-172)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts01tubedown#1
fts01assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -203. $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts01gasdown#1
fts01tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts01wiredown#1
fts01gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
#### tubi sotto la beam pipe
for($i=60; $i<71;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - $i);
print OUT "//*********************
fts01tubedown#$i
fts01assembly#1
$Xpos -203. $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

$i=59;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( 128/2 + 0.5 - $i);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts01tubeup#1
fts01assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 203. $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts01gasup#1
fts01tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts01wireup#1
fts01gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";

####tubi sopra la beam pipe
for($i=60; $i<71;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - $i);
print OUT "//*********************
fts01tubeup#$i
fts01assembly#1
$Xpos 203. $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

for($i=71; $i<129;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - $i);
print OUT "//*********************
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#############primo layer completto##################


#  ora metto le altre 128 copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=129; $i<187;$i++){
$Xpos = $diametrostraw*( 128/2  - ($i - 129) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=187; $i<199;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 129) );
print OUT "//*********************
fts01tubedown#$i
fts01assembly#1
$Xpos -203. $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=187; $i<199;$i++){
$Xpos = $diametrostraw*( 128/2 -  ($i - 129) );
print OUT "//*********************
fts01tubeup#$i
fts01assembly#1
$Xpos 203. $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=199; $i<257;$i++){
$Xpos = $diametrostraw*( 128/2  - ($i - 129) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########primo double layers completato#########
########secondo double layers: inclinazione dei tubi di +5 gradi######### 
#$Zpos = 3004-$diametrostraw/2;
$Zpos = 50-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=257; $i<315;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 257) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=315; $i<327;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 257)+1.6 );
print OUT "//*********************                                                           
fts01tubedown#$i
fts01assembly#1
$Xpos -203 $Zpos 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=315; $i<327;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 257)-1.6 );
print OUT "//*********************                                                           
fts01tubeup#$i
fts01assembly#1
$Xpos 203 $Zpos 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=327; $i<385;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 257) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#####secondo strato########
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=385; $i<443;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 385) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=443; $i<455;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 385)+1.6 );
print OUT "//*********************                                                           
fts01tubedown#$i
fts01assembly#1
$Xpos -203 $z2 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=443; $i<455;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 385)-1.6 );
print OUT "//*********************                                                           
fts01tubeup#$i
fts01assembly#1
$Xpos 203 $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=455; $i<513;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 385) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#########secondo double layers completato#########
########terzo double layers: inclinazione dei tubi di -5 gradi#########
#$Zpos = 3054-$diametrostraw/2;
$Zpos = 100-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=513; $i<571;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5  - ($i - 513) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=571; $i<583;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 513)-1.6 );
print OUT "//*********************                                                           
fts01tubedown#$i
fts01assembly#1
$Xpos -203 $Zpos 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=571; $i<583;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 513)+1.6 );
print OUT "//*********************                                                           
fts01tubeup#$i
fts01assembly#1
$Xpos 203 $Zpos 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=583; $i<641;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 513) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=641; $i<699;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 641) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=699; $i<711;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 641)-1.6 );
print OUT "//*********************                                                           
fts01tubedown#$i
fts01assembly#1
$Xpos -203 $z2 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=699; $i<711;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 641)+1.6 );
print OUT "//*********************                                                           
fts01tubeup#$i
fts01assembly#1
$Xpos 203 $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=711; $i<769;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 641) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#########terzo double layers completato#########
########quarto double layers: inclinazione dei tubi di 0 gradi#########
#$Zpos = 3104-$diametrostraw/2;
$Zpos = 150-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=769; $i<827;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 769) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=827; $i<839;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 769) );
print OUT "//*********************                                                           
fts01tubedown#$i
fts01assembly#1
$Xpos -203 $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=827; $i<839;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 769) );
print OUT "//*********************                                                           
fts01tubeup#$i
fts01assembly#1
$Xpos 203 $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=839; $i<897;$i++){
$Xpos = $diametrostraw*( 128/2 + 0.5 - ($i - 769) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=897; $i<955;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 897) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=955; $i<967;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 897) );
print OUT "//*********************                                                           
fts01tubedown#$i
fts01assembly#1
$Xpos -203 $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=955; $i<967;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 897) );
print OUT "//*********************                                                           
fts01tubeup#$i
fts01assembly#1
$Xpos 203 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=967; $i<1025;$i++){
$Xpos = $diametrostraw*( 128/2 - ($i - 897) );
print OUT "//*********************                                                           
fts01tube#$i
fts01assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

##########################################################
#################FTS2#####################################
##########################################################
$Zpos = 3274.-$diametrostraw/2;
print OUT "//*********************
fts01assembly#2
cave
ASSEMBLY
air
0.  0.   $Zpos
1.   0.   0.   0.   1.   0.   0.   0.   1.
";

##########################################################
#################FTS3#####################################
##Camere dentro il magnete: ogni DL ha lunghezza diversa##
##########################################################
##########################################################
$nstraws=192;
#  lunghezza di una straw
$len = 690.3 ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts31gas#1
fts31tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts31tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########bassi
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$shortHalf = 131.075; #(690.3-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts31tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -214.075 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts31gasdown#1
fts31tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts31wiredown#1
fts31gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts31tubedown#$i
fts03assembly#1
$Xpos -214.075 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
$i=91;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts31tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 214.075 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts31gasup#1
fts31tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts31wireup#1
fts31gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts31tubeup#$i
fts03assembly#1
$Xpos 214.075 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts31tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts31tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) );
print OUT "//*********************
fts31tubedown#$i
fts03assembly#1
$Xpos -214.075 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) );
print OUT "//*********************
fts31tubeup#$i
fts03assembly#1
$Xpos 214.075 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts31tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
########secondo double layer: inclinazione +5 gradi#############
$len = 703.4 ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts32gas#1
fts32tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts32tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########bassi
############short tube#########for the beam pipe hole#########
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.7);
$shortHalf = 134.35; #(703.4-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts32tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -217.35 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts32gasdown#1
fts32tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts32wiredown#1
fts32gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.7);
print OUT "//*********************
fts32tubedown#$i
fts03assembly#1
$Xpos -217.35 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=91;
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.7);
$shortHalf = 134.35; #(703.4-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts32tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 217.35 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts32gasup#1
fts32tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts32wireup#1
fts32gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.7);
print OUT "//*********************
fts32tubeup#$i
fts03assembly#1
$Xpos 217.35 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts32tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#  ora metto le altre 192 copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts32tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) + 1.7 );
print OUT "//*********************
fts32tubedown#$i
fts03assembly#1
$Xpos -217.35 $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) - 1.7 );
print OUT "//*********************
fts32tubeup#$i
fts03assembly#1
$Xpos 217.35 $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts32tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
####secondo double layer completo########
######terzo double layers: inclinazione -5 gradi#####
$len = 728.8 ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts33gas#1
fts33tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts33tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########bassi
############short tube#########for the beam pipe hole#########
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.75);
$shortHalf = 140.7; #(728.8-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts33tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -223.7 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts33gasdown#1
fts33tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts33wiredown#1
fts33gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.75);
print OUT "//*********************
fts33tubedown#$i
fts03assembly#1
$Xpos -223.7 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=91;
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.75);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts33tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 223.7 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts33gasup#1
fts33tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts33wireup#1
fts33gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.75);
print OUT "//*********************
fts33tubeup#$i
fts03assembly#1
$Xpos 223.7 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts33tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#  ora metto le altre 192 copie della seconda file di straws,                  
####################################                            
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts33tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) - 1.75 );
print OUT "//*********************
fts33tubedown#$i
fts03assembly#1
$Xpos -223.7 $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) + 1.75 );
print OUT "//*********************
fts33tubeup#$i
fts03assembly#1
$Xpos 223.7 $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts33tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#######terzo double layer completo##########
#######quarto double layers: inclinazione straw 0 gradi######
$len = 741.9 ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$semilen = $len/2.;
$raggio = $diametrostraw/2.;
$Zpos=294-$diametrostraw/2;
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
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts34gas#1
fts34tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts34tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$shortHalf = 143.975; #(741.9-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts34tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -227.25 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts34gasdown#1
fts34tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts34wiredown#1
fts34gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts34tubedown#$i
fts03assembly#1
$Xpos -227.25 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=91;
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts34tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 227.25 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts34gasup#1
fts34tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts34wireup#1
fts34gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts34tubeup#$i
fts03assembly#1
$Xpos 227.25 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts34tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts34tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) );
print OUT "//*********************
fts34tubedown#$i
fts03assembly#1
$Xpos -227.25 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) );
print OUT "//*********************
fts34tubeup#$i
fts03assembly#1
$Xpos 227.25 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts34tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

##########################################################
#################FTS4#####################################
##Camere dentro il magnete: ogni DL ha lunghezza diversa##
##########################################################
##########################################################
$nstraws=192;
#  lunghezza di una straw
$len = 767.3 ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts35gas#1
fts35tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts35tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########bassi
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$shortHalf = 150.325; #(767.3-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts35tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -233.325 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts35gasdown#1
fts35tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts35wiredown#1
fts35gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts35tubedown#$i
fts03assembly#1
$Xpos -233.325 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
$i=91;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts35tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 233.325 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts35gasup#1
fts35tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts35wireup#1
fts35gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts35tubeup#$i
fts03assembly#1
$Xpos 233.325 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts35tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws,
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts35tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) );
print OUT "//*********************
fts35tubedown#$i
fts03assembly#1
$Xpos -233.325 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) );
print OUT "//*********************
fts35tubeup#$i
fts03assembly#1
$Xpos 233.325 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts35tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#######Piromo double layer completo##############################
#######Secondo double layer: inclinazione straw +5 gradi#########
$len = 780.4;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts36gas#1
fts36tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts36tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########bassi
############short tube#########for the beam pipe hole#########
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.85);
$shortHalf = 153.6; #(780.4-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts36tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -236.6 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts36gasdown#1
fts36tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts36wiredown#1
fts36gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.85);
print OUT "//*********************
fts36tubedown#$i
fts03assembly#1
$Xpos -236.6 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=91;
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.85);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts36tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 236.6 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts36gasup#1
fts36tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts36wireup#1
fts36gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.85);
print OUT "//*********************
fts36tubeup#$i
fts03assembly#1
$Xpos 236.6 $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts36tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#  ora metto le altre 192 copie della seconda file di straws,
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts36tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) + 1.85 );
print OUT "//*********************
fts36tubedown#$i
fts03assembly#1
$Xpos -236.6 $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) - 1.85 );
print OUT "//*********************
fts36tubeup#$i
fts03assembly#1
$Xpos 236.6 $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts36tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
####secondo double layer completo############################
#######terzo double layers: inclinazione straw -5 gradi######
$len = 805.8;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts37gas#1
fts37tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts37tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########bassi
############short tube#########for the beam pipe hole#########
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.9);
$shortHalf = 159.95; #(805.8-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts37tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -242.95 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts37gasdown#1
fts37tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts37wiredown#1
fts37gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i - 1.9);
print OUT "//*********************
fts37tubedown#$i
fts03assembly#1
$Xpos -242.95 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=91;
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.9);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts37tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 242.95 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts37gasup#1
fts37tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts37wireup#1
fts37gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i + 1.9);
print OUT "//*********************
fts37tubeup#$i
fts03assembly#1
$Xpos 242.95 $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts37tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#  ora metto le altre 192 copie della seconda file di straws,                  
####################################                            
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts37tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) - 1.9 );
print OUT "//*********************
fts37tubedown#$i
fts03assembly#1
$Xpos -242.95 $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) + 1.9 );
print OUT "//*********************
fts37tubeup#$i
fts03assembly#1
$Xpos 242.95 $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts37tube#$i
fts03assembly#1  
$Xpos $Ypos $z2 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#######terzo double layer completo##########
#######quarto double layers: inclinazione straw 0 gradi######
$len = 818.9;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts38gas#1
fts38tube#1
TUBE
argon
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
for($i=2; $i<91;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts38tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$shortHalf = 163.225; #(818.9-166)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts38tubedown#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -246.225 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts38gasdown#1
fts38tubedown#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts38wiredown#1
fts38gasdown#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts38tubedown#$i
fts03assembly#1
$Xpos -246.225 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
############short tube#########for the beam pipe hole#########alti
$i=91;
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts38tubeup#1
fts03assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 246.225 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";
# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts38gasup#1
fts38tubeup#1
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts38wireup#1
fts38gasup#1
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
for($i=92; $i<103;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts38tubeup#$i
fts03assembly#1
$Xpos 246.225 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=103; $i<193;$i++){
$Xpos = $diametrostraw*( 192/2 + 0.5 - $i);
print OUT "//*********************
fts38tube#$i
fts03assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws,                  
# sono anche staggerate di mezzo posto verso sinistra                               
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=193; $i<283;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
print OUT "//*********************                                                           
fts38tube#$i
fts03assembly#1  
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 - ($i - 193) );
print OUT "//*********************
fts38tubedown#$i
fts03assembly#1
$Xpos -246.225 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=283; $i<295;$i++){
$Xpos = $diametrostraw*( 192/2 -  ($i - 193) );
print OUT "//*********************
fts38tubeup#$i
fts03assembly#1
$Xpos 246.225 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=295; $i<385;$i++){
$Xpos = $diametrostraw*( 192/2  - ($i - 193) );
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
$nstraws=400;
#  lunghezza di una straw
$len = 1180. ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
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

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts05gas#1
fts05tube#1
TUBE
argon
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

for($i=2; $i<192;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - $i);

print OUT "//*********************
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

############short tube#########for the beam pipe hole#########bassi
$Xpos = $diametrostraw*( 400/2 + 0.5 - $i);
$shortHalf = 235.5; #(1180-172)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts05tubedown#192
fts05assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -354.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts05gasdown#192
fts05tubedown#192
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts05wiredown#192
fts05gasdown#192
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
#### tubi sotto la beam pipe
for($i=193; $i<210;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - $i);
print OUT "//*********************
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

$i=192;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( 400/2 + 0.5 - $i);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts05tubeup#192
fts05assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 354.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts05gasup#192
fts05tubeup#192
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts05wireup#192
fts05gasup#192
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";

####tubi sopra la beam pipe
for($i=193; $i<210;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - $i);
print OUT "//*********************
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

for($i=210; $i<401;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - $i);
print OUT "//*********************
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#############primo layer completto##################
#  ora metto le altre 128 copie della seconda file di straws.
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=401; $i<592;$i++){
$Xpos = $diametrostraw*( 400/2  - ($i - 401) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=592; $i<610;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 401) );
print OUT "//*********************
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=592; $i<610;$i++){
$Xpos = $diametrostraw*( 400/2 -  ($i - 401) );
print OUT "//*********************
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=610; $i<801;$i++){
$Xpos = $diametrostraw*( 400/2  - ($i - 401) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########primo double layers completato#########
########secondo double layers: inclinazione dei tubi di +5 gradi######### 
$Zpos = 50-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=801; $i<992;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 801) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=992; $i<1010;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 801)+2.75);
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $Zpos 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=992; $i<1010;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 801)-2.75 );
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $Zpos 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1010; $i<1201;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 801) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#####secondo strato########
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;

for($i=1201; $i<1392;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 1201) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1392; $i<1410;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 1201)+2.75 );
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $z2 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1392; $i<1410;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 1201)-2.75 );
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1410; $i<1601;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 1201) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#########secondo double layers completato#########
########terzo double layers: inclinazione dei tubi di -5 gradi#########
$Zpos = 100-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=1601; $i<1792;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5  - ($i - 1601) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1792; $i<1810;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 1601)-2.75 );
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $Zpos 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1792; $i<1810;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 1601)+2.75 );
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $Zpos 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1810; $i<2001;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 1601) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#########secondo strato###############
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=2001; $i<2192;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2001) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2192; $i<2210;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2001)-2.75 );
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $z2 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2192; $i<2210;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2001)+2.75 );
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2210; $i<2401;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2001) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#########terzo double layers completato#########
########quarto double layers: inclinazione dei tubi di 0 gradi#########
$Zpos = 150-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;
for($i=2401; $i<2592;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 2401) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2592; $i<2610;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 2401) );
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2591; $i<2610;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 2401) );
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2610; $i<2801;$i++){
$Xpos = $diametrostraw*( 400/2 + 0.5 - ($i - 2401) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########secondo strato###############
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=2801; $i<2992;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2801) );
print OUT "//*********************                                                           
fts05tube#$i
fts05assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2992; $i<3010;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2801) );
print OUT "//*********************                                                           
fts05tubedown#$i
fts05assembly#1
$Xpos -354.5 $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=2992; $i<3010;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2801) );
print OUT "//*********************                                                           
fts05tubeup#$i
fts05assembly#1
$Xpos 354.5 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=3010; $i<3201;$i++){
$Xpos = $diametrostraw*( 400/2 - ($i - 2801) );
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
$nstraws=592;
#  lunghezza di una straw
$len = 1480. ;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$Ypos = 0.;
$Zpos = 7475-$diametrostraw/2;
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

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts06gas#1
fts06tube#1
TUBE
argon
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

for($i=2; $i<288;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i);

print OUT "//*********************
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

############short tube#########for the beam pipe hole#########bassi
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i);
$shortHalf = 310.5; #(1480-238)/4
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts06tubedown#288
fts06assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos -429.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts06gasdown#288
fts06tubedown#288
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
# ancora all'interno un filo di tungsteno
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts06wiredown#288
fts06gasdown#288
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
#### tubi sotto la beam pipe
for($i=289; $i<306;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i);
print OUT "//*********************
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

$i=288;
####tubi sopra la beam pipe
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i);
$raggio = $diametrostraw/2.;
print OUT "//*********************
fts06tubeup#288
fts06assembly#1
TUBE
mylar
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
$Xpos 429.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.
";

# all'interno un cilindro di argon
$raggio = $raggio - $spessoremylar;
print OUT "//*********************
fts06gasup#288
fts06tubeup#288
TUBE
argon
0. 0. -$shortHalf 
0. $raggio
0. 0. $shortHalf 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
$raggio = $diametrofilo/2.;
print OUT "//*********************
fts06wireup#288
fts06gasup#288
TUBE
tungsten
0. 0. -$shortHalf
0. $raggio
0. 0. $shortHalf
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.
";
####tubi sopra la beam pipe
for($i=289; $i<306;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i);
print OUT "//*********************
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}

for($i=306; $i<593;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - $i);
print OUT "//*********************
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#  ora metto le altre 128 copie della seconda file di straws.
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
for($i=593; $i<880;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - 593) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#### tubi sotto la beam pipe
for($i=880; $i<898;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 593) );
print OUT "//*********************
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=880; $i<898;$i++){
$Xpos = $diametrostraw*( $nstraws/2 -  ($i - 593) );
print OUT "//*********************
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=898; $i<1185;$i++){
$Xpos = $diametrostraw*( $nstraws/2  - ($i - 593) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########primo double layers completato#########
########secondo double layers: inclinazione dei tubi di +5 gradi######### 
$Zpos = 50-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=1185; $i<1472;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 1185) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1472; $i<1490;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 1185)+3.35);
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $Zpos 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1472; $i<1490;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 1185)-3.35 );
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $Zpos 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=1490; $i<1777;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 1185) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#####secondo strato########
#####secondo strato########
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=1777; $i<2064;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 1777) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2064; $i<2082;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 1777)+3.35 );
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $z2 
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2064; $i<2082;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 1777)-3.35 );
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2082; $i<2369;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 1777) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
0.99692 0. -0.07846 0.07846 0. 0.99692 0. 1. 0.\n"
}
#########secondo double layers completato#########
########terzo double layers: inclinazione dei tubi di -5 gradi#########
$Zpos = 100-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;

for($i=2369; $i<2656;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5  - ($i - 2369) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2656; $i<2674;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 2369)-3.35 );
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $Zpos 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2656; $i<2674;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 2369)+3.35 );
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $Zpos 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=2674; $i<2961;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 2369) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=2961; $i<3248;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 2961) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=3248; $i<3266;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 2961)-3.35 );
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $z2 
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=3248; $i<3266;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 2961)+3.35 );
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
for($i=3266; $i<3553;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 2961) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
0.99692 0. 0.07846 -0.07846 0. 0.99692 0. 1. 0.\n"
}
#########terzo double layers completato#########
########quarto double layers: inclinazione dei tubi di 0 gradi#########
$Zpos = 150-$diametrostraw/2;
$Xpos = $diametrostraw*( $nstraws/2 -0.5);
$raggio = $diametrostraw/2.;
for($i=3553; $i<3840;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 3553) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=3840; $i<3858;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 3553) );
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=3840; $i<3858;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 3553) );
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $Zpos 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=3858; $i<4145;$i++){
$Xpos = $diametrostraw*( $nstraws/2 + 0.5 - ($i - 3553) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $Zpos
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
#########secondo strato###############
$z2 = $Zpos + 0.5*sqrt(3.)*$diametrostraw;
$raggio = $diametrostraw/2.;
for($i=4145; $i<4432;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 4145) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=4432; $i<4450;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 4145) );
print OUT "//*********************                                                           
fts06tubedown#$i
fts06assembly#1
$Xpos -429.5 $z2 
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=4432; $i<4450;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 4145) );
print OUT "//*********************                                                           
fts06tubeup#$i
fts06assembly#1
$Xpos 429.5 $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
for($i=4450; $i<4737;$i++){
$Xpos = $diametrostraw*( $nstraws/2 - ($i - 4145) );
print OUT "//*********************                                                           
fts06tube#$i
fts06assembly#1
$Xpos $Ypos $z2
1. 0. 0. 0. 0. 1. 0. 1. 0.\n"
}
