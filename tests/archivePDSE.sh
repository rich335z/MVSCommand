tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.DAR'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(1000,1000\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

c89 -o"//'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD(PROGA)'" ../testsrc/progA.c
c89 -o"//'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD(PROGB)'" ../testsrc/progB.c
rm progA.o progB.o

# Archive 2 programs (progA and progB) to a 'dataset archive' file 
mvscmd --pgm=iebcopy --sysin=dummy --sysut1=${TESTHLQ}.MVSCMD.IEBCOPY.LOAD --sysut2=${TESTHLQ}.MVSCMD.IEBCOPY.DAR --sysprint=* | awk '!/IEB1135I|IEB1035I/'

(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'") >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

# Restore PDSE from dataset archive file
mvscmd --pgm=iebcopy --sysin=dummy --sysut2=${TESTHLQ}.MVSCMD.IEBCOPY.LOAD --sysut1=${TESTHLQ}.MVSCMD.IEBCOPY.DAR --sysprint=* | awk '!/IEB1135I|IEB1035I/'

# Run the programs (which return 1 and 2 respectively) to ensure they work
(export STEPLIB=${TESTHLQ}.MVSCMD.IEBCOPY.LOAD:$STEPLIB; mvscmd --pgm=PROGA; rcA=$?; mvscmd --pgm=PROGB; rcB=$?; echo "PROGA->${rcA},PROGB->${rcB}" )

(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.DAR'") >/dev/null 2>&1


