/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2004 LAAS/CNRS
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

/*
 * Sara Fleury 
 *
 * Fonctions communes aux taches d'"essay": affichage de menu, envoi de 
 * requetes, lecture de posters, ... . 
 *
 * Elles prennent (presque) toutes en paramètre un pointeur sur la 
 * structure ESSAY_STR qui comporte toutes info necessaires.
 * 
 *  REMARQUE: h2scanf ne peut plus retourner ERROR. Il faudra donc a 
 *            l'occasion supprimer les tests correspondants.
 */

/*----------------------------- INCLUSIONS ---------------------------------*/

/* Inclusions generales */
#ifdef VXWORKS
#include <vxWorks.h>
#include <taskLib.h>
#include "h2sysLibProto.h"
#else
#include <portLib.h>
#endif
/* XXX #include "xes.h" */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errnoLib.h>

#include "csLib.h"
#include "genom/modules.h"
#include "genom/genomReportsLib.h"
#include "genom/essayLib.h"

/*-------------------- PROTOTYPES FONCTIONS LOCALES -----------------------*/
static int  essayGetMenuSizes     (ESSAY_STR *eid, int *nblin, int *colwidth);
static int  essayPrintMainMenu    (ESSAY_STR *eid,
				  int nb_col, int nb_lin, int colwidth); 
static void essayPrintPosterMenu  (ESSAY_STR *eid);
static void essayPosterShow       (ESSAY_STR *eid, int poster);

static void essaySendRequest(ESSAY_STR *eid);
static void essayGetReplies (ESSAY_STR *eid);
static void essayAbortRqst  (ESSAY_STR *eid);
static void essayStopModule (ESSAY_STR *eid);

static BOOL essaySendRqst  (ESSAY_STR *eid, int rqstNum, int acti);
static BOOL essayRcvReply  (ESSAY_STR *eid, int acti, int block, 
			   GENOM_REPORTS_STRUCT *localFail);
static BOOL essayConfirmCntrlRqst (ESSAY_STR *eid);
static BOOL essayConfirmExecRqst  (BOOL *waitFinal, ESSAY_STR *eid);
static BOOL essayAllocData        (ESSAY_STR *essay, int rqst);
static int  essayRqstType2RqstIndex(ESSAY_STR *essay, int rqstType);
static void essayExitH2ScanfError(ESSAY_STR *eid);


/* ----------------- FONCTIONS D'INIT ET DE TERMINAISON ------------------ */

/**
 ** essayInit : Initialisation de la tache d'essay: 
 **         allocation de la structure ESSAY_STR
 **         init des bal
 **/

ESSAY_STR *
essayInit(int essayNum, char *module, int mboxsize, int abortRqstNum,
	 int nbRqstType, char *rqstName[], ESSAY_RQST_DESC_STR *rqstDesc, 
	 int nbPosters, char *posterName[],
	 ESSAY_POSTER_SHOW_FUNC_PTR posterShowFunc[])
{
  ESSAY_STR *eid;
  char buf[30];              
  int i;

  eid = (ESSAY_STR *) malloc(sizeof(ESSAY_STR));
  if (eid == NULL) {
    printf ("essayAlloc: cannot alloc\n");
    return eid;
  }

  /* Mettre la bufferisation a zero char */
  setbuf (stdin, NULL);
  setbuf (stdout, NULL);


#ifdef VXWORKS
  /* Verifie que xes_host est definie */
  if (xes_get_host() == -1) {
    printf ("xes host not defined (call: xes_set_host \"<host_name>\")\n");
    free(eid);
    return NULL;
  }
  /* Rediriger les E/S vers un xterm de la machine hote */
  xes_init (NULL); 
  sprintf(buf, "%sEssay %d   (VxWorks)", module, essayNum);
  xes_set_title(buf);
#endif

  /* Copier le nom de base des mboxes */
  sprintf(buf, "t%s%d", module, essayNum);

  /* Initialisation des boites aux lettres */
  if (csMboxInit (buf, 0, mboxsize) != OK) {
    h2perror("");
    (void) printf ("Init mbox %s failed\n", buf);
#ifdef UNIX
    printf("Is file .mboxrc correct ? Is ypmbox started ?\n");
#endif
    free(eid);
    return NULL;
  }

#ifdef UNIX
  sleep (1);
#endif

  /* Recupere les donnees static du module */
  ESSAY_NAME(eid) = module;

  ESSAY_NB_RQST(eid) = nbRqstType;
  ESSAY_RQST_DESC(eid) = rqstDesc;
  ESSAY_RQSTS_NAME(eid) = rqstName;
  ESSAY_RQST_ABORT(eid) = abortRqstNum;

  ESSAY_NB_POSTERS(eid) = nbPosters;
  ESSAY_POSTERS_NAME(eid) = posterName;
  ESSAY_POSTER_SHOW_FUNCS(eid) = posterShowFunc;

  /* Init des donnees dynamique de la tache d'essay */
  ESSAY_NUM(eid) = essayNum;
  ESSAY_MAIN_CMD(eid) = 88;
  ESSAY_POSTER_CMD(eid) = 0;

  /* Initialiser le tableau des requetes en cours */
  ESSAY_NB_ACTIVITY_ON(eid) = 0;
  if ((ESSAY_RQSTS(eid) = (ESSAY_RQST_STR *) 
       malloc(nbRqstType * sizeof(ESSAY_RQST_STR))) == NULL) {
    printf ("Cannot alloc rqst tab\n");
    free(eid);
    return NULL;
  }
  for (i=0; i<nbRqstType; i++) {
    ESSAY_RQST_INPUT(eid,i) = NULL;
    ESSAY_RQST_OUTPUT(eid,i) = NULL;
    ESSAY_RQST_GET_CMD(eid,i) = -1;
  }
  for (i=0; i<CLIENT_NMAX_RQST_ID; i++)
    ESSAY_ACTIVITY_ON(eid,i) = FALSE;

  return eid;
}


/**
 ** essayEnd : Terminaison de la tache d'essay: 
 **         libération de la structure ESSAY_STR
 **         libération des bal
 **/

void 
essayEnd(ESSAY_STR *eid)
{
  int i;

  /* Termine les communication */
  csClientEnd(ESSAY_CID(eid));
  csMboxEnd();

  /* Libere la structure */
  for (i=0; i<ESSAY_NB_RQST(eid); i++) {
    if(ESSAY_RQST_INPUT(eid,i) != NULL)
      free(ESSAY_RQST_INPUT(eid,i));
    if(ESSAY_RQST_OUTPUT(eid,i) != NULL)
      free(ESSAY_RQST_OUTPUT(eid,i));
  }
  free(ESSAY_RQSTS(eid));
  free(eid);

#ifdef UNIX
  printf ("\nType return again to remove this window");
  /* Pour que la fenetre ne disparaisse pas tout de suite */
  getchar();
  exit(0);
#else
  printf ("\nType C^d to remove this window");
  exit(0);
#endif
}

/* ------------------------- FONCTION PRINCIPALE ----------------------- */

void 
essayMain(ESSAY_STR *eid)
{
  int nblin, nbcol, colwidth;

  nbcol = essayGetMenuSizes(eid, &nblin, &colwidth);

  /* Boucler, en attente de commandes */
  while (1) {

      /* Imprimer le menu et recuperer la commande */
      errnoSet(0);
      essayPrintMainMenu(eid, nbcol, nblin, colwidth);

      switch(ESSAY_MAIN_CMD(eid)) {
	  
	/* -- Fin */
      case 99:
	essayEnd(eid);
	  
	/* -- Lecture du poster de controle */
      case 88:
	essayPosterShow(eid, ESSAY_POSTER_SHOW_CNTRL(eid));
	break;
	
	/* -- Lecture des posters d'execution */
      case 55:
	essayPrintPosterMenu(eid);
	break;
	
	/* -- Arret d'une activite */
      case 66:
	essayAbortRqst(eid);
	break;
	
	/* -- Arret du module */
      case -99:
	essayStopModule(eid);
	break;
	
	/* -- Affichage/Lecture des requetes en cours */
      case 77:
	essayGetReplies(eid);
	break;
	
	/* -- Emission d'une requete */
      default:
	essaySendRequest(eid);
	break;
	
      } /* switch */

    }	  /* FOREVER */
}


/* -------- FONCTIONS DE GESTION DES REQUETES DE CONTROLE ET D'EXEC  ------- */
/*        (appellees par les fonctions specifiques pour chaque requete:
	                  ESSAY_RQST_FUNC )                                 */

/**
 **  essaySendAndRcvCntrlRqst  -- Envoi d'une requete de controle
 **                               toujours en mode BLOQUANT
 **
 ** Utilise' par le pointeur sur fonction ESSAY_RQST_FUNC.
 **
 **  Retourne TRUE si reponse finale receptionnée (y compris avec erreur)
 **  et FALSE sinon
 **/

BOOL 
essaySendAndRcvCntrlRqst(ESSAY_STR *eid, int rqstNum, int acti,
			int silent, GENOM_REPORTS_STRUCT *localFail)
{

  /* Confiration de l'envoie */
  if (!silent)
    if (!essayConfirmCntrlRqst(eid))
      return FALSE;

  /* Emettre la requete */
  if (!essaySendRqst(eid,rqstNum,acti))
    return FALSE;
  
  /* Reception de la replique */
  return essayRcvReply(eid, acti, BLOCK_ON_FINAL_REPLY, localFail);
}


/**
 ** essaySendAndOrRcvExecRqst  --  Envoi d'une requete d'execution
 **                                ou lecture de la reponse si deja envoyee
 **
 ** Utilise' par le pointeur sur fonction ESSAY_RQST_FUNC.
 **
 ** Retourne TRUE si reponse finale receptionnée (y compris avec erreur?)
 **  et FALSE sinon
 **/

BOOL 
essaySendAndOrRcvExecRqst(ESSAY_STR *eid, int rqstNum, int acti,
			 int silent, GENOM_REPORTS_STRUCT *localFail)
{
  BOOL waitFinal;
  int block;

  /* Emission de la requete */
  if (!ESSAY_ACTIVITY_ON(eid,acti)) {

    /* Confirmation de l'envoie et du mode de lecture */
    if (!silent) {

      /* On n'envoie pas */
      if (!essayConfirmExecRqst(&waitFinal, eid))
	return FALSE;

      /* On lira la reponse en mode bloquant */
      if (waitFinal)
	block = BLOCK_ON_FINAL_REPLY;

      /* On lira la reponse en mode non bloquant */
      else
	block = NO_BLOCK;
    }

    /* Pas d'IO : mode bloquant par defaut */
    else
      block = BLOCK_ON_FINAL_REPLY;

    /* Emettre la requete */
    if (!essaySendRqst(eid,rqstNum,acti))
      return FALSE;
  
    /* Reception de la replique intermediaire */
    if (essayRcvReply(eid, acti, BLOCK_ON_INTERMED_REPLY, localFail))
      return TRUE; /* On a deja la replique finale */
  }

  /* Sinon c'est qu'on attend une juste reponse et 
     c'est generalement en mode NO_BLOCK (vider la BAL) */
  else {
    waitFinal = TRUE;
    block = NO_BLOCK;
  }

  /* Attente de la réponse finale */
  if (waitFinal && ESSAY_ACTIVITY_ON(eid,acti)) {
    return (essayRcvReply(eid, acti, block, localFail));
  }
  return FALSE;
}

/**
 ** essaySendAndGetInput  --  requete pour recupere les inputs :
 **                           envoie de requete et lecture de reponse
 **                           de facon "transparente" (pas d'IO)
 **
 ** Utilise' par le pointeur sur fonction ESSAY_RQST_FUNC.
 **
 **/

BOOL 
essaySendAndGetInput(ESSAY_STR *eid, int rqstGet, int acti, void *output)
{
  int rqstNum = essayRqstType2RqstIndex(eid, rqstGet);

  if (rqstNum == -1) {
    printf ("Essay error: unknown request %d\n", rqstGet);
    return FALSE;
  }
  if (!essayAllocData(eid, rqstNum))
    return FALSE;

  if((*ESSAY_RQST_FUNC(eid,rqstNum))(eid,rqstNum,acti, ESSAY_RCV_REPLY_SILENT))
    memcpy(output, ESSAY_RQST_OUTPUT(eid,rqstNum), 
	   ESSAY_RQST_OUTPUT_SIZE(eid,rqstNum));

  return TRUE;
}

/**
 ** essayPrintGetInputMenu  --  Affichage du menu pour recuperer les input
 ** 
 ** retourne TRUE s'il faut lire un input.
 **/

BOOL 
essayPrintGetInputMenu(ESSAY_STR *essayId, int nbItems,
		      const char **menu, int rqstNum)
{
  int i;
  int cmd = ESSAY_RQST_GET_CMD(essayId,rqstNum);
  char yesChar, getChar;

  if(cmd >= 0 && cmd < nbItems) 
    yesChar = 'y';
  else 
    yesChar = 'n';
   
  /* Une seule fonction de lecture disponible */
  if (nbItems == 1) {
    printf ("Get current %s using %s (y/n) ? (%c) ", 
	    menu[0], menu[1], yesChar);
    if (h2scanf ("%c\n", &getChar) == ERROR) {
      ESSAY_RQST_GET_CMD(essayId,rqstNum) = -1;
      return FALSE;
    }
    if (getChar == 'y' || (getChar != 'n' && yesChar == 'y')) {
      ESSAY_RQST_GET_CMD(essayId,rqstNum) = 0;
      return TRUE;
    }
    ESSAY_RQST_GET_CMD(essayId,rqstNum) = -1;
    return FALSE;
  }

  /* Selection de la fonction par menu */
  printf ("Get current %s (y/n) ? (%c) ", menu[0], yesChar);
  if (h2scanf ("%c", &getChar) == ERROR) 
	essayExitH2ScanfError(essayId);
  if (getChar == 'n' || (getChar != 'y' && yesChar == 'n')) {
    ESSAY_RQST_GET_CMD(essayId,rqstNum) = -1;
    return FALSE;
  }

  printf ("\t\t------------------------------------\n");
  for (i=0; i<nbItems; i++) {
    printf("\t\t%d:  %s\n", i, menu[i+1]);
  }
  printf("\t\tother: don't get\n");
  printf ("\t\t------------------------------------\n");
  printf ("Select function > (%d)  ", cmd);
  if (h2scanf ("%d", &cmd) == ERROR) {
    ESSAY_RQST_GET_CMD(essayId,rqstNum) = -1;
    return FALSE;
  }
  if(cmd >= 0 && cmd < nbItems) {
    ESSAY_RQST_GET_CMD(essayId,rqstNum) = cmd;
    return TRUE;
  }
  ESSAY_RQST_GET_CMD(essayId,rqstNum) = -1;
  return FALSE;
}


/* ------------------ FONCTIONS D'AFFICHAGE DES MENUS ------------------ */

/**
 ** essayGetMenuSizes  -  Calcul du nombre de lignes et colonnes d'un menu
 **                      selon la taille du plus grand element et de la 
 **                      fenetre.
 **/

static int 
essayGetMenuSizes(ESSAY_STR *eid, int *nblin, int *colwidth)
{
  int i, len, nbcol;
  int max=0;

  for (i=0; i<ESSAY_NB_RQST(eid); i++) {
    len = strlen(ESSAY_RQST_NAME(eid,i));
    if (len > max)
      max = len;
  }

  nbcol = ESSAY_TERM_WIDTH/(max+ESSAY_OFFSET_MENU);
  if (nbcol == 0) 
    nbcol = 1;
  *colwidth = ESSAY_TERM_WIDTH/nbcol - ESSAY_OFFSET_MENU;

  *nblin = (ESSAY_NB_RQST(eid)+nbcol-1)/nbcol;

  return nbcol;
}

/**
 ** essayPrintMainMenu  -  Affichage du menu principal
 **/

static int 
essayPrintMainMenu(ESSAY_STR *eid, int nb_col, int nb_lin, int colwidth) 
{
  int i, j;
  int cmd = ESSAY_MAIN_CMD(eid);

  while (1) {
    printf ("\n------------------------------------------------------------------------\n");
    for (i=0; i<nb_lin; i++) {
      for (j=0; j<nb_col && i+j*nb_lin<ESSAY_NB_RQST(eid); j++) 
	printf(" %2d: %-*s", j*nb_lin+i, 
	       j==nb_col-1?0:colwidth,
	       ESSAY_RQST_NAME(eid,j*nb_lin+i));
      printf("\n");
    }
    
    printf ("\n");
    printf ("55: posters   ");
    printf ("66: abort  ");
    printf ("77: replies(%d)   ", ESSAY_NB_ACTIVITY_ON(eid));
    printf ("88: state  ");
    printf ("99: QUIT   ");
    printf ("-99: END\n");
    printf ("------------------------------------------------------------------------\n");
    
    printf ("%s%d (%d)> ", 
	    ESSAY_NAME(eid), ESSAY_NUM(eid), ESSAY_MAIN_CMD(eid));
    
    h2scanf ("%d", &cmd);

    printf ("\n");
      
    /* Valid command number  */
    if (cmd == 55 || cmd == 66 || cmd == 77 || cmd == 88 || 
	cmd == 99 || cmd == -99 ||
	(cmd >= 0 && cmd < ESSAY_NB_RQST(eid))) {
      ESSAY_MAIN_CMD(eid) = cmd;
      return cmd;
    }

    /* Unvalid command number */
    printf("    Try again !\n");
  }

}

/**
 ** essayPrintPosterMenu  -  Affichage du menu des posters
 **/

static void 
essayPrintPosterMenu(ESSAY_STR *eid)
{
  int i; 
  int cmd = ESSAY_POSTER_CMD(eid);

  while (1) {
    printf ("\t--------------------------------------------------\n");
    for (i=0; i<ESSAY_NB_POSTERS(eid); i++) {
      printf("\t\t%d:  %s\n", i, ESSAY_POSTER_NAME(eid,i));
    }
    printf("\t\tother: main menu\n");
    printf ("\t--------------------------------------------------\n");
    printf ("\n%s%d poster (%d)> ", 
	    ESSAY_NAME(eid), ESSAY_NUM(eid), ESSAY_POSTER_CMD(eid));
    h2scanf ("%d", &cmd);
    printf("\n");
    
    /* Valid command number: executs it */
    if(cmd >= 0 && cmd  <ESSAY_NB_POSTERS(eid)) {
      ESSAY_POSTER_CMD(eid) = cmd;
      (*ESSAY_POSTER_SHOW_FUNC(eid,cmd))();
    }
    else
      return;
  }
}

static void 
essayPosterShow(ESSAY_STR *eid, int poster)
{
  (*ESSAY_POSTER_SHOW_FUNC(eid,poster))();
}


/* ------------- FONCTIONS GENERALES DE GESTION DES REQUETES --------------- */

/**
 ** essaySendRequest - Envoie des requetes de controle et d'execution
 **                    + lecture en mode bloquant pour les requetes de 
 **                    controle. Selon demande pour les autres (IO).
 **/

static void 
essaySendRequest(ESSAY_STR *eid)
{
  int rqstNum;
  int activity;

  /* Requete demandee */
  rqstNum = ESSAY_MAIN_CMD(eid);

  /* Recherche d'une requete d'exec libre */
  if (ESSAY_NB_ACTIVITY_ON(eid) >= CLIENT_NMAX_RQST_ID)  {
    printf ("** Too many requests on !\n.");
    return;
  }


  /* Remarque: 
   *   on attribue meme aux requetes de controle une activite temporaire
   *   afin de les gerer de la meme facon que les requetes d'exec 
   */
  activity = 0;
  while(activity < CLIENT_NMAX_RQST_ID && ESSAY_ACTIVITY_ON(eid,activity))
    activity++;
  if (activity == CLIENT_NMAX_RQST_ID) {
    printf ("** Too many requests on !\n.");
    return;
  }

  if (!essayAllocData(eid, rqstNum))
    return;

  /* Appel de la fonction de saisie des parametres ET d'emission de la requete
   *
   * Cette fonction specifique pour chaque requete (a cause de la saisie
   * et de l'affichage des parametres) utilise tout de meme les fonctions
   * communes essaySendAndAckExecRqst, essaySendAndRcvCntrlRqst,
   * essaySendRqst, essayRcvReply, ...
   */
  (*ESSAY_RQST_FUNC(eid,rqstNum))(eid,rqstNum,activity, ESSAY_RCV_REPLY_IO);
}


/**
 ** essayGetReplies  --  Lecture des repliques des requetes d'execution
 **                      Utilise' pour lire les reponses en attente dans 
 **                      la BAL suite a une requete d'exec en mode NO_BLOCK
 **/

static void 
essayGetReplies(ESSAY_STR *eid)
{
  int acti;
  int rqstNum;

  if (ESSAY_NB_ACTIVITY_ON(eid) == 0) {
    printf ("No request on\n");
    return;
  }
  
  /* On fait le tour des requetes en cours */
  for (acti=0; acti < CLIENT_NMAX_RQST_ID; acti++) {
    if (ESSAY_ACTIVITY_ON(eid, acti)) {

      rqstNum = ESSAY_ACTIVITY_RQST_NUM(eid, acti);

      /* Lecture de la replique */
      printf ("Request %s activity %d:  ", 
	      ESSAY_RQST_NAME(eid, rqstNum),
	      ESSAY_ACTIVITY_NUM(eid,acti));
      
      (*ESSAY_RQST_FUNC(eid,rqstNum))(eid,rqstNum,acti, ESSAY_RCV_REPLY_IO);
    }
    
  } /* for */

}

/**
 **  essayStopModule  --  Arrête le module
 **
 **/

static void 
essayStopModule(ESSAY_STR *eid)
{
  int activity, rqstId, status, bilan;

  /* "Activite" a interrompre */
  activity = GENOM_END_MODULE;

  /* Emettre la requete */
  if (csClientRqstSend (ESSAY_CID(eid), ESSAY_RQST_ABORT(eid),
			(void *) &activity,
			sizeof(int), (FUNCPTR) NULL, FALSE, 0, 
			ESSAY_TIME_WAIT_REPLY, &rqstId) == ERROR) {
    h2perror("essayAbortRqst csClientRqstSend");
    return;
  }
  
  /* Reception de la replique */
  status = csClientReplyRcv (ESSAY_CID(eid), rqstId, BLOCK_ON_FINAL_REPLY, 
			     (void *) NULL, 0, (FUNCPTR) NULL, 
			     (void *) NULL, 0, (FUNCPTR) NULL);
  
  bilan = errnoGet();
  h2perror("Final reply of request Abort");

  /* Requete d'abort DU MODULE qui a reussi */
  if (status == FINAL_REPLY_OK && bilan == OK) 
    essayEnd(eid);
}


/**
 **  essayAbortRqst  --  Envoi de la requete d'interruption
 **
 **/

static void 
essayAbortRqst(ESSAY_STR *eid)
{
  int rqstId;
  int activity;
  int status;
  int bilan;

  /* Affiche les activite en cours */
  essayPosterShow(eid, ESSAY_POSTER_SHOW_CNTRL_ACTI(eid));

  printf ("Activity to abort (or return): ");

  /* Nothing to abort */
  if(h2scanf("%d", &activity) != 1)
    return;

  /* Emettre la requete */
  if (csClientRqstSend (ESSAY_CID(eid), ESSAY_RQST_ABORT(eid), 
			(void *) &activity,
			sizeof(int), (FUNCPTR) NULL, FALSE, 0, 
			ESSAY_TIME_WAIT_REPLY, &rqstId) == ERROR) {
    h2perror("essayAbortRqst csClientRqstSend");
    return;
  }
  
  /* Reception de la replique */
  status = csClientReplyRcv (ESSAY_CID(eid), rqstId, BLOCK_ON_FINAL_REPLY, 
			     (void *) NULL, 0, (FUNCPTR) NULL, 
			     (void *) NULL, 0, (FUNCPTR) NULL);
  
  bilan = errnoGet();
  h2perror("Final reply of request Abort");

  /* Requete d'abort DU MODULE qui a reussi */
  if (activity == GENOM_END_MODULE && status == FINAL_REPLY_OK && bilan == OK) 
    essayEnd(eid);
}


/* ---- FONCTIONS LOCALES DE GESTION D'UNE REQUETE (interface csLib) ------- */

/**
 ** essaySendRqst  --  emission d'une requete
 ** 
 **  Retourne TRUE si requete correctement envoyée
 **  et FALSE sinon
 **/

static BOOL 
essaySendRqst(ESSAY_STR *eid, int rqstNum, int acti)
{
  if (csClientRqstSend (ESSAY_CID(eid),
			ESSAY_RQST_TYPE(eid,rqstNum),
			ESSAY_RQST_INPUT(eid,rqstNum),
			ESSAY_RQST_INPUT_SIZE(eid,rqstNum),
			(FUNCPTR) NULL, TRUE, 
			ESSAY_TIME_WAIT_REPLY, 0, 
			&ESSAY_ACTIVITY_RQST_ID(eid,acti)) == ERROR) {
    h2perror ("** Send request error");
    return FALSE;
  }

  /* On enregistre la requete */
  ESSAY_ACTIVITY_ON(eid,acti) = TRUE;
  ESSAY_ACTIVITY_RQST_NUM(eid, acti) = rqstNum;
  (ESSAY_NB_ACTIVITY_ON(eid))++;

  return TRUE;
}

/**
 ** essayRcvReply  --  reception de la reponse d'une requete
 ** 
 **  Retourne TRUE si reponse finale receptionnée 
 **  (ie, reponse finale receptionnee ou 
 **      block = BLOCK_ON_INTERMED_REPLY et reponse intermediaire receptionnee)
 **  et FALSE sinon
 **/

static BOOL
essayRcvReply(ESSAY_STR *eid, int acti, int block,
	     GENOM_REPORTS_STRUCT *localFail)
{
  int bilan;
  int rqstNum;
  BOOL finalreply=FALSE;

  rqstNum = ESSAY_ACTIVITY_RQST_NUM(eid, acti);

  switch (csClientReplyRcv (ESSAY_CID(eid),
			    ESSAY_ACTIVITY_RQST_ID(eid,acti),
			    block,
			    (void *) &ESSAY_ACTIVITY_NUM(eid,acti), 
			    sizeof(int), (FUNCPTR) NULL, 
			    ESSAY_RQST_OUTPUT(eid,rqstNum),
			    ESSAY_RQST_OUTPUT_SIZE(eid,rqstNum),
			    (FUNCPTR) NULL))
    {
      /* On attend toujours */
    case WAITING_FINAL_REPLY:
      if (block == BLOCK_ON_INTERMED_REPLY)
	printf("Activity %d started\n", ESSAY_ACTIVITY_NUM(eid,acti));
      else 
	printf("Wait final reply (activity %d)\n", 
	       ESSAY_ACTIVITY_NUM(eid,acti));
      return finalreply;

      /* On n'attend plus */
    case ERROR:
      bilan = errnoGet();
      if (H2_MODULE_ERR_FLAG(bilan))
	if (localFail)
	  printf("Final reply:\n        %s\n", 
		 h2getLocalMsgErrno(localFail, bilan));
	else
	  printf("Final reply:\n        %s\n", h2getMsgErrno(bilan));
      else 
	printf("Final reply ERROR:\n        %s\n", h2getMsgErrno(bilan));
      break;
    case FINAL_REPLY_TIMEOUT:
      printf ("Final reply: TIMEOUT\n");
      break;
    case FINAL_REPLY_OK:
      printf("Final reply: OK\n");
      finalreply = TRUE;
      break;
    default:
      printf("essayRcvReply ERROR: should not be there !\n");
    }

  /* On des-enregistre la requete */
  ESSAY_ACTIVITY_ON(eid,acti) = FALSE;
  (ESSAY_NB_ACTIVITY_ON(eid))--;
  return finalreply;
}


/* ---------------------- LOCAL FUNCTIONS ------------------------------- */

/**
 ** essayAllocData  -  Allocation des parametres (input, output) des requetes
 **/

static BOOL 
essayAllocData(ESSAY_STR *essay, int rqst)
{
  int inputSize = ESSAY_RQST_INPUT_SIZE(essay,rqst);
  int outputSize = ESSAY_RQST_OUTPUT_SIZE(essay,rqst);

  if(ESSAY_RQST_INPUT(essay,rqst) == NULL && inputSize != 0) {
    ESSAY_RQST_INPUT(essay,rqst) = malloc(inputSize);
    if(ESSAY_RQST_INPUT(essay,rqst) == NULL) {
      printf ("Alloc input failed\n");
      return FALSE;
    }
    memset(ESSAY_RQST_INPUT(essay,rqst), 0, inputSize);
  }

  if(ESSAY_RQST_OUTPUT(essay,rqst) == NULL && outputSize != 0) {
    ESSAY_RQST_OUTPUT(essay,rqst) = malloc(outputSize);
    if(ESSAY_RQST_OUTPUT(essay,rqst) == NULL) {
      if (ESSAY_RQST_INPUT(essay,rqst) != NULL) {
	free(ESSAY_RQST_INPUT(essay,rqst));
	ESSAY_RQST_INPUT(essay,rqst) = NULL;
      }
      printf ("Alloc output failed\n");
      return FALSE;
    }
    memset(ESSAY_RQST_OUTPUT(essay,rqst), 0, outputSize);
  }

  return TRUE;
}

/**
 ** essayConfirmCntrlRqst  -- confirmation de l'envoi d'une requete de controle
 **/
static BOOL 
essayConfirmCntrlRqst(ESSAY_STR *eid)
{
  char yesChar;

  printf ("(`enter' to send, `a' to abort) ");
  h2scanf ("%c", &yesChar);
  printf ("\n");
  if (yesChar == 'a') {
    printf ("** Request aborted\n");
    return FALSE;
  }
  return TRUE;
}

/**
 ** essayConfirmExecRqst  -- confirmation de l'envoi d'une requete d'exec
 **/
static BOOL 
essayConfirmExecRqst(BOOL *waitFinal, ESSAY_STR *eid)
{
  char yesChar;
  int res;

  do
    printf("Wait final reply (y/n/a) ? : ");
  while ((res = h2scanf("%c", &yesChar)) != 1 ||
	 ((yesChar != 'y') && (yesChar != 'n') && (yesChar != 'a')));

  if (res == ERROR) 
    essayExitH2ScanfError(eid);

  printf ("\n");
  /* Abort */
  if (yesChar == 'a') {
    printf ("** Request aborted\n");
    return FALSE;
  }
  if (yesChar == 'y')
    *waitFinal = TRUE;
  else
    *waitFinal = FALSE;
  return TRUE;
}

/**
 ** essayRqstType2RqstIndex  -- Retourne le numéro d'une requête (ie, son
 **                             indice dans les tableaux de description)
 **                             à partir de son type (-1 si pas trouvé)
 **/
static int
essayRqstType2RqstIndex(ESSAY_STR *essay, int rqstType)
{
  int i;
  for (i=0; i<ESSAY_NB_RQST(essay); i++)
    if (ESSAY_RQST_TYPE(essay,i) == rqstType)
      return i;
  return -1;
}

/**
 **  essayExitH2ScanfError
 ** 
 **/
static void
essayExitH2ScanfError(ESSAY_STR *eid)
{
  printf ("%s%d: h2scanf ERROR -> exit\n", 
	  ESSAY_NAME(eid), ESSAY_NUM(eid));
  fprintf (stderr, "%s%d: h2scanf ERROR -> exit\n", 
	   ESSAY_NAME(eid), ESSAY_NUM(eid));
  essayEnd(eid);
}
