#include "$module$Error.h"

const H2_ERROR $module$H2errMsgs[] = $MODULE$_H2_ERR_MSGS;

int $module$RecordH2errMsgs()
{
  /* std errors from genom */
  genomRecordH2errMsgs();

  /* $module$ errors */
  if (!h2recordErrMsgs("$module$RecordH2errMsgs", "$module$", M_$module$, 
		      sizeof($module$H2errMsgs)/sizeof(H2_ERROR),
		      $module$H2errMsgs))
    return 0;

  /* others modules errros */
  $h2recordOtherModules$;

  return 1;
}
