/*	$LAAS$ */

/* 
 * Copyright (c) 2003 LAAS/CNRS                       --  Fri Sep  5 2003
 * Sara Fleury
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
__RCSID("$LAAS$");

/***
 *** Print des types de base utilise's par les modules generator
 ***/

/** 
 ** Les fonctions suivantes attendent un pointeur sur une variable
 ** OU sur un tableau de variables.
 **
 **   out:    où afficher la variable (file descriptor)
 **   x:      le pointeur sur la variable
 **   indent: niveau d'indentation (pour l'affichage)
 **   nDim:   nombre de dimensions du tableau (ou 0)
 **   dims:   tableau de nDim "int": taille de chaque dimension (ou NULL)
 **   in:     où faire la saisie (file descriptor) des éléments à afficher 
 **           dans le cas de gros tableaux (ie, plus de «genomMaxArray» 
 **           éléments). Si c'est NULL, alors seuls les genomMaxArray premier 
 **           éléments seront affichés. 
 **           Remarque : genomMaxArray est une variable globale qui peut 
 **           donc être modifiée sous le shell VxWorks.
 **/


#ifdef VXWORKS
#  include <vxWorks.h>
#else
#  include <portLib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "genom/printXMLProto.h"
#include "genom/printScan.h"


int fprintfBuf(FILE *out, const char *fmt, ...)
{    
  va_list args;
  int nbc, nbci, count;
  char buf[1024];
  char *p=buf;
  
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  nbc = strlen(buf);
  nbci = nbc;
  do {
    errno = 0;
    count = fwrite(p, 1, nbc, out);
    p += count;
    nbc -= count;
    if (errno != 0) {
      printf ("dxml write %d miss %d err %d\n", count, nbc, errno);
      perror ("dxml:");
    }
  } while (nbc>0 && errno==EINTR);

  if (errno != 0 || nbc != 0) return 0;
  return nbci;
}


int printXML_char(FILE *out, char *name, char *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%d</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt), 
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_unsigned_char(FILE *out, char *name, unsigned char *x, int indent, 
			 int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%c</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_short_int(FILE *out, char *name, short int *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%d</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_int(FILE *out, char *name, int *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%d</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_long_int(FILE *out, char *name, long int *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%ld</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_long_long_int(FILE *out, char *name, long long int *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%lld</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_unsigned_short_int(FILE *out, char *name, unsigned short *x, int indent, 
			      int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%u</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_unsigned_int(FILE *out, char *name, unsigned int *x, int indent, 
			int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%u</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_unsigned_long_int(FILE *out, char *name, unsigned long *x, int indent,
			     int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%lu</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_unsigned_long_long_int(FILE *out, char *name, 
    unsigned long long *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%llu</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_float(FILE *out, char *name, float *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%f</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}



int printXML_double(FILE *out, char *name, double *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>%f</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}


int printXML_addr(FILE *out, char *name, void **x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    if (!(fprintfBuf(out, "%s<%s%s>0x%p</%s%s>\n", 
	    indentStr2(indent-1), name, getIndexesStr2(nDim, dims, elt),
		     *(x+elt), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}

int printXML_string(FILE *out, char *name, char *x, int indent, int nDim, int *dims, FILE *in) 
{

  if(dims[nDim-1] == 0) {
    if (!(fprintfBuf(out, "Null string length\n"))) return 0;
    return 1;
  }

  if (!printXML_string_len(out, name, x, dims[nDim-1], indent, nDim-1, dims, in)) return 0;
  return 1;
}


int printXML_string_len(FILE *out, char *name, char *x, int max_str_len,
		      int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims) 
    if (!(fprintfBuf(out, "%s<%s%s>%s</%s%s>\n", 
	    indentStr2(indent-1), name,
		     getIndexesStr2(nDim, dims, elt), (x+elt*max_str_len), name, getIndexesStr2(nDim, dims, elt)))) return 0;
  END_FOR
  return 1;
}
