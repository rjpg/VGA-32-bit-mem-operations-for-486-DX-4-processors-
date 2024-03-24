@echo off
set __opath=%path%
set path=c:\lp\watcom\bin;c:\lp\watcom\binb
set __oinc=%include%
set include=c:\lp\watcom\h
set watcom=c:\lp\watcom\.
wcl386 /fpc /fpi87 /4r /4s /ox /ot /om /l=dos4g %1
dos4gw %1
set include=%__oinc%
set __oinc=
set path=%__opath%
set __opath=
