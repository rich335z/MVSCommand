# here is where the PL/I compiler is: https://linuxvm:19443/RDT/edit/edit.html#/ZOS/MVSFiles/10.1.1.2:2016/IEL450.SIBMZPRC/IBMZC
tso alloc dsn\("TEST.PLI.SYSUT1"\) dsorg\(ps\) catalog tracks space\(100,10\)
(export STEPLIB=IEL450.SIBMZCMP:CEE.SCEERUN; mvscmd --pgm=ibmzpli --sysprint=* --sysout=* --sysin=ibmuser.test.pli\(main\) --syslin=ibmuser.test.obj\(main\) --sysut1=ibmuser.test.pli.sysut1 | awk '!/15655-W67|0 Compiler/' )
tsocmd delete "TEST.PLI.SYSUT1"
