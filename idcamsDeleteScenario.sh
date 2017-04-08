#
# Test IDCAMS
#
#set -x
mvscmd -v --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=ibmuser.test.in\(delete\) --sysprint=*

