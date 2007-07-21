include ./common.mk
all: rgis_target rcommands_target rgisPlot_target

clean:
	$(UNIXMAKE) -C rGIS clean
	$(UNIXMAKE) -C rCommands clean
	$(UNIXMAKE) -C rgisPlot clean
	$(UNIXMAKE) -C CMlib clean
	$(UNIXMAKE) -C DBlib clean
	$(UNIXMAKE) -C RGlib clean
	$(UNIXMAKE) -C UIlib clean
	$(UNIXMAKE) -C MFlib clean

rgis_target: cmlib_target dblib_target rglib_target uilib_target
	$(UNIXMAKE) -C rGIS all
rcommands_target: cmlib_target rglib_target mflib_target
	$(UNIXMAKE) -C rCommands all
rgisPlot_target: cmlib_target dblib_target
	$(UNIXMAKE) -C rgisPlot all

cmlib_target:
	$(UNIXMAKE) -C CMlib all
dblib_target:
	$(UNIXMAKE) -C DBlib all
rglib_target:
	$(UNIXMAKE) -C RGlib all
uilib_target:
	$(UNIXMAKE) -C UIlib all
mflib_target:
	$(UNIXMAKE) -C MFlib all
