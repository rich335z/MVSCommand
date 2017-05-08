#
# Test IDCAMS
#
#set -x
# First, delete the dataset if it already exists, and suppress any errors
mvscmd --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(delete\) --sysprint=* >/dev/null

# Repro the dataset 
mvscmd -v --pgm="IDCAMS" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(define\) --sysprint=* --datain=${TESTHLQ}.mvscmd.idcams.in\(idcams\) | awk '!/1IDCAMS/'

# Delete the (created) dataset
mvscmd --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(delete\) --sysprint=* | awk '!/1IDCAMS/' 
