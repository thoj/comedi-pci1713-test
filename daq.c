#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <comedilib.h>

comedi_t *device;


int main(int argc, char *argv[])
{
	int n_chans,chan;
	int n_ranges;
	int range;
	int maxdata;
	lsampl_t data;
	double voltage;
	device = comedi_open("/dev/comedi0");
	int subdevice = 0;
	if(!device){
		comedi_perror("/dev/comedi0");
		exit(-1);
	}

		subdevice = comedi_find_subdevice_by_type(device, COMEDI_SUBD_AI, 0);
		if(subdevice<0){
			printf("no analog input subdevice found\n");
			exit(-1);
		}
	n_ranges = comedi_get_n_ranges(device, subdevice, 0);
	for(range = 0; range < n_ranges; range++){
		comedi_range *r = comedi_get_range(device, subdevice, 0, range);
		printf("[%g,%g] ", r->min, r->max);
	}
	printf("\n");
	n_chans = comedi_get_n_channels(device, subdevice);
	for(chan = 0; chan < n_chans; chan++){
		printf("%2d: ", chan);


		maxdata = comedi_get_maxdata(device, subdevice, chan);
//		for(range = 0; range < n_ranges; range++){
			comedi_data_read(device, subdevice, chan, 5, AREF_DIFF, &data);
			voltage = comedi_to_phys(data, comedi_get_range(device, subdevice, chan, 5), maxdata);
			printf("%12g ", voltage);
//		}
		printf("[%d] \n", data);
	}
	return 0;
}


