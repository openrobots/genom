
/* 
 * Copyright (c) 1996-2003,2012 LAAS/CNRS
 * Sara Fleury - Oct 1996
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
 *** Utilitaires pour affichage des structures C dans les taches << essay >>
 *** Utilis�s par print.c et scan.c de genomLib, mais �galement directement
 *** par les biblioth�ques print et scan de chaque module.
 ***/

#include <portLib.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "genom/printScan.h"

static char bufindent[80];
static char bufindexes[80];

/* ---------------------------------------------------------------------
   indentStr -  Structures indentation

   Input : indentation level
   Output : string made of "indent*TAB_INDENT" characters.
   TAB_INDENT >=2, last characters being : "| "

   Remark : the returned string is static : no allocation but not
   "r�-entrante".
   --------------------------------------------------------------------- */
char *indentStr(int indent)
{
  return indentStr_r(indent, bufindent);
}

char *indentStr_r(int indent, char *retbuf)
{
  int i,j;

  retbuf[0]='\0';
  for(i=0;i<indent;i++) {
    for(j=0;j<TAB_INDENT-2;j++)
      strcat(retbuf, " ");
    if(i!=0) {
      strcat(retbuf, "|");
      strcat(retbuf, " ");
    }
  }
  /*  for(i=0;i<indent*TAB_INDENT;i++) strcat(bufindent, " ");*/
  return retbuf;
}

/* ---------------------------------------------------------------------
   indentStr2 -  Structures indentation
                 Like indentStr but with blanks only
   --------------------------------------------------------------------- */

char *indentStr2(int indent)
{
  return indentStr2_r(indent, bufindent);
}

char *indentStr2_r(int indent, char *retbuf)
{
  int i;

  retbuf[0]='\0';
/*   for(i=0;i<indent*TAB_INDENT;i++) strcat(bufindent, " "); */
  for(i=0;i<indent;i++) strcat(retbuf, "\t");
  return retbuf;
}

/* ---------------------------------------------------------------------
   getIndexesStr  -  Indexes of the element of an array :

   Input: nDim: array dimensions
            dims: array with the size of each dimension,
	    indice: indice of the current element
   Output: string : "[i][j]..."
	   or empty string if nDim==0
   --------------------------------------------------------------------- */
char *getIndexesStr(int nDim, int *dims, int indice) 
{
  return getIndexesStr_r(nDim, dims, indice, bufindexes);
}

char *getIndexesStr_r(int nDim, int *dims, int indice, char *retbuf) 
{
  int index, n, m;
  int remain;
  char buf[80];

  if(nDim>0) sprintf(retbuf, "[");
  else retbuf[0]='\0';
  for (n=0; n<nDim-1; n++) {
    for (remain=1,m=n+1; m<nDim; m++) remain *= dims[m];
    index = indice/remain;
    indice %= remain;
    sprintf(buf, "%d][", index);
    strcat(retbuf, buf);
  }
  if (nDim>0) {sprintf(buf, "%d]", indice); strcat(retbuf,buf);}
  return retbuf;
}

/* ---------------------------------------------------------------------
   getIndexesStr2 -  Indexes of the element of an array
                 Like getIndexesStr but a dot separator instead of [] 
   --------------------------------------------------------------------- */

/* A dot instead of [] */
char *getIndexesStr2(int nDim, int *dims, int indice) 
{
  return getIndexesStr2_r(nDim, dims, indice, bufindexes);
}


/* A dot instead of [], reentrant */
char *getIndexesStr2_r(int nDim, int *dims, int indice, char *retbuf)
{
  int index, n, m;
  int remain;
  char buf[80];

  if(nDim>0) sprintf(retbuf, ".");
  else retbuf[0]='\0';
  for (n=0; n<nDim-1; n++) {
    for (remain=1,m=n+1; m<nDim; m++) remain *= dims[m];
    index = indice/remain;
    indice %= remain;
    sprintf(buf, "%d.", index);
    strcat(retbuf, buf);
  }
  if (nDim>0) {sprintf(buf, "%d", indice); strcat(retbuf,buf);}
  return retbuf;
}



