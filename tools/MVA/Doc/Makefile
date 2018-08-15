# Define targets
LATEX = pdflatex
TARGETS = PndMvaDoc

all: $(TARGETS)

PndMvaDoc: % : %.tex
	$(LATEX) $@
	@echo "Done building:" $@

clean:
	rm -f *~ *.log *.toc *.pdf *out *.aux