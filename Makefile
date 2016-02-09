WERCKER_DIRS = _builds _cache _projects _steps _temp
CLEAN_SUBDIRS = src doc tests

all::
	@echo "This makefile has only 'clean' and 'check' targets."

clean::
	$(RM) -r $(WERCKER_DIRS)
	for d in $(CLEAN_SUBDIRS); do $(MAKE) -C $$d $@; done

distclean:: clean
	find . -name '*~' -exec rm '{}' \;

check::
	make -C tests $@
