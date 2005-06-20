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

#include "genom.h"

char *
get_pkgconfig_cflags(char *package)
{
    char* cflags = 0;
    int outpipe[2];
    if (pipe(outpipe) == -1)
    {
	fprintf(stderr, "error running pkg-config: ");
	perror("");
	exit(2);
    }

    pid_t pid = fork();
    if (! pid)
    {
	close(outpipe[0]);
	close(fileno(stdout));
	dup2(outpipe[1], fileno(stdout));

	if (execlp("pkg-config", "pkg-config", "--cflags", package, (char*)NULL) == -1)
	{
	    fprintf(stderr, "error running pkg-config: ");
	    perror("");
	    exit(1);
	}
	// never reached
	return 0;
    }
    else
    {
	char buffer[1024];
	char* eol;
	int status;

	close(outpipe[1]);

	if (waitpid(pid, &status, 0) == -1 || !WIFEXITED(status) || WEXITSTATUS(status)) 
	{
	    fprintf(stderr, "error running pkg-config\n");
	    exit(1);
	}

	while (read(outpipe[0], buffer, 1024) > 0)
	    bufcat(&cflags, buffer);

	/* Remove \n and spaces at end of line*/
	if (cflags)
	{
	    char* end = cflags + strlen(buffer) - 1;
	    while (*end == '\n' || *end == ' ')
		--end;
	    *(++end) = 0;
	}

	if (verbose)
	{
	    if (cflags)
		fprintf(stderr, "pkg-config: found '%s' for package %s", cflags, package);
	    else
		fprintf(stderr, "pkg-config: no cflags needed for package %s", package);
	}
	return cflags;
    }
}

/**
 ** Appel du preprocesseur C 
 **/

#ifndef STDINCPP
# define STDINCPP "gcc -E -"
#endif /* STDINCPP */

char *
callCpp(char *nomFic, char *cppOptions[])
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
	char** display;
	if (verbose)
	    fputs("genom: running cpp with options\n", stderr);

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
	if (verbose)
	{
	    for (display = cppArg + 1; *display; ++display)
		fprintf(stderr, "genom:   %s\n", *display);
	}

	execvp(cppArg[0], cppArg);
    }
    free(cppArg[0]);
    wait(&status);
    if (! WIFEXITED(status) || WEXITSTATUS(status) != 0) {
	    fprintf(stderr, "genom: cpp failure\n");
	    exit(2);
    }
    return(tmpName);
} /* callCpp */

