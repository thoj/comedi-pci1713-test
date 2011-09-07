#include <stdio.h>
#include <comedilib.h>

int subdev = 0;
int chan = 0;
int range = 0;
int aref = AREF_GROUND;

int
main ( int argc, char *argb[] ) {
	comedi_t *it;
	lsampl_t data;
	it = comedi_open("/dev/comedi0");
	int i = 0;
	for ( i = 0; i < 16; i++ ) {
		comedi_data_read(it, subdev, i, range, aref, &data);
		printf("%d\n", data);
	}
}

