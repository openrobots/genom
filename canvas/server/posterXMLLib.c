/*	$LAAS$ */

/* 
 * Copyright (c) 2003 LAAS/CNRS
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

/* DESCRIPTION :
   Bibliotheque de fonctions qui permettent la lecture au format xml
   du contenu des posters du module depuis un XML
*/

#ifdef VXWORKS
#include <vxWorks.h>
#else
#include "portLib.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "$module$PosterXMLLib.h"
#include "$module$PrintXML.h"

/*  prototypes de la bibliotheque printState */
extern char const * h2GetEvnStateString(int num);

  $execTaskNameTabDeclare$


static GENOM_POSTER_XML tabPosterXML[] = {
{"Cntrl", "", $module$CntrlPosterXML},
$tabPosterXML$};


/*****************************************
   MAIN PROGRAM
*****************************************/

void
web$module$(FILE *f, int argc, char **argv, char **argn)
{
  int poster;
  
  fprintfBuf(f,
	  "HTTP/1.0 200 OK\nPragma: no-cache\n"
	  "Content-Type: text/plain\n\n");

  /* debut du xml */
  xmlHeader(f);
  xmlBalise("$module$",BEGIN_BALISE_NEWLINE,f,0);
  xmlBalise("errorParam",BEGIN_BALISE,f,1);

  /* get parameters */
  poster = getPosterXML(f, argc, argv, argn, $nbPosterXML$, tabPosterXML);

  fprintfBuf(f, "</errorParam>\n");

  if (poster == -2) {
    posterListXML(f, $nbPosterXML$, tabPosterXML);
  }
  else if (poster != -1) {
    tabPosterXML[poster].posterXMLFunc(f);
  }

  xmlBalise("$module$",TERMINATE_BALISE, f, 0);
  
}

/* ---------------- LE POSTER DE CONTROLE ------------------------------ */

/* ----------------------------------------------------------------------
 *
 *  $module$CntrlPosterXML - Affichage du poster de controle du module
 *
 */

STATUS $module$CntrlPosterXML (FILE *f)
{
  $MODULE$_CNTRL_STR *sdic;
  int i;

  /* Lecture de la SDI de controle */
  sdic = ($MODULE$_CNTRL_STR *)malloc(sizeof($MODULE$_CNTRL_STR));
  if (sdic == NULL) {
    h2perror ("$module$CntrlPosterXML");
    return ERROR;
  }
  if ($module$CntrlPosterRead (sdic) != OK) {
    free(sdic);
    return ERROR;
  }

  /* 
   * Affichage 
   */

  fprintfBuf (f, "** TASKS           (status)      (last duration)         (bilan)\n");
  /* Tache de controle */
  fprintfBuf (f, " Control Task        %-5s                               %s\n",  
	  M_CNTRL_TASK_STATUS(sdic)==OK ? "OK":"ERROR",
	  h2getMsgErrno(M_CNTRL_TASK_BILAN(sdic)));
  
  /* Taches d'execution */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++) {
    fprintfBuf (f, " %d:%-15s   %-5s ",  
	    i, $module$ExecTaskNameTab[i],
	    M_EXEC_TASK_STATUS(sdic,i) == OK ? "OK": "ERROR");
    if (M_EXEC_TASK_PERIOD(sdic,i) != 0) {
      fprintfBuf (f, "    %3lums ", M_EXEC_TASK_ON_PERIOD(sdic,i));
      fprintfBuf (f, "(max %lu, ", M_EXEC_TASK_MAX_PERIOD(sdic,i));
      fprintfBuf (f, "th %d)", (int)(M_EXEC_TASK_PERIOD(sdic,i)*1000.));
    }
    else {
      fprintfBuf (f, "    %3lums ", M_EXEC_TASK_ON_PERIOD(sdic,i));
      fprintfBuf (f, "(max %lu)        ", M_EXEC_TASK_MAX_PERIOD(sdic,i));      
    }
    fprintfBuf (f, "\t %s\n", h2getMsgErrno(M_EXEC_TASK_BILAN(sdic,i)));

  }
  fprintfBuf (f, "\n");

  /* Activites */

  free(sdic);
  return OK;
}

STATUS $module$CntrlPosterActivityXML (FILE *f)
{
  $MODULE$_CNTRL_STR *sdic;

  /* Lecture de la SDI de controle */
  sdic = ($MODULE$_CNTRL_STR *)malloc(sizeof($MODULE$_CNTRL_STR));
  if (sdic == NULL) {
    h2perror ("$module$CntrlPosterActivityXML");
    return ERROR;
  }
  if ($module$CntrlPosterRead (sdic) != OK) {
    free(sdic);
    return ERROR;
  }

  free(sdic);
  return OK;
}

/* ---------------- LES POSTERS FONCTIONNELS ------------------------------ */

