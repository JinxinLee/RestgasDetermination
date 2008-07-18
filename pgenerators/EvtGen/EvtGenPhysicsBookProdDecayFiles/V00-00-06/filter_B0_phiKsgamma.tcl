###################################
##
## Generator level filter 
##
## B0 -> phi K0 gamma + C.C.
##
## 6th Nov 2001
##
## jswain@slac.stanford.edu 
##
## Filter on the decay B0 -> Xsd gamma for
## desired events.  Contains two filters,
## General and Specific.
## General filter accepts all phi's and K0's
## Specific filter accepts only K0 -> K_S0 
## and phi -> K+ K-
##
##################################

module talk GefSelectFilter

### Xsd -> phi K0 -> anything
###         |    
###          ------> anything
BooNew     xfk = GefPdtList
BooObjects xfk and phi K0
BooObjects xfk ancestors Xsd
BooObjects xfk ancestorsMode EXCLUSIVE

### Xsd -> phi K0 -> K_S0
###         |
###          -> K+ K-

BooNew     xf2kks = GefPdtList
BooObjects xf2kks and phi K0 K_S0 K+ K-
BooObjects xf2kks ancestors Xsd
BooObjects xf2kks ancestorsMode EXCLUSIVE 

### Xsd -> phi -> K+ K-

BooNew     xfkk = GefPdtList
BooObjects xfkk and K+ K- 
BooObjects xfkk ancestors phi Xsd
BooObjects xfkk ancestorsMode EXCLUSIVE

### Xsd -> K0 -> K_S0

BooNew     xkks = GefPdtList 
BooObjects xkks and K_S0
BooObjects xkks ancestors K0 Xsd
BooObjects xkks ancestorsMode EXCLUSIVE

### Xsd -> phi  K0 -> K_S0
###         |
###          -> K+ K-
BooCompose xfkkks = and xf2kks xfkk xkks

##############################################

### anti-Xsd -> phi anti-K0 -> anything
###              |  
###               -----------> anything
BooNew     axfk = GefPdtList
BooObjects axfk and phi anti-K0
BooObjects axfk ancestors anti-Xsd
BooObjects axfk ancestorsMode EXCLUSIVE

### anti-Xsd -> phi anti-K0 -> K_S0
###              |
###               -> K+ K-
BooNew     axf2kks = GefPdtList
BooObjects axf2kks and phi anti-K0 K_S0 K+ K-
BooObjects axf2kks ancestors anti-Xsd
BooObjects axf2kks ancestorsMode EXCLUSIVE 

### anti-Xsd -> anti-K0 -> K_S0
BooNew     axkks = GefPdtList 
BooObjects axkks and K_S0
BooObjects axkks ancestors anti-K0 anti-Xsd
BooObjects axkks ancestorsMode EXCLUSIVE

### anti-Xsd -> phi -> K+ K-
BooNew     axfkk = GefPdtList
BooObjects axfkk and K+ K- 
BooObjects axfkk ancestors phi anti-Xsd
BooObjects axfkk ancestorsMode EXCLUSIVE

### anti-Xsd -> phi  anti-K0 -> K_S0
###              |
###               -> K+ K-
BooCompose axfkkks = and axf2kks axfkk axkks

### Xsd -> phi K0 -> anything + C.C.
###         |    
###          ------> anything
BooCompose General = or xfk axfk

### Xsd -> phi  K0 -> K_S0 + C.C.
###         |
###          -> K+ K-
BooCompose Specific = or xfkkks axfkkks

### Use specific filter to force K0 -> K_S0
### and phi -> K+ K-
beforeFilter set Specific

exit
