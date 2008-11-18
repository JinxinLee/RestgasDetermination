
# run only once
#root -b -q create_gem_rootgeo.C
#root -b -q Helper.C+

root -b -q runGemSim.C"($1)"

# run one of these two
root -l anaGemPointrate.C"($1)"
#root -l eventdisplay.C

