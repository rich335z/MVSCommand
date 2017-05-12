#
# It is important that the C code be built with XPLink. If it is not, then if you try to system() call to an XPLink built program (such as the C compiler), LE will abend.
# It is important that OSCALL(UPSTACK) be used because the assembler code that is called to do a LOAD expects traditional upstack OS Linkage
# GONUM and OFFSET make it easier to debug with no real downside
# LANGLVL(EXTENDED) is required because of the z-specific extensions that are utilized
# Listings aren't required
# 
cd bin
rm -f mvscmd *.o *.lst
c89 -c -Wa,list ../src/mvsload.s  >mvsload.lst 
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) ../src/mvsutil.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) ../src/mvsargs.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) ../src/mvsdataset.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) ../src/mvssys.c
c89 -c -O2 -Wc,xplink\(OSCALL\(UPSTACK\)\),gonum,offset,langlvl\(extended\),list\(./\) ../src/mvscmd.c
c89 -o mvscmd -O2 -Wl,xplink,ac=1 mvscmd.o mvsargs.o mvsdataset.o mvssys.o mvsutil.o mvsload.o 
cp mvscmd //"'"${AUTHHLQ}.${AUTHSFX}\(MVSCMD\)"'"
rm -rf mvscmdauth
ln -e MVSCMD mvscmdauth
