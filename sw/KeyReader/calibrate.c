#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define NUM_KEYS 24

int main(int argc, char *argv[]) {
	bool useFile = false;
	int option;
	char *filename;

	// Print usage
	if (argc <= 1) {
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "\t%s -f <filename>", argv[0]);
		return 1;
	}

	// Read command line arguments
	while ((option = getopt(argc, argv, "f:")) != -1) {
		switch (option) {
		case 'f': {
			useFile = true;
			filename = strdup(optarg);
			fprintf(stderr, "Reading from file: %s\n", filename);
			break;
		}
		default: {
			fprintf(stderr, "CLI argument '%c' not supported\n", option);
			break;
		}
		}
	}

	if (!useFile) {
		fprintf(stderr, "Must specify a filename\n");
		return 1;
	}

	char hallValues[NUM_KEYS][20];

	// Convert the file to some good shit
	FILE *fp = fopen(filename, "r");
	char line[50] = {0};
	int n = 0;
	while (fgets(line, sizeof(line), fp)) {
    // Remove the newline character, if any
    line[strcspn(line, "\n")] = 0;  // Removing the newline
    char *keyName = strtok(line, " ");
		char *value = strtok(NULL, "\n");
		strcpy(hallValues[n], value);
		n++;
  }

	// Print out all values
	printf("int values[] = {");
	for (int i = 0; i < n; i++) {
		printf("\n\t%s,", hallValues[i]);
	}
	printf("\n};\n");

	return 0;
}
