#   Copyright 1989, University Corporation for Atmospheric Research
#
#  DOS and OS/2 Makefile for ncdump
#
#  NOTE:  Set the definition of the OS2 macro to match OS as follows:
#         OS2 = 0     -> DOS
#         OS2 = 1     -> OS/2

OS2      = 0

AR        = lib
ARFLAGS   =

CC        = cl
CFLAGS    = /c /AL /Za

LINK      = link
LFLAGS    = /st:15000 /nod

INCDIR1   = ..\libsrc 
INCDIR2   = ..\xdr
INCLUDES  = /I$(INCDIR1) /I$(INCDIR2)

DESTDIR   = C:

BINDIR    = $(DESTDIR)\bin
INCLUDE   = $(DESTDIR)\include
LIBDIR    = $(DESTDIR)\lib
NCDUMPLIB = ncdump.lib
NETCDFLIB = ..\libsrc\netcdf.lib
CLIB      = llibc7.lib
!IF $(OS2)
OS2LIB    = os2.lib
!ELSE
OS2LIB    = 
!ENDIF
XDRLIB    = ..\xdr\xdr.lib
LIBS      = $(NCDUMPLIB) $(NETCDFLIB) $(XDRLIB) $(OS2LIB) $(CLIB)

.c.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $<

LOADLIBS =

GOAL =  ncdump.exe

SRCS =  ncdump.c vardata.c dumplib.c

MAIN = ncdump.obj

OBJS =  vardata.obj dumplib.obj
LOBJS = -+vardata.obj -+dumplib.obj

all:	$(GOAL)

$(GOAL): $(MAIN) $(NCDUMPLIB) $(NETCDFLIB) $(XDRLIB)
	$(LINK) $(LFLAGS) $(MAIN),$(GOAL),,$(LIBS);

$(NCDUMPLIB): $(OBJS)
	$(AR) $@ $(ARFLAGS) $(LOBJS),LIB.LST;

install: $(GOAL)
	copy $(GOAL) $(BINDIR)
	del $(GOAL)

test:	ncdump.exe test0.cdl FORCE
	..\ncgen\ncgen -o test0.cdf -n test0.cdl
	ncdump test0.cdf > test1.cdl
	..\ncgen\ncgen -o test1.cdf -n test1.cdl
	ncdump -n test0 test1.cdf > test2.cdl
	diff test1.cdl test2.cdl 
	@echo "Test successful."

ncdump: ncdump.exe

clean:
	rm -f *.obj *.map *.lst *.bak ncdump.lib $(GOAL) \
		test0.cdf test1.cdf test1.cdl test2.cdl

FORCE:

# DO NOT DELETE THIS LINE -- make depend depends on it.

ncdump.obj:  $(INCDIR1)\netcdf.h
vardata.obj: $(INCDIR1)\netcdf.h
