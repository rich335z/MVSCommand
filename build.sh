#
# It is important that the C code be built with XPLink. If it is not, then if you try to system() call to an XPLink built program (such as the C compiler), LE will abend.
# It is important that OSCALL(UPSTACK) be used because the assembler code that is called to do a LOAD expects traditional upstack OS Linkage
# GONUM and OFFSET make it easier to debug with no real downside
# LANGLVL(EXTENDED) is required because of the z-specific extensions that are utilized
# Listings aren't required
# 

c89 -o mvscmd -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./mvscmd.lst\) -Wl,xplink -Wa,list load.s  mvscmd.c  >load.lst
