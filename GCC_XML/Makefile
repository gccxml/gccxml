# Installation makefile for GCC-XML's support library.
# Run just "make" for usage information.

# Default prefix setting.
PREFIX=/usr/local

# Setup installation program for anything installed directly by this
# makefile.
INSTALL=`pwd`/install-sh -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA =    $(INSTALL) -m 644

# If the user types just "make", then print out usage information.
all:
	@echo -e \
        "This Makefile is a GCC-XML support library installer.\n" \
        "Usage:\n\n" \
        "  make install PREFIX=/path/to/prefix\n\n" \
        "If PREFIX= is not given, the default is /usr/local.\n" \
        "The files will be placed in PREFIX/share/GCC_XML.\n" \
        "You should place the gccxml_cc1plus executable in PREFIX/bin.\n\n" \
        "This will install support libraries for all supported compilers.\n" \
        "If you would like to install support for only one compiler, use one\n" \
        "of these targets instead of \"install\":\n" \
        "  install_gcc  = Install GCC support.\n" \
        "  install_mpro = Install MipsPro support.\n\n" \
        "You may also use:\n\n" \
        "  make uninstall PREFIX=/path/to/prefix\n\n" \
	"to uninstall the GCC-XML support library.  You must remove the\n" \
        "gccxml_cc1plus executable by hand (since it is installed by hand).\n"

# Installation of main GCC-XML support library directory.
install_main: gccxml
	@echo "Installing with PREFIX="$(PREFIX)" ..."
	@if [ ! -d $(PREFIX)/share/GCC_XML ] ; then \
           echo "Making directory $(PREFIX)/share/GCC_XML "; \
           mkdir -p $(PREFIX)/share/GCC_XML; \
           chmod 755 $(PREFIX)/share/GCC_XML; \
         else true; \
         fi
	@$(INSTALL_PROGRAM) gccxml_find_flags $(PREFIX)/share/GCC_XML
	@if [ ! -d $(PREFIX)/bin ] ; then \
           echo "Making directory $(PREFIX)/bin "; \
           mkdir -p $(PREFIX)/bin; \
           chmod 755 $(PREFIX)/bin; \
         else true; \
         fi
	@$(INSTALL_PROGRAM) gccxml $(PREFIX)/bin

# Uninstallation of main GCC-XML support library directory.
uninstall_main:
	@echo "Uninstalling with PREFIX="$(PREFIX)" ..."
	@if [ -d $(PREFIX)/share/GCC_XML ] ; then \
           echo "Removing directory $(PREFIX)/share/GCC_XML "; \
           rm -rf $(PREFIX)/share/GCC_XML; \
         else true; \
         fi
	@if [ -f $(PREFIX)/bin/gccxml ] ; then \
	   echo "Removing $(PREFIX)/bin/gccxml "; \
           rm -f $(PREFIX)/bin/gccxml; \
         else true; \
         fi

# Installation of MipsPro header support.
install_mpro: install_main
	@MproInclude/mpro_install $(PREFIX) `pwd`

# Installation of GCC header support.
install_gcc: install_main
	@GccInclude/gcc_install $(PREFIX) `pwd`

# Main installation target.
install: install_mpro install_gcc

# Main uninstallation target.
uninstall: uninstall_main

GXFRONT_OBJS=GXFront/gxSystemTools.o \
             GXFront/gxConfiguration.o \
             GXFront/gxFront.o

gccxml: $(GXFRONT_OBJS)
	$(CXX) -o gccxml $(GXFRONT_OBJS)

GXFront/gxSystemTools.o: GXFront/gxSystemTools.cxx
	$(CXX) $(CXX_FLAGS) -I GXFront -o GXFront/gxSystemTools.o -c GXFront/gxSystemTools.cxx

GXFront/gxConfiguration.o: GXFront/gxConfiguration.cxx
	$(CXX) $(CXX_FLAGS) -I GXFront -o GXFront/gxConfiguration.o -c GXFront/gxConfiguration.cxx

GXFront/gxFront.o: GXFront/gxFront.cxx
	$(CXX) $(CXX_FLAGS) -I GXFront -o GXFront/gxFront.o -c GXFront/gxFront.cxx

clean:
	$(RM) $(GXFRONT_OBJS) gccxml
