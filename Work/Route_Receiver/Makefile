all :	alclib sdplib flutelib flute multis_flute #flute_unittest

alclib ::
	@echo "-------------------"
	@echo "*** ALC library ***"
	@echo "-------------------"
	mkdir lib; cd alclib; make clean; make                   
	@echo "done"

sdplib ::
	@echo "-------------------"
	@echo "*** SDP library ***"
	@echo "-------------------"
	mkdir lib; cd sdplib; make clean; make                   
	@echo "done"

flutelib ::
	@echo "---------------------"
	@echo "*** FLUTE library ***"
	@echo "---------------------"
	mkdir lib; cd flutelib; make clean; make
	@echo "done"

flute ::
	@echo "-------------------------"
	@echo "*** FLUTE application ***"
	@echo "-------------------------"
	mkdir bin; cd flute; make clean; make
	@echo "done"

multis_flute ::
	@echo "----------------------------------------------"
	@echo "*** Example multisession FLUTE application ***"
	@echo "----------------------------------------------"
	mkdir bin; cd multis_flute; make clean; make
	@echo "done"

flute_unittest ::
	@echo "------------------------"
	@echo "*** FLUTE unit test ***"
	@echo "------------------------"
	mkdir bin; cd UnitTest; make clean; make
	@echo "done"

clean :
	@echo "----------------------------"
	@echo "*** Cleaning ALC library ***"
	@echo "----------------------------"
	cd alclib; make clean
	@echo "----------------------------"
	@echo "*** Cleaning SDP library ***"
	@echo "----------------------------"
	cd sdplib; make clean
	@echo "------------------------------"
	@echo "*** Cleaning FLUTE library ***"
	@echo "------------------------------"
	cd flutelib; make clean
	@echo "----------------------------------"
	@echo "*** Cleaning FLUTE application ***"
	@echo "----------------------------------"
	cd flute; make clean
	@echo "-------------------------------------------------------"
	@echo "*** Cleaning example multisession FLUTE application ***"
	@echo "-------------------------------------------------------"
	cd multis_flute; make clean
	@echo "--------------------------------"
	@echo "*** Cleaning FLUTE unit test ***"
	@echo "--------------------------------"
	cd UnitTest; make clean
	@echo "done"

