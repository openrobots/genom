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



int getPosterXML(FILE *stream, int argc, char **argv, char **argn, 
		 int nbPostersData,
		 GENOM_POSTER_XML posterXML[])
{
  int i=1,j;
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

void posterListXML(FILE *f, int nbPoster, GENOM_POSTER_XML posterXML[])
{
  int i;
  char posterDataName[1024];

  fprintfBuf(f, "\t<posterList>\n");
  if (nbPoster==0)
    return;

  for (i=0; i<nbPoster-1; i++) {
    posterDataName[0] = '\0';
    strcat(posterDataName, posterXML[i].posterName);
    strcat(posterDataName, posterXML[i].posterData);
    fprintfBuf(f, "\t\t\"%s\",\n", posterDataName);
  }
  posterDataName[0] = '\0';
  strcat(posterDataName, posterXML[i].posterName);
  strcat(posterDataName, posterXML[i].posterData);
  fprintfBuf(f, "\t\t\"%s\"\n", posterDataName);

  fprintfBuf(f, "\t</posterList>\n");

  return;
}


/**************************************************************************
 * Paquage contenant quelques fonctions simplifiant
 * la construction d'un fichier xml
 * - xmlHeader: ecrire l'entete
 * - xmlBalise: ecrire une balise ouvrante ou fermante
 * - xmlElement: ecrire un nouvel element (on peut specifier la profondeur)
 ***************************************************************************/



/* Envoie sur la sortie stream l'entete xml */

int xmlHeader(FILE *stream){
  int result=0;
  result+=fprintfBuf(stream,XML_HEADER);
  result+=fprintfBuf(stream,"\n");
  return result;
}


/**********************************
 Creation d'un balise.
 - beginEnd peut prendre pour valeur BEGIN_BALISE, BEGIN_BALISE_NEWLINE
   ou TERMINATE_BALISE
 - level indique le niveau de profondeur (>=0)
*/


int xmlBalise(char *balise,int beginEnd,FILE *stream,int level){
  int result=0;
  int i;


  for(i=0;i<level;i++){
    result+=fprintfBuf(stream,"\t");
  }


  switch(beginEnd){
  case TERMINATE_BALISE: result+=fprintfBuf(stream,END_BALISE);break;
  default: result+=fprintfBuf(stream,BALISE_HEAD);
   
  }

  result+=fprintfBuf(stream,balise); 
  result+=fprintfBuf(stream,BALISE_END);


  if(beginEnd==TERMINATE_BALISE|| beginEnd==BEGIN_BALISE_NEWLINE){
    result+=fprintfBuf(stream,"\n");
  }


  return result;
}  
