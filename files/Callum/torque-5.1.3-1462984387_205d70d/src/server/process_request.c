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
#include "process_request.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <memory.h>
#include <time.h>
#include <pwd.h>
#include <sys/param.h>
#if HAVE_SYS_UCRED_H
#include <sys/ucred.h>
#endif
#if HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#include <pthread.h>

#include "libpbs.h"
#include "lib_ifl.h" /* netaddr_long */
#include "pbs_error.h"
#include "server_limits.h"
#include "pbs_nodes.h"
#include "list_link.h"
#include "attribute.h"
#include "pbs_job.h"
#include "server.h"
#include "credential.h"
#include "batch_request.h"
#include "net_connect.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "../lib/Liblog/log_event.h"
#include "svrfunc.h"
#include "pbs_proto.h"
#include "csv.h"
#include "u_tree.h"
#include "threadpool.h"
#include "dis.h"
#include "array.h"
#include "req_stat.h"
#include "../lib/Libutils/u_lock_ctl.h" /* lock_node, unlock_node */
#include "../lib/Libnet/lib_net.h" /* globalset_del_sock */
#include "svr_func.h" /* get_svr_attr_* */
#include "req_getcred.h" /* req_altauthenuer */ 
#include "req_quejob.h" /* req_quejob, req_jobcredential, req_mvjobfile */ 
#include "req_holdjob.h" /* req_holdjob, req_checkpointjob */ 
#include "req_stat.h" /* req_stat_node */ 
#include "req_jobobit.h" /* req_jobobit */ 
#include "req_runjob.h" /* req_runjob, req_stagein */ 
#include "req_deletearray.h" /* req_deletearray */ 
#include "req_delete.h" /* req_deletejob */ 
#include "req_movejob.h" /* req_orderjob */ 
#include "req_modify.h" /* req_modifyjob, req_modifyarray */ 
#include "req_track.h" /* req_track */ 
#include "req_rescq.h" /* req_rescreserve, req_rescfree */ 
#include "req_shutdown.h" /* req_shutdown */
#include "req_signal.h" /* req_signaljob */
#include "req_message.h" /* req_messagejob */
#include "req_locate.h" /* req_locatejob */
#include "req_manager.h" /* req_manager */
#include "req_rerun.h" /* req_rerunjob */
#include "req_select.h" /* req_selectjobs */
#include "req_register.h" /* req_register, req_registerarray */
#include "req_modify_node.h" /* req_modify_node */
#include "job_func.h" /* svr_job_purge */
#include "tcp.h" /* tcp_chan */
#include "ji_mutex.h"
#include "mutex_mgr.hpp"

/*
 * process_request - this function gets, checks, and invokes the proper
 * function to deal with a batch request received over the network.
 *
 * All data encoding/decoding dependencies are moved to a lower level
 * routine.  That routine must convert
 * the data received into the internal server structures regardless of
 * the data structures used by the encode/decode routines.  This provides
 * the "protocol" and "protocol generation tool" freedom to the bulk
 * of the server.
 */

/* global data items */

pthread_mutex_t *svr_requests_mutex = NULL;

extern struct    connection svr_conn[];

extern struct    credential conn_credent[PBS_NET_MAX_CONNECTIONS];

extern struct    server server;
extern char      server_host[];
all_jobs          newjobs;

extern char     *msg_err_noqueue;
extern char     *msg_err_malloc;
extern char     *msg_request;
extern char      server_name[];

extern AvlTree   okclients;

extern int       LOGLEVEL;

/* private functions local to this file */

#ifdef MUNGE_AUTH
static const int munge_on = 1;
#else
static const int munge_on = 0;
#endif 

static void freebr_manage(struct rq_manage *);
static void freebr_cpyfile(struct rq_cpyfile *);
static void free_rescrq(struct rq_rescq *);
void        close_quejob(int sfds);

/* END private prototypes */



#ifdef ENABLE_UNIX_SOCKETS
int get_creds(
    
  int   sd,
  char *username,
  char *hostname)

  {
  int             nb/*, sync*/;
  char            ctrl[CMSG_SPACE(sizeof(struct ucred))];
  size_t          size;

  struct iovec    iov[1];

  struct msghdr   msg;

  struct cmsghdr  *cmptr;
  ucreds *credentials;

  struct passwd *cpwd;
  char dummy;

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  msg.msg_control = ctrl;
  msg.msg_controllen = sizeof(ctrl);
  msg.msg_flags = 0;

#ifdef LOCAL_CREDS
  nb = 1;

  if (setsockopt(sd, 0, LOCAL_CREDS, &nb, sizeof(nb)) == -1) return 0;

#else
#ifdef SO_PASSCRED
  nb = 1;

  if (setsockopt(sd, SOL_SOCKET, SO_PASSCRED, &nb, sizeof(nb)) == -1)
    return 0;

#endif

  dummy = '\0';

  do
    {
    msg.msg_iov->iov_base = (void *) & dummy;
    msg.msg_iov->iov_len  = sizeof(dummy);
    nb = recvmsg(sd, &msg, 0);
    }
  while (nb == -1 && (errno == EINTR || errno == EAGAIN));

  if (nb == -1) return 0;

  if ((unsigned)msg.msg_controllen < sizeof(struct cmsghdr)) return 0;

  cmptr = CMSG_FIRSTHDR(&msg);

#ifndef __NetBSD__
  size = sizeof(ucreds);

#else
  if (cmptr->cmsg_len < SOCKCREDSIZE(0))
    return(0);

  size = SOCKCREDSIZE(((cred *)CMSG_DATA(cmptr))->sc_ngroups);

#endif
  if ((unsigned)cmptr->cmsg_len != CMSG_LEN(size))
    return(0);

  if (cmptr->cmsg_level != SOL_SOCKET)
    return 0;

  if (cmptr->cmsg_type != SCM_CREDS)
    return 0;

  if (!(credentials = (ucreds *)calloc(1, size)))
    return 0;

  *credentials = *(ucreds *)CMSG_DATA(cmptr);

  cpwd = getpwuid(SPC_PEER_UID(credentials));

  if (cpwd)
    strcpy(username, cpwd->pw_name);

  strcpy(hostname, server_name);

  free(credentials);

  return 0;
  }

#endif
#endif /* END ENABLE_UNIX_SOCKETS */


bool request_passes_acl_check(

  batch_request *request,
  unsigned long  conn_addr)
  
  {
  long acl_enable = FALSE;

  get_svr_attr_l(SRV_ATR_acl_host_enable, &acl_enable);
  if (acl_enable)
    {
    /* acl enabled, check it; always allow myself and nodes */
    struct array_strings *pas = NULL;
    struct pbsnode       *isanode;

    get_svr_attr_arst(SRV_ATR_acl_hosts, &pas);

    isanode = find_nodebyname(request->rq_host);

    if ((isanode == NULL) &&
        (strcmp(server_host, request->rq_host) != 0) &&
        (acl_check_my_array_string(pas, request->rq_host, ACL_Host) == 0))
      {
      return(false);
      }

    if (isanode != NULL)
      unlock_node(isanode, __func__, NULL, LOGLEVEL);
    }
  
  return(true);
  } /* END request_passes_acl_check() */



batch_request *read_request_from_socket(

  tcp_chan *chan)

  {
  int                   rc = PBSE_NONE;
  struct batch_request *request = NULL;
  char                  log_buf[LOCAL_LOG_BUF_SIZE];

  time_t                time_now = time(NULL);
  char                  tmpLine[MAXLINE];
  enum conn_type        conn_active;
  unsigned short        conn_socktype;
#ifdef ENABLE_UNIX_SOCKETS
  unsigned short        conn_authen;
#endif
  unsigned long         conn_addr;
  int                   sfds = chan->sock;

  if ((sfds < 0) ||
      (sfds >= PBS_NET_MAX_CONNECTIONS))
    return(NULL);

  pthread_mutex_lock(svr_conn[sfds].cn_mutex);
  conn_active = svr_conn[sfds].cn_active;
  conn_socktype = svr_conn[sfds].cn_socktype;
#ifdef ENABLE_UNIX_SOCKETS
  conn_authen = svr_conn[sfds].cn_authen;
#endif
  conn_addr = svr_conn[sfds].cn_addr;
  svr_conn[sfds].cn_lasttime = time_now;
  pthread_mutex_unlock(svr_conn[sfds].cn_mutex);

  if ((request = alloc_br(0)) == NULL)
    return(NULL);

  request->rq_conn = sfds;

  /*
   * Read in the request and decode it to the internal request structure.
   */
  if ((conn_active == FromClientDIS) ||
      (conn_active == ToServerDIS))
    {
#ifdef ENABLE_UNIX_SOCKETS

    if ((conn_socktype & PBS_SOCK_UNIX) &&
        (conn_authen != PBS_NET_CONN_AUTHENTICATED))
      {
      /* get_creds interestingly always returns 0 */
      get_creds(sfds, conn_credent[sfds].username, conn_credent[sfds].hostname);
      }

#endif /* END ENABLE_UNIX_SOCKETS */
    rc = dis_request_read(chan, request);

    if ((rc == PBSE_SYSTEM) || (rc == PBSE_INTERNAL) || (rc == PBSE_SOCKET_CLOSE))
      {
      /* read error, likely cannot send reply so just disconnect, indicate permanent
       * failure by setting the type to PBS_BATCH_Disconnect */
      request->rq_type = PBS_BATCH_Disconnect;
      return(request);
      }
    else if (rc > 0)
      {
      /* FAILURE */

      /*
       * request didn't decode, either garbage or unknown
       * request type, in either case, return reject-reply
       */
      request->rq_failcode = rc;
      return(request);
      }
    }
  else
    {
    char out[80];

    snprintf(tmpLine, MAXLINE, "request on invalid type of connection: %d, sock type: %d, from address %s", 
                conn_active,conn_socktype, netaddr_long(conn_addr, out));
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_REQUEST,
      "process_req", tmpLine);
    snprintf(tmpLine, sizeof(tmpLine),
        "request on invalid type of connection (%d) from %s",
        conn_active,
        netaddr_long(conn_addr, out));
    req_reject(PBSE_BADHOST, 0, request, NULL, tmpLine);
    return(NULL);
    }

  if (get_connecthost(sfds, request->rq_host, PBS_MAXHOSTNAME) != 0)
    {
    char ipstr[80];
    sprintf(log_buf, "%s: %s",
      pbse_to_txt(PBSE_BADHOST),
      netaddr_long(conn_addr, ipstr));

    log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_REQUEST, "", log_buf);

    snprintf(log_buf, sizeof(log_buf), "%s", tmpLine);
    snprintf(tmpLine, sizeof(tmpLine),
        "cannot determine hostname for connection from %s",
        log_buf);

    req_reject(PBSE_BADHOST, 0, request, NULL, tmpLine);
    return(NULL);
    }

  if (LOGLEVEL >= 8)
    {
    sprintf(log_buf,
      msg_request,
      reqtype_to_txt(request->rq_type),
      request->rq_user,
      request->rq_host,
      sfds);

    log_event(PBSEVENT_DEBUG2, PBS_EVENTCLASS_REQUEST, "", log_buf);
    }

  /* is the request from a host acceptable to the server */
  if (conn_socktype & PBS_SOCK_UNIX)
    strcpy(request->rq_host, server_name);

  if (request_passes_acl_check(request, conn_addr) == false)
    {
    /* See if the request is in the limited acl list */
    if (limited_acls.is_authorized(request->rq_host, request->rq_user) == false)
      {
      char tmpLine[MAXLINE];
      snprintf(tmpLine, sizeof(tmpLine), "request not authorized from host %s",
        request->rq_host);
      req_reject(PBSE_BADHOST, 0, request, NULL, tmpLine);
      return(NULL);
      }
    }

  return(request);
  } /* END read_request_from_socket() */



/*
 * process_request - process an request from the network:
 *
 * - Call function to read in the request and decode it.
 * - Validate requesting host and user.
 * - Call function to process request based on type.
 *    That function MUST free the request by calling free_br()
 *
 * NOTE: the caller functions hold the mutex for the connection
 * NOTE: The socket connection must be returned in an open state.
 *       The connection will be closed by start_process_pbs_server_port.
 */

int process_request(

  struct tcp_chan *chan) /* file descriptor (socket) to get request */

  {
  int                   rc = PBSE_NONE;
  struct batch_request *request = NULL;
  long                  state = SV_STATE_DOWN;

  time_t                time_now = time(NULL);
  char                 *auth_err = NULL;
  unsigned short        conn_socktype;
  unsigned short        conn_authen;
  int                   sfds = chan->sock;

  if ((sfds < 0) ||
      (sfds >= PBS_NET_MAX_CONNECTIONS))
    return(PBSE_SOCKET_CLOSE);

  pthread_mutex_lock(svr_conn[sfds].cn_mutex);
  conn_socktype = svr_conn[sfds].cn_socktype;
  conn_authen = svr_conn[sfds].cn_authen;
  svr_conn[sfds].cn_lasttime = time_now;
  pthread_mutex_unlock(svr_conn[sfds].cn_mutex);

  request = read_request_from_socket(chan);

  if (request == NULL)
    rc = -1;
  else if (request->rq_type == PBS_BATCH_Disconnect)
    rc = PBSE_SOCKET_CLOSE;
  else
    rc = request->rq_failcode;

  if (rc != PBSE_NONE)
    {
    free_br(request);
    return(rc);
    }

  /*
   * determine source (user client or another server) of request.
   * set the permissions granted to the client
   */

  if (conn_authen == PBS_NET_CONN_FROM_PRIVIL)
    {
    /* request came from another server */

    request->rq_fromsvr = 1;

    request->rq_perm =
      ATR_DFLAG_USRD | ATR_DFLAG_USWR |
      ATR_DFLAG_OPRD | ATR_DFLAG_OPWR |
      ATR_DFLAG_MGRD | ATR_DFLAG_MGWR |
      ATR_DFLAG_SvWR;
    }
  else
    {
    /* request not from another server */
    conn_credent[sfds].timestamp = time_now;

    request->rq_fromsvr = 0;

    /*
     * Client must be authenticated by an Authenticate User Request, if not,
     * reject request and close connection.  -- The following is retained for
     * compat with old cmds -- The exception to this is of course the Connect
     * Request which cannot have been authenticated, because it contains the
     * needed ticket; so trap it here.  Of course, there is no prior
     * authentication on the Authenticate User request either, but it comes
     * over a reserved port and appears from another server, hence is
     * automatically granted authentication.
     *
     * The above is only true with inet sockets.  With unix domain sockets, the
     * user creds were read before the first dis_request_read call above.
     * We automatically granted authentication because we can trust the socket
     * creds.  Authorization is still granted in svr_get_privilege below
     */

    if (request->rq_type == PBS_BATCH_Connect)
      {
      return(req_connect(request));
      }

    if (conn_socktype & PBS_SOCK_UNIX)
      {
      pthread_mutex_lock(svr_conn[sfds].cn_mutex);
      svr_conn[sfds].cn_authen = PBS_NET_CONN_AUTHENTICATED;
      pthread_mutex_unlock(svr_conn[sfds].cn_mutex);
      }

    if (ENABLE_TRUSTED_AUTH == TRUE )
      rc = PBSE_NONE;  /* bypass the authentication of the user--trust the client completely */
#ifdef MUNGE_AUTH
    else if (munge_on)
      {
      /* If munge_on is true we will validate the connection now */
      if (request->rq_type == PBS_BATCH_AltAuthenUser)
        {
        rc = req_altauthenuser(request);
        return(rc);
        }
      else
        {
        rc = authenticate_user(request, &conn_credent[sfds], &auth_err);
        }
      }
#endif
    else if (conn_authen != PBS_NET_CONN_AUTHENTICATED)
      /* skip checking user if we did not get an authenticated credential */
      rc = PBSE_BADCRED;
    else
      rc = authenticate_user(request, &conn_credent[sfds], &auth_err);

    if (rc != 0)
      {
      req_reject(rc, 0, request, NULL, auth_err);
      if (auth_err != NULL)
        free(auth_err);

      return(rc);
      }

    /*
     * pbs_mom and checkpoint restart scripts both need the authority to do
     * alters and releases on checkpointable jobs.  Allow manager permission
     * for root on the jobs execution node.
     */
     
    if (((request->rq_type == PBS_BATCH_ModifyJob) ||
        (request->rq_type == PBS_BATCH_ReleaseJob)) &&
        (strcmp(request->rq_user, PBS_DEFAULT_ADMIN) == 0))
      {
      job *pjob;
      char *dptr;
      int skip = FALSE;
      char short_host[PBS_MAXHOSTNAME+1];

      /* make short host name */

      strcpy(short_host, request->rq_host);
      if ((dptr = strchr(short_host, '.')) != NULL)
        {
        *dptr = '\0';
        }
      
      if ((pjob = svr_find_job(request->rq_ind.rq_modify.rq_objname, FALSE)) != NULL)
        {
        mutex_mgr pjob_mutex = mutex_mgr(pjob->ji_mutex, true);
        if (pjob->ji_qs.ji_state == JOB_STATE_RUNNING)
          {

          if ((pjob->ji_wattr[JOB_ATR_checkpoint].at_flags & ATR_VFLAG_SET) &&
              ((csv_find_string(pjob->ji_wattr[JOB_ATR_checkpoint].at_val.at_str, "s") != NULL) ||
               (csv_find_string(pjob->ji_wattr[JOB_ATR_checkpoint].at_val.at_str, "c") != NULL) ||
               (csv_find_string(pjob->ji_wattr[JOB_ATR_checkpoint].at_val.at_str, "enabled") != NULL)) &&
              (strstr(pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str, short_host) != NULL))
            {
            request->rq_perm = svr_get_privilege(request->rq_user, server_host);
            skip = TRUE;
            }
          }
          pjob_mutex.unlock();
        }
      
      if (!skip)
        {
        request->rq_perm = svr_get_privilege(request->rq_user, request->rq_host);
        }
      }
    else
      {
      request->rq_perm = svr_get_privilege(request->rq_user, request->rq_host);
      }
    }  /* END else (conn_authen == PBS_NET_CONN_FROM_PRIVIL) */

  if (threadpool_is_too_busy(request_pool, request->rq_perm))
    {
    req_reject(PBSE_SERVER_BUSY, 0, request, NULL, NULL);
    return(PBSE_SERVER_BUSY);
    }

  /* if server shutting down, disallow new jobs and new running */
  get_svr_attr_l(SRV_ATR_State, &state);

  if (state > SV_STATE_RUN)
    {
    switch (request->rq_type)
      {
      case PBS_BATCH_AsyrunJob:
      case PBS_BATCH_JobCred:
      case PBS_BATCH_MoveJob:
      case PBS_BATCH_QueueJob:
      case PBS_BATCH_RunJob:
      case PBS_BATCH_StageIn:
      case PBS_BATCH_jobscript:

        req_reject(PBSE_SVRDOWN, 0, request, NULL, NULL);

        return(PBSE_SVRDOWN);
      }
    }

  /*
   * dispatch the request to the correct processing function.
   * The processing function must call reply_send() to free
   * the request struture.
   */

  rc = dispatch_request(sfds, request);

  return(rc);
  }  /* END process_request() */


/*
 * dispatch_request - Determine the request type and invoke the corresponding
 * function.  The function will perform the request action and return the
 * reply.  The function MUST also reply and free the request by calling
 * reply_send().
 *
 * @see process_request() - parent
 */

int dispatch_request(

  int                   sfds,    /* I */
  struct batch_request *request) /* I */

  {
  int   rc = PBSE_NONE;
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  if (LOGLEVEL >= 5)
    {
    sprintf(log_buf,"dispatching request %s on sd=%d",
      reqtype_to_txt(request->rq_type),
      sfds);

    log_record(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
    }

  switch (request->rq_type)
    {
    case PBS_BATCH_QueueJob:

      net_add_close_func(sfds, close_quejob);
      rc = req_quejob(request);
      
      break;


    case PBS_BATCH_JobCred:
      rc = req_jobcredential(request);
      break;


    case PBS_BATCH_jobscript:
     
      rc = req_jobscript(request);
      
      break;


    case PBS_BATCH_RdytoCommit:
     
      rc = req_rdytocommit(request);
      
      break;


    case PBS_BATCH_Commit:
      
      rc = req_commit(request);
      
      break;


    case PBS_BATCH_DeleteJob:

      /* if this is a server size job delete request, then the request
       * could also be for an entire array.  we check to see if the
       * request object name is an array id. If so we hand off the the
       * req_deletearray() function.  If not we pass along to the normal
       * req_deletejob() function.
      */

      if (is_array(request->rq_ind.rq_delete.rq_objname))
        rc = req_deletearray(request);
      else
        rc = req_deletejob(request);

      break;

    case PBS_BATCH_HoldJob:
      if (is_array(request->rq_ind.rq_hold.rq_orig.rq_objname))
        rc = req_holdarray(request);
      else
        rc = req_holdjob(request);

      break;

    case PBS_BATCH_CheckpointJob:

      rc = req_checkpointjob(request);

      break;

    case PBS_BATCH_LocateJob:

      rc = req_locatejob(request);

      break;

    case PBS_BATCH_ModifyNode:

      rc = req_modify_node(request);

      break;

    case PBS_BATCH_Manager:

      rc = req_manager(request);

      break;

    case PBS_BATCH_MessJob:

      rc = req_messagejob(request);

      break;

    case PBS_BATCH_AsyModifyJob:

    case PBS_BATCH_ModifyJob:
      if (is_array(request->rq_ind.rq_delete.rq_objname))
        rc = req_modifyarray(request);
      else
        req_modifyjob(request);

      break;

    case PBS_BATCH_Rerun:

      rc = req_rerunjob(request);

      break;

    case PBS_BATCH_MoveJob:

      rc = req_movejob(request);

      break;

    case PBS_BATCH_OrderJob:

      rc = req_orderjob(request);

      break;

    case PBS_BATCH_Rescq:

      rc = req_rescq(request);

      break;

    case PBS_BATCH_ReserveResc:

      rc = req_rescreserve(request);

      break;

    case PBS_BATCH_ReleaseResc:

      rc = req_rescfree(request);

      break;

    case PBS_BATCH_ReleaseJob:

      if (is_array(request->rq_ind.rq_delete.rq_objname))
        rc = req_releasearray(request);
      else
        rc = req_releasejob(request);

      break;

    case PBS_BATCH_RunJob:

    case PBS_BATCH_AsyrunJob:
      globalset_del_sock(request->rq_conn);
      rc = req_runjob(request);

      break;

    case PBS_BATCH_SelectJobs:

    case PBS_BATCH_SelStat:

    case PBS_BATCH_SelStatAttr:

      /* handle special 'truncated' keyword */

      if (!strncasecmp(request->rq_ind.rq_status.rq_id, "truncated", strlen("truncated")))
        rc =req_stat_job(request);
      else 
        rc = req_selectjobs(request);

      break;

    case PBS_BATCH_Shutdown:

      req_shutdown(request);

      break;

    case PBS_BATCH_SignalJob:

    case PBS_BATCH_AsySignalJob:

      rc = req_signaljob(request);

      break;

    case PBS_BATCH_GpuCtrl:

      rc = req_gpuctrl_svr(request);

      break;

    case PBS_BATCH_MvJobFile:

      rc = req_mvjobfile(request);

      break;

    case PBS_BATCH_StatusQue:

      rc = req_stat_que(request);

      break;

    case PBS_BATCH_StatusNode:
      
      rc = req_stat_node(request);

      break;

    case PBS_BATCH_StatusSvr:

      rc = req_stat_svr(request);

      break;

      /* DIAGTODO: handle PBS_BATCH_StatusDiag and define req_stat_diag() */

    case PBS_BATCH_TrackJob:

      rc = req_track(request);

      break;

    case PBS_BATCH_RegistDep:

      if (is_array(request->rq_ind.rq_register.rq_parent))
        {
        rc = req_registerarray(request);
        }
      else
        {
        rc = req_register(request);
        }

      break;

    case PBS_BATCH_AuthenUser:

      /* determine if user is valid */
      rc = req_authenuser( request); 

      break;

    case PBS_BATCH_AltAuthenUser:

      break;

    case PBS_BATCH_JobObit:

      rc = req_jobobit(request);

      break;

    case PBS_BATCH_StageIn:

      rc = req_stagein(request);

      break;

    case PBS_BATCH_StatusJob:

      rc = req_stat_job(request);
      
      break;

    default:

      req_reject(PBSE_UNKREQ, 0, request, NULL, NULL);

      if (sfds != PBS_LOCAL_CONNECTION)
        close_conn(sfds, FALSE);

      break;
    }  /* END switch (request->rq_type) */

  return(rc);
  }  /* END dispatch_request() */




/*
 * alloc_br - allocate and clear a batch_request structure
 */

struct batch_request *alloc_br(

  int type)

  {

  struct batch_request *req = NULL;

  if ((req = (struct batch_request *)calloc(1, sizeof(struct batch_request))) == NULL)
    {
    log_err(errno, __func__, msg_err_malloc);
    }
  else
    {

    req->rq_type = type;

    req->rq_conn = -1;  /* indicate not connected */
    req->rq_orgconn = -1;  /* indicate not connected */
    req->rq_time = time(NULL);
    req->rq_reply.brp_choice = BATCH_REPLY_CHOICE_NULL;
    req->rq_noreply = FALSE;  /* indicate reply is needed */
    }

  return(req);
  } /* END alloc_br() */



/*
 * close_quejob()
 *
 * Cleans up a quejob request that is closed mid-flight
 * @param sfds - the socket the request was taking place on
 */

void close_quejob(

  int sfds)

  {
  job *pjob;

  all_jobs_iterator *iter;

  newjobs.lock();
  iter = newjobs.get_iterator();
  newjobs.unlock();

  while ((pjob = next_job(&newjobs, iter)) != NULL)
    {
    mutex_mgr pjob_mutex = mutex_mgr(pjob->ji_mutex, true);

    if (pjob->ji_qs.ji_un.ji_newt.ji_fromsock == sfds)
      {
      // This job needs to be cleaned up, ignore remove job errors because it is the newjobs container
      remove_job(&newjobs, pjob);

      if (pjob->ji_qs.ji_substate == JOB_SUBSTATE_TRANSICM)
        {
        if (pjob->ji_qs.ji_svrflags & JOB_SVFLG_HERE)
          {
          // If the job was created here, attempt to queue it. If not, just
          // leave it hanging until the sending server contacts us again
          pjob->ji_qs.ji_state = JOB_STATE_QUEUED;
          pjob->ji_qs.ji_substate = JOB_SUBSTATE_QUEUED;

          int rc = svr_enquejob(pjob, FALSE, NULL, false, false);
          
          if ((rc == PBSE_JOBNOTFOUND) ||
              (rc == PBSE_JOB_RECYCLED))
            {
            pjob_mutex.set_unlock_on_exit(false);
            }
          else if (rc != PBSE_NONE)
            {
            job_abt(&pjob, msg_err_noqueue); /* pjob will be set to null in job_abt */
            pjob_mutex.set_unlock_on_exit(false);
            }
          }
        }
      else
        {
        // Delete the job
        svr_job_purge(pjob);
       
        pjob_mutex.set_unlock_on_exit(false);
        }

      break;
      }
    }

  delete iter;
  } // END close_quejob()



/*
 * free_br - free space allocated to a batch_request structure
 * including any sub-structures
 */

void free_br(

  struct batch_request *preq)

  {
  if (preq == NULL)
    return;

  if (preq->rq_id != NULL)
    {
    remove_batch_request(preq->rq_id);
    free(preq->rq_id);
    preq->rq_id = NULL;
    }

  reply_free(&preq->rq_reply);

  if (preq->rq_extend) 
    {
    free(preq->rq_extend);
    preq->rq_extend = NULL;
    }

  switch (preq->rq_type)
    {
    case PBS_BATCH_QueueJob:

      free_attrlist(&preq->rq_ind.rq_queuejob.rq_attr);

      break;

    case PBS_BATCH_JobCred:

      if (preq->rq_ind.rq_jobcred.rq_data)
        {
        free(preq->rq_ind.rq_jobcred.rq_data);
        preq->rq_ind.rq_jobcred.rq_data = NULL;
        }

      break;

    case PBS_BATCH_MvJobFile:

    case PBS_BATCH_jobscript:

      if (preq->rq_ind.rq_jobfile.rq_data)
        {
        free(preq->rq_ind.rq_jobfile.rq_data);
        preq->rq_ind.rq_jobfile.rq_data = NULL;
        }
      break;

    case PBS_BATCH_HoldJob:

      freebr_manage(&preq->rq_ind.rq_hold.rq_orig);

      break;

    case PBS_BATCH_CheckpointJob:

      freebr_manage(&preq->rq_ind.rq_manager);

      break;

    case PBS_BATCH_MessJob:

      if (preq->rq_ind.rq_message.rq_text)
        {
        free(preq->rq_ind.rq_message.rq_text);
        preq->rq_ind.rq_message.rq_text = NULL;
        }

      break;

    case PBS_BATCH_ModifyJob:

    case PBS_BATCH_AsyModifyJob:

      freebr_manage(&preq->rq_ind.rq_modify);

      break;

    case PBS_BATCH_StatusJob:

    case PBS_BATCH_StatusQue:

    case PBS_BATCH_StatusNode:

    case PBS_BATCH_StatusSvr:
      /* DIAGTODO: handle PBS_BATCH_StatusDiag */

      free_attrlist(&preq->rq_ind.rq_status.rq_attr);

      break;

    case PBS_BATCH_JobObit:

      free_attrlist(&preq->rq_ind.rq_jobobit.rq_attr);

      break;

    case PBS_BATCH_CopyFiles:

    case PBS_BATCH_DelFiles:

      freebr_cpyfile(&preq->rq_ind.rq_cpyfile);

      break;

    case PBS_BATCH_ModifyNode:

    case PBS_BATCH_Manager:

      freebr_manage(&preq->rq_ind.rq_manager);

      break;

    case PBS_BATCH_ReleaseJob:

      freebr_manage(&preq->rq_ind.rq_release);

      break;

    case PBS_BATCH_Rescq:

      free_rescrq(&preq->rq_ind.rq_rescq);

      break;

    case PBS_BATCH_SelectJobs:

    case PBS_BATCH_SelStat:

      free_attrlist(&preq->rq_ind.rq_select);

      break;

    case PBS_BATCH_SelStatAttr:

      free_attrlist(&preq->rq_ind.rq_select);
      free_attrlist(&preq->rq_ind.rq_status.rq_attr);

      break;

    case PBS_BATCH_RunJob:

    case PBS_BATCH_AsyrunJob:

      if (preq->rq_ind.rq_run.rq_destin)
        {
        free(preq->rq_ind.rq_run.rq_destin);
        preq->rq_ind.rq_run.rq_destin = NULL;
        }
      break;

    default:

      /* NO-OP */

      break;
    }  /* END switch (preq->rq_type) */

  free(preq);

  return;
  }  /* END free_br() */





static void freebr_manage(

  struct rq_manage *pmgr)

  {
  free_attrlist(&pmgr->rq_attr);

  return;
  }  /* END freebr_manage() */




static void freebr_cpyfile(

  struct rq_cpyfile *pcf)

  {

  struct rqfpair *ppair;

  while ((ppair = (struct rqfpair *)GET_NEXT(pcf->rq_pair)) != NULL)
    {
    delete_link(&ppair->fp_link);

    if (ppair->fp_local != NULL)
      free(ppair->fp_local);

    if (ppair->fp_rmt != NULL)
      free(ppair->fp_rmt);

    free(ppair);
    }

  return;
  }  /* END freebr_cpyfile() */





static void free_rescrq(

  struct rq_rescq *pq)

  {
  int i;

  i = pq->rq_num;

  while (i--)
    {
    if (*(pq->rq_list + i) != NULL)
      free(*(pq->rq_list + i));
    }

  if (pq->rq_list != NULL)
    free(pq->rq_list);

  return;
  }  /* END free_rescrq() */

/* END process_requests.c */


