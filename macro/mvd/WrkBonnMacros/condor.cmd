universe = vanilla
executable = ./go.sh
output = LOG/DMesons.out$(Process)
error = LOG/DMesons.err$(Process)
log =  LOG/DMesons.log$(Process)
getenv = true
nice_user = true
arguments = DATA D+D- 15 2 1234 $(Process)
queue 3
