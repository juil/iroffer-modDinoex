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
#include "dinoex_utilities.h"
#include "dinoex_badip.h"

int is_in_badip4(long remoteip)
{
  badip4 *b;

  for (b = irlist_get_head(&gdata.http_bad_ip4);
       b;
       b = irlist_get_next(b)) {
    if (b->remoteip == remoteip) {
      b->lastcontact = gdata.curtime;
      if (b->count > 10)
        return 1;
      break;
    }
  }
  return 0;
}

int is_in_badip6(struct in6_addr *remoteip)
{
  badip6 *b;

  for (b = irlist_get_head(&gdata.http_bad_ip6);
       b;
       b = irlist_get_next(b)) {
    if (memcmp(&(b->remoteip), remoteip, sizeof(struct in6_addr)) == 0) {
      b->lastcontact = gdata.curtime;
      if (b->count > 10)
        return 1;
      break;
    }
  }
  return 0;
}

int is_in_badip(ir_sockaddr_union_t *sa)
{
  if (sa->sa.sa_family == AF_INET) {
    return is_in_badip4(ntohl(sa->sin.sin_addr.s_addr));
  } else {
    return is_in_badip6(&(sa->sin6.sin6_addr));
  }
}

void count_badip4(long remoteip)
{
  badip4 *b;

  for (b = irlist_get_head(&gdata.http_bad_ip4);
       b;
       b = irlist_get_next(b)) {
    if (b->remoteip == remoteip) {
      b->count ++;
      b->lastcontact = gdata.curtime;
      return;
    }
  }

  b = irlist_add(&gdata.http_bad_ip4, sizeof(badip4));
  b->remoteip = remoteip;
  b->connecttime = gdata.curtime;
  b->lastcontact = gdata.curtime;
  b->count = 1;
}

void count_badip6(struct in6_addr *remoteip)
{
  badip6 *b;

  for (b = irlist_get_head(&gdata.http_bad_ip6);
       b;
       b = irlist_get_next(b)) {
    if (memcmp(&(b->remoteip), remoteip, sizeof(struct in6_addr)) == 0) {
      b->count ++;
      b->lastcontact = gdata.curtime;
      return;
    }
  }

  b = irlist_add(&gdata.http_bad_ip6, sizeof(badip6));
  b->remoteip = *remoteip;
  b->connecttime = gdata.curtime;
  b->lastcontact = gdata.curtime;
  b->count = 1;
}

void count_badip(ir_sockaddr_union_t *sa)
{
  if (sa->sa.sa_family == AF_INET) {
    count_badip4(ntohl(sa->sin.sin_addr.s_addr));
  } else {
    count_badip6(&(sa->sin6.sin6_addr));
  }
}
 
void expire_badip4(void)
{
  badip4 *b;

  b = irlist_get_head(&gdata.http_bad_ip4);
  while (b) {
    if ((gdata.curtime - b->lastcontact) > 3600) {
      b = irlist_delete(&gdata.http_bad_ip4, b);
    } else {
      b = irlist_get_next(b);
    }
  }
}

void expire_badip6(void)
{
  badip6 *b;

  b = irlist_get_head(&gdata.http_bad_ip6);
  while (b) {
    if ((gdata.curtime - b->lastcontact) > 3600) {
      b = irlist_delete(&gdata.http_bad_ip6, b);
    } else {
      b = irlist_get_next(b);
    }
  }
}

void expire_badip(void)
{
  expire_badip4();
  expire_badip6();
}

/* End of File */