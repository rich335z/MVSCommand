tso alloc dsn\("'"${TESTHLQ}".MVSCMD.PLI.OBJ'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.PLI.SYSUT1'"\) dsorg\(ps\) catalog tracks space\(100,10\) >/dev/null 2>&1

(export STEPLIB=${PLIHLQ}.SIBMZCMP:${LEHLQ}.SCEERUN; mvscmd --pgm=ibmzpli --sysprint=* --sysout=* --sysin=${TESTHLQ}.mvscmd.pli\(main\) --syslin=${TESTHLQ}.mvscmd.pli.obj\(main\) --sysut1=${TESTHLQ}.mvscmd.pli.sysut1 | awk '!/15655-W67|0 Compiler/' )

(tsocmd delete "'"${TESTHLQ}".MVSCMD.PLI.SYSUT1'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.PLI.OBJ'") >/dev/null 2>&1
