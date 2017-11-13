/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/
#include <pbs_config.h>   /* the master config generated by configure */

#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <memory.h>
#if defined(NTOHL_NEEDS_ARPA_INET_H) && defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

#include "portability.h"
#include "server_limits.h"
#include "net_connect.h"
#include "pbs_error.h"
#include "log.h"
#include "../Liblog/log_event.h"
#include "../Libnet/lib_net.h"

#if !defined(H_ERRNO_DECLARED) && !defined(_AIX)
/*extern int h_errno;*/
#endif

#ifdef _AIX
extern const char *hstrerror(int);
#endif

/*
   ** Put a human readable representation of a network address passed
   ** in as a long in host byte order and return pointer to out.
   */
char * netaddr_long(long ap, char *out)
  {
  u_long  ipadd;

  ipadd = ap;

  sprintf(out, "%ld.%ld.%ld.%ld",
           (ipadd & 0xff000000) >> 24,
           (ipadd & 0x00ff0000) >> 16,
           (ipadd & 0x0000ff00) >> 8,
           (ipadd & 0x000000ff));

  return out;
  }

/*
   ** Put a human readable representation of a network addres into
   ** a staticly allocated string.
   */
char * netaddr(struct sockaddr_in *ap)
  {
  static char out[80];
  char tmp[80];

  if (ap == NULL)
    return (char *)"unknown";

  netaddr_long( ntohl(ap->sin_addr.s_addr), tmp);

  sprintf(out, "%s:%d", tmp, ntohs(ap->sin_port));

  return out;
  }


/*
 * get_hostaddr.c - contains functions to provide the internal
 * internet address for a host and to provide the port
 * number for a service.
 *
 * get_hostaddr - get internal internet address of a host
 *
 * Returns a pbs_net_t (unsigned long) containing the network
 * address in host byte order.  A Null value is returned on
 * an error.
 */


pbs_net_t get_hostaddr(

  int  *local_errno, /* O */    
  char *hostname)    /* I */

  {
  pbs_net_t       rval = 0;
  int             rc = PBSE_NONE;
  char           *tmp_addr = NULL;
  struct in_addr  hostaddr;
  int             tmp_addr_len = 0;
  unsigned short  af_family = AF_INET;

  rc = get_hostaddr_hostent_af(local_errno, hostname, &af_family, &tmp_addr, &tmp_addr_len);
  if (rc == PBSE_NONE)
    {
    memcpy((void *)&hostaddr, (void *)tmp_addr, tmp_addr_len);
    rval = (pbs_net_t)ntohl(hostaddr.s_addr);
    }

  if (tmp_addr != NULL)
    free(tmp_addr);

  return(rval);
  }




int get_hostaddr_hostent(

  int             *rc,
  char            *hostname,
  char           **host_addr,
  int             *host_addr_len)

  {
  unsigned short af_family;
  return get_hostaddr_hostent_af(rc, hostname, &af_family, host_addr, host_addr_len);
  }




int  get_hostaddr_hostent_af(

  int             *rc,
  char            *hostname,
  unsigned short  *af_family,
  char           **host_addr,
  int             *host_addr_len)

  {
  int                    addr_rc;
  struct sockaddr_in    *sa;
  struct addrinfo       *pAddrInfo = NULL;
  char                   log_buf[LOCAL_LOG_BUF_SIZE];
  char                  *tmp_ip = NULL;

#ifdef NUMA_SUPPORT
  /* if this is a numa host, just get the parent node's address */
  char *dash;

  if ((dash = strchr(hostname,'-')) != NULL)
    {
    char *tmp;

    /* make sure to use the last dash */
    while ((tmp = strchr(dash+1,'-')))
      dash = tmp;

    if (isdigit(*(dash+1)))
      {
      /* terminate string temporarily */
      *dash = '\0';

      /* check if this resolves to a hostname without the dash */
      if ((addr_rc = pbs_getaddrinfo(hostname, NULL, &pAddrInfo)) != 0)
        {
        /* not a numa-owned node, act normal */
        *dash = '-';
      
        addr_rc = pbs_getaddrinfo(hostname, NULL, &pAddrInfo);
        }
      }
    /* otherwise proceed with just the parent hostname so 
     * it can be resolved */
    else
      addr_rc = pbs_getaddrinfo(hostname, NULL, &pAddrInfo);
    }
  else
    addr_rc = pbs_getaddrinfo(hostname, NULL, &pAddrInfo);
#else
  addr_rc = pbs_getaddrinfo(hostname, NULL, &pAddrInfo);
#endif /* NUMA_SUPPORT */

  *rc = PBSE_NONE;

  if (addr_rc != 0)
    {
    snprintf(log_buf, sizeof(log_buf),
      "cannot resolve IP address for host '%s' herror=%d: %s",
      hostname,
      h_errno,
      hstrerror(h_errno));
   /* This is for cases where the client is running this command */
      if (!log_available(PBSEVENT_SYSTEM))
        fprintf(stderr, "%s\n", log_buf);
      else
        log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buf);

    if (h_errno == TRY_AGAIN)
      *rc = PBS_NET_RC_RETRY;
    else
      *rc = PBS_NET_RC_FATAL;

    return(*rc);
    }

  if ((tmp_ip = (char *)calloc(1, sizeof(struct in_addr) + 1)) == NULL)
    {
    *rc = PBS_NET_RC_FATAL;
    }
  else
    {
    sa = (struct sockaddr_in *)pAddrInfo->ai_addr;
    memcpy(tmp_ip, &sa->sin_addr, sizeof(struct in_addr));
    *host_addr = tmp_ip;
    *host_addr_len = sizeof(struct in_addr);
    *af_family = sa->sin_family;
    }


  return(*rc);
  }  /* END get_hostaddr_hostent() */


/* END get_hostaddr.c */
