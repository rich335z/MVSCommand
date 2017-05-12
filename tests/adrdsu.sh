tso alloc dsn\("'"${TESTHLQ}".MVSCMD.ADRDSU.DAR'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(1000,1000\) >/dev/null 2>&1

(
 export STEPLIB=${AUTHHLQ}.${AUTHSFX}:$STEPLIB; 
 mvscmdauth -v --pgm=ADRDSSU --archive=${TESTHLQ}.mvscmd.adrdsu.dar,old --sysin=${TESTHLQ}.mvscmd.adrdsu.cmd --sysprint=* | awk '!/1PAGE 0001|ADR109I|ADR006I|ADR801I|ADR006I|ADR013I|ADR012I/'
)

(tsocmd delete "'"${TESTHLQ}".MVSCMD.ADRDSU.DAR'") >/dev/null 2>&1
