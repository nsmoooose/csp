# Makefile to assist with creating data recorder reports.
#
# Generates a title page from header.tex and merges it with 
# the rest of the graphs in .out.ps~.  The combined output
# is converted to pdf (out-header.pdf).  The original graphs
# are also converted to pdf without a title page (out.pdf).

all: out.pdf out-header.pdf
	@rm .out.ps~
	@echo "Graphs written to 'out.pdf' and 'out-header.pdf'"

header.ps: header.tex
	@echo "Reprocessing header.  If an error occurs, see header.log."
	@latex -interaction=nonstopmode header.tex >/dev/null
	@dvips header.dvi -oheader.ps >/dev/null 2>&1

out-header.pdf: header.ps .out.ps~
	gs -sDEVICE=pdfwrite -sOutputFile=$@ -dBATCH -dNOPAUSE -q $^

out.pdf: .out.ps~
	@ps2pdf .out.ps~ out.pdf	
