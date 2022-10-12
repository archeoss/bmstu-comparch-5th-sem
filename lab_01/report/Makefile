report:
	pdflatex report.tex
	biber report
	pdflatex report.tex
	pdflatex report.tex
	make clean

clean:
	rm -f *.aux *.bbl *.blg *.log *.out *.toc *.lof *.lot *.dvi *.ps *.bcf *.run.xml

.PHONY: report clean