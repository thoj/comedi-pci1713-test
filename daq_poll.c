#include <stdio.h>
#include <comedilib.h>

int subdev = 0;
int chan = 0;
int range = 0;
int aref = AREF_GROUND;
unsigned int chanlist[16];

int
main ( int argc, char *argb[] ) {
	comedi_t *dev;
	comedi_cmd cmd;
	dev = comedi_open("/dev/comedi0");
	int i = 0;
	for ( i = 0; i < 16; i++ ) {
		chanlist[i] = CR_PACK(i, 0, 0);
	}
	prepare_cmd(dev, &cmd, 0);
	do_cmd(dev, &cmd);
}

void do_cmd(comedi_t *dev, comedi_cmd *cmd) {
	int total = 0;
	int ret = 0;
	int go = 0;
	fd_set rdset;
	struct timeval timeout;
	ret = comedi_command_test(dev, cmd);
	printf("test ret = %d\n", ret);
	if ( ret < 0 ) {
		printf("errorno = %d\n", errno);
		comedi_perror("comedi_command_test");
		return;
	}
	dump_cmd(stdout, cmd);
	go = 1;
	while(go) {
		FD_ZERO(&rdset);
		FD_SET(comedi_fileno(dev), &rdset);
		timeout.tv_sec = 0;
		timeout.tv_usec = 50000;
		ret = select(comedi_fileno(dev) + 1, &rdset, NULL, NULL, &timeout);
		if ( ret < 0 ) {
			perror("select");
		}
		else if ( ret == 0 ) {
			printf("Timeout, polling...\n");
			ret = comedi_poll(dev)
			ret = read(comedi_fileno(dev), &rdset
		}
	}
}

int 
prepare_cmd(comedi_t *dev, comedi_cmd *cmd, unsigned int subdev) {
	memset(cmd, 0, sizeof(*cmd));
	cmd->subdev = 0;
	cmd->flags = 0;
	cmd->start_src = TRIG_NOW;
	cmd->start_arg = 0;
	cmd->scan_begin_src = TRIG_TIMER;
	cmd->scan_begin_arg = msec_to_nsec(100);
	cmd->convert_src = TRIG_TIMER;
	cmd->convert_arg = msec_to_nsec(1);
	cmd->scan_end_src = TRIG_COUNT;
	cmd->scan_end_arg = 16;
	cmd->stop_src = TRIG_COUNT;
	cmd->stop_arg = 10;
	cmd->chanlist = chanlist;
	cmd->chanlist_len = 16;	
}
