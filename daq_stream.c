#include <comedilib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  comedi_t *device;
  comedi_cmd cmd;
  unsigned int chans[16];
  int i = 0;
  for (i = 0; i < 16; i++) {
    chans[i] = CR_PACK (i * 2, 5, AREF_DIFF);
  }

  memset (&cmd, 0, sizeof (cmd));
  cmd.subdev = 0;
  cmd.flags = 0;		//TRIG_WAKE_EOS;
  cmd.start_src = TRIG_NOW;
  cmd.start_arg = 0;
  cmd.scan_begin_src = TRIG_FOLLOW;
  cmd.scan_begin_arg = 0;
  cmd.convert_src = TRIG_TIMER;
  cmd.convert_arg = 100000;	//1000000/16;
  cmd.scan_end_src = TRIG_COUNT;
  cmd.scan_end_arg = 16;
  cmd.stop_src = TRIG_NONE;
  cmd.stop_arg = 0;
  cmd.chanlist = &chans[0];
  cmd.chanlist_len = 16;
  cmd.data = NULL;
  cmd.data_len = 0;

  if ((device = comedi_open ("/dev/comedi0")) < 0) {
    comedi_perror ("comedi_open");
    exit (1);
  }

  unsigned int test = comedi_command_test (device, &cmd);
  printf ("Test: %d\n");
  comedi_perror ("comedi_open");
  switch (test) {
  case 0:
    printf ("Valid command.\n");
    break;
  case 1:
    printf ("Command contained an unsupported trigger.\n");
    break;
  case 2:
    printf ("Settings is not supported by the driver.\n");
    break;
  case 3:
    printf
      ("One of the members of the command is set outside the range of allowable values.\n");
    break;
  case 4:
    printf ("One of the members required adjustment.\n");
    break;
  case 5:
    printf
      ("Some aspect of the command's chanlist is unsupported by the board.\n");
    break;
  default:
    printf ("Invalid command.\n");
    exit (1);
    break;
  }

  fcntl (comedi_fileno (device), F_SETFL, O_NONBLOCK);

  fd_set rdset;
  struct timeval timeout;
  unsigned int total = 0;
  int ret;
  int go = 1;
  sampl_t buf[10000];
  if (comedi_command (device, &cmd) < 0) {
    comedi_perror ("comedi_command");
    exit (1);
  }
  struct timeval time_last_write;
  struct timeval time_current;

  double t1, t2;
  unsigned int last_total = 0;
  int channel = 0;
  int samplecount[16];
  int samples[16];
  int avg_count = 100;

  while (go) {
    FD_ZERO (&rdset);
    FD_SET (comedi_fileno (device), &rdset);
    timeout.tv_sec = 0;
    timeout.tv_usec = 1e2;
    ret = select (comedi_fileno (device) + 1, &rdset, NULL, NULL, &timeout);
    //printf ("select returned %d\n", ret);
    if (ret < 0) {
      perror ("select");
    }
    else if (ret == 0) {
    }
    else if (FD_ISSET (comedi_fileno (device), &rdset)) {
      ret = read (comedi_fileno (device), buf, sizeof (buf));
      if (ret < 0) {
	go = 0;
	perror ("read");
      }
      else if (ret == 0) {
	go = 0;
      }
      else {
	int i;
	total += ret;
	for (i = 0; i < ret / sizeof (sampl_t); i++) {

	  samples[channel] += buf[i];
	  if (samplecount[channel] == avg_count) {
	    printf ("%2d: %.4f ", channel, ((((double) samples[channel] / avg_count) - 0.0) * (10.0 - 0.0)) / (4095.0 - 0.0) + 0.0);
	    samplecount[channel] = 0;
	    samples[channel] = 0;
	    if (channel == 15) {
	      printf ("\n");
	    }
	  }
	  channel++;
	  if (channel == 16) {
	    channel = 0;
	  }
	  samplecount[channel]++;
	}
	gettimeofday (&time_current, NULL);
	t1 = time_current.tv_sec + (time_current.tv_usec / 1000000.0);
	if (t1 - t2 > 1) {
	  if (last_total > 0) {
//	    printf ("%g samples/sec (Interval: %g)\n",
//		    (total - last_total) / 2 / (t1 - t2), t1 - t2);
	  }
	  last_total = total;
	  gettimeofday (&time_last_write, NULL);
	  t2 = time_last_write.tv_sec + (time_last_write.tv_usec / 1000000.0);
	}
      }
    }
    else {
      /* unknown file descriptor became ready */
      printf ("unknown file descriptor ready\n");
    }
  }
  return 0;
}
