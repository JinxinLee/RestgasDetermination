  FTF generator is taken from GEANT4 toolkit.

 FTF generator is aimed for generation of Pbar-P and Pbar-nucleus events.
 To compile FTF, it is needed to give command:

 >  make -f FTFmake  

 After this, you will obtain "FTFGen" exe-file.
 FTFGen can generate Pbar-P events or Pbar-A events.

-----------------------------------------------------------------
 To generate Pbar-P events, you need
 to insert two values in the file "PbarP.mac" :

 after line 

#events  
 Number of Events 

 after line 

#Plab(GeV/c)
Momentum of projectile antiproton

Next command must be:

 > ./FTFGen PbarP.mac 
 
After run, you will obtain "FTF.root" file with all information.

----------------------------------------------------------------------

 To generate Pbar-A events, you need to insert in the file "PbarA.mac"
 some values.  First of all,
 Geant4 name of material- G4_Name, for example, 
 for carbon - G4_C, for gold - G4_Au, for cupper -G4_Cu and so on,
  
after line

#material
G4_Name

after line  

#targetA
mass number of target nucleus

after line 

#generator
ftfp  or ftfb
 
after line 

#events
 Number of Events

 after line

#Plab(GeV/c)
Momentum of projectile antiproton 

------------------------------------------------------------
Most of the variables are understandable. However, "#targetA" is atomic 
mass given in the periodic table (it must be integer). #generator can be 
"ftfp" or "ftfb". "ftfp" generator is couple of FTF model and  simple 
Preco model of Geant4. "ftfb" generator is couple of FTF model and Binary 
cascade model of Geant4. "ftfp" works faster than "ftfb", but "ftfb" gives 
more exact predictions for evaporated nucleon production.   
---------------------------------------------------------------

Next command must be:

 > ./FTFGen PbarA.mac

After run, you will obtain "FTF.root" file with all information in the 
ROOT-Tree, similar Root-file after standalone run of DPMGen.

You can operate with file "FTF.root" as with "Background-micro.root"
obtained after DPMGen run.
