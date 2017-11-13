# Here are the configuration bits
SUBDIR := cr_module
TARGET := blcr
IMPORTS_FROM := /home/sharp67/cloud/files/blcr/blcr-0.8.6_b4/builddir/blcr_imports/kbuild

# Extract source and includes/defines from non-kbuild Makefile.am
_SOURCES := $(shell make --no-print-directory -C /home/sharp67/cloud/files/blcr/blcr-0.8.6_b4/builddir/$(SUBDIR) echoval VARNAME=cr_sources)
SOURCES := $(filter-out %.h,$(_SOURCES))
_INCLUDES := $(shell make --no-print-directory -C /home/sharp67/cloud/files/blcr/blcr-0.8.6_b4/builddir/$(SUBDIR) echoval VARNAME=INCLUDES)
EXTRA_CFLAGS = -Wall -I$(srcdir) -I$(top_builddir) $(subst -I.,-I$(builddir)/../.,$(_INCLUDES))

# Here is all the "real" stuff
include /home/sharp67/cloud/files/blcr/blcr-0.8.6_b4/builddir/config/kbuild.mak
