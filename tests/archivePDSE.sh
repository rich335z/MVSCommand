#
# Use IEBCOPY to archive a PDSE into a sequential dataset suitable for (binary) transmission via FTP, SCP, XMIT, ...
# The example also restores the PDSE and then runs the programs to ensure they are ok.
# Pros of IEBCOPY over ADRDSU and XMIT: Does not require APF authority. Uses JCL instead of TSO 
# Cons of IEBCOPY: Uses JCL instead of TSO so if you are sending it to someone, they may prefer the file in XMIT format
#

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.DAR'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(1000,1000\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

c89 -o"//'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD(PROGA)'" ../testsrc/progA.c
c89 -o"//'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD(PROGB)'" ../testsrc/progB.c
rm progA.o progB.o

# Archive 2 programs (progA and progB) to a 'dataset archive' file 
mvscmd --pgm=iebcopy --sysin=dummy --sysut1=${TESTHLQ}.MVSCMD.IEBCOPY.LOAD --sysut2=${TESTHLQ}.MVSCMD.IEBCOPY.DAR --sysprint=* | awk '!/IEB1135I|IEB1035I/' | /bin/sed -e 's/ \+VOL=[^ ]\+/ VOL=******/'

(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'") >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

# Restore PDSE from dataset archive file
mvscmd --pgm=iebcopy --sysin=dummy --sysut2=${TESTHLQ}.MVSCMD.IEBCOPY.LOAD --sysut1=${TESTHLQ}.MVSCMD.IEBCOPY.DAR --sysprint=* | awk '!/IEB1135I|IEB1035I/' | /bin/sed -e 's/ \+VOL=[^ ]\+/ VOL=******/'

# Run the programs (which return 1 and 2 respectively) to ensure they work
(export STEPLIB=${TESTHLQ}.MVSCMD.IEBCOPY.LOAD:$STEPLIB; mvscmd --pgm=PROGA; rcA=$?; mvscmd --pgm=PROGB; rcB=$?; echo "PROGA->${rcA},PROGB->${rcB}" )

(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.LOAD'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.DAR'") >/dev/null 2>&1


