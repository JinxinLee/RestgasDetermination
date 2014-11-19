=========== README ===================



1)

a)GEOMETRY

first: you need the geo file which can be found at
https://subversion.gsi.de/trac/fairroot/browser/pandaroot/trunk/hypGe/hypGeTools

please copy the root files to the geometry directory

you should add some materials to the media_pnd.geo file:

siliconinactive    1  28.0855 14.0 2.33
                   0  1  20.  .001
                   0

Al97Mg3            2  26.98 24.305 13. 12. 2.67 0.97 0.03
                   1  1  20.  .001
                   0

b) at trunk/CmakeList.txt
add this line before the 'add_subdirectory (hyp)'

add_subdirectory (hyp/razhyp)


1) run run_test.C

a) box generator by default
b) reading GiBUU file for background (use of UrqmdSmm)
1.b) change the DecFile parameter in the run_test function to "giBUU"

> root -l -b -q 'run_test.C()'

2) ideal reco : smearing of the position with some resolution

root -l hit_hypmvd.C 
