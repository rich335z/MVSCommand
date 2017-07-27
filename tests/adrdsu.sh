#
# Run ADRDSU to take a set of datasets and write them to an archive.
# Pros of ADRDSU over IKJEFT01 and IEBCOPY: easy to take a very large number of datasets and archive them
# Cons: requires APF authorization to run the command because of this power
#

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.ADRDSU.DAR'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

(
# export STEPLIB=${AUTHHLQ}.${AUTHSFX}:$STEPLIB; 
 mvscmdauth -v --pgm=ADRDSSU --archive=${TESTHLQ}.mvscmd.adrdsu.dar,old --sysin=${TESTHLQ}.mvscmd.adrdsu.cmd --sysprint=* | awk '!/1PAGE 0001|ADR109I|ADR006I|ADR801I|ADR006I|ADR013I|ADR012I/'
)

(tsocmd delete "'"${TESTHLQ}".MVSCMD.ADRDSU.DAR'") >/dev/null 2>&1
