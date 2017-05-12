tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.OBJ'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT1'"\) dsorg\(ps\) catalog tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT2'"\) dsorg\(ps\) catalog tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT3'"\) dsorg\(ps\) catalog tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT4'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT5'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT6'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT7'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT8'"\) dsorg\(ps\) catalog  tracks space\(100,10\)	>/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT9'"\) dsorg\(ps\) catalog  tracks space\(100,10\)	>/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT10'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1	
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT11'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT12'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT13'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT14'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSUT15'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL.SYSMDECK'"\) dsorg\(ps\) catalog  tracks space\(100,10\) >/dev/null 2>&1

(export STEPLIB=${COBOLHLQ}.V5R2M0:${LEHLQ}.SCEERUN:${LEHLQ}.SCEERUN2:$STEPLIB; mvscmd -v --pgm="IGYCRCTL" --args="NONAME,NOTERM,NOLIST,NOSOURCE,NOXREF" --syslin=${TESTHLQ}.mvscmd.cobol.obj\(cobol\) --sysin=${TESTHLQ}.mvscmd.cobol\(main\) --sysprint=dummy --sysmdeck=${TESTHLQ}.mvscmd.cobol.sysmdeck \
--sysut1=${TESTHLQ}.mvscmd.cobol.sysut1 \
--sysut2=${TESTHLQ}.mvscmd.cobol.sysut2 \
--sysut3=${TESTHLQ}.mvscmd.cobol.sysut3 \
--sysut4=${TESTHLQ}.mvscmd.cobol.sysut4 \
--sysut5=${TESTHLQ}.mvscmd.cobol.sysut5 \
--sysut6=${TESTHLQ}.mvscmd.cobol.sysut6 \
--sysut7=${TESTHLQ}.mvscmd.cobol.sysut7 \
--sysut8=${TESTHLQ}.mvscmd.cobol.sysut8 \
--sysut9=${TESTHLQ}.mvscmd.cobol.sysut9 \
--sysut10=${TESTHLQ}.mvscmd.cobol.sysut10 \
--sysut11=${TESTHLQ}.mvscmd.cobol.sysut11 \
--sysut12=${TESTHLQ}.mvscmd.cobol.sysut12 \
--sysut13=${TESTHLQ}.mvscmd.cobol.sysut13 \
--sysut14=${TESTHLQ}.mvscmd.cobol.sysut14 \
--sysut15=${TESTHLQ}.mvscmd.cobol.sysut15\
)

(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT1'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT2'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT3'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT4'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT5'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT6'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT7'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT8'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT9'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT10'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT11'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT12'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT13'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT14'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSUT15'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.SYSMDECK'") >/dev/null 2>&1

(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL.OBJ'") >/dev/null 2>&1
