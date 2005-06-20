#ifndef GENOM_EXTERN_PROCESSES_H
#define GENOM_EXTERN_PROCESSES_H

/* Returns the cflags needed by package, by calling pkg-config --cflags
 * or 0 if --cflags is not found */
int
get_pkgconfig_cflags(char *package, char** cpp_options, int first_option);

#define MAX_CPP_OPT 20

char *
callCpp(char *nomFic, char *cppOptions[]);

#endif

