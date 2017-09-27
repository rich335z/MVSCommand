#
# Run IDCAMS three 3 times. 
# First to delete a VSAM dataset, redirecting errors (such as dataset does not exist) to /dev/null.
# Second to create the VSAM dataset, based on a sequential dataset (piping messages with timestamps to awk to remove them)
# Third to delete the VSAM dataset that was created on the second step
#
#set -x

# First, delete the dataset if it already exists, and suppress any errors
. setcc DeleteVSAMDataset
mvscmdauth --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(delete\) --sysprint=* >/dev/null
. unsetcc

# Repro the dataset 
. setcc ReproVSAMDataset
mvscmdauth -v --pgm="IDCAMS" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(define\) --sysprint=* --datain=${TESTHLQ}.mvscmd.idcams.in\(idcams\) | awk '!/1IDCAMS/' | /bin/sed -e 's/ \+VOLUMES([0-9A-Z]*)/ VOLUMES(******)/' | /bin/sed -e 's/ \+VOLUME [^ ]\+/ VOLUME ******/'
. unsetcc

# Delete the (created) dataset
. setcc DeleteVSAMDatasetAgain
mvscmdauth --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=${TESTHLQ}.mvscmd.idcams.cmd\(delete\) --sysprint=* | awk '!/1IDCAMS/' 
. unsetcc

