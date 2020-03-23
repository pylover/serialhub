
#include "common.h"
#include "logging.h"
#include "tty.h"

#include <stdio.h>
#include <argp.h>


#define DEFAULT_DEVICE  "/dev/ttyUSB0"

#define ARG_BACKLOG 0x1
#define ARG_EPOLL_MAXEVENTS 0x2


volatile struct Settings settings = {
	NULL, 
	LOG_INFO,
    NULL,
	5671, 
	2,
	10,
};


const char *argp_program_version = "1.0a";
const char *argp_program_bug_address = "http://github.com/pylover/serialhub";
static char doc[] = "serialhub -- Serial Interface multiplexer";
static char args_doc[] = "[DEVICE]";

/* Options definition */
static struct argp_option options[] = {
	{"verbosity", 'v', "LEVEL", 0, "Verbosity level: 0-3. default: 2"},
	{"port", 'p', "PORT", 0, "Listen port. default: 5671"},
	{"backlog", ARG_BACKLOG, "TCP_BACKLOG", 0, 
		"TCP backlog, default: 10, see listen(2)."},
	{"bind", 'b', "ADDRESS", 0, "Listen address. default: 0.0.0.0"},
	{"epoll-maxevents", ARG_EPOLL_MAXEVENTS, "NUM", 0, 
		"Maximum events per event loop cycle, default: 10, see epoll_wait(2)"},
	{0}
};


/* Parse a single option. */
static int parse_opt(int key, char *arg, struct argp_state *state) {
	switch (key) {
		case 'v':
			settings.verbosity = atoi(arg);
			break;
		
		case 'p':
			settings.tcpport = atoi(arg);
			break;

		case 'b':
			settings.bind = arg;
			break;
	
		case ARG_BACKLOG:
			settings.tcpbacklog = atoi(arg);
			break;
		
		case ARG_EPOLL_MAXEVENTS:
			settings.epollmaxevents = atoi(arg);
			break;

		case ARGP_KEY_ARG:
			if (state->arg_num >= 1) {
				/* Too many arguments. */
		        argp_usage(state);
			    return ARGP_ERR_UNKNOWN;
			}
			settings.device = arg;
			break;
  
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return OK;
}

//static struct argp argp = {options, parse_opt, args_doc, doc};
static struct argp argp = {options, parse_opt, args_doc, doc};


void settingsparse(int argc, char **argv) {
	int err = argp_parse(&argp, argc, argv, 0, 0, 0);
	if (err) {
        exit(FAILURE_SETTINGS);
	}

	if (settings.device == NULL) {
		settings.device = DEFAULT_DEVICE;
	}
}


int main(int argc, char **argv) {
    //printf("%d, %s\n", argc, argv[0]);
    settingsparse(argc, argv);
	logginginit(settings.verbosity);

    printf("Hello\r\n");
    return 0;
}
