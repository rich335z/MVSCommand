#
# Runs the C compiler on a simple source file, writing to the console
#

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.C.OBJ'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
(
 export STEPLIB=${CHLQ}.SCCNCMP;
 . setcc cCompilation;
 mvscmd -v --pgm="CCNDRVR" --args="/CXX" --syslin=${TESTHLQ}.mvscmd.c.obj\(main\) --sysin=${TESTHLQ}.mvscmd.c\(main\) --sysprint=*
 . unsetcc
)
(tsocmd delete "'"${TESTHLQ}".MVSCMD.C.OBJ'") >/dev/null 2>&1
