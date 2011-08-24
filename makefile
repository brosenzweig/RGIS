include ./common.mk
all: rgis_target rcommands_target nccommands_target rgisPlot_target tfcommands_target

install: $(INSTALLDIR)/bin $(INSTALLDIR)/XResources/bitmaps $(INSTALLDIR)/Messages $(INSTALLDIR)/html $(INSTALLDIR)/Scripts $(INSTALLDIR)/f
	$(UNIXMAKE) -C rGIS       install
	$(UNIXMAKE) -C rCommands  install
	$(UNIXMAKE) -C rgisPlot   install
	$(UNIXMAKE) -C tfCommands install
	cp XResources/bitmaps/* $(INSTALLDIR)/XResources/bitmaps/
	cp XResources/ghaas     $(INSTALLDIR)/XResources/
	cp -rp html/*.*         $(INSTALLDIR)/html/
	cp Messages/*.*         $(INSTALLDIR)/Messages/
	cp Scripts/*.sh         $(INSTALLDIR)/Scripts/
	cp f/*                  $(INSTALLDIR)/f/
	ln -s $(INSTALLDIR)/Scripts/rgis.sh $(INSTALLDIR)/bin/rgis

reinstall: $(INSTALLDIR)/bin $(INSTALLDIR)/XResources/bitmaps $(INSTALLDIR)/Messages $(INSTALLDIR)/html $(INSTALLDIR)/Scripts $(INSTALLDIR)/f
	$(UNIXMAKE) -C rGIS       install
	$(UNIXMAKE) -C rCommands  install
	$(UNIXMAKE) -C rgisPlot   install
	$(UNIXMAKE) -C tfCommands install
	cp XResources/bitmaps/* $(INSTALLDIR)/XResources/bitmaps/
	cp XResources/ghaas     $(INSTALLDIR)/XResources/
	cp -rp html/*.*         $(INSTALLDIR)/html/
	cp Messages/*.*         $(INSTALLDIR)/Messages/
	cp Scripts/*.sh         $(INSTALLDIR)/Scripts/
	cp f/*                  $(INSTALLDIR)/f/

uninstall:
	$(UNIXMAKE) -C rGIS       uninstall
	$(UNIXMAKE) -C tfCommands uninstall
	$(UNIXMAKE) -C rgisPlot   uninstall
	$(UNIXMAKE) -C rCommands  uninstall
	rm -rf $(INSTALLDIR)/XResources
	rm -rf $(INSTALLDIR)/Messages
	rm -rf $(INSTALLDIR)/html
	rm -rf $(INSTALLDIR)/Scripts
	rm -rf $(INSTALLDIR)/f
	rmdir  $(INSTALLDIR)/bin
	rmdir  $(INSTALLDIR)

clean:
	$(UNIXMAKE) -C rGIS       clean
	$(UNIXMAKE) -C rgisPlot   clean
	$(UNIXMAKE) -C ncCommands clean
	$(UNIXMAKE) -C rCommands  clean
	$(UNIXMAKE) -C tfCommands clean
	$(UNIXMAKE) -C CMlib      clean
	$(UNIXMAKE) -C DBlib      clean
	$(UNIXMAKE) -C NClib      clean
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
$(INSTALLDIR)/html:
	mkdir -p $(INSTALLDIR)/html
$(INSTALLDIR)/Scripts:
	mkdir -p $(INSTALLDIR)/Scripts
$(INSTALLDIR)/f:
	mkdir -p $(INSTALLDIR)/f

rgis_target: cmlib_target dblib_target rglib_target uilib_target
	$(UNIXMAKE) -C rGIS all
rcommands_target: cmlib_target rglib_target mflib_target
	$(UNIXMAKE) -C rCommands all
nccommands_target: cmlib_target nclib_target
	$(UNIXMAKE) -C ncCommands all
tfcommands_target: cmlib_target dblib_target flib_target
	$(UNIXMAKE) -C tfCommands all
rgisPlot_target: cmlib_target dblib_target
	$(UNIXMAKE) -C rgisPlot all

cmlib_target:
	$(UNIXMAKE) -C CMlib all
dblib_target:
	$(UNIXMAKE) -C DBlib all
nclib_target:
	$(UNIXMAKE) -C NClib all
rglib_target:
	$(UNIXMAKE) -C RGlib all
uilib_target:
	$(UNIXMAKE) -C UIlib all
mflib_target:
	$(UNIXMAKE) -C MFlib all
flib_target:
	$(UNIXMAKE) -C Flib all
