#
# Run ADRDSU to take a set of datasets and write them to an archive.
# Run ADRDSU again, but getting input from stdin instead of a dataset
# Pros of ADRDSU over IKJEFT01 and IEBCOPY: easy to take a very large number of datasets and archive them
# Cons: requires APF authorization to run the command because of this power
#

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.ADRDSU.DARV1'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

(
 . setcc adrdsuCreateViaDataset;
 mvscmdauth -v --pgm=ADRDSSU --archive=${TESTHLQ}.mvscmd.adrdsu.dar,old --sysin=${TESTHLQ}.mvscmd.adrdsu.cmd --sysprint=* | awk '!/1PAGE 0001|ADR109I|ADR006I|ADR801I|ADR006I|ADR013I|ADR012I/'
 . unsetcc

)

(tsocmd delete "'"${TESTHLQ}".MVSCMD.ADRDSU.DARV1'") >/dev/null 2>&1

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.ADRDSU.DARV2'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

(
  . setcc adrdsuCreateViaStdin;
  echo " DUMP OUTDD(ARCHIVE) -\n    DS(INCL(${TESTHLQ}.MVSCMD.IEBCOPY.**))" | 
    mvscmdauth --pgm=ADRDSSU --archive=${TESTHLQ}.mvscmd.adrdsu.dar,old --sysin=stdin --sysprint=stdout |
    awk '!/1PAGE 0001|ADR109I|ADR006I|ADR801I|ADR006I|ADR013I|ADR012I/'
 . unsetcc
)

(tsocmd delete "'"${TESTHLQ}".MVSCMD.ADRDSU.DARV2'") >/dev/null 2>&1
