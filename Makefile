build:
	gcc -o ./bin/ano main.c -Wall

clean:
	rm -rf ./bin/ano

run:
	./bin/ano
