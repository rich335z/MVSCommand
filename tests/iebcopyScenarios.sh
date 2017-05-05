tso alloc dsn\("TEST.COPYSOME"\) recfm\(v,b\) lrecl\(137\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\)
tso alloc dsn\("TEST.COPYALL"\) recfm\(v,b\) lrecl\(137\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\)

mvscmd --pgm=iebcopy --sysprint=* --sysin=ibmuser.test.iebcpy\(copymem\) --sysut2=ibmuser.test.copysome --sysut1=ibmuser.test.c | awk '!/IEB1035I/'
mvscmd --pgm=iebcopy --sysprint=* --sysin=dummy --sysut1=ibmuser.test.c --sysut2=ibmuser.test.copyall | awk '!/IEB1035I/'
tsocmd delete "TEST.COPYSOME"
tsocmd delete "TEST.COPYALL"
