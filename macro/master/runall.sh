root -l -b -q sim_complete.C\($1\)
root -l -b -q digi_complete.C
root -l -b -q reco_complete.C
root -l -b -q pid_complete.C
mv ../data/evtcomplete_pid.root ../data/evtcomplete_pid_chain.root
root -l -b -q full_complete.C
root -l -b -q ana_complete.C
