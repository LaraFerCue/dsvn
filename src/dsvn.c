/*-
 * Copyright (c) 2017 Roberto Fernandez Cueto
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the authors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.

 */

/* 
 * Subversion tool based on dialog(3) which allows the user to have a program
 * during the use of the console which can store credentials for later use.
 */

#include <stdio.h>
#include <dialog.h>
#include <strings.h>
#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#define	MAIN_PROMPT	\
	"Please enter the URL to connect with (empty to browse local)"
#define SVN_OPTS	\
	"--non-interactive --no-auth-cache --trust-server-cert"
#define SILENT_OPT	">/dev/null 2>/dev/null"

struct svn_option {
	char *	option;
	char *	value;
};

typedef int (* check_func)(struct svn_option**, int);

static int
run_svn_cmd(const char * cmd, int silent, int amount,  ...)
{
	struct svn_option * svn_optlist;
	int	cnt, length = sizeof("svn " SVN_OPTS) + strlen(cmd) + 1;
	va_list	ap;
	char	* buffer;

	svn_optlist = (struct svn_option *) malloc(
			amount * sizeof(struct svn_option));
	if (svn_optlist == NULL)
		return ENOMEM;

	va_start(ap, amount);
	for (cnt = 0; cnt < amount; cnt++) {
		svn_optlist[cnt].option = va_arg(ap, char *);
		svn_optlist[cnt].value = va_arg(ap, char*);
		length += strlen(svn_optlist[cnt].option) +
			strlen(svn_optlist[cnt].value) + 2;
	}
	va_end(ap);

	if (silent)
		length += sizeof(SILENT_OPT);
	
	/* Here would go the check of the list, none by now. */

	/* Allocate memory for the command. */
	buffer = malloc(length + 1);
	if (buffer == NULL)
		return ENOMEM;

	sprintf(buffer, "svn %s %s", cmd, SVN_OPTS);
	length = strlen(buffer);
	for (cnt = 0; cnt < amount; cnt++) {
		if (* svn_optlist[cnt].value == '\0')
			sprintf(buffer + length, " %s",
				svn_optlist[cnt].option);
		else
			sprintf(buffer + length, " %s \"%s\"",
				svn_optlist[cnt].option,
				svn_optlist[cnt].value);
		length = strlen(buffer);
	}
	if (silent)
		sprintf(buffer + length, SILENT_OPT);
	return system(buffer);
}

int
main(int argc, char** argv)
{
	char *	buffer;
	int ret = 0;

	buffer = malloc(MAX_LEN);
	if (buffer == NULL)
		err(ENOMEM, "Not enough memory for the buffer");

	bzero(buffer, MAX_LEN);
	init_dialog(stdin, stderr);
	dialog_vars.input_result = buffer;
	dialog_vars.backtitle = "Dialog SVN";
	dialog_inputbox("Welcome to dsvn", MAIN_PROMPT, 10, 72, "", 0);
	end_dialog();
	if ( buffer[0] != '\0')
		ret = run_svn_cmd("ls", 1, 3, "--username", "", "--password", 
				"", buffer, "");
	printf("ret = %d\n", ret);
	return 0;
}
