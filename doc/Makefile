TEXIS = pintos-ic.texi intro.texi codebase.texi threads.texi userprog.texi vm.texi	\
license.texi reference.texi 44bsd.texi standards.texi	\
doc.texi sample.tmpl.texi devel.texi debug.texi installation.texi	\
bibliography.texi localsettings.texi task0_questions.texi localgitinstructions.texi

all: pintos-ic.html pintos-ic.info pintos-ic.dvi pintos-ic.ps pintos-ic.pdf task0_sheet.pdf alarmclock.pdf

pintos-ic.html: $(TEXIS) texi2html
	./texi2html -toc_file=$@ -split=chapter -nosec_nav -nomenu -init_file pintos-t2h.init $<

pintos-ic.info: $(TEXIS)
	makeinfo $<

pintos-ic.text: $(TEXIS)
	makeinfo --plaintext -o $@ $<

pintos-ic.dvi: $(TEXIS)
	texi2dvi $< -o $@

pintos-ic.ps: pintos-ic.dvi
	dvips $< -o $@

pintos-ic.pdf: $(TEXIS)
	texi2pdf $< -o $@

task0_sheet.pdf : task0_sheet.texi task0_questions.texi
	texi2pdf $< -o $@

alarmclock.pdf : alarmclock.tex
	pdflatex alarmclock

task2_design_doc.pdf : task2_design_doc.tex task2-0-preliminaries.tex task2-A-argument-passing.tex task2-B-system-calls.tex
	pdflatex task2_design_doc

%.texi: %
	sed < $< > $@ 's/\([{}@]\)/\@\1/g;'

clean:
	rm -f *.info* *.html
	rm -f *.dvi *.pdf *.ps *.log *~
	rm -rf WWW
	rm -f sample.tmpl.texi
	rm -f alarmclock.pdf
	rm -f task2_design_doc.pdf

dist: pintos-ic.html pintos-ic.pdf
	rm -rf WWW
	mkdir WWW WWW/specs
	cp *.html *.pdf *.css *.tmpl WWW
	(cd ../specs && cp -r *.pdf freevga kbd sysv-abi-update.html ../doc/WWW/specs)
