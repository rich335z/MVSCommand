#
# Run batch TSO (IKJEFT01) to archive a PDSE of 2 programs, then delete the PDSE, then restore it, then run the programs to ensure the restore worked.
# Pros of IKJEFT01 over ADRDSU and IEBCOPY: It is the most common way to archive/restore a dataset on z/OS
# Cons of IKJEFT01: It is TSO based - some people do not like TSO and prefer JCL (IEBCOPY) or a more powerful archiving system (ADRDSU)
#

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.TSO.DAR'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(1000,1000\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.TSO.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

c89 -o"//'"${TESTHLQ}".MVSCMD.TSO.LOAD(PROGA)'" ../testsrc/progA.c
c89 -o"//'"${TESTHLQ}".MVSCMD.TSO.LOAD(PROGB)'" ../testsrc/progB.c
rm progA.o progB.o

# Archive 2 programs (progA and progB) to a 'dataset archive' file 
mvscmdauth --pgm=ikjeft01 --sysin=dummy --systsin=${TESTHLQ}.MVSCMD.TSO.CMD\(XMIT\) --sysprint=* --systsprt=* | awk '!/INMX00/' # XMIT 'transmits' PDSE to archive

(tsocmd delete "'"${TESTHLQ}".MVSCMD.TSO.LOAD'") >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.TSO.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

# Restore PDSE from dataset archive file
mvscmdauth --pgm=ikjeft01 --tsodar=${TESTHLQ}.MVSCMD.TSO.DAR --systsin=${TESTHLQ}.MVSCMD.TSO.CMD\(RCV\) --sysprint=* --systsprt=* | awk '!/INMR908A/' # RECEIVE 'receives' archive to PDSE

# Run the programs (which return 1 and 2 respectively) to ensure they work
(export STEPLIB=${TESTHLQ}.MVSCMD.TSO.LOAD:$STEPLIB; mvscmd --pgm=PROGA; rcA=$?; mvscmd --pgm=PROGB; rcB=$?; echo "PROGA->${rcA},PROGB->${rcB}" )

(tsocmd delete "'"${TESTHLQ}".MVSCMD.TSO.LOAD'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.TSO.DAR'") >/dev/null 2>&1
