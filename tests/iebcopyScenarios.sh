tso alloc dsn\("TEST.COPYSOME"\) recfm\(v,b\) lrecl\(137\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\)

mvscmd --pgm=iebcopy --sysprint=* --sysin=ibmuser.test.iebcopy\(copymem\) --sysut1=ibmuser.test.copysome --sysut2=ibmuser.test.c

tsocmd delete "TEST.COPYSOME"