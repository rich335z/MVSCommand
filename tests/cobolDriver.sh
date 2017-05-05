tso alloc dsn\("TEST.COBOL.SYSUT1"\) dsorg\(ps\) catalog tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT2"\) dsorg\(ps\) catalog tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT3"\) dsorg\(ps\) catalog tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT4"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT5"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT6"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT7"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT8"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT9"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT10"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT11"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT12"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT13"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT14"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSUT15"\) dsorg\(ps\) catalog  tracks space\(100,10\)
tso alloc dsn\("TEST.COBOL.SYSMDECK"\) dsorg\(ps\) catalog  tracks space\(100,10\)

(export STEPLIB=IGY.V5R2M0:CEE.SCEERUN:CEE.SCEERUN2:$STEPLIB; mvscmd -v --pgm="IGYCRCTL" --args="NONAME,NOTERM,LIST" --syslin=ibmuser.test.obj\(cobol\) --sysin=ibmuser.test.cobol\(test\) --sysprint=ibmuser.test.cobol.list --sysmdeck=ibmuser.test.cobol.sysmdeck \
--sysut1=ibmuser.test.cobol.sysut1 \
--sysut2=ibmuser.test.cobol.sysut2 \
--sysut3=ibmuser.test.cobol.sysut3 \
--sysut4=ibmuser.test.cobol.sysut4 \
--sysut5=ibmuser.test.cobol.sysut5 \
--sysut6=ibmuser.test.cobol.sysut6 \
--sysut7=ibmuser.test.cobol.sysut7 \
--sysut8=ibmuser.test.cobol.sysut8 \
--sysut9=ibmuser.test.cobol.sysut9 \
--sysut10=ibmuser.test.cobol.sysut10 \
--sysut11=ibmuser.test.cobol.sysut11 \
--sysut12=ibmuser.test.cobol.sysut12 \
--sysut13=ibmuser.test.cobol.sysut13 \
--sysut14=ibmuser.test.cobol.sysut14 \
--sysut15=ibmuser.test.cobol.sysut15\
)

tsocmd delete "TEST.COBOL.SYSUT1"
tsocmd delete "TEST.COBOL.SYSUT2"
tsocmd delete "TEST.COBOL.SYSUT3"
tsocmd delete "TEST.COBOL.SYSUT4"
tsocmd delete "TEST.COBOL.SYSUT5"
tsocmd delete "TEST.COBOL.SYSUT6"
tsocmd delete "TEST.COBOL.SYSUT7"
tsocmd delete "TEST.COBOL.SYSUT8"
tsocmd delete "TEST.COBOL.SYSUT9"
tsocmd delete "TEST.COBOL.SYSUT10"
tsocmd delete "TEST.COBOL.SYSUT11"
tsocmd delete "TEST.COBOL.SYSUT12"
tsocmd delete "TEST.COBOL.SYSUT13"
tsocmd delete "TEST.COBOL.SYSUT14"
tsocmd delete "TEST.COBOL.SYSUT15"
tsocmd delete "TEST.COBOL.SYSMDECK"
