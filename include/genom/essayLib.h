#ifndef ESSAY_LIB_H
#define ESSAY_LIB_H

/* Etat des requetes en cours */
/* Normalement de sert que pour les requetes d'exec mais
   en pratique utilise aussi pour les requetes de controle 
   pour uniformiser la gestion des requetes */
typedef struct ESSAY_ACTI_STR {
  BOOL on;            /* Flag activity on */
  int rqstNum;        /* Numero de requete dans le tableau de description */
  int rqstId;         /* Identificateur de requete (csLib) */
  int num;            /* Numero de l'activite (pour le module) */
} ESSAY_ACTI_STR;

/* Type des fonctions de gestion des requetes */
typedef struct ESSAY_STR *ESSAY_STR_ID;
typedef BOOL (*ESSAY_RQST_FUNC_PTR)(ESSAY_STR_ID, int, int, BOOL);

/* Mode d'envoi de requetes */
#define ESSAY_RCV_REPLY_IO 0
#define ESSAY_RCV_REPLY_SILENT 1

/* Description des requetes : partie commune (static)  */
typedef struct ESSAY_RQST_DESC_STR {
  int rqstType;               /* Numero de requete */
  ESSAY_RQST_FUNC_PTR func;   /* Fonction pour envoyer/lire la requete */
  int inputSize;              /* Taille et pointeur sur input */
  int outputSize;             /* Taille et pointeur sur output */
} ESSAY_RQST_DESC_STR;

/* Description des requetes : partie dynamique  */
typedef struct ESSAY_RQST_STR {
  int getInputCmd;            /* Commande de lecture de l'input */
  void *input;
  void *output;
} ESSAY_RQST_STR;

/* Type des fonctions de gestion des posters */
typedef STATUS (*ESSAY_POSTER_SHOW_FUNC_PTR)(void);

/* Description du module : partie commune (static) */
typedef struct ESSAY_STATIC_STR {
  char *name;
  int abortRqstNum;
  int nbRqstType;                 /* Nombre de requetes definies */
  ESSAY_RQST_DESC_STR *rqst;      /* Tableau de description des requetes */
  char **rqstName;                /* Tableau des noms des requetes */
  int nbPosters;
  char **posterName;              /* Tableau des noms des posters */
  ESSAY_POSTER_SHOW_FUNC_PTR *posterShowFuncPtr;
} ESSAY_STATIC_STR;

/* Description du module */
typedef struct ESSAY_STR {
  ESSAY_STATIC_STR mDesc; /* Description du module */
  CLIENT_ID essayCId;     /* Id client csLib */
  int essayNumber;        /* Numero de l'essay */
  int mainCmd;            /* Fonction a executer */
  int posterCmd;          /* Fonction a executer */
  int nbRqstOn;           /* Nombre de requete en cours */
  ESSAY_RQST_STR *rqst;   /* Partie dynamique des requetes (input,output) */
  ESSAY_ACTI_STR acti[CLIENT_NMAX_RQST_ID];  /* Les requetes en cours */
} ESSAY_STR;

/* Acces aux données static du module */
#define ESSAY_NAME(id)                    ((id)->mDesc.name)

#define ESSAY_NB_RQST(id)                 ((id)->mDesc.nbRqstType)
#define ESSAY_RQSTS_NAME(id)              ((id)->mDesc.rqstName)
#define ESSAY_RQST_NAME(id,r)             ((id)->mDesc.rqstName[(r)])
#define ESSAY_RQST_ABORT(id)              ((id)->mDesc.abortRqstNum)
#define ESSAY_RQST_DESC(id)               ((id)->mDesc.rqst)
#define ESSAY_RQST_TYPE(id,r)             ((id)->mDesc.rqst[(r)].rqstType)
#define ESSAY_RQST_INPUT_SIZE(id,r)       ((id)->mDesc.rqst[(r)].inputSize)
#define ESSAY_RQST_OUTPUT_SIZE(id,r)      ((id)->mDesc.rqst[(r)].outputSize)
#define ESSAY_RQST_FUNC(id,r)             ((id)->mDesc.rqst[(r)].func)

#define ESSAY_NB_POSTERS(id)              ((id)->mDesc.nbPosters)
#define ESSAY_POSTERS_NAME(id)            ((id)->mDesc.posterName)
#define ESSAY_POSTER_NAME(id,p)           ((id)->mDesc.posterName[(p)])
#define ESSAY_POSTER_SHOW_FUNCS(id)       ((id)->mDesc.posterShowFuncPtr)
#define ESSAY_POSTER_SHOW_FUNC(id,p)      ((id)->mDesc.posterShowFuncPtr[(p)])
#define ESSAY_POSTER_SHOW_CNTRL(id)       (ESSAY_NB_POSTERS(id))
#define ESSAY_POSTER_SHOW_CNTRL_ACTI(id)  (ESSAY_NB_POSTERS(id)+1)

/* Acces aux données dynamiques de la tache d'essay */
#define ESSAY_CID(id)                     ((id)->essayCId)
#define ESSAY_NUM(id)                     ((id)->essayNumber)
#define ESSAY_MAIN_CMD(id)                ((id)->mainCmd)
#define ESSAY_POSTER_CMD(id)              ((id)->posterCmd)

#define ESSAY_NB_ACTIVITY_ON(id)          ((id)->nbRqstOn)
#define ESSAY_ACTIVITY_ON(id,r)           ((id)->acti[(r)].on)
#define ESSAY_ACTIVITY_RQST_NUM(id,r)     ((id)->acti[(r)].rqstNum)
#define ESSAY_ACTIVITY_RQST_ID(id,r)      ((id)->acti[(r)].rqstId)
#define ESSAY_ACTIVITY_NUM(id,r)          ((id)->acti[(r)].num) 

#define ESSAY_RQSTS(id)                   ((id)->rqst)
#define ESSAY_RQST_INPUT(id,r)            ((id)->rqst[(r)].input)
#define ESSAY_RQST_OUTPUT(id,r)           ((id)->rqst[(r)].output)
#define ESSAY_RQST_GET_CMD(id,r)          ((id)->rqst[(r)].getInputCmd)


#define ESSAY_TERM_WIDTH 78
#define ESSAY_OFFSET_MENU 6

#ifdef UNIX
#define ESSAY_TIME_WAIT_REPLY           /*10*/ 0
#else
#define ESSAY_TIME_WAIT_REPLY           /*2000*/ 0
#endif

#include "essayLibProto.h"

#endif
