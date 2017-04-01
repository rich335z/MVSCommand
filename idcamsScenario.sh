#
# Test IDCAMS
#
#set -x
mvsjobs -v --pgm="IDCAMS" --args="MARGINS(2 72)" --sysin=ibmuser.test.in\(delete\) --sysprint=ibmuser.test.print\(idcams\)


