intheory: src/main/c/intheory.c src/main/c/network.c
	mkdir -p target
	gcc -c -fPIC -Wall -g -o target/intheory.o src/main/c/intheory.c -Isrc/main/c/include
	gcc -c -fPIC -Wall -g -o target/network.o src/main/c/network.c -Isrc/main/c/include
	ar rcs target/libintheory.a target/*.o
	gcc -shared -Wl,--allow-multiple-definition,-soname,libintheory.so.1 -o target/libintheory.so.1.0.1 target/*.o -lc

test: clean intheory
	gcc -Wl,--allow-multiple-definition -o target/simulator src/test/c/simulator.c -L target -lintheory -Isrc/main/c/include    
	target/simulator

clean:
	rm -Rf target