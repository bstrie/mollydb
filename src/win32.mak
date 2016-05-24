# src/win32.mak

# Top-file makefile for building Win32 libpq with Visual C++ 7.1.
# (see src/tools/msvc for tools to build with Visual C++ 2005 and newer)

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

ALL:
   cd include
   if not exist mdb_config.h copy mdb_config.h.win32 mdb_config.h
   if not exist mdb_config_ext.h copy mdb_config_ext.h.win32 mdb_config_ext.h
   if not exist mdb_config_os.h copy port\win32.h mdb_config_os.h
   cd ..
   cd interfaces\libpq
   nmake /f win32.mak $(MAKEMACRO)
   cd ..\..
   echo All Win32 parts have been built!

CLEAN:
   cd interfaces\libpq
   nmake /f win32.mak CLEAN
   cd ..\..
   echo All Win32 parts have been cleaned!

DISTCLEAN: CLEAN
   cd include
   del mdb_config.h mdb_config_ext.h mdb_config_os.h
   cd ..
