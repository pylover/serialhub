#include <stdlib.h>
#include <argp.h>

#include "logging.h"
#include "settings.h"
#include "common.h"
#include "cli.h"

#define ARG_BACKLOG 0x1
#define ARG_EPOLL_MAXEVENTS 0x2


const char *argp_program_version = "1.0a";
const char *argp_program_bug_address = "http://github.com/pylover/serialhub";
static char doc[] = "Serial Interface multiplexer";
static char args_doc[] = "DEVICE";

/* Options definition */
static struct argp_option options[] = {
	{"port", 'p', "PORT", 0, 
        "Listen port. default: " STR(DEFAULT_TCPPORT)},
	{"baudrate", 'b', "BAUDRATE", 0, 
        "Baudrate, default: " STR(DEFAULT_BAUDRATE)},
	{"backlog", ARG_BACKLOG, "TCP_BACKLOG", 0, 
		"TCP backlog, default: " STR(DEFAULT_BACKLOG) ", see listen(2)"},
	{"tcpbind", 't', "ADDRESS", 0, 
        "Listen address for TCP. default: " DEFAULT_BIND},
	{0}
};


/* Parse a single option. */
static int parse_opt(int key, char *arg, struct argp_state *state) {
	switch (key) {
		case 'p':
			settings.tcpport = atoi(arg);
			break;

		case 'b':
			settings.baudrate = atoi(arg);
			break;

		case 't':
			settings.tcpbind = arg;
			break;
	
		case ARG_BACKLOG:
			settings.tcpbacklog = atoi(arg);
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

static struct argp argp = {options, parse_opt, args_doc, doc};


void cliparse(int argc, char **argv) {
	int err = argp_parse(&argp, argc, argv, 0, 0, 0);
	if (err) {
        exit(FAILURE_CLIPARSE);
	}

	if (settings.device == NULL) {
        L_ERROR("Please provide device:");
        argp_help(&argp, stderr, ARGP_HELP_USAGE, argv[0]);
        exit(FAILURE_CLIPARSE);
	}
}

