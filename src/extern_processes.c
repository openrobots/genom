#include "extern_processes.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

#include "genom.h"

/** Removes all whitespaces at the end of \c string */
static char*
string_strip_end(char* string)
{
    char* end = string + strlen(string) - 1;
    while (isspace((unsigned char)*end))
	--end;
    *(++end) = 0;
    return string;
}

/** Splits the string at \c delim
 * If lookahead is non-null, we consider that 'lookahead' characters of delim
 * are part of the next field
 *
 * @param max_fields \c fields cannot handle more than \c max_fields
 * @return the count of options found, or -1 if there were too much
 */
static int
split(char* string, char* delim, int lookahead, char** fields, int max_fields)
{
    int delim_length = strlen(delim);
    char* tempstring = strdup(string);
    int last_option = 0;

    char* field_begin = tempstring;
    char* field_end = 0;
    while( (field_end = strstr(field_begin, delim)) )
    {
	*field_end = 0;
	fields[last_option++] = string_strip_end( strdup(field_begin) );
	field_begin = field_end + delim_length - lookahead;

	if (last_option == max_fields)
	{
	    int i = 0;
	    for (i = 0; i < last_option; ++i)
		free(fields[i]);
	    return -1;
	}
    }
    fields[last_option++] = string_strip_end(strdup(field_begin));

    free(tempstring);
    return last_option;
}

int
get_pkgconfig_cflags(char *package, char** cpp_options, int first_option)
{
    const char *pkgconfig = getenv("PKG_CONFIG");
    int outpipe[2];
    pid_t pid;

    if (pkgconfig == NULL) pkgconfig = "pkg-config";

    if (pipe(outpipe) == -1)
    {
        fprintf(stderr, "error running %s: ", pkgconfig);
	perror("");
	exit(2);
    }

    pid = fork();
    if (! pid)
    {
	close(outpipe[0]);
	close(fileno(stdout));
	dup2(outpipe[1], fileno(stdout));

	if (execlp(pkgconfig, pkgconfig, "--cflags", package, (char*)NULL) == -1)
	{
	    fprintf(stderr, "error running %s: ", pkgconfig);
	    perror("");
	    exit(1);
	}
	// never reached
	return -1;
    }
    else
    {
	char* cflags = 0;
	int option_count = 0;
	char buffer[1024];
	int read_chars;
	int status;

	close(outpipe[1]);

	if (waitpid(pid, &status, 0) == -1 || !WIFEXITED(status) || WEXITSTATUS(status))
	{
            fprintf(stderr, "error running %s\n", pkgconfig);
	    exit(1);
	}

	while ( (read_chars = read(outpipe[0], buffer, 1023)) > 0)
	{
	    buffer[read_chars] = 0;
	    string_strip_end(buffer); /* remove whitespace at the end */
	    bufcat(&cflags, buffer);
	}

	/* Split cflags at \s+- and add the fields in cpp_options */
	if (!cflags || cflags[0] == '\0')
	    return 0;
	/* XXX pkgconfig < 0.15 leaves a space in front of cflags
	 * this causes problems here. This is why pkg-config >= 0.15
	 * is required */

	option_count = split(cflags, " -", 1, cpp_options + first_option, MAX_CPP_OPT - first_option);
	if (option_count == -1)
	{
	    fprintf(stderr, "too many options for the preprocessor");
	    exit(1);
	}
	free(cflags);
	cflags = 0;

	if (verbose)
	{
	    int i;
	    if (option_count > 0)
		    for (i = 0; i < option_count; ++i) {
			    fprintf(stderr, "%s: found '%s' for package %s\n",
				    pkgconfig, cpp_options[first_option + i],
				    package);
		    }
	    else
		fprintf(stderr, "%s: no cflags needed for package %s\n",
			pkgconfig, package);
	}
	return option_count;
    }
}

/**
 ** Appel du preprocesseur C
 **/

#ifndef STDINCPP
# define STDINCPP "gcc -E -"
#endif /* STDINCPP */

char *
callCpp(char *nomFic, char *cppOptions[], int ignore_error)
{
    char *tmpName;
    char *cppArg[MAX_CPP_OPT + 3], *cpp;
    int in, out, i, j, status;

    /* open input and output files */
    in = open(nomFic, O_RDONLY, 0);
    if (in < 0) {
       fprintf(stderr, "cannot open %s for reading: ", nomFic);
       perror("");
       exit(2);
    }

    tmpName = strdup("/tmp/genomXXXXXX");
    /* use a safe temporary file */
    out = mkstemp(tmpName);
    if (out < 0) {
       fprintf(stderr, "genom: cannot open %s for writing: ", tmpName);
       perror("");
       exit(2);
    }

    /* build the argv array: split cpp into argvs and copy options */
    cpp = strdup(STDINCPP);
    if (!cpp) {
       perror("genom: cannot exec cpp");
       exit(2);
    }

    for(i=0; *cpp != '\0'; i++) {
       if (i > MAX_CPP_OPT) {
	  fputs("too many options to cpp\n", stderr);
	  free(cppArg[0]);
	  exit(2);
       }

       cppArg[i] = cpp;
       cpp += strcspn(cpp, " \t");
       if (*cpp != '\0') {
	  *cpp = '\0';
	  cpp++;
	  cpp += strspn(cpp, " \t");
       }
    }

    for(j=0; cppOptions[j] != NULL;) {
       if (i > MAX_CPP_OPT) {
	  fputs("too many options to cpp\n", stderr);
	  free(cppArg[0]);
	  exit(2);
       }

       cppArg[i++] = cppOptions[j++];
    }

    /* cpp output goes to stdout */
    cppArg[i] = NULL;

    if (fork() == 0) {

	/* read stdin from nomFic */
	if (dup2(in, fileno(stdin)) < 0) {
	    fprintf(stderr, "genom: cannot redirect cpp stdin: %s\n",
		    strerror(errno));
	    exit(2);
	}

	/* redirect stdout to tmpName */
	if (dup2(out, fileno(stdout)) < 0) {
	    fprintf(stderr, "genom: cannot redirect cpp output: %s\n",
		    strerror(errno));
	    exit(2);
	}

        if (ignore_error) {
	  /* Open /dev/null to redirect stderr. Simply closing stderr
	   * make gcc-4.1.2 segfault on FC6... */
	  int null = open("/dev/null", O_RDWR);
	  if (null >= 0)
            dup2(null, fileno(stderr));
	  else
	    fprintf(stderr, "genom: cannot suppress error diagnostics: %s\n",
		    strerror(errno));
	}

	if (verbose) {
	    char** display;
	    fprintf(stderr, "genom: running cpp command: '%s'", cppArg[0]);
	    for (display = cppArg + 1; *display; ++display)
		fprintf(stderr, " '%s'", *display);
	    fputs("\n", stderr);
	}

	execvp(cppArg[0], cppArg);
    }
    free(cppArg[0]);
    wait(&status);
    if ( (!ignore_error) && (! WIFEXITED(status) || WEXITSTATUS(status) != 0) ) {
	    fprintf(stderr, "genom: cpp failure\n");
	    exit(2);
    }
    return(tmpName);
} /* callCpp */

