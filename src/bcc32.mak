# src/bcc32.mak

# Makefile for Borland C++ 5.5 (or compat)
# Top-file makefile for building Win32 libpq with Borland C++.

!IF "$(CFG)" != "Release" && "$(CFG)" != "Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running MAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE make  -DCFG=[Release | Debug] /f bcc32.mak
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Release" (Win32 Release)
!MESSAGE "Debug" (Win32 Debug)
!MESSAGE
!ENDIF

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
   make -N -DCFG=$(CFG) /f bcc32.mak
   cd ..\..
   echo All Win32 parts have been built!

CLEAN:
   cd interfaces\libpq
   make -N -DCFG=Release /f bcc32.mak CLEAN
   make -N -DCFG=Debug /f bcc32.mak CLEAN
   cd ..\..
   echo All Win32 parts have been cleaned!

DISTCLEAN: CLEAN
   cd include
   del mdb_config.h mdb_config_ext.h mdb_config_os.h
   cd ..
