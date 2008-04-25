/*	$LAAS$ */

/* --- FILE GENERATED WITH GENOM, DO NOT EDIT BY HAND ------------------ */

/* 
 * Copyright (c) 2003-2005 LAAS/CNRS
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

#include <stdio.h>
#include <stdlib.h>

#include <portLib.h>
#include <h2timeLib.h>

#include "$module$PosterLib.h"
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

static void 
$module$ActivitiesXML (FILE *f, $MODULE$_CNTRL_STR *sdic)
{
#define $MODULE$_NB_RQST_EXEC $nbExecRqst$
#if $MODULE$_NB_RQST_EXEC != 0

  $requestNameTabDeclare$
  int i;
  ACTIVITY_EVENT evn;
  ACTIVITY_STATE status;
  int bilan;
  int rqst;
  char strerr[64];

  for (i=0; i<MAX_ACTIVITIES; i++) {
    status = M_ACTIVITY_STATUS(sdic,i);
    bilan = M_ACTIVITY_BILAN(sdic,i);

    if (status != ETHER || bilan != OK) {
      evn = M_ACTIVITY_EVN(sdic,i);
      
      xmlBalise("activity", BEGIN_BALISE_NEWLINE, f, 1);

      xmlBalise("id", BEGIN_BALISE, f, 2);
      fprintf(f, "%d", M_ACTIVITY_ID(sdic, i));
      xmlBalise("id", TERMINATE_BALISE, f, 0);
      
      /* find the name */
      rqst=0;
      while($module$ExecRqstNumTab[rqst] != M_ACTIVITY_RQST_TYPE(sdic,i) 
	    && rqst<$MODULE$_NB_RQST_EXEC)
	rqst++;
      xmlBalise("name", BEGIN_BALISE, f, 2);
      fprintf(f, "%s", 
	  rqst == $MODULE$_NB_RQST_EXEC?
	  "Unknown" : $module$ExecRqstNameTab[rqst]);
      xmlBalise("name", TERMINATE_BALISE, f, 0);
      
      xmlBalise("task", BEGIN_BALISE, f, 2);
      fprintf(f, "%s", 
	  M_ACTIVITY_TASK_NUM(sdic,i) == -1 ? 
	  "not exec" : $module$ExecTaskNameTab[M_ACTIVITY_TASK_NUM(sdic,i)]);
      xmlBalise("task", TERMINATE_BALISE, f, 0);

      xmlBalise("status", BEGIN_BALISE, f, 2);
      fprintf(f, "%s", 
	  evn == NO_EVENT ?
	  h2GetEvnStateString(status) : h2GetEvnStateString(evn));
      xmlBalise("status", TERMINATE_BALISE, f, 0);
      
      xmlBalise("errno", BEGIN_BALISE, f, 2);
      fprintf(f, "%s", h2getErrMsg(bilan, strerr, 64));
      xmlBalise("errno", TERMINATE_BALISE, f, 0);

      xmlBalise("activity", TERMINATE_BALISE, f, 1);
    }

  }	/* for */

#endif /* $MODULE$_NB_RQST_EXEC != 0 */
}

/* ----------------------------------------------------------------------
 *
 *  $module$CntrlPosterXML - Affichage du poster de controle du module
 *
 */

STATUS $module$CntrlPosterXML (FILE *f)
{
  $MODULE$_CNTRL_STR *sdic;
  int i;
  char strerr[64];

  /* Read the control IDS */
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
   * display 
   */
  /* control task */
  xmlBalise("task", BEGIN_BALISE_NEWLINE, f, 1);

  xmlBalise("name", BEGIN_BALISE, f, 2);
  fprintf(f, "ControlTask");
  xmlBalise("name", TERMINATE_BALISE, f, 0);

  xmlBalise("status", BEGIN_BALISE, f, 2);
  fprintf(f, "%s", 
	  M_CNTRL_TASK_STATUS(sdic)==OK ? "OK":"ERROR");
  xmlBalise("status", TERMINATE_BALISE, f, 0);
  if (M_CNTRL_TASK_STATUS(sdic) != OK) {
    xmlBalise("errno", BEGIN_BALISE, f, 2);
    fprintf(f, "%s", h2getErrMsg(M_CNTRL_TASK_BILAN(sdic), strerr, 64));
    xmlBalise("errno", TERMINATE_BALISE, f, 0);
  }
  xmlBalise("task", TERMINATE_BALISE, f, 1);

  /* execution tasks */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++) {
    xmlBalise("task", BEGIN_BALISE_NEWLINE, f, 1);

    xmlBalise("name", BEGIN_BALISE, f, 2);
    fprintf(f, "%s", $module$ExecTaskNameTab[i]);
    xmlBalise("name", TERMINATE_BALISE, f, 0);

    xmlBalise("status", BEGIN_BALISE, f, 2);
    fprintf(f, "%s", M_EXEC_TASK_STATUS(sdic,i) == OK ? "OK": "ERROR");
    xmlBalise("status", TERMINATE_BALISE, f, 0);
    if (M_EXEC_TASK_STATUS(sdic, i) != OK) {
      xmlBalise("errno", BEGIN_BALISE, f, 2);
      fprintf(f, "%s", h2getErrMsg(M_EXEC_TASK_BILAN(sdic,i), strerr, 64));
      xmlBalise("errno", TERMINATE_BALISE, f, 0);
    }

    if (M_EXEC_TASK_PERIOD(sdic,i) != 0) {
      xmlBalise("period", BEGIN_BALISE, f, 2);
      fprintfBuf (f, "%d", (int)(M_EXEC_TASK_PERIOD(sdic,i)*1000.));
      xmlBalise("period", TERMINATE_BALISE, f, 0);
    }
    xmlBalise("task", TERMINATE_BALISE, f, 1);

  }

  /* Activities */
  $module$ActivitiesXML(f, sdic);

  free(sdic);
  return OK;
}

/**
 ** Dump only activities. XXX not used ? 
 **/
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
  $module$ActivitiesXML(f, sdic);
  free(sdic);
  return OK;
}

/* ---------------- LES POSTERS FONCTIONNELS ------------------------------ */

