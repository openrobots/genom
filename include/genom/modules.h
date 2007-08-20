/****************************************************************************/
/*   LABORATOIRE D'AUTOMATIQUE ET D'ANALYSE DE SYSTEMES - LAAS / CNRS       */
/*   PROJET HILARE II -   Structure communes modules                        */
/*   FICHIER D'EN-TETE "modules.h"                                          */
/****************************************************************************/
 
/** 
 ** DESCRIPTION : 
 ** 
 ** Constantes et structures de la SDI_C communes a tous les modules
 **/

/* Indiquer le chargement du fichier */
#ifndef H_GENOM_MODULE
#define H_GENOM_MODULE

#ifndef NEW_MODULES
#define NEW_MODULES
#endif

#include <csLib.h>   /* Pour SERV_NMAX_RQST_ID (seule reference a csLib !) */
#include <posterLib.h> /* Pour POSTER_ID */

/**
 ** Etats et transitions des activites.
 **/

#define GENOM_NB_ACTIVITY_STATE 10

typedef enum {
  ETHER,          /* pas d'activite */
  INIT,           /* en attente de demarrage */
  START,          /* demande/execution du start */
  EXEC,           /* demande/execution */
  END,            /* demande/execution du end */
  FAIL,           /* demande/execution du fail (end) */
  INTER,          /* demande/execution de l'interuption (end) */
  ZOMBIE,         /* suspendue apres echec */
  NO_EVENT,       /* pas d'evenement */
  SLEEP           /* en attente d'un evenement externe */
} ACTIVITY_STATE, ACTIVITY_EVENT;


typedef enum GENOM_BOOL {
	GENOM_TRUE=TRUE,
	GENOM_FALSE=FALSE
}GENOM_BOOL;

typedef enum GENOM_STATUS {
	GENOM_OK = OK,
	GENOM_ERROR = ERROR
}GENOM_STATUS;

/**
 ** Les composants des SDI_C
 **
 ** (Les SDI_C different par le nombre de tache d'exec - cf xxxPosterLib.h)
 **
 **/

/* Nombre max d'activites: nombre max de requetes(csLib) - 1 */
#define MAX_ACTIVITIES (SERV_NMAX_RQST_ID - 1)

/* Nombre max de serveurs et de posters dont le module est client
 *
 * On peut les augmenter en respectant: . des multiples de 2 
 *                                      . MAX_SERVERS < CLIENT_NMAX_RQST_ID
 */
#define MAX_SERVERS    4
#define MAX_POSTERS    16     /* ATTENTION: verifier coherence avec genom.h */

/* Numéro d'abort pour arrêter le module */
#define GENOM_END_MODULE -99
/* Numéro d'abort pour débloquer les tâches d'exec suspendues */
#define GENOM_RESUME_EXEC_TASK -88
/* time */
#define GENOM_PRINT_TIME_RQST -77
/* verbose */
#define GENOM_VERBOSE_RQST -66

/* Description d'une tache d'execution */
typedef struct {
  int taskId;              /* (CONST.) Id de la tache (process) */
  STATUS status;           /* status de la tache OK ou ERROR */
  double period;           /* (CONST.) Periode requise (sec) */
  unsigned long maxPeriod; /* Periode max (msec) */
  unsigned long onPeriod;  /* Periode courante (msec) */
  unsigned long timeBeginLoop; /* starting time of the exec task loop */
  unsigned long timeEndLoop;   /* end time of the exec task loop */
  unsigned long durationLoop;  /* duration of last loop */
  unsigned long unused;        /* duration of last loop */
  int bilan;               /* bilan de la tache et de l'activite permanente */
  int currentActivity;     /* Activite courante */
  int wakeUp;              /* Eveil de la tache d'exec requis */
  int nbActivities;        /* Nombre d'activites en cours pour cette tache */
  POSTER_ID posterId[MAX_POSTERS];  /* (CONST.!) Posters de la tache d'exec */
  CLIENT_ID clientId[MAX_SERVERS];  /* (CONST.!) Serveurs qu'elle utilise */
} EXEC_TASK_DESCRIPTION;

/* Description d'une requete d'execution */
typedef struct {
  int rqstId;               /* Id de la requete (pour la communication) */
  int rqstNum;              /* Numero de la requete (caracterise sa nature) */
  int activityId;           /* Numero unique d'activite */
  ACTIVITY_STATE status;    /* Etat courant de l'activite correspondante */
  ACTIVITY_EVENT event;     /* Transition requise de l'activite */
  int bilan;                /* Bilan courant de l'activite */
  int execTaskNum;          /* (CONST.) Tache d'exec qui gere l'activite */
  int reentranceFlag;       /* (CONST.) Activite reentrante */
  void *outputRef;          /* Adresse du parametre de sortie */
  int outputSize;           /* (CONST.) Taille du parametre de sortie */
  void *inputRef;           /* Adresse du parametre d'entre */
  int inputSize;            /* (CONST.) Taille du parametre d'entre */
  int nbIncompatibles;      /* Nombre d'activites en cours incompatibles */
  int incompatibleActivities[MAX_ACTIVITIES];  /* Numeros de ces activites */
} ACTIVITY_DESCRIPTION;

/* Description de la tache de controle */
typedef struct {
  int taskId;              /* (CONST) Id de la tache (process) */
  STATUS status;           /* status de la tache de controle: OK ou ERROR */
  int bilan;               /* bilan de la tache de controle */
  int nbActivities;        /* Nombre total d'activites en cours */
  BOOL stopModule;         /* Flag pour arrêter le module */
  int verbose;             /* 0: quiet, other, verbose level */
  BOOL printTime;          /* Flag print times */
  int initRqst;            /* Numéro de la requête d'init ou -1 */
  int nbExecTasks;         /* Nombre de taches d'execution */
  void *sdif;              /* sdi_f adresse */
} CNTRL_TASK_DESCRIPTION;


/**
 ** Macros d'acces a une SDI_C
 **/

/* Les taches d'execution */
#define M_EXEC_TASK_ID(p,i)         ((p)->execTaskTab[(i)].taskId)
#define M_EXEC_TASK_STATUS(p,i)     ((p)->execTaskTab[(i)].status)
#define M_EXEC_TASK_PERIOD(p,i)     ((p)->execTaskTab[(i)].period)
#define M_EXEC_TASK_MAX_PERIOD(p,i) ((p)->execTaskTab[(i)].maxPeriod)
#define M_EXEC_TASK_ON_PERIOD(p,i)  ((p)->execTaskTab[(i)].onPeriod)
#define M_EXEC_TASK_TIME_BEGIN_LOOP(p,i)  ((p)->execTaskTab[(i)].timeBeginLoop)
#define M_EXEC_TASK_TIME_END_LOOP(p,i)  ((p)->execTaskTab[(i)].timeEndLoop)
#define M_EXEC_TASK_DURATION_LOOP(p,i)  ((p)->execTaskTab[(i)].durationLoop)
#define M_EXEC_TASK_BILAN(p,i)      ((p)->execTaskTab[(i)].bilan)
#define M_EXEC_TASK_NB_ACTI(p,i)    ((p)->execTaskTab[(i)].nbActivities)
#define M_EXEC_TASK_POSTER_ID(p,i)  ((p)->execTaskTab[(i)].posterId)
#define M_CURRENT_ACTIVITY_NUM(p,i) ((p)->execTaskTab[(i)].currentActivity)
#define M_EXEC_TASK_WAKE_UP_FLAG(p,i) ((p)->execTaskTab[(i)].wakeUp)

/* Les activites */
#define M_ACTIVITY_OUTPUT_ID(p,i)   ((p)->activityTab[(i)].outputRef)
#define M_ACTIVITY_OUTPUT_SIZE(p,i) ((p)->activityTab[(i)].outputSize)
#define M_ACTIVITY_INPUT_ID(p,i)    ((p)->activityTab[(i)].inputRef)
#define M_ACTIVITY_INPUT_SIZE(p,i)  ((p)->activityTab[(i)].inputSize)
#define M_ACTIVITY_RQST_ID(p,i)     ((p)->activityTab[(i)].rqstId)
#define M_ACTIVITY_RQST_TYPE(p,i)   ((p)->activityTab[(i)].rqstNum)
#define M_ACTIVITY_ID(p,i)          ((p)->activityTab[(i)].activityId)
#define M_ACTIVITY_TASK_NUM(p,i)    ((p)->activityTab[(i)].execTaskNum)
#define M_ACTIVITY_STATUS(p,i)      ((p)->activityTab[(i)].status)
#define M_ACTIVITY_EVN(p,i)         ((p)->activityTab[(i)].event)
#define M_ACTIVITY_BILAN(p,i)       ((p)->activityTab[(i)].bilan)
#define M_ACTIVITY_REENTRANCE(p,i)  ((p)->activityTab[(i)].reentranceFlag)
#define M_ACTIVITY_NB_INCOMP(p,i)   ((p)->activityTab[(i)].nbIncompatibles)
#define M_ACTIVITY_TAB_INCOMP(p,i)  ((p)->activityTab[(i)].incompatibleActivities)
 
/* La tache de controle */
#define M_CNTRL_TASK_ID(p)          ((p)->cntrlTask.taskId)
#define M_CNTRL_TASK_STATUS(p)      ((p)->cntrlTask.status)
#define M_CNTRL_TASK_BILAN(p)       ((p)->cntrlTask.bilan)
#define M_NB_ACTIVITIES(p)          ((p)->cntrlTask.nbActivities)
#define M_STOP_MODULE_FLAG(p)       ((p)->cntrlTask.stopModule)
#define M_GENOM_VERBOSE_LEVEL(p)    ((p)->cntrlTask.verbose)
#define M_GENOM_PRINT_TIME_FLAG(p)  ((p)->cntrlTask.printTime)
#define M_INIT_RQST(p)              ((p)->cntrlTask.initRqst)
#define M_CNTRL_NB_EXEC_TASKS(p)    ((p)->cntrlTask.nbExecTasks)
#define M_CNTRL_SDI_F(p)            ((p)->cntrlTask.sdif)
 
#include "genom/genomModulesLibProto.h"

#define M_GENOM_UNLOCK_SDI(p)       (genomGiveSDIs(p))
#define M_GENOM_LOCK_SDI(p)         (genomTakeSDIs(p))

#endif  /* H_GENOM_MODULE */
