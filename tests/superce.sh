#
# Run superce on newfile and oldfile and write the results to the console, suppressing messages with time stamps
#
. setcc compareFiles
mvscmd --pgm=isrsupc --args="DELTAL,LINECMP" --newdd=${TESTHLQ}.mvscmd.superce.in\(newfile\) --olddd=${TESTHLQ}.mvscmd.superce.in\(oldfile\) --sysin=${TESTHLQ}.mvscmd.superce.cmd\(superce\) --outdd=* | awk '!/1  ISRSUPC/'
. unsetcc