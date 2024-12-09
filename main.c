// Copyright 2024 Vahid Mardani
/*
 * This file is part of serialhub.
 *  serialhub is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  serialhub is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with serialhub. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
#include <stdlib.h>

#include <clog.h>
#include <yacap.h>


static int
_main(const struct yacap *c, const struct yacap_command *cmd) {
    yacap_help_print(c);
    return 0;
}


/* Root yacap structure */
static struct yacap cli = {
    .commands = (const struct yacap_command*[]) {
        NULL
    },
    .entrypoint = _main,
};


int
main(int argc, const char *argv[]) {
    int ret = EXIT_FAILURE;
    const struct yacap_command *cmd;
    enum yacap_status status = yacap_parse(&cli, argc, argv, &cmd);

    if (status == YACAP_OK_EXIT) {
        ret = EXIT_SUCCESS;
        goto terminate;
    }

    if ((status == YACAP_OK) && cmd) {
        if (cmd->entrypoint == NULL) {
            goto terminate;
        }
        ret = cmd->entrypoint(&cli, cmd);
    }

terminate:
    yacap_dispose(&cli);
    return ret;
}
