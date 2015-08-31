all :	mbms-rel6-FLUTE-repair redirect.cgi

mbms-rel6-FLUTE-repair ::
	@echo "------------------------------"
	@echo "*** mbms-rel6-flute-repair ***"
	@echo "------------------------------"
	mkdir bin; cd mbms-rel6-flute-repair; make clean; make                   
	@echo "done"

redirect.cgi ::
	@echo "--------------------"
	@echo "*** redirect.cgi ***"
	@echo "--------------------"
	mkdir bin; cd redirect; make clean; make                   
	@echo "done"

clean :
	@echo "---------------------------------------"
	@echo "*** Cleaning mbms-rel6-flute-repair ***"
	@echo "---------------------------------------"
	cd mbms-rel6-flute-repair; make clean
	@echo "-----------------------------"
	@echo "*** Cleaning redirect.cgi ***"
	@echo "-----------------------------"
	cd redirect; make clean
	@echo "done"



















