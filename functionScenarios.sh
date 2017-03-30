#
# Test functional scenarios for mvsjobs 
#
#set -x
(export STEPLIB=IBMUSER.RDT.LOAD; mvsjobs -v --pgm="RDTD") # APF authorized, 64-bit
(export STEPLIB=IGY520.SIGYCOMP; mvsjobs -v --pgm=IGYCRCTL) # COBOL compiler, 31-bit
(mvsjobs -v) # IEFBR14, 24-bit
