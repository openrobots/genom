/*
 * Copyright (c) 2012,2013 CNRS/LAAS
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genom/genom.h"

#define GENOM_MAX_INSTANCE_NAME 256

static char *genomInstance;

static int genomInstanceInitialized = 0;

const char *
genomInstanceName(const char *moduleName)
{
	char *instance;
	char *envvar;

	if (genomInstanceInitialized)
		return genomInstance;

	if (asprintf(&envvar, "GENOM_INSTANCE_%s", moduleName) == -1)
		goto oom;
	instance = getenv(envvar);
	free(envvar);
	if (instance == NULL) {
		genomInstance = strdup(moduleName);
		if (genomInstance == NULL) 
			goto oom;
	} else {
		if (asprintf(&genomInstance,"%s%s",
			moduleName, instance) == -1)
			goto oom;
	}
	genomInstanceInitialized++;
	return &genomInstance[0];
oom:
	fprintf(stderr, "genomInstanceName: out of memory\n");
	return NULL;
}

/* XXX mem leak */
const char *
genomInstanceSuffixName(const char *moduleName, const char *suffix)
{
	char *result;
	char *instance;
	char *envvar;

	if (asprintf(&envvar, "GENOM_INSTANCE_%s", moduleName) == -1)
		goto oom;
	instance = getenv(envvar);
	free(envvar);
	if (instance == NULL) {
		if (asprintf(&result, "%s%s", moduleName, suffix) == -1)
			goto oom;
	} else {
		if (asprintf(&result,"%s%s%s",
			moduleName, instance, suffix) == -1)
			goto oom;
	}
	return result;
oom:
	fprintf(stderr, "genomInstanceSuffixName: out of memory\n");
	return NULL;
}

