#
# Test functional scenarios for mvsjobs 
#
#set -x
(export STEPLIB=IGY520.SIGYCOMP; mvscmd -v --pgm=IGYCRCTL) # COBOL compiler, 31-bit
(mvscmd -v) # IEFBR14, 24-bit
