main:
	make -f Makefile.main

Encode_Tests:
	make -f Makefile.Encode_Tests


Performance_Tests:
	make -f Makefile.Performance_Tests

all:
	make -f Makefile.main
	make -f Makefile.Encode_Tests
	make -f Makefile.Performance_Tests

runTests:
	./Encode_Tests
	./Performance_Tests $(ARGS)

clean:
	rm -f main
	rm -f Encode_Tests
	rm -f Performance_Tests
