#
# It is important that the C code be built with XPLink. If it is not, then if you try to system() call to an XPLink built program (such as the C compiler), LE will abend.
# It is important that OSCALL(UPSTACK) be used because the assembler code that is called to do a LOAD expects traditional upstack OS Linkage
# GONUM and OFFSET make it easier to debug with no real downside
# LANGLVL(EXTENDED) is required because of the z-specific extensions that are utilized
# Listings aren't required
# 

c89 -c -Wa,list mvsload.s  >mvsload.lst 
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) mvsutil.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) mvsargs.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) mvsdataset.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) mvssys.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) mvscmd.c
c89 -o mvscmd -O2 -Wl,xplink mvscmd.o mvsargs.o mvsdataset.o mvssys.o mvsutil.o mvsload.o 
