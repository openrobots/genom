/**
 ** Copyright (c) 1996 LAAS/CNRS
 **
 ** Sara Fleury - Oct 1996
 **
 ** $Id$
 **/

#ifndef PRINT_SCAN_H
#define PRINT_SCAN_H

/*
 * Fonctions de print
 */
#include "printProto.h"

/*
 * Fonctions de scan
 */
#include "scanProto.h"
#define ABORT (-2)

/*
 * Declaration de strdup 
 */
#ifdef VXWORKS
char *strdup(const char *s);  /* /opt/hilare2/src/tools/ansiLib */
#else
#include <string.h>
#endif

/*
 * Affichage des tabulation.
 * Retourne: une chaine de "indent*TAB_INDENT" blancs 
 */
#define TAB_INDENT 4 
char *indentStr(int indent);
char *indentStr2(int indent);


/*
 * Indexes d'un element d'un tableau:
 * Retourne: . la chaine correspondante sous la forme: "[i,j...] "
 *           . rien si le tableau est de dimension 0
 */
char *getIndexesStr(int nDim, int *dims, int indice);
char *getIndexesStr2(int nDim, int *dims, int indice);

/*
 * Macro pour parcourir les elements "elt" d'un tableau
 * de "nDim" dimensions definies dans le tableau "dims" 
 * Remarque: doit etre imperativement dans la suite immediate 
 *           de la partie de declarations de la fonction.
 */
#define FOR_EACH_elt(nDim,dim) \
{ int nElts=1,i,elt;\
for (i=0;i<(nDim);i++) nElts *= (dims)[i];\
for (elt=0;elt<nElts;elt++)

#define END_FOR_EACH_elt }
#define END_FOR }

/* la version ci-dessous va bientot disparaitre (productrice de warnings) 
   le temps que tous les modules aient ete regeneres une fois */
#define FOREACH_elt(nDim,dim) \
int nElts=1,i,elt;\
indent++;\
for (i=0;i<(nDim);i++) nElts *= (dims)[i];\
for (elt=0;elt<nElts;elt++)
  

/* Valeur pour initialiser la variable globale <<genomMaxArray>>
   Remarque : cette variable peut etre modifiee dynamiquement */
#define GENOM_MAX_ARRAY_DEFAULT 10

/*
 * Idem que FOREACH_elt sauf que pour les gros tableaux (ie, plus de 
 * <<genomMaxArray>> elements) on demande a l'operateur 
 * l'indice du premier element et le nombre total d'elements a parcourir
 * (par defaut: genomMaxArray elements a partir de l'indice 0)
 */

#define FOR_NB_elt(nDim,dim) \
{ int elt,nbd,start=0;\
nbd = get_sub_array_indexes(out,in,indent,nDim,dims,&start);\
for (elt=start;elt<start+nbd;elt++)

#define END_FOR_NB_elt }

/* la version ci-dessous va bientot disparaitre (productrice de warnings) 
   le temps que tous les modules aient ete regeneres une fois */
#define FORNB_elt(nDim,dim) \
int elt,nbd,start=0;\
indent++;\
nbd = get_sub_array_indexes(out,in,indent,nDim,dims,&start);\
for (elt=start;elt<start+nbd;elt++)


#endif /* PRINT_SCAN_H */
