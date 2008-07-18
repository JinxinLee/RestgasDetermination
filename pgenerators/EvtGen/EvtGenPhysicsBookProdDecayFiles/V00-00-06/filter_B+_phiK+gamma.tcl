######################################
##
## Generator level filter 
##
## B+ -> phi K+ gamma + C.C.
##
## 6th Nov 2001
##
## jswain@slac.stanford.edu 
##
## Filter on the decay B -> Xsu gamma for
## desired events.
## Contains two filters, General and Specific.
## General allows the phi to decay to anything,
## Specific requires phi -> K+ K-.
##
######################################

module talk GefSelectFilter

### Xsu -> phi -> K+ K-
BooNew     xfkk = GefPdtList
BooObjects xfkk and K+ K-
BooObjects xfkk ancestors phi Xsu
BooObjects xfkk ancestorsMode EXCLUSIVE

### phi !-> K+ K-
BooNew   noxfkk = ! xfkk

### Xsu -> phi  K+ 
###         |
###          -> K+ K-
BooNew     xf3k = GefPdtList
BooObjects xf3k and K+ phi K+ K-
BooObjects xf3k ancestors Xsu
BooObjects xf3k ancestorsMode EXCLUSIVE

BooCompose xkfkk = and xf3k xfkk

### Xsu -> phi  ?  where the two ?'s include 
###         |      at least one K+
###          -> ?
BooNew     xkf = GefPdtList
BooObjects xkf and K+ phi
BooObjects xkf ancestors Xsu
BooObjects xkf ancestorsMode EXCLUSIVE

### Xsu -> phi  K+
###         |
###         !-> K+ K- 
BooCompose xk   = and noxfkk xkf

################################################

### anti-Xsu -> phi -> K+ K-
BooNew     axfkk = GefPdtList
BooObjects axfkk and K+ K-
BooObjects axfkk ancestors phi anti-Xsu
BooObjects axfkk ancestorsMode EXCLUSIVE

### anti-Xsu -> phi !-> K+ K-
BooNew   noaxfkk = ! axfkk

### anti-Xsu -> phi  K-
###              |
###               -> K- K+
BooNew     axf3k = GefPdtList
BooObjects axf3k and K- phi K+ K-
BooObjects axf3k ancestors anti-Xsu
BooObjects axf3k ancestorsMode EXCLUSIVE

BooCompose axkfkk = and axf3k axfkk

### anti-Xsu -> phi  ? where the two ?'s include
###              |     at least one K-
###               -> ?
BooNew     axkf = GefPdtList
BooObjects axkf and K- phi
BooObjects axkf ancestors anti-Xsu
BooObjects axkf ancestorsMode EXCLUSIVE

### anti-Xsu -> phi  K-
###              |
###              !-> K- K+
BooCompose axk  = and noaxfkk axkf

###############################################

### Xsu -> phi  K+ + C.C.
###         |
###          -> ?
BooCompose General = or xkfkk xk axkfkk axk

### Xsu -> phi  K+ + C.C.
###         |
###          -> K+ K-
BooCompose Specific = or xkfkk axkfkk

### Use the Specific filter to force phi -> K+ K-
beforeFilter set Specific

exit
