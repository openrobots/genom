/****************************************************************************/
/*   LABORATOIRE D'AUTOMATIQUE ET D'ANALYSE DE SYSTEMES - LAAS / CNRS       */
/*   PROJET HILARE II - BIBLIOTHEQUE DES ERREURS                           */
/*   FICHIER D'EN-TETE "h2errorLib.c"                                      */
/****************************************************************************/
 
/* VERSION ACTUELLE / HISTORIQUE DES MODIFICATIONS :
   version 1.1; Dec92; sf; 1ere version;
*/
 
/* DESCRIPTION :
   Fichier d'en-tete de la bibliotheque de fonctions qui permettent 
   l'utilisation de l'agent locamotion par les autres agents du systeme,
   au moyen de messages.
*/

/*-------------------------- INCLUSIONS ------------------------------------*/
 
#ifdef VXWORKS
#include <vxWorks.h>
#include <stdioLib.h>
#include <string.h>
#else
#include <portLib.h>
#include <stdio.h>
#endif
 
#include "h2errorLib.h"
#include "genom/genomReportsLib.h"
#include "genomReportsStd.h"

/*------------------------- VARIABLES GLOBALES --------------------------*/
/* message d'erreur */
char h2msgLocalErrno[1024];

/*--------------------------------------------------------------------------*/

/*****************************************************************************
 *
 *  h2printLocalErrno - 
 *
 *  Description: Affiche le message d'erreur correspondant au numero d'erreur
 */
 
void h2printLocalErrno(GENOM_REPORTS_STRUCT *failuresMsg, int numErr)
{

  printf(h2getLocalMsgErrno(failuresMsg, numErr));
  printf("\n");

}


/*****************************************************************************
 *
 *  h2getLocalMsgErrno - Obtention d'un message d'erreur
 *
 *  Description: Retourne un char* sur la chaine de caractere 
 *               correspondant au numero d'erreur. 
 */

char const * h2getLocalMsgErrno(GENOM_REPORTS_STRUCT *failuresMsg,
				int error)

{
  int errorId;
  int i=0;
  int numErr;
  int testNum;
  H2_FAILED_STRUCT *tabErr;
  int nbErrs;

  /* OK */
  if (error == 0) {
    sprintf (h2msgLocalErrno, "OK");
    return(h2msgLocalErrno);
  }

  /* Ce n'est pas ce module */
  testNum = H2_SOURCE_ERR(error) - failuresMsg->numModule;
  if (testNum < 0 || testNum > 9)
    return h2getMsgErrno(error);

  /* Numéro d'erreur pour ce module */
  numErr = H2_NUMBER_ERR(error);

  /* Erreur standard */
  if (numErr >=0 && numErr<=8) {
    sprintf (h2msgLocalErrno, "%s: %s", 
	     failuresMsg->moduleName, GENOM_STD_TAB_FAIL[numErr].errorName); 
    return(h2msgLocalErrno);
  }

  /* Erreur locale */
  nbErrs = failuresMsg->nbFailures;
  tabErr = failuresMsg->tab;
  while (i < nbErrs && tabErr[i].errorNum != numErr)
    i++;
  errorId = i;
  
  /* Erreur inconnue */
  if (errorId == nbErrs) {
    sprintf (h2msgLocalErrno, "%s: unknown error %d", 
	     failuresMsg->moduleName, numErr); 
    return(h2msgLocalErrno);
  }
  
  /* Message trouve */
  sprintf (h2msgLocalErrno, "%s: %s", 
	   failuresMsg->moduleName, tabErr[errorId].errorName);
  return(h2msgLocalErrno);
  /*  return(tabErr[errorId].errorName);*/
}

