#
# Run IDCAMS three 3 times. 
# First to delete a VSAM dataset, redirecting errors (such as dataset does not exist) to /dev/null.
# Second to create the VSAM dataset, based on a sequential dataset (piping messages with timestamps to awk to remove them)
# Third to delete the VSAM dataset that was created on the second step
#
#set -x
# First, delete the dataset if it already exists, and suppress any errors
mvscmd --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(delete\) --sysprint=* >/dev/null

# Repro the dataset 
mvscmd -v --pgm="IDCAMS" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(define\) --sysprint=* --datain=${TESTHLQ}.mvscmd.idcams.in\(idcams\) | awk '!/1IDCAMS/'

# Delete the (created) dataset
mvscmd --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(delete\) --sysprint=* | awk '!/1IDCAMS/' 
