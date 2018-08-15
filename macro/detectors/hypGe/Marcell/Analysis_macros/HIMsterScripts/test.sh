#for number in 106 120 172
#do
#root -l -q -b ../GammaSpectraAnalysis_NoH_Task.C\(\"Sim_Geo36_E1.000MeV_Evts10000000_FileEvts20000_Gen1_ST0__$number.root\"\,\"Sim_Geo36_E1.000MeV_Evts10000000_FileEvts20000_Gen1_ST0\"\) #&> /data/work/kpha1/steinen/Gamma/Ana/analogs/ana_Sim_Geo36_E1.000MeV_Evts10000000_FileEvts20000_Gen1_ST0__$number.root.log
#echo $number done
#done

#for number in 5 18 19 24 37 38 39 46 49 58 60 61 66 76 77 79 80 86 88 91 93 166 169 190 197 207 215 217 226 229 250 251 253 254 265 268 282 290 294 296 297 301 319 321 327 331 334 338 342 347 349 354 356 360 368 379 389 394 396 409 410 442 444 453 464 469 473 475 485 489 494 498
#do
#root -l -q -b ../GammaSpectraAnalysis_NoH_Task.C\(\"Sim_Geo36_E0.500MeV_Evts10000000_FileEvts20000_Gen1_ST3__$number.root\"\,\"Sim_Geo36_E0.500MeV_Evts10000000_FileEvts20000_Gen1_ST3\"\) 
#echo $number done
#done

root -l -q -b ../GammaSpectraAnalysis_NoH_Task.C\(\"Sim_Geo36_E0.500MeV_Evts10000000_FileEvts20000_Gen1_ST3__125.root\"\,\"Sim_Geo36_E0.500MeV_Evts10000000_FileEvts20000_Gen1_ST3\"\) 