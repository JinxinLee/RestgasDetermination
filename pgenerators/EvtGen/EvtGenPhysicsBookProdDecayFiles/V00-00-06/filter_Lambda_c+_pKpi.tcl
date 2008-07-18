# FILE:  filter_Lambda_c+_pKpi.tcl
# PURPOSE:  set up a filter that requires one and only one of:
#           Lambda_c+, anti-Lambda_c-.  
#
#  This filter was originally made to run using a DEC file that
#  produces only pKpi decays of the Lambda_c decay, and which
#  forces continuum->ccbar.  This file can then be run on 
#  Upsilon(4S) or continuum events, getting events with asingle
#  Lambda_c from B decay or ccbar fragmentation, respectively. 
#
# AUTHOR: Chris Roat croat@stanford.edu

#------------------------------------------------------------------#
#-- Helpful procs to define ---------------------------------------#
#------------------------------------------------------------------#

sourceFoundFile FrameScripts/talkto.tcl

#------------------------------------------------------------------#
#-- filter for Lambda_c -------------------------------------------#
#------------------------------------------------------------------#

module enable GefSelectFilter 
talkto GefSelectFilter {

    #
    # Tell our "GefPdtList" to require Lambda_c+ OR anti-Lambda_c-
    #

    # make the "Lambda_c+" and "no Lambda_c+" filters

    BooNew Lambda_c+ = GefPdtList
    BooObjects Lambda_c+ or Lambda_c+

    BooNew !Lambda_c+ = !Lambda_c+


    # make the "anti-Lambda_c-" and "no anti-Lambda_c-" filters

    BooNew anti-Lambda_c- = GefPdtList
    BooObjects anti-Lambda_c- or anti-Lambda_c-

    BooNew !anti-Lambda_c- = ! anti-Lambda_c-


    # compose "particle & no anti-particle" filter

    BooCompose ParticleOnly = and Lambda_c+ !anti-Lambda_c-

    # compose "no particle & anti-particle" filter

    BooCompose AntiParticleOnly = and !Lambda_c+ anti-Lambda_c-

    # compose the filter and set it as a before-transformation-to COM
    # filter

    BooCompose SingleParticleFilter = or ParticleOnly AntiParticleOnly
    beforeFilter set SingleParticleFilter

}


