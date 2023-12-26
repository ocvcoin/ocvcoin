.PHONY: FORCE
all: FORCE
	$(MAKE) -C .. ocvcoin_qt test_ocvcoin_qt
clean: FORCE
	$(MAKE) -C .. ocvcoin_qt_clean test_ocvcoin_qt_clean
check: FORCE
	$(MAKE) -C .. test_ocvcoin_qt_check
ocvcoin-qt ocvcoin-qt.exe: FORCE
	 $(MAKE) -C .. ocvcoin_qt
apk: FORCE
	$(MAKE) -C .. ocvcoin_qt_apk
