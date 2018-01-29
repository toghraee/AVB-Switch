/************************************************************************ 
 * RSTP library - 802.1AS GPTP Timing (802.1AS) 
 * Copyright (C) 2011-2012
 * Author: Abhay D.S
 * 
 * This file is part of GPTP Library
 * 
 * GPTP library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; version 2.1 
 * 
 * GPTP library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with RSTP library; see the file COPYING.  If not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA. 
 **********************************************************************/


 
#ifndef _GPTPSTP_STATMCH_H__
#define _GPTPSTP_STATMCH_H__

#include "vtss_gptp.h"
#include "vtss_gptp_private.h"

#define SIGNED_SHIFT(a, b) (((b) >= 0) ? ((a)<<(b)) : ((a)>>(-b)))
#define ANNOUNCE_INTERVAL_TICKS(ptp, port)   \
  SIGNED_SHIFT((1000/VTSS_GPTP_TICKS_PER_SEC), ((ptp)->ports[(port)].currentLogAnnounceInterval))
#define SYNC_INTERVAL_TICKS(ptp, port)       \
  SIGNED_SHIFT((1000/VTSS_GPTP_TICKS_PER_SEC), ((ptp)->ports[(port)].currentLogSyncInterval))
#define PDELAY_REQ_INTERVAL_TICKS(ptp, port) \
  SIGNED_SHIFT((1000/VTSS_GPTP_TICKS_PER_SEC), ((ptp)->ports[(port)].currentLogPdelayReqInterval))

void PortRoleSelection_StateMachine(struct gptpm *ptp);


void set_port_number(vars_uchar8 *portNumber, vars_ushort16 setValue);



extern void extract_announce(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer, struct PtpPriorityVector *pv);




/* ------------------------------------------------------------------------ */
static int rcvInfo(struct gptpm *ptp, vars_ulong32 port);

static void recordOtherAnnounceInfo(struct gptpm *ptp, vars_ulong32 port);


/* 802.1AS PortAnnounceInformation state machine (10.3.11) entry actions */
static void PortAnnounceInformation_StateMachine_SetState(struct gptpm *ptp, vars_uchar8 port, unsigned char newState);


/* 802.1AS PortAnnounceInformation state machine (10.3.11) transitions*/
void PortAnnounceInformation_StateMachine(struct gptpm *ptp, vars_uchar8 port);


/* ------------------------------------------------------------------------ */

static void updtRoleDisabledTree(struct gptpm *ptp);


static void clearReselectTree(struct gptpm *ptp);


static void updtRolesTree(struct gptpm *ptp);


static void setSelectedTree(struct gptpm *ptp);


/* 802.1AS PortRoleSelection state machine (10.3.12) transitions*/
void PortRoleSelection_StateMachine(struct gptpm *ptp);


static void computePdelayRateRatio(struct gptpm *ptp, vars_uchar8 port);


static void computePropTime(struct gptpm *ptp, vars_uchar8 port);


/* 802.1AS MDPdelayReq state machine (11.2.15.3) entry actions */
static void MDPdelayReq_StateMachine_SetState(struct gptpm *ptp, vars_uchar8 port, unsigned char newState);



int compare_port_ids(const vars_uchar8 *portIdA, const vars_uchar8 *portIdB) ;


/* 802.1AS MDPdelayReq state machine (11.2.15.3) transitions */
void MDPdelayReq_StateMachine(struct gptpm *ptp, vars_uchar8 port);


/* 802.1AS LinkDelaySyncIntervalSetting state machine (11.2.17.2) entry actions */
static void LinkDelaySyncIntervalSetting_StateMachine_SetState(struct gptpm *ptp, vars_uchar8 port, unsigned char newState);


/* 802.1AS LinkDelaySyncIntervalSetting state machine (11.2.17.2) transitions */
void LinkDelaySyncIntervalSetting_StateMachine(struct gptpm *ptp, vars_uchar8 port);

void PortAnnounceReceive_StateMachine(struct gptpm *ptp, vars_ulong32 port);

void get_source_port_id(struct gptpm *ptp, vars_uchar8 port, int bufferDirection,vars_uchar8 *packetBuffer, vars_uchar8 *sourcePortId);
vars_ushort16 get_port_number(const vars_uchar8 *portNumber);









#endif /* _GPTP_STATMCH_H__ */
