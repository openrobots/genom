/* 
 * Copyright (c) 1994-2010 LAAS/CNRS
 * Arnaud Degroote - 10/08/2010
 * All rights reserved.
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
/***
 *** Scan des types de base utilise's par les modules generator
 ***/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "genom/printScan.h"

static char buffer[256];
static int scan_buf_string_len(char**, char *, size_t , int , int *);  

static int
scan_buf_type(char** buf, char *format, void *ptr)
{
	char* p = *buf;
	char* begin = *buf;
	size_t len;

	// consume white space
	if (*p == ' ') {
		while (*p == ' ') p++;
	}

	while (*p != ' ' && *p != 0) p++;
	len = p - begin;
	if (len > sizeof(buf))
		return -1;

	memcpy(buffer, begin, len);
	buffer[len] = 0;

	if (sscanf(buffer, format, ptr) != 1)
		return -1;

	// if it is not the end, skip the expected next whitespace
	if (*p == 0)
		*buf = p;
	else 
		*buf = ++p;

	return 0;
}

/** 
 ** Les fonctions suivantes attendent un pointeur sur une variable
 ** OU sur un tableau de variables.
 **    x:      le pointeur
 **    indent: niveau d'indentation (pour l'affichage)
 **    nDim:   nombre de dimensions du tableau (ou 0)
 **    dims:   tableau de nDim "int": taille de chaque dimension (ou NULL)
 **
 ** Remarque: 
 ** Dans le doute, les variables attendues devraient être de type (void *).
 ** Cependant, c'est sympa d'avoir un type pas tout a fait anonyme pour 
 ** verifier un minimum de cohérence.
 ** Le paramètre est donc un pointeur sur le type considéré (et les tableaux 
 ** doivent être castés).
 **/

int 
scan_buf_char(char** buf, char *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%c", (x+elt))) != 0) 
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_unsigned_char(char** buf, unsigned char *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%c", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_short_int(char** buf, short int *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%hd", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_int(char** buf, int *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%d", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_long_int(char** buf, long int *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%d", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_long_long_int(char** buf, long long int *x,  int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%lld", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_unsigned_short_int(char** buf, unsigned short *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%u", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_unsigned_int(char** buf, unsigned int *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%u", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_unsigned_long_int(char** buf, unsigned long *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%u", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_unsigned_long_long_int(char** buf, unsigned long long *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%llu", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}
int 
scan_buf_float(char** buf, float *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%f", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}

int 
scan_buf_double(char** buf, double *x, int nDim, int *dims) 
{
  int status = 0;
  FOR_EACH_elt(nDim,dims) {
    if ((status = scan_buf_type(buf, "%lf", (x+elt))) != 0)
      return status;
  } END_FOR
  return status;
}


int 
scan_buf_string(char** buf, char *x, int nDim, int *dims)
{
	if(nDim<1) {
		fprintf (stderr, "Null length string !\n");
		return -1;
	}
  
  return scan_buf_string_len(buf, x, dims[nDim-1], nDim-1, dims);

}

/* 
 * max_str_len: taille max pour la chaine (y compris le \0).
 */

int 
scan_buf_string_len(char** buf, char *x, size_t max_str_len, int nDim, int *dims)  
{
  int size;
  FOR_EACH_elt(nDim,dims) {

	  char *p, *begin;
	  size_t len;
	  begin = *buf;
	  p = *buf;
	  while (*p != '0' && *p != ' ') 
		  p++;

	  len = p - begin;
	  if (len > max_str_len) {
		  fprintf(stderr, "Sorry, string too long\n");
		  return -1;
	  }

	  strncpy(x+elt*max_str_len, begin, len);
	  (x+elt*max_str_len)[len] = '\0';

	  if (*p == '0')
		  *buf = p;
	  else
		  *buf = ++p;
  } END_FOR

  return 0;
}


