
/* 
 * Copyright (c) 1994-2003,2012 LAAS/CNRS
 * Matthieu Herrb - Sat Oct  1 1994
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
#include "genom-config.h"

/***
 *** Scan des types de base utilise's par les modules generator
 ***/

#include <portLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "genom/printScan.h"

static char buf[256];

/**
 ** Scan du type spe'cifie' par "format" dans ptr.
 ** un '.' en de'but de ligne indique un abort.
 **
 ** Attention: o cette fonction ne permet pas de taper des trucs a l'avance
 **            o tjs l'appeler avec un seul element a lire
 **/

int
scan_type(FILE *in, FILE *out, char *format, void *ptr)
{
  int ok = 1;

  do {
    if (ok == 0)
      fprintf(out, "Try again: ");

    if (fgets(buf, sizeof(buf), in) == NULL)
      return ERROR;

    if (strlen(buf) > 0 && buf[0] == '\n') 
      return OK;

    if (strlen(buf) > 0 && buf[0] == '.' && buf[1] == '\n')
      return ABORT;

    ok = 0;
  } while (sscanf(buf, format, ptr) != 1);
  
  return OK;
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

int scan_char(FILE *in, FILE *out, char *x, 
	      int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%c) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%c", (x+elt))) != OK) 
      return status;
  } END_FOR
  return status;
}

int scan_unsigned_char(FILE *in, FILE *out, unsigned char *x, 
		       int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%c) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%c", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_short_int(FILE *in, FILE *out, short int *x,
		   int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%d) ", 
	    indentStr_r(indent-1,buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%hd", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_int(FILE *in, FILE *out, int *x, 
	     int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%d) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%d", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_long_int(FILE *in, FILE *out, long int *x, 
		  int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%ld) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%d", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_long_long_int(FILE *in, FILE *out, long long int *x, 
		  int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%lld) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%lld", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_unsigned_short_int(FILE *in, FILE *out, unsigned short *x, 
			    int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%hu) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%u", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_unsigned_int(FILE *in, FILE *out, unsigned int *x, 
		      int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%u) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%u", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_unsigned_long_int(FILE *in, FILE *out, unsigned long *x, 
			   int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%lu) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%u", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_unsigned_long_long_int(FILE *in, FILE *out, unsigned long long *x, 
			   int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%llu) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%llu", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}
int scan_float(FILE *in, FILE *out, float *x, 
	       int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%f) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%f", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}

int scan_double(FILE *in, FILE *out, double *x, 
		int indent, int nDim, int *dims) 
{
  char buf1[80], buf2[80];
  int status=OK;
  indent++;
  FOR_EACH_elt(nDim,dims) {
    fprintf(out, "%s%s (%f) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2), *(x+elt));
    if ((status = scan_type(in, out, "%lf", (x+elt))) != OK)
      return status;
  } END_FOR
  return status;
}


int scan_string(FILE *in, FILE *out, char *x, int indent, int nDim, int *dims)
{
  if(nDim<1) {
    fprintf (out, "Null length string !\n");
    return ERROR;
  }
  
  return scan_string_len(in, out, x, dims[nDim-1], indent, nDim-1, dims);

}

/* 
 * max_str_len: taille max pour la chaine (y compris le \0).
 */

int scan_string_len(FILE *in, FILE *out, char *x, int max_str_len, 
		    int indent, int nDim, int *dims)  
{
  char buf1[80], buf2[80];
  int size;

  indent++;
  FOR_EACH_elt(nDim,dims) {
    
    fprintf(out, "%s%s (%s) ", 
	    indentStr_r(indent-1, buf1),
            getIndexesStr_r(nDim, dims, elt, buf2),
	    (x+elt*max_str_len));

    do {

      if (fgets(buf, sizeof(buf), in) == NULL) 
	return ERROR;
      
      if (strlen(buf) > 0 && buf[0] == '\n')
	break;

      if (strlen(buf) > 0 && buf[0] == '.' && buf[1] == '\n')
	return ABORT;

      if ((size = strlen(buf)) > max_str_len)
	fprintf (out, "Sorry, string too long (max %d-1). Try again: ", 
		 max_str_len);
      else 
	if (strlen(buf) != 0) {
	  strncpy(x+elt*max_str_len, buf, size-1);
	  (x+elt*max_str_len)[size-1] = '\0';
	}

    } while (size > max_str_len);

  } END_FOR

  return OK;
}


