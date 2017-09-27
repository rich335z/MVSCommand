#
# Runs the C compiler against code that has an error in it
#
(
 export STEPLIB=${CHLQ}.SCCNCMP; 
 . setcc cCompilationWithError;
 mvscmd --pgm="CCNDRVR" --args="/CXX " --sysin=${TESTHLQ}.mvscmd.c\(err\) --sysprint=dummy
 . unsetcc
)
