// C program to illustrate fopen()

#include <stdio.h>

int main()
{

	// pointer demo to FILE
	FILE* demo;
	int display;

	// Creates a file "demo_file"
	// with file access as read mode
	demo = fopen("flashdata.bin", "rb");

	// loop to extract every characters
	while (1) {
		// reading file
		display = fgetc(demo);

		// end of file indicator
		if (feof(demo))
			break;

		// displaying every characters
        printf("%X", display);
        printf("\n"); 

	}

	// closes the file pointed by demo
	fclose(demo);

	return 0;
}
