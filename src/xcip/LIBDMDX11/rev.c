#include <stdio.h>
main()
{
	int x;
	int b;
	int y;
	
	for (x = 0; x < 256; x++) {
		y = 0;
		for (b = 0; b < 8; b++) {
			if (x & (1<<b)) y += 1<<(7-b);
		}
		if ((x&7)==0) printf ("\n\t");
		printf ("0x%02x, ",y);
	}
	printf ("\n");
}


