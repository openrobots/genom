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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "genom/genom.h"

#define GENOM_MAX_INSTANCE_NAME 256

struct INSTANCE_CACHE {
	const char *module;
	const char *instance;
};

static pthread_mutex_t icMutex = PTHREAD_MUTEX_INITIALIZER;
static struct INSTANCE_CACHE *ic = NULL;
static int icSize = 0; 

static const char *
add_ic(const char *module, const char *instance)
{
	struct INSTANCE_CACHE *new; 
	const char *ret;
	char *buf;

	new = (struct INSTANCE_CACHE *)realloc(ic, 
	    (icSize + 1)*sizeof(struct INSTANCE_CACHE));
	if (new == NULL) {
		return NULL;
	}
	new[icSize].module = strdup(module);
	if (instance == NULL) 
		ret = new[icSize].module;
	else {
		if (asprintf(&buf, "%s%s", module, instance) == -1) {
			free(new);
			return NULL;
		}
		ret = buf;
	}
	new[icSize].instance = ret;
	ic = new;
	icSize++;
	return ret;
}

static const char *
find_ic(const char *module)
{
	int i;

	for (i = 0; i < icSize; i++)
		if (strcmp(ic[i].module, module) == 0) 
			break;
	if (i == icSize)
		return NULL;
	return ic[i].instance;
}


const char *
genomInstanceName(const char *moduleName)
{
	const char *instance, *genomInstance, *ret;;
	char *envvar;

	pthread_mutex_lock(&icMutex);
	genomInstance = find_ic(moduleName);
	if (genomInstance != NULL) {
		ret = genomInstance;
		goto done;
	}
	if (asprintf(&envvar, "GENOM_INSTANCE_%s", moduleName) == -1) {
		ret = NULL;
		goto done;
	}
	instance = getenv(envvar);
	free(envvar);
	ret = add_ic(moduleName, instance);
done:
	pthread_mutex_unlock(&icMutex);
	return ret;
}

/* XXX mem leak, should use a cache like above */
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

