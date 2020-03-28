#include <stdlib.h>
#include <argp.h>

#include "common.h"
#include "cli.h"

#define ARG_TCPBACKLOG  0x1
#define ARG_UNIXBACKLOG 0x2


const char *argp_program_version = "1.0a";
const char *argp_program_bug_address = "http://github.com/pylover/serialhub";
static char doc[] = "Serial Interface multiplexer";
static char args_doc[] = "DEVICE";

/* Options definition */
static struct argp_option options[] = {
	{"baudrate", 'b', "BAUDRATE", 0, 
        "Baudrate, default: " STR(DEFAULT_BAUDRATE)},
	{"tcpport", 'p', "PORT", 0, 
        "Listen port. default: " STR(DEFAULT_TCPPORT)},
	{"tcpbacklog", ARG_TCPBACKLOG, "TCP_BACKLOG", 0, 
		"TCP backlog, default: " STR(DEFAULT_TCPBACKLOG) ", see listen(2)"},
	{"tcpbind", 't', "ADDRESS", 0, 
        "Listen address for TCP. default: " DEFAULT_TCPBIND},
	{"unixbacklog", ARG_UNIXBACKLOG, "UNIX_BACKLOG", 0, 
		"Unix socket  backlog, default: " STR(DEFAULT_UNIXBACKLOG) ", see listen(2)"},
    {"unixfile", 'u', "UNIXFILE", 0, 
        "Unix domain socket file to listen, default: " DEFAULT_UNIXFILE},
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

		case 'u':
			settings.unixfile = arg;
			break;
	
		case ARG_TCPBACKLOG:
			settings.tcpbacklog = atoi(arg);
			break;
		
		case ARG_UNIXBACKLOG:
			settings.unixbacklog = atoi(arg);
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
        perrorf("Cannot parse arguments");
        exit(EXIT_FAILURE);
	}

	if (settings.device == NULL) {
        perrorf("Please provide device:");
        argp_help(&argp, stderr, ARGP_HELP_USAGE, argv[0]);
        exit(EXIT_FAILURE);
	}
}

