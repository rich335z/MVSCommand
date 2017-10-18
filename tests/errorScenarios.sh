#
# Test error scenarios for mvscmd
#
#set -x
. setcc errInvalidOptionTooShort
mvscmd --ar=myardd     
. setcc errInvalidOptionTooLong
mvscmd --pgmName=mypgmdd  
. setcc errNoProgramSpecified
mvscmd --pgm
. setcc errNoArgSpecified
mvscmd --args
. setcc errPgmNameTooLong
mvscmd --pgm="Greater#8" #name greater than 8 characters
. setcc errArgsTooLong
mvscmd --pgm=Prog --args="parameter that is more than 256 characters......................................................................................................................................................................................................................."
. setcc errPgmNameInvalidCharacter
mvscmd --pgm="~xx"
. setcc errPgmNameInvalidFirstCharacter
mvscmd --pgm="1xx"
. setcc errPgmNameInvalidMiddleCharacter
mvscmd --pgm="x~x"
. setcc errPgmNameInvalidLastCharacter
mvscmd --pgm="xx&"

. setcc errNoFileForDDName
mvscmd --sysout=
. setcc errDatasetNameTooLong
mvscmd --sysout=IS#MORE.THAN44.CHARACTR.DATASET.LONGNAME.HERE\(MEMBER\)
. setcc errInvalidDatasetPeriod
mvscmd --sysout=.
. setcc errQualifierInDatasetTooLong
mvscmd --sysout=TOOLONGQUALIFIER.SRC
. setcc errMemberInDatasetTooLong
mvscmd --sysout=DATASET\(MEMTOOBIG\)
. setcc errBlankPDSAndMemberName
mvscmd --sysout=\(\)
. setcc errMisMatchedParenInMemberName
mvscmd --sysout=A\(
. setcc errQualifierHasEmptyMemberName
mvscmd --sysout=A\(\)

(
 . setcc errTryingToRun31BitProgram;
 mvscmdauth --pgm=RDTD;
 . unsetcc;
) # 64-bit program from 31-bit mvscmdauth

(
 . setcc errTryingToRunUnauthorizedFromAuthorized;
 mvscmdauth -p=iebcopy;
 . unsetcc;
) # un-authorized from authorized

(
 . setcc errTryingToRunAuthorizedFromUnauthorized;
 mvscmd -p=ikjeft01;
 . unsetcc
) # authorized from unauthorized
