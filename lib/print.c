/*	$LAAS$ */

/* 
 * Copyright (c) 1994-2003 LAAS/CNRS
 * Sara Fleury - Sat Oct  1 1994
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
#include "config.h"
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
#include "genom/printProto.h"
#include "genom/printScan.h"

/* Taille max des tableaux pouvant être affichés directement sans saisie
   du nombre d'éléments à afficher -- variable globale : peut être modifiée 
   dynamiquement dans le shell VxWorks */
int genomMaxArray = GENOM_MAX_ARRAY_DEFAULT;


void print_char(FILE *out, char *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
/*     fprintf(out, "%s%s %c\n",  */
    fprintf(out, "%s%s %d\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_unsigned_char(FILE *out, unsigned char *x, int indent, 
			 int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %c\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_short_int(FILE *out, short int *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %d\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_int(FILE *out, int *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %d\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_long_int(FILE *out, long int *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %ld\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_unsigned_short_int(FILE *out, unsigned short *x, int indent, 
			      int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %u\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_unsigned_int(FILE *out, unsigned int *x, int indent, 
			int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %u\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_unsigned_long_int(FILE *out, unsigned long *x, int indent,
			     int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %lu\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_float(FILE *out, float *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %f\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_double(FILE *out, double *x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s %f\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_addr(FILE *out, void **x, int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims)
    fprintf(out, "%s%s 0x%p\n", 
	    indentStr(indent-1), getIndexesStr(nDim, dims, elt), *(x+elt));
  END_FOR
}

void print_string(FILE *out, char *x, int indent, int nDim, int *dims, FILE *in) 
{

  if(dims[nDim-1] == 0) {
    fprintf(out, "Null string length\n");
    return;
  }

  print_string_len(out, x, dims[nDim-1], indent, nDim-1, dims, in);
}


void print_string_len(FILE *out, char *x, int max_str_len,
		      int indent, int nDim, int *dims, FILE *in) 
{
  indent++;
  FOR_NB_elt(nDim,dims) 
    fprintf(out, "%s%s %s\n", 
	    indentStr(indent-1),
	    getIndexesStr(nDim, dims, elt), (x+elt*max_str_len));
  END_FOR
}

/**
 ** get_sub_array_indexes  - fonction annexe pour déterminer quelle partie 
 **                      on considère (ie, de quel à quel indice) pour les 
 **                      gros tableaux (ie, plus de genomMaxArray elements).
 **
 ** Cette fonction est appelée dans la macro FOR_NB_elt (définie dans
 ** printScan.h). Elle est utilisée dans cette bibliothèque, mais 
 ** également dans les bibliothèques printLib de chaque module.
 ** 
 ** Entrées : file-descriptors <<out>> pour affichage et <<in>> pour saisie
 **           (pour gros tableaux) sauf si NULL, niveau d'indentation
 **           dimensions du tableau origine (nDim et dims)
 ** Sorties : premier élément considéré (*start) et nombre d'éléments
 **           (retourné par la fonction)
 ** 
 ** REMARQUE : 
 ** genomMaxArray est une variable globale dont on peut changer 
 ** sa valeur sous le shell vxworks.
 ** 
 ** ATTENTION : 
 ** Par defaut la saisie de la dimension du sous-tableau a 
 ** afficher se fait sur << stdin >> car les fonctions << print_* >> 
 ** n'avaient pas a l'origine de parametre pour definir ce << file 
 ** descriptor >>. Il faudra peut etre changer ca un jour ...
 ** 
 **/


int get_sub_array_indexes (FILE *out, FILE *in, int indent, 
			   int nDim, int *dims, int *start)
{
  int nElts=1, i, nbd=genomMaxArray, first=0;
  *start = 0;

  /* Calcul nombre d'éléments du tableau */
  for (i=0;i<(nDim);i++) 
    nElts *= (dims)[i];
  
  /* Petit tableau : on affiche tout */
  if (nElts<=genomMaxArray) 
    return nElts;

  /* Gros tableau */

  /* Pas de file descriptor de saisie */
  if (in == NULL) 
    return nElts;

  fprintf(out, "%sArray of %d elts. ", indentStr(indent-1), nElts);

  /* Choix premier élément à afficher */
  fprintf(out, "Display from element : (0) ");
  fflush(out);
  if (scan_type(in, out, "%d", &first) == ABORT) 
    return (0);
  if (first<0 || first>=nElts)
    return (0);

  /* Choix nombre d'élément à afficher */
  fprintf(out, "%snb elts : (%d) ", indentStr(indent-1), nbd);
  fflush(out);
  scan_type(in, out, "%d", &nbd);
  if(nbd <= 0) 
    return (0);

  /* Retourne premier élément et nombre d'éléments à afficher */
  if(first+nbd > nElts)
    nbd = nElts-first;
  *start = first;
  return nbd;
}

