#
# Test error scenarios for mvscmd
#
set -x
mvscmd --pgm="Greater#8" #name greater than 8 characters
mvscmd --pgm=Prog --args="parameter that is more than 256 characters......................................................................................................................................................................................................................."
mvscmd --pgm="~xx"
mvscmd --pgm="1xx"
mvscmd --pgm="x~x"
mvscmd --pgm="xx&"

mvscmd --sysout=
mvscmd --sysout=IS#MORE.THAN44.CHARACTR.DATASET.LONGNAME.HERE\(MEMBER\)
mvscmd --sysout=.
mvscmd --sysout=TOOLONGQUALIFIER.SRC
mvscmd --sysout=DATASET\(MEMTOOBIG\)
mvscmd --sysout=\(\)
mvscmd --sysout=A\(
mvscmd --sysout=A\(\)

(mvscmdauth --pgm=RDTD) # 64-bit program from 31-bit mvscmdauth

(mvscmdauth -p=iebcopy) # un-authorized from authorized
(mvscmd -p=ikjeft01) # authorized from unauthorized
