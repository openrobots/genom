
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

/***
 *** Utilities for CGI for pico web that read posters and return
 *** XML data.
 *** 
 ***/

#ifdef VXWORKS
#  include <vxWorks.h>
#else
#  include <portLib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "genom/posterXMLLib.h"
#include "genom/printXMLProto.h"


/* ----------------------------------------------------------------------
 * 
 * Test if poster exists and select command
 *
 * Returns the id of the data to be read or
 * -2 if the parameters are not correct or
 * -1 if the command or the poster is unknown
 * 
 */
int getPosterXML(FILE *stream, int argc, char **argv, char **argn, 
		 int nbPostersData,
		 GENOM_POSTER_XML posterXML[])
{
  int i=1, j=0;
  char * name=NULL;
  char * value;
  static char *cmds[] = POSTER_XML_CMDS;
  int nbCmds = sizeof(cmds)/sizeof(char *);
  char posterDataName[1024];


  if (argc<=1) {
    return -2;
  }

  while(i<argc) {

    name=argn[i];
    value=argv[i];
    
    /* test command */
    for (j=0; j<nbCmds; j++) {
      if (!strcmp(name, cmds[j])) {
	break;
      }
    }
    if (j==nbCmds) {
      fprintfBuf(stream,"getPosterXML Syntax error: unknown cmd %s\n", name);
      return -1;
    }

    /* look for the poster */
    for (j=0; j<nbPostersData; j++) {
      posterDataName[0] = '\0';
      strcat(posterDataName, posterXML[j].posterName);
      strcat(posterDataName, posterXML[j].posterData);
      if (!strcmp(value, posterDataName)) {
	break;
      }
    }
    if (j==nbPostersData) {
      fprintfBuf(stream,"getPosterXML Syntax error: unknown poster data %s\n", 
	      value);
      return -1;
    }

    /* passe au parametre suivant */
    i++;
  }
  
  return j;
}

int posterListXML(FILE *f, int nbPoster, GENOM_POSTER_XML posterXML[])
{
  int i;
  char posterDataName[1024];

  if (!(fprintfBuf(f, "\t<posterList>\n"))) return 0;
  if (nbPoster==0) {
    if (!(fprintfBuf(f, "\t</posterList>\n"))) return 0;
    return 1;
  }

  for (i=0; i<nbPoster-1; i++) {
    posterDataName[0] = '\0';
    strcat(posterDataName, posterXML[i].posterName);
    strcat(posterDataName, posterXML[i].posterData);
    if (!(fprintfBuf(f, "\t\t\"%s\",\n", posterDataName))) return 0;
  }
  posterDataName[0] = '\0';
  strcat(posterDataName, posterXML[i].posterName);
  strcat(posterDataName, posterXML[i].posterData);
  if (!(fprintfBuf(f, "\t\t\"%s\"\n", posterDataName))) return 0;

  if (!(fprintfBuf(f, "\t</posterList>\n"))) return 0;
  return 1;
}


/**************************************************************************
 * Paquage contenant quelques fonctions simplifiant
 * la construction d'un fichier xml
 * - xmlHeader: ecrire l'entete
 * - xmlBalise: ecrire une balise ouvrante ou fermante
 * - xmlElement: ecrire un nouvel element (on peut specifier la profondeur)
 ***************************************************************************/



/* Envoie sur la sortie stream l'entete xml */

int xmlHeader(FILE *stream)
{
  int res, result=0;
  if (!(res=fprintfBuf(stream,XML_HEADER))) return 0;
  result += res;
  if (!(res=fprintfBuf(stream,"\n"))) return 0;
  result += res;
  return result;
}


/**********************************
 Creation d'un balise.
 - beginEnd peut prendre pour valeur BEGIN_BALISE, BEGIN_BALISE_NEWLINE
   ou TERMINATE_BALISE
 - level indique le niveau de profondeur (>=0)
*/


int xmlBalise(char *balise,int beginEnd,FILE *stream,int level)
{
  int res, result=0;
  int i;

  for(i=0;i<level;i++){
    if (!(res = fprintfBuf(stream,"\t"))) return 0;
    result += res;
  }
  
  switch(beginEnd){
  case TERMINATE_BALISE: 
    if (!(res=fprintfBuf(stream,END_BALISE))) return 0;
    result += res;
    break;
  default: if (!(res=fprintfBuf(stream,BALISE_HEAD))) return 0;
    result += res;
  }
  
  if (!(res=fprintfBuf(stream,balise))) return 0; 
  result += res;
  if (!(res=fprintfBuf(stream,BALISE_END))) return 0;
  result += res;
  
  if(beginEnd==TERMINATE_BALISE|| beginEnd==BEGIN_BALISE_NEWLINE){
    if (!(res=fprintfBuf(stream,"\n"))) return 0;
    result += res;
  }
  
  return result;
}  
