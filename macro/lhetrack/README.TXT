1.) root -b run_sim_tpcmvd.C

-> runs Geant for tpc and mvd
-> write out "points_tpcmvd.root"

2.) root -b run_track_tpcmvd.C

-> run the track finder
-> runs the track fitter
-> writes out "tracks_tpcmvd.root"

3.) root plot_pT.C
