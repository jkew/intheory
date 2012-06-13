intheory: src/main/c/intheory.c
	mkdir -p target
	gcc -o target/it src/main/c/intheory.c -Isrc/main/c

clean:
	rm -Rf target