#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <comedilib.h>

comedi_t *device;


int
main (int argc, char *argv[])
{
  int n_chans, chan;
  int n_ranges;
  int range;
  int maxdata;
  lsampl_t data;
  double data_real;
  double voltage;
  device = comedi_open ("/dev/comedi0");
  int subdevice = 0;
  if (!device) {
    comedi_perror ("/dev/comedi0");
    exit (-1);
  }
  comedi_sv_t sv;

  subdevice = comedi_find_subdevice_by_type (device, COMEDI_SUBD_AI, 0);
  if (subdevice < 0) {
    printf ("no analog input subdevice found\n");
    exit (-1);
  }
  n_chans = comedi_get_n_channels (device, subdevice);
  maxdata = comedi_get_maxdata (device, subdevice, 0);
  int i = 0;
  int x = 0;
  for (i = 0; i < 50; i++) {
    voltage = 0;
    for (x = 0; x < 1000; x++) {
      comedi_data_read (device, subdevice, 0, 5, AREF_DIFF, &data);
      voltage +=
	comedi_to_phys (data, comedi_get_range (device, subdevice, 0, 5),
			maxdata);
    }
    data_real = voltage / 1000;
    printf ("%g\r\n", data_real);
  }
  return 0;
}
