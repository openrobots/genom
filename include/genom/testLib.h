#ifndef TEST_LIB_H
#define TEST_LIB_H

/* Etat des requetes en cours */
/* Normalement de sert que pour les requetes d'exec mais
   en pratique utilise aussi pour les requetes de controle 
   pour uniformiser la gestion des requetes */
typedef struct TEST_ACTI_STR {
  BOOL on;            /* Flag activity on */
  int rqstNum;        /* Numero de requete dans le tableau de description */
  int rqstId;         /* Identificateur de requete (csLib) */
  int num;            /* Numero de l'activite (pour le module) */
} TEST_ACTI_STR;

/* Type des fonctions de gestion des requetes */
typedef struct TEST_STR *TEST_STR_ID;
typedef BOOL (*TEST_RQST_FUNC_PTR)(TEST_STR_ID, int, int, BOOL);

/* Mode d'envoi de requetes */
#define TEST_RCV_REPLY_IO 0
#define TEST_RCV_REPLY_SILENT 1

/* Description des requetes : partie commune (static)  */
typedef struct TEST_RQST_DESC_STR {
  int rqstType;               /* Numero de requete */
  TEST_RQST_FUNC_PTR func;   /* Fonction pour envoyer/lire la requete */
  int inputSize;              /* Taille et pointeur sur input */
  int outputSize;             /* Taille et pointeur sur output */
} TEST_RQST_DESC_STR;

/* Description des requetes : partie dynamique  */
typedef struct TEST_RQST_STR {
  int getInputCmd;            /* Commande de lecture de l'input */
  void *input;
  void *output;
} TEST_RQST_STR;

/* Type des fonctions de gestion des posters */
typedef STATUS (*TEST_POSTER_SHOW_FUNC_PTR)(void);

/* Description du module : partie commune (static) */
typedef struct TEST_STATIC_STR {
  char *name;
  int abortRqstNum;
  int nbRqstType;                 /* Nombre de requetes definies */
  TEST_RQST_DESC_STR *rqst;      /* Tableau de description des requetes */
  char **rqstName;                /* Tableau des noms des requetes */
  int nbPosters;
  char **posterName;              /* Tableau des noms des posters */
  TEST_POSTER_SHOW_FUNC_PTR *posterShowFuncPtr;
} TEST_STATIC_STR;

/* Description du module */
typedef struct TEST_STR {
  TEST_STATIC_STR mDesc; /* Description du module */
  CLIENT_ID testCId;     /* Id client csLib */
  int testNumber;        /* Numero de l'test */
  int mainCmd;            /* Fonction a executer */
  int posterCmd;          /* Fonction a executer */
  int nbRqstOn;           /* Nombre de requete en cours */
  TEST_RQST_STR *rqst;   /* Partie dynamique des requetes (input,output) */
  TEST_ACTI_STR acti[CLIENT_NMAX_RQST_ID];  /* Les requetes en cours */
} TEST_STR;

/* Acces aux données static du module */
#define TEST_NAME(id)                    ((id)->mDesc.name)

#define TEST_NB_RQST(id)                 ((id)->mDesc.nbRqstType)
#define TEST_RQSTS_NAME(id)              ((id)->mDesc.rqstName)
#define TEST_RQST_NAME(id,r)             ((id)->mDesc.rqstName[(r)])
#define TEST_RQST_ABORT(id)              ((id)->mDesc.abortRqstNum)
#define TEST_RQST_DESC(id)               ((id)->mDesc.rqst)
#define TEST_RQST_TYPE(id,r)             ((id)->mDesc.rqst[(r)].rqstType)
#define TEST_RQST_INPUT_SIZE(id,r)       ((id)->mDesc.rqst[(r)].inputSize)
#define TEST_RQST_OUTPUT_SIZE(id,r)      ((id)->mDesc.rqst[(r)].outputSize)
#define TEST_RQST_FUNC(id,r)             ((id)->mDesc.rqst[(r)].func)

#define TEST_NB_POSTERS(id)              ((id)->mDesc.nbPosters)
#define TEST_POSTERS_NAME(id)            ((id)->mDesc.posterName)
#define TEST_POSTER_NAME(id,p)           ((id)->mDesc.posterName[(p)])
#define TEST_POSTER_SHOW_FUNCS(id)       ((id)->mDesc.posterShowFuncPtr)
#define TEST_POSTER_SHOW_FUNC(id,p)      ((id)->mDesc.posterShowFuncPtr[(p)])
#define TEST_POSTER_SHOW_CNTRL(id)       (TEST_NB_POSTERS(id))
#define TEST_POSTER_SHOW_CNTRL_ACTI(id)  (TEST_NB_POSTERS(id)+1)

/* Acces aux données dynamiques de la tache d'test */
#define TEST_CID(id)                     ((id)->testCId)
#define TEST_NUM(id)                     ((id)->testNumber)
#define TEST_MAIN_CMD(id)                ((id)->mainCmd)
#define TEST_POSTER_CMD(id)              ((id)->posterCmd)

#define TEST_NB_ACTIVITY_ON(id)          ((id)->nbRqstOn)
#define TEST_ACTIVITY_ON(id,r)           ((id)->acti[(r)].on)
#define TEST_ACTIVITY_RQST_NUM(id,r)     ((id)->acti[(r)].rqstNum)
#define TEST_ACTIVITY_RQST_ID(id,r)      ((id)->acti[(r)].rqstId)
#define TEST_ACTIVITY_NUM(id,r)          ((id)->acti[(r)].num) 

#define TEST_RQSTS(id)                   ((id)->rqst)
#define TEST_RQST_INPUT(id,r)            ((id)->rqst[(r)].input)
#define TEST_RQST_OUTPUT(id,r)           ((id)->rqst[(r)].output)
#define TEST_RQST_GET_CMD(id,r)          ((id)->rqst[(r)].getInputCmd)


#define TEST_TERM_WIDTH 78
#define TEST_OFFSET_MENU 6

#ifdef UNIX
#define TEST_TIME_WAIT_REPLY           /*10*/ 0
#else
#define TEST_TIME_WAIT_REPLY           /*2000*/ 0
#endif

#include "testLibProto.h"

#endif
