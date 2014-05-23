
/* 
 * Copyright (c) 1993-2005 LAAS/CNRS
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

/*
 * Sara Fleury 
 *
 * Fonctions communes aux taches d'"test": affichage de menu, envoi de 
 * requetes, lecture de posters, ... . 
 *
 * Elles prennent (presque) toutes en paramètre un pointeur sur la 
 * structure TEST_STR qui comporte toutes info necessaires.
 * 
 *  REMARQUE: h2scanf ne peut plus retourner ERROR. Il faudra donc a 
 *            l'occasion supprimer les tests correspondants.
 */

/*----------------------------- INCLUSIONS ---------------------------------*/

/* Inclusions generales */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include <taskLib.h>
#include <errnoLib.h>
#include <portLib.h>

#include "csLib.h"
#include "h2errorLib.h"

#include "genom/modules.h"
#include "genom/testLib.h"

/*-------------------- PROTOTYPES FONCTIONS LOCALES -----------------------*/
static int  testGetMenuSizes     (TEST_STR *eid, int *nblin, int *colwidth);
static int  testPrintMainMenu    (TEST_STR *eid,
				  int nb_col, int nb_lin, int colwidth); 
static void testPrintPosterMenu  (TEST_STR *eid);
static void testPosterShow       (TEST_STR *eid, int poster);

static void testSendRequest(TEST_STR *eid);
static void testGetReplies (TEST_STR *eid);
static void testAbortRqst  (TEST_STR *eid);
static void testStopModule (TEST_STR *eid);

static BOOL testSendRqst  (TEST_STR *eid, int rqstNum, int acti);
static BOOL testRcvReply  (TEST_STR *eid, int acti, int block);
static BOOL testConfirmCntrlRqst (TEST_STR *eid);
static BOOL testConfirmExecRqst  (BOOL *waitFinal, TEST_STR *eid);
static BOOL testAllocData        (TEST_STR *test, int rqst);
static int  testRqstType2RqstIndex(TEST_STR *test, int rqstType);
static void testExitH2ScanfError(TEST_STR *eid);

static int h2scanf(const char *fmt, void *addr);


/* ----------------- FONCTIONS D'INIT ET DE TERMINAISON ------------------ */

/**
 ** testInit : Initialisation de la tache d'test: 
 **         allocation de la structure TEST_STR
 **         init des bal
 **/

TEST_STR *
testInit(int testNum, char *module, int mboxsize, int abortRqstNum,
	 int nbRqstType, char *rqstName[], TEST_RQST_DESC_STR *rqstDesc, 
	 int nbPosters, char *posterName[],
	 TEST_POSTER_SHOW_FUNC_PTR posterShowFunc[])
{
  TEST_STR *eid;
  char buf[30];              
  int i;

  eid = (TEST_STR *) malloc(sizeof(TEST_STR));
  if (eid == NULL) {
    printf ("testAlloc: cannot alloc\n");
    return eid;
  }

  /* Mettre la bufferisation a zero char */
  setbuf (stdin, NULL);
  setbuf (stdout, NULL);

  /* Copier le nom de base des mboxes */
  sprintf(buf, "t%s%d", module, testNum);

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
  TEST_NAME(eid) = module;

  TEST_NB_RQST(eid) = nbRqstType;
  TEST_RQST_DESC(eid) = rqstDesc;
  TEST_RQSTS_NAME(eid) = rqstName;
  TEST_RQST_ABORT(eid) = abortRqstNum;

  TEST_NB_POSTERS(eid) = nbPosters;
  TEST_POSTERS_NAME(eid) = posterName;
  TEST_POSTER_SHOW_FUNCS(eid) = posterShowFunc;

  /* Init des donnees dynamique de la tache d'test */
  TEST_NUM(eid) = testNum;
  TEST_MAIN_CMD(eid) = 88;
  TEST_POSTER_CMD(eid) = 0;

  /* Initialiser le tableau des requetes en cours */
  TEST_NB_ACTIVITY_ON(eid) = 0;
  if ((TEST_RQSTS(eid) = (TEST_RQST_STR *) 
       malloc(nbRqstType * sizeof(TEST_RQST_STR))) == NULL) {
    printf ("Cannot alloc rqst tab\n");
    free(eid);
    return NULL;
  }
  for (i=0; i<nbRqstType; i++) {
    TEST_RQST_INPUT(eid,i) = NULL;
    TEST_RQST_OUTPUT(eid,i) = NULL;
    TEST_RQST_GET_CMD(eid,i) = -1;
  }
  for (i=0; i<CLIENT_NMAX_RQST_ID; i++)
    TEST_ACTIVITY_ON(eid,i) = FALSE;

  return eid;
}


/**
 ** testEnd : Terminaison de la tache d'test: 
 **         libération de la structure TEST_STR
 **         libération des bal
 **/

void 
testEnd(TEST_STR *eid)
{
  int i;

  /* Termine les communication */
  csClientEnd(TEST_CID(eid));
  csMboxEnd();

  /* Libere la structure */
  for (i=0; i<TEST_NB_RQST(eid); i++) {
    if(TEST_RQST_INPUT(eid,i) != NULL)
      free(TEST_RQST_INPUT(eid,i));
    if(TEST_RQST_OUTPUT(eid,i) != NULL)
      free(TEST_RQST_OUTPUT(eid,i));
  }
  free(TEST_RQSTS(eid));
  free(eid);
  exit(0);
}

/* ------------------------- FONCTION PRINCIPALE ----------------------- */

void 
testMain(TEST_STR *eid)
{
  int nblin, nbcol, colwidth;

  nbcol = testGetMenuSizes(eid, &nblin, &colwidth);

  /* Boucler, en attente de commandes */
  while (1) {

      /* Imprimer le menu et recuperer la commande */
      errnoSet(0);
      testPrintMainMenu(eid, nbcol, nblin, colwidth);

      switch(TEST_MAIN_CMD(eid)) {
	  
	/* -- Fin */
      case 99:
	testEnd(eid);
	  
	/* -- Lecture du poster de controle */
      case 88:
	testPosterShow(eid, TEST_POSTER_SHOW_CNTRL(eid));
	break;
	
	/* -- Lecture des posters d'execution */
      case 55:
	testPrintPosterMenu(eid);
	break;
	
	/* -- Arret d'une activite */
      case 66:
	testAbortRqst(eid);
	break;
	
	/* -- Arret du module */
      case -99:
	testStopModule(eid);
	break;
	
	/* -- Affichage/Lecture des requetes en cours */
      case 77:
	testGetReplies(eid);
	break;
	
	/* -- Emission d'une requete */
      default:
	testSendRequest(eid);
	break;
	
      } /* switch */

    }	  /* FOREVER */
}


/* -------- FONCTIONS DE GESTION DES REQUETES DE CONTROLE ET D'EXEC  ------- */
/*        (appellees par les fonctions specifiques pour chaque requete:
	                  TEST_RQST_FUNC )                                 */

/**
 **  testSendAndRcvCntrlRqst  -- Envoi d'une requete de controle
 **                               toujours en mode BLOQUANT
 **
 ** Utilise' par le pointeur sur fonction TEST_RQST_FUNC.
 **
 **  Retourne TRUE si reponse finale receptionnée (y compris avec erreur)
 **  et FALSE sinon
 **/

BOOL 
testSendAndRcvCntrlRqst(TEST_STR *eid, int rqstNum, int acti,
			int silent)
{

  /* Confiration de l'envoie */
  if (!silent)
    if (!testConfirmCntrlRqst(eid))
      return FALSE;

  /* Emettre la requete */
  if (!testSendRqst(eid,rqstNum,acti))
    return FALSE;
  
  /* Reception de la replique */
  return testRcvReply(eid, acti, BLOCK_ON_FINAL_REPLY);
}


/**
 ** testSendAndOrRcvExecRqst  --  Envoi d'une requete d'execution
 **                                ou lecture de la reponse si deja envoyee
 **
 ** Utilise' par le pointeur sur fonction TEST_RQST_FUNC.
 **
 ** Retourne TRUE si reponse finale receptionnée (y compris avec erreur?)
 **  et FALSE sinon
 **/

BOOL 
testSendAndOrRcvExecRqst(TEST_STR *eid, int rqstNum, int acti,
			 int silent)
{
  BOOL waitFinal;
  int block;

  /* Emission de la requete */
  if (!TEST_ACTIVITY_ON(eid,acti)) {

    /* Confirmation de l'envoie et du mode de lecture */
    if (!silent) {

      /* On n'envoie pas */
      if (!testConfirmExecRqst(&waitFinal, eid))
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
    if (!testSendRqst(eid,rqstNum,acti))
      return FALSE;
  
    /* Reception de la replique intermediaire */
    if (testRcvReply(eid, acti, BLOCK_ON_INTERMED_REPLY))
      return TRUE; /* On a deja la replique finale */
  }

  /* Sinon c'est qu'on attend une juste reponse et 
     c'est generalement en mode NO_BLOCK (vider la BAL) */
  else {
    waitFinal = TRUE;
    block = NO_BLOCK;
  }

  /* Attente de la réponse finale */
  if (waitFinal && TEST_ACTIVITY_ON(eid,acti)) {
    return (testRcvReply(eid, acti, block));
  }
  return FALSE;
}

/**
 ** testSendAndGetInput  --  requete pour recupere les inputs :
 **                           envoie de requete et lecture de reponse
 **                           de facon "transparente" (pas d'IO)
 **
 ** Utilise' par le pointeur sur fonction TEST_RQST_FUNC.
 **
 **/

BOOL 
testSendAndGetInput(TEST_STR *eid, int rqstGet, int acti, void *output)
{
  int rqstNum = testRqstType2RqstIndex(eid, rqstGet);

  if (rqstNum == -1) {
    printf ("Test error: unknown request %d\n", rqstGet);
    return FALSE;
  }
  if (!testAllocData(eid, rqstNum))
    return FALSE;

  if((*TEST_RQST_FUNC(eid,rqstNum))(eid,rqstNum,acti, TEST_RCV_REPLY_SILENT))
    memcpy(output, TEST_RQST_OUTPUT(eid,rqstNum), 
	   TEST_RQST_OUTPUT_SIZE(eid,rqstNum));

  return TRUE;
}

/**
 ** testPrintGetInputMenu  --  Affichage du menu pour recuperer les input
 ** 
 ** retourne TRUE s'il faut lire un input.
 **/

BOOL 
testPrintGetInputMenu(TEST_STR *testId, int nbItems,
		      const char **menu, int rqstNum)
{
  int i;
  int cmd = TEST_RQST_GET_CMD(testId,rqstNum);
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
      TEST_RQST_GET_CMD(testId,rqstNum) = -1;
      return FALSE;
    }
    if (getChar == 'y' || (getChar != 'n' && yesChar == 'y')) {
      TEST_RQST_GET_CMD(testId,rqstNum) = 0;
      return TRUE;
    }
    TEST_RQST_GET_CMD(testId,rqstNum) = -1;
    return FALSE;
  }

  /* Selection de la fonction par menu */
  printf ("Get current %s (y/n) ? (%c) ", menu[0], yesChar);
  if (h2scanf ("%c", &getChar) == ERROR) 
	testExitH2ScanfError(testId);
  if (getChar == 'n' || (getChar != 'y' && yesChar == 'n')) {
    TEST_RQST_GET_CMD(testId,rqstNum) = -1;
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
    TEST_RQST_GET_CMD(testId,rqstNum) = -1;
    return FALSE;
  }
  if(cmd >= 0 && cmd < nbItems) {
    TEST_RQST_GET_CMD(testId,rqstNum) = cmd;
    return TRUE;
  }
  TEST_RQST_GET_CMD(testId,rqstNum) = -1;
  return FALSE;
}


/* ------------------ FONCTIONS D'AFFICHAGE DES MENUS ------------------ */

/**
 ** testGetMenuSizes  -  Calcul du nombre de lignes et colonnes d'un menu
 **                      selon la taille du plus grand element et de la 
 **                      fenetre.
 **/

static int 
testGetMenuSizes(TEST_STR *eid, int *nblin, int *colwidth)
{
  int i, len, nbcol;
  int max=0;

  for (i=0; i<TEST_NB_RQST(eid); i++) {
    len = strlen(TEST_RQST_NAME(eid,i));
    if (len > max)
      max = len;
  }

  nbcol = TEST_TERM_WIDTH/(max+TEST_OFFSET_MENU);
  if (nbcol == 0) 
    nbcol = 1;
  *colwidth = TEST_TERM_WIDTH/nbcol - TEST_OFFSET_MENU;

  *nblin = (TEST_NB_RQST(eid)+nbcol-1)/nbcol;

  return nbcol;
}

/**
 ** testPrintMainMenu  -  Affichage du menu principal
 **/

static int 
testPrintMainMenu(TEST_STR *eid, int nb_col, int nb_lin, int colwidth) 
{
  int i, j;
  int cmd = TEST_MAIN_CMD(eid);

  while (1) {
    printf ("\n------------------------------------------------------------------------\n");
    for (i=0; i<nb_lin; i++) {
      for (j=0; j<nb_col && i+j*nb_lin<TEST_NB_RQST(eid); j++) 
	printf(" %2d: %-*s", j*nb_lin+i, 
	       j==nb_col-1?0:colwidth,
	       TEST_RQST_NAME(eid,j*nb_lin+i));
      printf("\n");
    }
    
    printf ("\n");
    printf ("55: posters   ");
    printf ("66: abort  ");
    printf ("77: replies(%d)   ", TEST_NB_ACTIVITY_ON(eid));
    printf ("88: state  ");
    printf ("99: QUIT   ");
    printf ("-99: END\n");
    printf ("------------------------------------------------------------------------\n");
    
    printf ("%s%d (%d)> ", 
	    TEST_NAME(eid), TEST_NUM(eid), TEST_MAIN_CMD(eid));
    
    h2scanf ("%d", &cmd);

    printf ("\n");
      
    /* Valid command number  */
    if (cmd == 55 || cmd == 66 || cmd == 77 || cmd == 88 || 
	cmd == 99 || cmd == -99 ||
	(cmd >= 0 && cmd < TEST_NB_RQST(eid))) {
      TEST_MAIN_CMD(eid) = cmd;
      return cmd;
    }

    /* Unvalid command number */
    printf("    Try again !\n");
  }

}

/**
 ** testPrintPosterMenu  -  Affichage du menu des posters
 **/

static void 
testPrintPosterMenu(TEST_STR *eid)
{
  int i; 
  int cmd = TEST_POSTER_CMD(eid);

  while (1) {
    printf ("\t--------------------------------------------------\n");
    for (i=0; i<TEST_NB_POSTERS(eid); i++) {
      printf("\t\t%d:  %s\n", i, TEST_POSTER_NAME(eid,i));
    }
    printf("\t\tother: main menu\n");
    printf ("\t--------------------------------------------------\n");
    printf ("\n%s%d poster (%d)> ", 
	    TEST_NAME(eid), TEST_NUM(eid), TEST_POSTER_CMD(eid));
    h2scanf ("%d", &cmd);
    printf("\n");
    
    /* Valid command number: executs it */
    if(cmd >= 0 && cmd  <TEST_NB_POSTERS(eid)) {
      TEST_POSTER_CMD(eid) = cmd;
      (*TEST_POSTER_SHOW_FUNC(eid,cmd))();
    }
    else
      return;
  }
}

static void 
testPosterShow(TEST_STR *eid, int poster)
{
  (*TEST_POSTER_SHOW_FUNC(eid,poster))();
}


/* ------------- FONCTIONS GENERALES DE GESTION DES REQUETES --------------- */

/**
 ** testSendRequest - Envoie des requetes de controle et d'execution
 **                    + lecture en mode bloquant pour les requetes de 
 **                    controle. Selon demande pour les autres (IO).
 **/

static void 
testSendRequest(TEST_STR *eid)
{
  int rqstNum;
  int activity;

  /* Requete demandee */
  rqstNum = TEST_MAIN_CMD(eid);

  /* Recherche d'une requete d'exec libre */
  if (TEST_NB_ACTIVITY_ON(eid) >= CLIENT_NMAX_RQST_ID)  {
    printf ("** Too many requests on !\n.");
    return;
  }


  /* Remarque: 
   *   on attribue meme aux requetes de controle une activite temporaire
   *   afin de les gerer de la meme facon que les requetes d'exec 
   */
  activity = 0;
  while(activity < CLIENT_NMAX_RQST_ID && TEST_ACTIVITY_ON(eid,activity))
    activity++;
  if (activity == CLIENT_NMAX_RQST_ID) {
    printf ("** Too many requests on !\n.");
    return;
  }

  if (!testAllocData(eid, rqstNum))
    return;

  /* Appel de la fonction de saisie des parametres ET d'emission de la requete
   *
   * Cette fonction specifique pour chaque requete (a cause de la saisie
   * et de l'affichage des parametres) utilise tout de meme les fonctions
   * communes testSendAndAckExecRqst, testSendAndRcvCntrlRqst,
   * testSendRqst, testRcvReply, ...
   */
  (*TEST_RQST_FUNC(eid,rqstNum))(eid,rqstNum,activity, TEST_RCV_REPLY_IO);
}


/**
 ** testGetReplies  --  Lecture des repliques des requetes d'execution
 **                      Utilise' pour lire les reponses en attente dans 
 **                      la BAL suite a une requete d'exec en mode NO_BLOCK
 **/

static void 
testGetReplies(TEST_STR *eid)
{
  int acti;
  int rqstNum;

  if (TEST_NB_ACTIVITY_ON(eid) == 0) {
    printf ("No request on\n");
    return;
  }
  
  /* On fait le tour des requetes en cours */
  for (acti=0; acti < CLIENT_NMAX_RQST_ID; acti++) {
    if (TEST_ACTIVITY_ON(eid, acti)) {

      rqstNum = TEST_ACTIVITY_RQST_NUM(eid, acti);

      /* Lecture de la replique */
      printf ("Request %s activity %d:  ", 
	      TEST_RQST_NAME(eid, rqstNum),
	      TEST_ACTIVITY_NUM(eid,acti));
      
      (*TEST_RQST_FUNC(eid,rqstNum))(eid,rqstNum,acti, TEST_RCV_REPLY_IO);
    }
    
  } /* for */

}

/**
 **  testStopModule  --  Arrête le module
 **
 **/

static void 
testStopModule(TEST_STR *eid)
{
  int activity, rqstId, status, bilan;

  /* "Activite" a interrompre */
  activity = GENOM_END_MODULE;

  /* Emettre la requete */
  if (csClientRqstSend (TEST_CID(eid), TEST_RQST_ABORT(eid),
			(void *) &activity,
			sizeof(int), (FUNCPTR) NULL, FALSE, 0, 
			TEST_TIME_WAIT_REPLY, &rqstId) == ERROR) {
    h2perror("testAbortRqst csClientRqstSend");
    return;
  }
  
  /* Reception de la replique */
  status = csClientReplyRcv (TEST_CID(eid), rqstId, BLOCK_ON_FINAL_REPLY, 
			     (void *) NULL, 0, (FUNCPTR) NULL, 
			     (void *) NULL, 0, (FUNCPTR) NULL);
  
  bilan = errnoGet();
  if (bilan != OK) 
	  h2perror("Final reply of request Abort");

  /* Requete d'abort DU MODULE qui a reussi */
  if (status == FINAL_REPLY_OK && bilan == OK) 
    testEnd(eid);
}


/**
 **  testAbortRqst  --  Envoi de la requete d'interruption
 **
 **/

static void 
testAbortRqst(TEST_STR *eid)
{
  int rqstId;
  int activity;
  int status;
  int bilan;

  /* Affiche les activite en cours */
  testPosterShow(eid, TEST_POSTER_SHOW_CNTRL_ACTI(eid));

  printf ("Activity to abort (or return): ");

  /* Nothing to abort */
  if(h2scanf("%d", &activity) != 1)
    return;

  /* Emettre la requete */
  if (csClientRqstSend (TEST_CID(eid), TEST_RQST_ABORT(eid), 
			(void *) &activity,
			sizeof(int), (FUNCPTR) NULL, FALSE, 0, 
			TEST_TIME_WAIT_REPLY, &rqstId) == ERROR) {
    h2perror("testAbortRqst csClientRqstSend");
    return;
  }
  
  /* Reception de la replique */
  status = csClientReplyRcv (TEST_CID(eid), rqstId, BLOCK_ON_FINAL_REPLY, 
			     (void *) NULL, 0, (FUNCPTR) NULL, 
			     (void *) NULL, 0, (FUNCPTR) NULL);
  
  bilan = errnoGet();
  if (bilan != OK) 
	  h2perror("Final reply of request Abort");

  /* Requete d'abort DU MODULE qui a reussi */
  if (activity == GENOM_END_MODULE && status == FINAL_REPLY_OK && bilan == OK) 
    testEnd(eid);
}


/* ---- FONCTIONS LOCALES DE GESTION D'UNE REQUETE (interface csLib) ------- */

/**
 ** testSendRqst  --  emission d'une requete
 ** 
 **  Retourne TRUE si requete correctement envoyée
 **  et FALSE sinon
 **/

static BOOL 
testSendRqst(TEST_STR *eid, int rqstNum, int acti)
{
  if (csClientRqstSend (TEST_CID(eid),
			TEST_RQST_TYPE(eid,rqstNum),
			TEST_RQST_INPUT(eid,rqstNum),
			TEST_RQST_INPUT_SIZE(eid,rqstNum),
			(FUNCPTR) NULL, TRUE, 
			TEST_TIME_WAIT_REPLY, 0, 
			&TEST_ACTIVITY_RQST_ID(eid,acti)) == ERROR) {
    h2perror ("** Send request error");
    return FALSE;
  }

  /* On enregistre la requete */
  TEST_ACTIVITY_ON(eid,acti) = TRUE;
  TEST_ACTIVITY_RQST_NUM(eid, acti) = rqstNum;
  (TEST_NB_ACTIVITY_ON(eid))++;

  return TRUE;
}

/**
 ** testRcvReply  --  reception de la reponse d'une requete
 ** 
 **  Retourne TRUE si reponse finale receptionnée 
 **  (ie, reponse finale receptionnee ou 
 **      block = BLOCK_ON_INTERMED_REPLY et reponse intermediaire receptionnee)
 **  et FALSE sinon
 **/

static BOOL
testRcvReply(TEST_STR *eid, int acti, int block)
{
  int bilan;
  int rqstNum;
  BOOL finalreply=FALSE;

  rqstNum = TEST_ACTIVITY_RQST_NUM(eid, acti);

  switch (csClientReplyRcv (TEST_CID(eid),
			    TEST_ACTIVITY_RQST_ID(eid,acti),
			    block,
			    (void *) &TEST_ACTIVITY_NUM(eid,acti), 
			    sizeof(int), (FUNCPTR) NULL, 
			    TEST_RQST_OUTPUT(eid,rqstNum),
			    TEST_RQST_OUTPUT_SIZE(eid,rqstNum),
			    (FUNCPTR) NULL))
    {
      char str[68];
      /* On attend toujours */
    case WAITING_FINAL_REPLY:
      if (block == BLOCK_ON_INTERMED_REPLY)
	printf("Activity %d started\n", TEST_ACTIVITY_NUM(eid,acti));
      else 
	printf("Wait final reply (activity %d)\n", 
	       TEST_ACTIVITY_NUM(eid,acti));
      return finalreply;

      /* On n'attend plus */
    case ERROR:
      bilan = errnoGet();
      if (H2_MODULE_ERR_FLAG(bilan))
	printf("Final reply:\n        %s\n", 
	       h2getErrMsg(bilan, str, 68));
      else 
	printf("Final reply ERROR:\n        %s\n", h2getErrMsg(bilan, str, 68));
      break;
    case FINAL_REPLY_TIMEOUT:
      printf ("Final reply: TIMEOUT\n");
      break;
    case FINAL_REPLY_OK:
      printf("Final reply: OK\n");
      finalreply = TRUE;
      break;
    default:
      printf("testRcvReply ERROR: should not be there !\n");
    }

  /* On des-enregistre la requete */
  TEST_ACTIVITY_ON(eid,acti) = FALSE;
  (TEST_NB_ACTIVITY_ON(eid))--;
  return finalreply;
}


/* ---------------------- LOCAL FUNCTIONS ------------------------------- */

/**
 ** testAllocData  -  Allocation des parametres (input, output) des requetes
 **/

static BOOL 
testAllocData(TEST_STR *test, int rqst)
{
  int inputSize = TEST_RQST_INPUT_SIZE(test,rqst);
  int outputSize = TEST_RQST_OUTPUT_SIZE(test,rqst);

  if(TEST_RQST_INPUT(test,rqst) == NULL && inputSize != 0) {
    TEST_RQST_INPUT(test,rqst) = malloc(inputSize);
    if(TEST_RQST_INPUT(test,rqst) == NULL) {
      printf ("Alloc input failed\n");
      return FALSE;
    }
    memset(TEST_RQST_INPUT(test,rqst), 0, inputSize);
  }

  if(TEST_RQST_OUTPUT(test,rqst) == NULL && outputSize != 0) {
    TEST_RQST_OUTPUT(test,rqst) = malloc(outputSize);
    if(TEST_RQST_OUTPUT(test,rqst) == NULL) {
      if (TEST_RQST_INPUT(test,rqst) != NULL) {
	free(TEST_RQST_INPUT(test,rqst));
	TEST_RQST_INPUT(test,rqst) = NULL;
      }
      printf ("Alloc output failed\n");
      return FALSE;
    }
    memset(TEST_RQST_OUTPUT(test,rqst), 0, outputSize);
  }

  return TRUE;
}

/**
 ** testConfirmCntrlRqst  -- confirmation de l'envoi d'une requete de controle
 **/
static BOOL 
testConfirmCntrlRqst(TEST_STR *eid)
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
 ** testConfirmExecRqst  -- confirmation de l'envoi d'une requete d'exec
 **/
static BOOL 
testConfirmExecRqst(BOOL *waitFinal, TEST_STR *eid)
{
  char yesChar;
  int res;

  do
    printf("Wait final reply (y/n/a) ? : ");
  while ((res = h2scanf("%c", &yesChar)) != 1 ||
	 ((yesChar != 'y') && (yesChar != 'n') && (yesChar != 'a')));

  if (res == ERROR) 
    testExitH2ScanfError(eid);

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
 ** testRqstType2RqstIndex  -- Retourne le numéro d'une requête (ie, son
 **                             indice dans les tableaux de description)
 **                             à partir de son type (-1 si pas trouvé)
 **/
static int
testRqstType2RqstIndex(TEST_STR *test, int rqstType)
{
  int i;
  for (i=0; i<TEST_NB_RQST(test); i++)
    if (TEST_RQST_TYPE(test,i) == rqstType)
      return i;
  return -1;
}

/**
 **  testExitH2ScanfError
 ** 
 **/
static void
testExitH2ScanfError(TEST_STR *eid)
{
  printf ("%s%d: h2scanf ERROR -> exit\n", 
	  TEST_NAME(eid), TEST_NUM(eid));
  fprintf (stderr, "%s%d: h2scanf ERROR -> exit\n", 
	   TEST_NAME(eid), TEST_NUM(eid));
  testEnd(eid);
}

/*----------------------------------------------------------------------*/

static int 
h2scanf(const char *fmt, void *addr)
{
    static char buf[1024];
    int n;

    while (fgets(buf, sizeof(buf), stdin) == NULL) {
	if (errno != EINTR) {
	    return ERROR;
	}
    } /* while */
    n = sscanf(buf, fmt, addr);
    return(n < 0 ? 0 : n);
}
