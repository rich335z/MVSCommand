#
# Test IDCAMS
#
#set -x
# First, delete the dataset if it already exists, and suppress any errors
mvscmd --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=ibmuser.test.in\(delete\) --sysprint=* >/dev/null

# Repro the dataset from test.sortout(test)
mvscmd -v --pgm="IDCAMS" --sysin=ibmuser.test.in\(define\) --sysprint=* --datain=ibmuser.test.sortout\(test\) | grep 0IDC # suppress time stamps

# Delete the (created) dataset
mvscmd --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=ibmuser.test.in\(delete\) --sysprint=* | grep 0IDC # suppress time stamps