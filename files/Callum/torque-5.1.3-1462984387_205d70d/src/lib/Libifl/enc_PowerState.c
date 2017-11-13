/*
 * encode_DIS_PowerState() - write the requested power state to a Batch Request
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include "dis.h"
#include "tcp.h" /* tcp_chan */

int encode_DIS_PowerState(

  struct tcp_chan *chan,
  unsigned short power_state)

  {
  return diswui(chan, power_state);
  }

