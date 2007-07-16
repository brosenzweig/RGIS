include ./common.mk
all: rgis rcommands

clean:
	$(UNIXMAKE) -C rGIS clean
	$(UNIXMAKE) -C rCommands clean
	$(UNIXMAKE) -C CMlib clean
	$(UNIXMAKE) -C DBlib clean
	$(UNIXMAKE) -C RGlib clean
	$(UNIXMAKE) -C UIlib clean
	$(UNIXMAKE) -C MFlib clean

rgis: cmlib dblib rglib uilib
	$(UNIXMAKE) -C rGIS all
rcommands: cmlib rglib mflib
	$(UNIXMAKE) -C rCommands all

cmlib:
	$(UNIXMAKE) -C CMlib all
dblib:
	$(UNIXMAKE) -C DBlib all
rglib:
	$(UNIXMAKE) -C RGlib all
uilib:
	$(UNIXMAKE) -C UIlib all
mflib:
	$(UNIXMAKE) -C MFlib all
