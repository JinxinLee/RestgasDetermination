# Please see SoftRelTools/HOWTO-dependency for documentation
# $Id: bin_RhoBase.mk,v 1.1.1.1 2001-05-04 16:10:03 marcel Exp $
#++ NON-STANDARD
        LINKLISTDEPENDS += [LINK_RhoBase, $(LINK_RhoBase)]

# Stop gap solution to link Rho (it is not yet in PackageList)
include $(TOPDIR)/RhoMakefiles/link_Rho.mk

override LINK_RHO	+= $(PACKAGE)GNUmakefile
override LINK_ROOT	+= $(PACKAGE)GNUmakefile

