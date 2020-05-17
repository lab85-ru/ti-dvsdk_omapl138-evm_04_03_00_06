-include DIRS
-include ../../Rules.make

all: debug release

debug:
	for dir in $(DIRS); do \
                make -C $(ARCH)/$$dir/Debug; \
	done
	echo ${ARCH}

release:
	for dir in $(DIRS); do \
                make -C $(ARCH)/$$dir/Release; \
	done

clean:
	for dir in $(DIRS); do \
                make -C $(ARCH)/$$dir/Release clean; \
                make -C $(ARCH)/$$dir/Debug clean; \
	done
	@rm -f `find . -name "*.map"`
	@rm -f `find . -name "*.lst"`

install:
	@install -d $(DESTDIR)/usr/bin
	@if [ -e $(ARCH)/dhrystone/Release/dhrystone ] ; then \
		install $(ARCH)/dhrystone/Release/dhrystone $(DESTDIR)/usr/bin/ ; \
		echo "dhrystone release version installed."; \
	else \
		echo "dhrystone release version not built - nothing to install!"; \
	fi
	@if [ -e $(ARCH)/whetstone/Release/whetstone ] ; then \
		install $(ARCH)/whetstone/Release/whetstone $(DESTDIR)/usr/bin/ ; \
		echo "whetstone release version installed."; \
	else \
		echo "whetstone release version not built - nothing to install!" ; \
	fi
	@if [ -e $(ARCH)/linpack/Release/linpack ] ; then \
		install $(ARCH)/linpack/Release/linpack $(DESTDIR)/usr/bin/ ; \
		echo "linpack release version installed."; \
	else \
		echo "linpack release version not built - nothing to install!" ; \
	fi
	@install $(ARCH)/scripts/* $(DESTDIR)/usr/bin

install_debug:
	@install -d $(DESTDIR)/usr/bin
	@if [ -e $(ARCH)/dhrystone/Debug/dhrystone ] ; then \
		install $(ARCH)/dhrystone/Debug/dhrystone $(DESTDIR)/usr/bin/ ; \
		echo "dhrystone debug version installed."; \
		echo "NOTE: DEBUG VERSION WILL NOT YIELD MEANINGFUL BENCHMARK RESULTS."; \
	else \
		echo "drhystone debug version not built - nothing to install!"; \
	fi
	@if [ -e $(ARCH)/whetstone/Debug/whetstone ] ; then \
		install $(ARCH)/whetstone/Debug/whetstone $(DESTDIR)/usr/bin/ ; \
		echo "whetstone debug version installed."; \
		echo "NOTE: DEBUG VERSION WILL NOT YIELD MEANINGFUL BENCHMARK RESULTS."; \
	else \
		echo "whetstone debug version not built - nothing to install!" ; \
	fi
	@if [ -e $(ARCH)/linpack/Debug/linpack ] ; then \
		install $(ARCH)/linpack/Debug/linpack $(DESTDIR)/usr/bin/ ; \
		echo "linpack debug version installed."; \
		echo "NOTE: DEBUG VERSION WILL NOT YIELD MEANINGFUL BENCHMARK RESULTS."; \
	else \
		echo "linpack debug version not built - nothing to install!" ; \
	fi
	@install $(ARCH)/scripts/* $(DESTDIR)/usr/bin
