=========== README ===================

Running the simulation for the Secondary active target(hypernuclei)
in the present macros, the HPGe array is for the moment missing

1)

a)GEOMETRY

first: you need the geo file which can be found at
https://subversion.gsi.de/trac/fairroot/browser/pandaroot/trunk/macro/hyp/Sebastian/TargetSystem_WindowAbsB_insideAbsB.root

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

