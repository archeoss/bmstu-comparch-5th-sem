report:
	pdflatex report
	biber report
	pdflatex report
	pdflatex report
	make clean


clean:
	rm -f *.aux *.bbl *.blg *.log *.out *.toc *.lof *.lot *.dvi *.ps *.bcf *.run.xml

.PHONY: report clean