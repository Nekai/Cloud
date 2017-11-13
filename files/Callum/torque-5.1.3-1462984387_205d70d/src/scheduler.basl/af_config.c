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

#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "portability.h"
#include "log.h"
#include "net_connect.h"
#include "af_config.h"
#include "af_cnodemap.h"
#include "af_resmom.h"
#include "af_server.h"
/* Macros */
#define  CONF_LINE_LEN 120 /* configuration file line length */

/* File Scope Variables. Variables available only within this file. */

static char ident[] = "@(#) $RCSfile$ $Revision$";

static pbs_net_t *okClients  = NULL;     /* accept connections from */

/* Global Variables */

/* External Variables */
extern char *msg_daemonname;

/* External Functions */

/* Structures and Unions */
/* External Functions */
/* Private Functions */
static void
badconn(char *msg, struct sockaddr_in saddr)
  {
  static  char    id[] = "badconn";

  struct  in_addr addr;
  char            buf[5*sizeof(addr) + 100];

  struct  hostent *phe;

  addr = saddr.sin_addr;
  phe = gethostbyaddr((void *) & addr, sizeof(addr), AF_INET);

  if (phe == NULL)
    {
    char    hold[6];
    int     i;
    union
      {

      struct  in_addr aa;
      u_char          bb[sizeof(addr)];
      } uu;

    uu.aa = addr;
    (void)sprintf(buf, "%u", uu.bb[0]);

    for (i = 1; i < sizeof(addr); i++)
      {
      (void)sprintf(hold, ".%u", uu.bb[i]);
      (void)strcat(buf, hold);
      }
    }
  else
    {
    (void)strncpy(buf, phe->h_name, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';
    }

  (void)sprintf(log_buffer, "%s on port %d %s", buf,
                ntohs(saddr.sin_port), msg);
  log_err(-1, id, log_buffer);
  return;
  }

/* Functions */
/* addClient: returns 0 if client is validated. 1 otherwise */
int
addClient(char *name)
  {
  static  char    id[] = "addClient";

  struct hostent *host;

  struct  in_addr saddr;

  int beforecnt, aftercnt;

  assert(name != NULLSTR);

  if ((host = gethostbyname(name)) == NULL)
    {
    (void)sprintf(log_buffer, "clienthost %s not found", name);
    log_err(-1, id, log_buffer);
    return -1;
    }

  /* ignore duplicates */
  beforecnt = dynamicArraySize(okClients);

  okClients = (pbs_net_t *)extendDynamicArray(okClients, beforecnt + 1,
              sizeof(pbs_net_t));

  aftercnt = dynamicArraySize(okClients);

  if (beforecnt == aftercnt)
    {
    log_err(-1, id, (char *)"Unable to realloc okClients");
    return -1;
    }

  memcpy((char *)&saddr, host->h_addr, host->h_length);

  okClients[beforecnt] = saddr.s_addr;
  (void)sprintf(log_buffer, "added client host %s", name);
  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  return 0;
  }

void okClientPrint(void)
  {
  printf("OkClient: %x\n", okClients);
  }

/*
 * validateClient - returns 0 if client whose address is specified by 'saddr'
 *                  matches one of the valid client addresses. Otherwise, a
 *      non-zero is returned.
 */
int
validateClient(void *saddr)
  {
  pbs_net_t  addr;
  int   i;
  int   numClients;

  if (ntohs(((struct sockaddr_in *)saddr)->sin_port) >= IPPORT_RESERVED)
    {
    badconn("non-reserved port", *(struct sockaddr_in *)saddr);
    return(1);
    }


  addr = (pbs_net_t)((struct sockaddr_in *)saddr)->sin_addr.s_addr;

  numClients = dynamicArraySize(okClients);

  for (i = 0; i < numClients; i++)
    {
    if (addr == okClients[i])
      {
      break;
      }
    }

  if (i == numClients)
    {
    badconn("unauthorized host", *(struct sockaddr_in *)saddr);
    return(1);
    }

  return(0);
  }

void freeClients(void)
  {
  freeDynamicArray(okClients);
  okClients = NULL;
  }

/* freeConfig - frees up the storage of various arrays filled in via the */
/*   configuration file */
void freeConfig(void)
  {
  freeClients();
  AllServersFree();
  ResFree();
  }

/* Returns the next token of flagged by a previous call to strtok. Returns
 NULL if no more token or if an error has occurred */
static char *
getNextToken(char *line)
  {
  char *token;

  token = strtok(line, " \t");

  if (token == NULLSTR)
    {
    return(NULLSTR);
    }

  if (*(token + strlen(token) - 1) == '\n')
    {
    *(token + strlen(token) - 1) = '\0';
    }

  return(token);
  }

/*
 * readConfig - read and process the configuration file (see -c option)
 *
 *      Valid statements are:
 *           $clienthost to specify which systems can contact the scheduler.
 *  SYNTAX:     $clienthost <hostname>
 *           $momhost    to specify the (resmom, port) to contact.
 *              SYNTAX:     $momhost  <host> <port> (if port=0 means def port)
 *      $node  to specify the 4-tuple
 *    <mach-type, nodeAttr, hostQuery_keyword, jobRequire_keyword>
 *
 *           NOTE: The jobRequire keyword is optional.
 */

int
readConfig(char *file)
  {
  static  char *id = "readConfig";
  FILE    *conf;
  int     i;
  char    line[CONF_LINE_LEN];
  char    *token = NULLSTR;
  char    *token1 = NULLSTR;
  char    *token2 = NULLSTR;
  char *localhostname;

  struct  specialconfig
    {
    char    *name;
    int (*handler)();
    CNode *(*handler2)();
    };

  struct specialconfig special[] =
    {
      {"clienthost",  addClient,     NULL
      },

    {"serverhost",  AllServersAdd, NULL },
    {"momhost",     NULL,          ServerNodesAdd },
    {"node",        addRes,        NULL },
    { NULLSTR,      NULL,          NULL }
    };

#if !defined(DEBUG) && !defined(NO_SECURITY_CHECK)

  if (chk_file_sec(file, 0, 0, S_IWGRP | S_IWOTH, 1, NULL))
    return (-1);

#endif

  if ((conf = fopen(file, "r")) == NULL)
    {
    sprintf(log_buffer, "cannot open config file %s\n", file);
    log_err(errno, id, log_buffer);
    return (-1);
    }

  while (fgets(line, CONF_LINE_LEN, conf))
    {

    if ((line[0] == '#') || (line[0] == '\n'))
      continue;                  /* ignore comment & null line */

    if (line[0] == '$')
      {

      token = getNextToken(line);

      if (token == NULLSTR)
        {
        token = "";
        }

      for (i = 0; special[i].name; i++)
        {
        if (STRCMP(token + 1, == , special[i].name))
          break;
        }

      if (special[i].name == NULLSTR)
        {
        (void)sprintf(log_buffer,
                      "config name %s not known or has no value",
                      token);
        log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                   msg_daemonname, log_buffer);
        (void)fclose(conf);
        return (-1);
        }

      if (STRCMP(special[i].name, == , "clienthost"))
        {
        /* expects one argument */
        token = getNextToken(NULLSTR);

        if (token == NULLSTR)
          {

          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname, "no parameter to config name $clienthost");
          (void)fclose(conf);
          return(-1);
          }

        if (special[i].handler(token))
          {
          (void)fclose(conf);
          return (-1);
          }

        token = getNextToken(NULLSTR);

        if (token != NULLSTR)
          {
          log_err(-1, id,
                  "Ignoring extra pameter to config name $clienthost");
          }

        }
      else if (STRCMP(special[i].name, == , "momhost"))
        {
        /* expects 2 arguments */

        token = getNextToken(NULLSTR);

        if (token == NULLSTR)
          {

          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname,
                     "no host parameter to config name $momhost");
          (void)fclose(conf);
          return(-1);
          }

        token1 = getNextToken(NULLSTR);

        if (token1 == NULLSTR)
          {
          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname,
                     "no port parameter to config name $momhost");
          (void)fclose(conf);
          return(-1);
          }

        if (special[i].handler2(AllServersLocalHostGet(), token,
                                atoi(token1), 1) == NULL)
          {
          (void)fclose(conf);
          return (-1);
          }

        token = getNextToken(NULLSTR);

        if (token != NULLSTR)
          {
          log_err(-1, id,
                  "Ignoring extra pameter to config name $momhost");
          }

        }
      else if (STRCMP(special[i].name, == , "serverhost"))
        {
        /* expects 2-3 arguments */

        token = getNextToken(NULLSTR);

        if (token == NULLSTR)
          {

          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname,
                     "no host parameter to config name $serverhost");
          (void)fclose(conf);
          return(-1);
          }

        token1 = getNextToken(NULLSTR);

        if (token1 == NULLSTR)
          {
          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname,
                     "no port parameter to config name $serverhost");
          (void)fclose(conf);
          return(-1);
          }

        if (special[i].handler(token, atoi(token1)))
          {
          (void)fclose(conf);
          return (-1);
          }

        token = getNextToken(NULLSTR);

        if (token != NULLSTR)
          {
          log_err(-1, id,
                  "Ignoring extra pameter to config name $serverhost");
          }

        }
      else if (STRCMP(special[i].name, == , "node"))
        {
        /* expects at least arguments */

        token = getNextToken(NULLSTR);

        if (token == NULLSTR)
          {

          (void)sprintf(log_buffer,
                        "no mach_type parameter to config name %s",
                        special[i].name);
          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname, log_buffer);
          (void)fclose(conf);
          return(-1);
          }

        token1 = getNextToken(NULLSTR);

        if (token1 == NULLSTR)
          {

          (void)sprintf(log_buffer,
                        "no nodeAttr parameter to config name %s",
                        special[i].name);
          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname, log_buffer);
          (void)fclose(conf);
          return(-1);
          }

        token2 = getNextToken(NULLSTR);

        if (token2 == NULLSTR)
          {
          (void)sprintf(log_buffer,
                        "no hostQuery_keyword parameter to config name %s",
                        special[i].name);
          log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER,
                     msg_daemonname, log_buffer);
          (void)fclose(conf);
          return(-1);
          }

        if (special[i].handler(token, token1, token2))
          {
          (void)fclose(conf);
          return (-1);
          }

        token = getNextToken(NULLSTR);

        if (token != NULLSTR)
          {
          (void)sprintf(log_buffer,
                        "Ignoring extra parameter %s to config name %s\n",
                        token, special[i].name);
          log_err(-1, id, log_buffer);

          }
        }
      } /* outer-if */
    }

  (void)fclose(conf);
  return (0);
  }
