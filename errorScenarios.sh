#
# Test error scenarios for OSCMD 
#
set -x
mvsjobs --pgm="Greater#8" #name greater than 8 characters
mvsjobs --pgm=Prog --args="parameter that is more than 256 characters......................................................................................................................................................................................................................."
mvsjobs --pgm="~xx"
mvsjobs --pgm="1xx"
mvsjobs --pgm="x~x"
mvsjobs --pgm="xx&"

mvsjobs --sysout=
mvsjobs --sysout=IS#MORE.THAN44.CHARACTR.DATASET.LONGNAME.HERE\(MEMBER\)
mvsjobs --sysout=.
mvsjobs --sysout=TOOLONGQUALIFIER.SRC
mvsjobs --sysout=DATASET\(MEMTOOBIG\) 
mvsjobs --sysout=\(\)
mvsjobs --sysout=A\(
mvsjobs --sysout=A\(\)
