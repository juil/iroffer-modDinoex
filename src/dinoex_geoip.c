/*
 * by Dirk Meyer (dinoex)
 * Copyright (C) 2004-2007 Dirk Meyer
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the GNU General Public License.  More information is
 * available in the LICENSE file.
 *
 * If you received this file without documentation, it can be
 * downloaded from http://iroffer.dinoex.net/
 *
 * $Id$
 *
 */

/* include the headers */
#include "iroffer_config.h"
#include "iroffer_defines.h"
#include "iroffer_headers.h"
#include "iroffer_globals.h"
#include "dinoex_geoip.h"

#ifdef USE_GEOIP

#include <GeoIP.h>

#define GEOIP_FLAGS GEOIP_MEMORY_CACHE

GeoIP *gi = NULL;

static char *check_geoip(transfer *const t)
{
  static char hostname[20];
  static char code[20];
  const char *result;

  if (gi == NULL) {
    if (gdata.geoipdatabase != NULL)
      gi = GeoIP_open(gdata.geoipdatabase, GEOIP_FLAGS);
    else
      gi = GeoIP_new(GEOIP_FLAGS);
  }

  if (gi == NULL) {
    code[0] = 0;
    return code;
  }

  snprintf(hostname, sizeof(hostname), "%ld.%ld.%ld.%ld",
            t->remoteip>>24, (t->remoteip>>16) & 0xFF, (t->remoteip>>8) & 0xFF, t->remoteip & 0xFF );
  result = GeoIP_country_code_by_addr(gi, hostname);
  if (result == NULL) {
    code[0] = 0;
    return code;
  }

  code[0] = tolower(result[0]);
  code[1] = tolower(result[1]);
  code[2] = 0;
  return code;
}
#endif /* USE_GEOIP */

void geoip_new_connection(transfer *const tr)
{
#ifdef USE_GEOIP
  const char *country;
  char *msg;
#endif /* USE_GEOIP */

  if (tr->family != AF_INET)
    return;

#ifdef USE_GEOIP
  country = check_geoip(tr);
  ioutput(CALLTYPE_NORMAL, OUT_S|OUT_L|OUT_D, COLOR_YELLOW,
            "GeoIP [%s on %s]: Info %ld.%ld.%ld.%ld -> %s)",
            tr->nick,
            gdata.networks[ tr->net ].name,
            tr->remoteip>>24, (tr->remoteip>>16) & 0xFF,
            (tr->remoteip>>8) & 0xFF, tr->remoteip & 0xFF,
            country);

   if (irlist_size(&gdata.geoipcountry)) {
     if (!verifyshell(&gdata.geoipcountry, country)) {
       if (!verifyshell(&gdata.geoipexcludenick, tr->nick)) {
          msg = mycalloc(maxtextlength);
          if (country == NULL)
            country = "error";
          snprintf(msg, maxtextlength - 1, "Sorry, no downloads to your country = \"%s\", ask owner.", country);
          t_closeconn(tr, msg, 0);
          ioutput(CALLTYPE_NORMAL, OUT_S|OUT_L|OUT_D, COLOR_NO_COLOR,
                  "IP from other country (%s) detected", country);
          mydelete(msg);
          return;
       }
     }
   }
#endif /* USE_GEOIP */
}

void geoip_shutdown(void)
{
#ifdef USE_GEOIP
  if (gi != NULL) {
    GeoIP_delete(gi);
    gi = NULL;
  }
#endif /* USE_GEOIP */
}

/* End of File */