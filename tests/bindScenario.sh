#
# Test binding
#
#set -x
mvscmd -v --pgm="HEWL" --args="MAP" --syslin=ibmuser.test.obj\(test\) --syslmod=ibmuser.test.load\(test\) --sysprint=* --syslib=CEE.SCEELKED:CEE.SCEELKEX:CEE.SCEECPP | awk '!/^1z\/OS V2 R2 BINDER|BATCH EMULATOR|TIME OF SAVE/'

