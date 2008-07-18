#------------------------------------------------------------------------------
# $Id: TurtleRead.tcl,v 1.1.1.1 2005/03/29 17:12:43 steinke Exp $
#
# tcl file to select GfiTurtleRead generator
#
#------------------------------------------------------------------------------

#
sourceFoundFile ErrLogger/ErrLog.tcl
sourceFoundFile FrameScripts/talkto.tcl
#
disableGenerators 0
module enable GfiTurtleRead

#
if ![info exists TurtleFile] {
    ErrMsg fatal "TurtleFile not defined."
}

#
talkto GfiTurtleRead {
    readFile      set $TurtleFile
    longFormat    set true
    useStorePoint set true
}

ErrMsg trace " completed OK"

