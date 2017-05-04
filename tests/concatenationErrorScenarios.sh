#
# Test error scenarios around concatenation
#
set -x
mvscmd --concat=UNKA:UNKB:UNKC #Error allocating non-existant datasets
mvscmd --concat=UNKA: #Error missing dataset in concatenation list
mvscmd --concat=:UNKA #Error missing dataset in concatenation list
mvscmd --concat=CEE.SCEERUN:::CEE.SCEERUN #Error missing dataset in concatenation list