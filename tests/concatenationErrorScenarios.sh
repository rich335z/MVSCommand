#
# Test error scenarios around concatenation
#
#set -x
. setcc errNonExistingDataset; mvscmd --concat1=UNKA:UNKB:UNKC ; . unsetcc                            #Error allocating non-existant datasets
. setcc errSyntaxErrorDanglingColon; mvscmd --concat2=UNKA: ; . unsetcc                               #Error missing dataset in concatenation list
. setcc errSyntaxErrorEarlyColon; mvscmd --concat3=:UNKA ; . unsetcc                                  #Error missing dataset in concatenation list
. setcc errSyntaxErrorExtraColon; mvscmd --concat4=${LEHLQ}.SCEERUN:::${LEHLQ}.SCEERUN ; . unsetcc    #Error missing dataset in concatenation list
