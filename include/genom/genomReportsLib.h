#ifndef GENOM_REPORTS_LIB_H
#define GENOM_REPORTS_LIB_H

#include "h2errorLib.h"


typedef struct GENOM_REPORTS_STRUCT {
  int numModule;
  int nbFailures;
  char *moduleName;
  H2_FAILED_STRUCT *tab;
} GENOM_REPORTS_STRUCT;

#include "genomReportsLibProto.h"


#endif
