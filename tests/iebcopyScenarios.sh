tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.COPYSOME'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.COPYALL'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1

mvscmd --pgm=iebcopy --sysprint=* --sysin=${TESTHLQ}.mvscmd.iebcopy.cmd\(copysome\) --sysut2=${TESTHLQ}.mvscmd.iebcopy.copysome --sysut1=${TESTHLQ}.mvscmd.iebcopy.in | awk '!/IEB1035I/'
mvscmd --pgm=iebcopy --sysprint=* --sysin=dummy --sysut1=${TESTHLQ}.mvscmd.iebcopy.in --sysut2=${TESTHLQ}.mvscmd.iebcopy.copyall | awk '!/IEB1035I/'

(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.COPYSOME'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.COPYALL'") >/dev/null 2>&1
