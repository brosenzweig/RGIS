include ./common.mk
all: rgis_target rgisPlot_target rcommands_target tfcommands_target

install: $(INSTALLDIR)/bin $(INSTALLDIR)/XResources/bitmaps $(INSTALLDIR)/Messages $(INSTALLDIR)/Scripts
	$(UNIXMAKE) -C rGIS       install
#	$(UNIXMAKE) -C rgisPlot   install
	$(UNIXMAKE) -C rCommands  install
	$(UNIXMAKE) -C tfCommands install
	cp XResources/bitmaps/* $(INSTALLDIR)/XResources/bitmaps/
	cp XResources/ghaas     $(INSTALLDIR)/XResources/
	cp Messages/*.*         $(INSTALLDIR)/Messages/
	cp Scripts/*.sh         $(INSTALLDIR)/Scripts/

uninstall:
	$(UNIXMAKE) -C rGIS       uninstall
#	$(UNIXMAKE) -C rgisPlot   uninstall
	$(UNIXMAKE) -C rCommands  uninstall
	$(UNIXMAKE) -C tfCommands uninstall
	rm -rf $(INSTALLDIR)/XResources
	rm -rf $(INSTALLDIR)/Messages
	rm -rf $(INSTALLDIR)/Scripts
	rmdir  $(INSTALLDIR)/bin
	rmdir  $(INSTALLDIR)

clean:
	$(UNIXMAKE) -C rGIS       clean
	$(UNIXMAKE) -C rCommands  clean
	$(UNIXMAKE) -C tfCommands clean
#	$(UNIXMAKE) -C rgisPlot   clean
	$(UNIXMAKE) -C CMlib      clean
	$(UNIXMAKE) -C DBlib      clean
	$(UNIXMAKE) -C RGlib      clean
	$(UNIXMAKE) -C UIlib      clean
	$(UNIXMAKE) -C MFlib      clean
	$(UNIXMAKE) -C Flib       clean

$(INSTALLDIR)/bin:
	mkdir -p $(INSTALLDIR)/bin
$(INSTALLDIR)/XResources/bitmaps:
	mkdir -p $(INSTALLDIR)/XResources/bitmaps
$(INSTALLDIR)/Messages:
	mkdir -p $(INSTALLDIR)/Messages
$(INSTALLDIR)/Scripts:
	mkdir -p $(INSTALLDIR)/Scripts

rgis_target: cmlib_target dblib_target rglib_target uilib_target
	$(UNIXMAKE) -C rGIS all
rcommands_target: cmlib_target rglib_target mflib_target
	$(UNIXMAKE) -C rCommands all
tfcommands_target: cmlib_target dblib_target flib_target
	$(UNIXMAKE) -C tfCommands all
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
flib_target:
	$(UNIXMAKE) -C Flib all
