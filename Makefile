all: maestro-apply maestro-translate

maestro-apply: src/maestro-apply.c
	$(CC) $< -o $@

maestro-translate: src/maestro-translate.c
	$(CC) $< -o $@

clean:
	rm -f *.o maestro-apply maestro-translate

.PHONY: all clean
