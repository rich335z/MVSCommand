tso alloc dsn\("'"${TESTHLQ}".MVSCMD.C.OBJ'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
(export STEPLIB=${CHLQ}.SCCNCMP; mvscmd -v --pgm="CCNDRVR" --args="/CXX" --syslin=${TESTHLQ}.mvscmd.c.obj\(main\) --sysin=${TESTHLQ}.mvscmd.c\(main\) --sysprint=*)
(tsocmd delete "'"${TESTHLQ}".MVSCMD.C.OBJ'") >/dev/null 2>&1
