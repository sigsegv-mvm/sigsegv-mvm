$(OUTFILE): $(INFILE)
	$(CXX) -x c++-header $(INFILE) -o $(OUTFILE)
