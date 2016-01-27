$(OUTFILE): $(INFILE)
	#-rm $(OUTFILE)
	#-touch $(OUTFILE)
	$(CXX) -x c++-header $(INFILE) -o $(OUTFILE)
