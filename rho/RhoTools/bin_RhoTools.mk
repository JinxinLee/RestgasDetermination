# Please see SoftRelTools/HOWTO-dependency for documentation
# $Id: bin_RhoTools.mk,v 1.1.1.1 2001-05-04 16:20:27 marcel Exp $
#++ NON-STANDARD
        LINKLISTDEPENDS += [LINK_RhoTools, $(LINK_RhoTools)]

# Stop gap solution to link Rho (it is not yet in PackageList)
include $(TOPDIR)/RhoMakefiles/link_Rho.mk

override LINK_Rho	+= $(PACKAGE)GNUmakefile
override LINK_ROOT	+= $(PACKAGE)GNUmakefile

