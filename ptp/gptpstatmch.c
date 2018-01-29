/************************************************************************ 
 * Author: Reza Toghraee
 * 
 * This file is part of GPTP library. 
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

/* Generic (abstract) state machine : */
 

#include "vtss_gptp.h"
#include "vtss_gptp_private.h"
#include "gptpstatmch.h"
/* #include "gptp_bpdu.h"*/


#ifdef VTSS_GPTP_NOT_WANTED
#if defined(__CX51__) || defined(__C51__)
const char *vtss_gptp_statmch_coseg = "This creates the CO segment of statmch";
#endif /* __CX51__ || __C51__ */
#else

#if defined(__CX51__) || defined(__C51__)
#include "print.h"
#endif

/* Enumerated type identifying the results of a BMCA comparison */
typedef enum {
  IS_PRESENT_MASTER,
  RETAIN_PRESENT_MASTER,
  REPLACE_PRESENT_MASTER
} BmcaResult;


static BmcaResult bmca_comparison(PtpPriorityVector* presentMaster, PtpPriorityVector* challenger) {

  int comparison = memcmp(presentMaster, challenger, sizeof(PtpPriorityVector));


  if(comparison > 0) {
    return REPLACE_PRESENT_MASTER;
  } else if (comparison == 0) {
    return IS_PRESENT_MASTER;
  } else {
    return RETAIN_PRESENT_MASTER;
  }
}


vars_ushort16 get_port_number(const vars_uchar8 *portNumber) {
  /* Fetch the big-endian packed value */
  return((vars_ushort16) ((portNumber[0] << 8) | portNumber[1]));
}



/* Gets the source port identity from a received packet */
void get_source_port_id(gptpm_t *ptp, vars_uchar8 port, int bufferDirection,
                        vars_uchar8 *packetBuffer, vars_uchar8 *sourcePortId) 
{
    vars_uchar8 i = 0;
    ComMessageHdr *msg   = NULL;
    bufferDirection = bufferDirection;  
    msg =  (ComMessageHdr *)packetBuffer;
	port = port;
	ptp = ptp;

    for(i = 0; i < 10 ; i ++)
    {
        sourcePortId[i] = msg->sourcePortIdentity[i];
    }
    return;
    
}


void get_receiving_port_id(gptpm_t *ptp, vars_ulong32 port, 
                        vars_uchar8 *packetBuffer, vars_uchar8 *sourcePortId)
{


   vars_uchar8 i = 0;
   ComMessageHdr *pComMesgHdr = NULL;
   PdelayRespMessage *pResp = NULL;

   pComMesgHdr = (ComMessageHdr *) packetBuffer;
   pResp = (PdelayRespMessage *) ((char *) pComMesgHdr+sizeof(ComMessageHdr));
   port = port;
   ptp = ptp;


    for(i = 0; i < 10 ; i ++)
    {
        sourcePortId[i] = pResp->requestingPortIdentity[i];
    }
    return;


}


void get_source_port_id_from_port (struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *sourcePortIdentity)
{

    sourcePortIdentity[0] = ptp->ports[port].port_properties.port_macaddr.macaddr[0];
    sourcePortIdentity[1] = ptp->ports[port].port_properties.port_macaddr.macaddr[1];  
    sourcePortIdentity[2] = ptp->ports[port].port_properties.port_macaddr.macaddr[2];
    sourcePortIdentity[3] = 0xff;
    sourcePortIdentity[4] = 0xfe;
    sourcePortIdentity[5] = ptp->ports[port].port_properties.port_macaddr.macaddr[3];
    sourcePortIdentity[6] = ptp->ports[port].port_properties.port_macaddr.macaddr[4];
    sourcePortIdentity[7] = ptp->ports[port].port_properties.port_macaddr.macaddr[5];
    sourcePortIdentity[8] = (vars_uchar8)(port << 8);
    sourcePortIdentity[9] = (vars_uchar8)(port);


}


vars_ushort16 get_rx_announce_steps_removed(gptpm_t *ptp, vars_ulong32  port, vars_uchar8 *rxBuffer) {
  vars_uchar8 i;
  vars_ushort16  stepsRemoved = 0;
  ComMessageHdr *pComMesgHdr = (ComMessageHdr *) rxBuffer;
  AnnounceMessage *pAnnounceMesg = (AnnounceMessage *) ((vars_uchar8 *) pComMesgHdr+sizeof(ComMessageHdr));
  ptp = ptp;
  port = port;

  for(i = 0; i < 2; i++)
  {
      stepsRemoved = (stepsRemoved << 8) + pAnnounceMesg->stepsRemoved[i];
  }  
  
  return(stepsRemoved);
}


vars_ushort16 get_rx_announce_path_trace(gptpm_t *ptp, vars_ulong32  port, vars_uchar8 *rxBuffer, PtpClockIdentity *pathTrace)
{

    vars_ushort16 pathTraceLength = 0;

    vars_uchar8 i = 0;
    int j = 0;
  
    ComMessageHdr *pComMesgHdr = (ComMessageHdr *) rxBuffer;
    AnnounceMessage *pAnnounceMesg = (AnnounceMessage *) ((vars_uchar8 *) pComMesgHdr+sizeof(ComMessageHdr));
    char *pPathTrace = ((vars_uchar8 *) pComMesgHdr+sizeof(ComMessageHdr)+sizeof(AnnounceMessage));
  
    pathTraceLength = *((unsigned short*)pAnnounceMesg->tlvLength)/8;

	port = port;
	ptp = ptp;


    while(i < GPTP_MAX_PATH_TRACE && j < (*((unsigned short*)pAnnounceMesg->tlvLength)))
    {
        pathTrace[i][0] = pPathTrace[j++];
        pathTrace[i][1] = pPathTrace[j++];    
        pathTrace[i][2] = pPathTrace[j++];    
        pathTrace[i][3] = pPathTrace[j++];    
        pathTrace[i][4] = pPathTrace[j++];    
        pathTrace[i][5] = pPathTrace[j++];    
        pathTrace[i][6] = pPathTrace[j++];    
        pathTrace[i][7] = pPathTrace[j++];    
        i++;

    } 
     
  return pathTraceLength;
}



vars_char8 qualifyAnnounce(struct gptpm *ptp, vars_ulong32 port) {
  PtpPortIdentity  sourcePortId;
  vars_ushort16 stepsRemoved;
  vars_ulong32 pathTraceLength;
  PtpClockIdentity pathTrace[GPTP_MAX_PATH_TRACE];
  vars_ulong32         i;

  get_source_port_id(ptp, port, RECEIVED_PACKET, ptp->ports[port].rcvdAnnouncePtr, (vars_uchar8*)&sourcePortId);

  

  if (0 == memcmp(sourcePortId.clockIdentity, ptp->systemPriority.sourcePortIdentity.clockIdentity, sizeof(PtpClockIdentity))) {
    return FALSE;
  }

  stepsRemoved = get_rx_announce_steps_removed(ptp, port, ptp->ports[port].rcvdAnnouncePtr);

  
  
  if (stepsRemoved > 255) {
    return FALSE;
  }

  pathTraceLength = get_rx_announce_path_trace(ptp, port, ptp->ports[port].rcvdAnnouncePtr, pathTrace);

  

  for (i=0; i<pathTraceLength; i++) {
    if (0 == memcmp(pathTrace[i], ptp->systemPriority.sourcePortIdentity.clockIdentity, sizeof(PtpClockIdentity))) {  
      return FALSE;
    }
  }

  if (ptp->ports[port].selectedRole == PTP_SLAVE) {
    memcpy(ptp->pathTrace, pathTrace, sizeof(PtpClockIdentity)*pathTraceLength);
    ptp->pathTraceLength = pathTraceLength;
  }

  return TRUE;
}

/* 802.1AS PortAnnounceReceive state machine (10.3.10) */
void PortAnnounceReceive_StateMachine(struct gptpm *ptp, vars_ulong32 port)
{
  struct vtss_gptp_port_vars *pPort = &ptp->ports[port];

  if (!pPort->portEnabled || !pPort->pttPortEnabled || !pPort->asCapable) {
    pPort->rcvdMsg = FALSE;
  } else {
    pPort->rcvdMsg = qualifyAnnounce(ptp, port);
  }
}

#define ALTERNATE_MASTER_FLAG(msgHdr)        (msgHdr->flagField[0] & 0x1)

#define TWO_STEP_FLAG(msgHdr)                (msgHdr->flagField[0] & 0x2)

#define UNICAST_FLAG(msgHdr)                 (msgHdr->flagField[0] & 0x4)

#define PTP_PROFILE_SPECIFIC1_FLAG(msgHdr)   (msgHdr->flagField[0] & 0x8)

#define PTP_PROFILE_SPECIFIC2_FLAG(msgHdr)   (msgHdr->flagField[0] & 0x10)

#define LEAP61_FLAG(msgHdr)                  (msgHdr->flagField[1] & 0x1)

#define LEAP59_FLAG(msgHdr)                  (msgHdr->flagField[1] & 0x2)

#define CURRENT_UTC_OFFSET_VALID_FLAG(msgHdr)(msgHdr->flagField[1] & 0x4)

#define PTP_TIMESCALE_FLAG(msgHdr)           (msgHdr->flagField[1] & 0x8)

#define TIME_TRACEABLE_FLAG(msgHdr)          (msgHdr->flagField[1] & 0x10)

#define FREQUENCY_TRACEABLE_FLAG(msgHdr)     (msgHdr->flagField[1] & 0x20)





void set_port_number(vars_uchar8 *portNumber, vars_ushort16 setValue) {
  portNumber[0] = (vars_uchar8) (setValue << 8);
  portNumber[1] = (vars_uchar8) setValue;
}



void extract_announce(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer,PtpPriorityVector *pv)
{

  ComMessageHdr *pComMesgHdr = (ComMessageHdr *) rxBuffer;
  AnnounceMessage *pAnnounceMesg = (AnnounceMessage *) ((vars_uchar8 *) pComMesgHdr+sizeof(ComMessageHdr));

  /* Get the source port identity*/
  get_source_port_id(ptp, port, RECEIVED_PACKET, rxBuffer, (vars_uchar8*)&pv->sourcePortIdentity);

#if 0
  /* Exract the domain number */
  wordOffset = DOMAIN_NUMBER_OFFSET;
  packetWord = read_packet(rxBuffer, &wordOffset);
  properties->domainNumber = ((packetWord >> 8) & 0x0FF);

  /* Extract the current UTC offset */
  wordOffset = UTC_OFFSET_OFFSET;
  packetWord = read_packet(rxBuffer, &wordOffset);
  properties->currentUtcOffset = (packetWord & 0x0FFFF);
#endif



  
  /*Extract the the grandmaster priorities */
  pv->rootSystemIdentity.priority1 = pAnnounceMesg->grandmasterPriority1;
  pv->rootSystemIdentity.clockClass  = pAnnounceMesg->clockClass;
  pv->rootSystemIdentity.clockAccuracy = pAnnounceMesg->clockAccuracy;
  pv->rootSystemIdentity.offsetScaledLogVariance[0] = pAnnounceMesg->clockOffsetScaledLogVariance[0];
  pv->rootSystemIdentity.offsetScaledLogVariance[1] = pAnnounceMesg->clockOffsetScaledLogVariance[1];
  pv->rootSystemIdentity.priority2 = pAnnounceMesg->grandmasterPriority2;
  pv->rootSystemIdentity.clockIdentity[0] = pAnnounceMesg->grandmasterIdentity[0];
  pv->rootSystemIdentity.clockIdentity[1] = pAnnounceMesg->grandmasterIdentity[1];
  pv->rootSystemIdentity.clockIdentity[2] = pAnnounceMesg->grandmasterIdentity[2];
  pv->rootSystemIdentity.clockIdentity[3] = pAnnounceMesg->grandmasterIdentity[3];
  pv->rootSystemIdentity.clockIdentity[4] = pAnnounceMesg->grandmasterIdentity[4];
  pv->rootSystemIdentity.clockIdentity[5] = pAnnounceMesg->grandmasterIdentity[5];
  pv->rootSystemIdentity.clockIdentity[6] = pAnnounceMesg->grandmasterIdentity[6];
  pv->rootSystemIdentity.clockIdentity[7] = pAnnounceMesg->grandmasterIdentity[7];
  pv->stepsRemoved[0] = pAnnounceMesg->stepsRemoved[0];
  pv->stepsRemoved[1] = pAnnounceMesg->stepsRemoved[1];  

  /* Port number is 1 based and is the port it came in on */
  set_port_number(pv->portNumber, (port + 1));
}




/* ------------------------------------------------------------------------ */
static int rcvInfo(struct gptpm *ptp, vars_ulong32 port) {
  int result;
  extract_announce(ptp, port, ptp->ports[port].rcvdAnnouncePtr, &ptp->ports[port].messagePriority);
  result = memcmp(&ptp->ports[port].portPriority, &ptp->ports[port].messagePriority, sizeof(PtpPriorityVector));

  if (result > 0) {
    return SuperiorMasterInfo;
  } else if (result == 0) {
    return RepeatedMasterInfo;
  } else {
    return InferiorMasterInfo;
  }
}

static void recordOtherAnnounceInfo(struct gptpm *ptp, vars_ulong32 port)
{

  ComMessageHdr *pComMesgHdr = (ComMessageHdr *)(ptp->ports[port].rcvdAnnouncePtr);
  AnnounceMessage *pAnnounceMesg = (AnnounceMessage *) ((vars_uchar8 *) pComMesgHdr+sizeof(ComMessageHdr));


  /* Extract the following: leap61 */
  ptp->ports[port].annleap_61 = LEAP61_FLAG(pComMesgHdr);

  /* leap 59 */
  ptp->ports[port].annleap_59 = LEAP59_FLAG(pComMesgHdr);

  /*currentUtcOffsetValid */

  ptp->ports[port].anncurrent_utc_offset_valid = CURRENT_UTC_OFFSET_VALID_FLAG(pComMesgHdr);

  /*timeTraceable */

  ptp->ports[port].anntime_traceable = TIME_TRACEABLE_FLAG(pComMesgHdr);

  /* frequenceTraceable */

  ptp->ports[port].annfrequency_traceable = FREQUENCY_TRACEABLE_FLAG(pComMesgHdr);

  /* currentUtcOffset */

  ptp->ports[port].ann_current_utc_offset = VTSS_COMMON_UNALIGNED_GET_2B(pAnnounceMesg->currentUtcOffset);

  /* timeSource */

  ptp->ports[port].anntime_source = pAnnounceMesg->timeSource;

}

/* 802.1AS PortAnnounceInformation state machine (10.3.11) entry actions */
static void PortAnnounceInformation_StateMachine_SetState(struct gptpm *ptp, vars_uchar8 port, unsigned char newState)
{
  struct vtss_gptp_port_vars *pPort = &ptp->ports[port];

 
  pPort->portAnnounceInformation_State = newState;

  print_str("Entering Port Announce Information State Machine\r\n");

  vtss_printf("Port Announce Info SM Set State Int Port No:%d, State: %d: Ex Port Number %d\r\n",(int)pPort->port_number,(int)newState,(int)port);

  switch (newState)
  {
    default:
    case PortAnnounceInformation_BEGIN:
    case PortAnnounceInformation_DISABLED:
      pPort->rcvdMsg                = FALSE;
      pPort->announceTimeoutCounter = 0;
      pPort->infoIs                 = InfoIs_Disabled;
      pPort->reselect               = TRUE;
      pPort->selected               = FALSE;
      memset(&pPort->portPriority, 0xFF, sizeof(PtpPriorityVector));
      break;

    case PortAnnounceInformation_AGED:
      pPort->infoIs   = InfoIs_Aged;
      pPort->reselect = TRUE;
      pPort->selected = FALSE;
      break;

    case PortAnnounceInformation_UPDATE:
      memcpy(&pPort->portPriority, &pPort->masterPriority, sizeof(PtpPriorityVector));
      pPort->portStepsRemoved = ptp->masterStepsRemoved;
      pPort->updtInfo         = FALSE;
      pPort->infoIs           = InfoIs_Mine;
      pPort->newInfo          = TRUE;
      break;

    case PortAnnounceInformation_SUPERIOR_MASTER_PORT:
      memcpy(&pPort->portPriority, &pPort->messagePriority, sizeof(PtpPriorityVector));
      //pPort->portStepsRemoved = rcvdAnnouncePtr->stepsRemoved; ...
      recordOtherAnnounceInfo(ptp, port);
      pPort->announceTimeoutCounter = 0;
      pPort->infoIs                 = InfoIs_Received;
      pPort->reselect               = TRUE;
      pPort->selected               = FALSE;
      pPort->rcvdMsg                = FALSE;
      break;

    case PortAnnounceInformation_REPEATED_MASTER_PORT:
      pPort->announceTimeoutCounter = 0;
      pPort->rcvdMsg = FALSE;
      break;

    case PortAnnounceInformation_INFERIOR_MASTER_OR_OTHER_PORT:
      pPort->rcvdMsg = FALSE;
      break;

    case PortAnnounceInformation_CURRENT:
      break;

    case PortAnnounceInformation_RECEIVE:
      pPort->rcvdInfo = rcvInfo(ptp, port);
      break;
  }
}

/* 802.1AS PortAnnounceInformation state machine (10.3.11) transitions*/
void PortAnnounceInformation_StateMachine(struct gptpm *ptp, vars_uchar8 port)
{
  struct vtss_gptp_port_vars *pPort = &ptp->ports[port];

  

  PortAnnounceInformation_State_t prevState;

  //print_str("PortAnnounceInformation State Machine\r\n");
  do
  {
    prevState = pPort->portAnnounceInformation_State;
	vtss_printf("PortAnnounceInformation_StateMachine : %d %d %d PORTNO\r\n",pPort->portEnabled,pPort->pttPortEnabled,pPort->asCapable,port);

    if (!pPort->portEnabled || !pPort->pttPortEnabled || !pPort->asCapable)
    {
      if (pPort->portAnnounceInformation_State != PortAnnounceInformation_DISABLED)
      {
        /* Disabling the port immediately forces the state machine into the disabled state */
		vtss_printf("Port Announce Information State Machine, disabling ports %d\r\n");
        PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_DISABLED);
      }
    }
    else
    {
      switch (pPort->portAnnounceInformation_State)
      {
        default:
        case PortAnnounceInformation_BEGIN:
          PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_DISABLED);
          break;
            
        case PortAnnounceInformation_DISABLED:
          if (pPort->portEnabled && pPort->pttPortEnabled && pPort->asCapable) {
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_AGED);
          } else if (ptp->ports[port].rcvdMsg) {
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_DISABLED);
          }
          break;

        case PortAnnounceInformation_AGED:
          if (pPort->selected && pPort->updtInfo) {
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_UPDATE);
          }
          break;

        case PortAnnounceInformation_UPDATE:
        case PortAnnounceInformation_SUPERIOR_MASTER_PORT:
        case PortAnnounceInformation_REPEATED_MASTER_PORT:
        case PortAnnounceInformation_INFERIOR_MASTER_OR_OTHER_PORT:
          PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_CURRENT);
          break;

        case PortAnnounceInformation_CURRENT:
          if (pPort->selected && pPort->updtInfo) {
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_UPDATE);
          } else if (pPort->rcvdMsg && !pPort->updtInfo) {
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_RECEIVE);
          } else {
            
            int syncTimeout = (pPort->syncTimeoutCounter >= SYNC_INTERVAL_TICKS(ptp, port) * pPort->syncReceiptTimeout * 2);
            int announceTimeout = (pPort->announceTimeoutCounter >= ANNOUNCE_INTERVAL_TICKS(ptp, port) * pPort->announceReceiptTimeout);
			
            if ((pPort->infoIs == InfoIs_Received) &&
                (announceTimeout || (syncTimeout && ptp->gmPresent)) &&
                !pPort->updtInfo && !pPort->rcvdMsg) {
             	VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Announce AGED: (announce %d >= %d || sync %d >= %d)\n",
                pPort->announceTimeoutCounter, ANNOUNCE_INTERVAL_TICKS(ptp, port) * pPort->announceReceiptTimeout,
                pPort->syncTimeoutCounter, SYNC_INTERVAL_TICKS(ptp, port) * pPort->syncReceiptTimeout * 2));

              PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_AGED);

              /* Update stats */
              if (announceTimeout) {
                pPort->stats.announceReceiptTimeoutCount++;
              }
              if (syncTimeout) {
                pPort->stats.syncReceiptTimeoutCount++;
              }
            }
          }
          break;

        case PortAnnounceInformation_RECEIVE:
          if (pPort->rcvdInfo == SuperiorMasterInfo) {
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_SUPERIOR_MASTER_PORT);
          } else if (pPort->rcvdInfo == RepeatedMasterInfo) {
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_REPEATED_MASTER_PORT);
          } else { /* InferiorMasterInfo or OtherInfo */
            PortAnnounceInformation_StateMachine_SetState(ptp, port, PortAnnounceInformation_INFERIOR_MASTER_OR_OTHER_PORT);
          }
          break;
      }
    }
  } while (prevState != pPort->portAnnounceInformation_State);
}

/* ------------------------------------------------------------------------ */

static void updtRoleDisabledTree(struct gptpm *ptp)
{
  vars_uchar8 i;

  print_str("Entering updRoleDisabled Tree\r\n");
  /* Set all port roles to disabled */
  for (i = 0; i<ptp->numPorts; i++) {
    ptp->ports[i].selectedRole = PTP_DISABLED;
  }

  /* Set the lastGmPriority to all 1's */
  memset(&ptp->lastGmPriority, 0xFF, sizeof(PtpPriorityVector));
  ptp->gmPriority = &ptp->lastGmPriority;

  /* Init the path array with thisClock */
  ptp->pathTraceLength = 1;
  memcpy(&ptp->pathTrace[0], ptp->systemPriority.rootSystemIdentity.clockIdentity, sizeof(PtpClockIdentity));
  print_str("Exiting updtRoleDisabledTree\r\n");
}

static void clearReselectTree(struct gptpm *ptp)
{
  vars_uchar8 i;
  for (i = 0; i<ptp->numPorts; i++) {
    ptp->ports[i].reselect = FALSE;
  }
}






static void updtRolesTree(struct gptpm *ptp)
{
  vars_uchar8 i;

  vars_uchar8 set_sys_time_global_vars_from_port = 0;

  print_str("Entering updtRoles Tree\r\n");

  /* Save a copy of the last gm priority before we change any of the path priority vectors */
  memcpy(&ptp->lastGmPriority, ptp->gmPriority, sizeof(PtpPriorityVector));

  /* Compute gmPathPriority vectors */

  print_str("Computing Path Priority Vectors\r\n");
  for (i = 0; i<ptp->numPorts; i++) {
    struct vtss_gptp_port_vars *pPort = &ptp->ports[i];
    
    int syncTimeout = (pPort->syncTimeoutCounter >= SYNC_INTERVAL_TICKS(ptp, i) * pPort->syncReceiptTimeout * 2);
    int announceTimeout = (pPort->announceTimeoutCounter >= ANNOUNCE_INTERVAL_TICKS(ptp, i) * pPort->announceReceiptTimeout);
    if (!announceTimeout & (!ptp->gmPresent || !syncTimeout)) {
      memcpy(&pPort->gmPathPriority, &pPort->portPriority, sizeof(PtpPriorityVector));
      VTSS_COMMON_UNALIGNED_PUT_2B(pPort->gmPathPriority.stepsRemoved,(VTSS_COMMON_UNALIGNED_GET_2B(pPort->gmPathPriority.stepsRemoved)+1));      
    } else {
      memset(&pPort->gmPathPriority, 0xFF, sizeof(PtpPriorityVector));
      if (syncTimeout) {
        pPort->syncTimeoutCounter = 0;
      }
    }
  }

  print_str("Completed: Computing Path Priority Vectors\r\n");

  /* Update the gmPriority vector */
  print_str("Started : GM Priority Vectors\r\n");
  ptp->gmPriority = &ptp->systemPriority;
  ptp->masterStepsRemoved = 0;
  for (i = 0; i<ptp->numPorts; i++) {
    if (0 != compare_clock_identity(ptp->systemPriority.rootSystemIdentity.clockIdentity,
                                    ptp->ports[i].gmPathPriority.rootSystemIdentity.clockIdentity)) {
      if (REPLACE_PRESENT_MASTER == bmca_comparison(ptp->gmPriority, &ptp->ports[i].gmPathPriority)) {
        ptp->gmPriority = &ptp->ports[i].gmPathPriority;
        set_sys_time_global_vars_from_port = 1;
        ptp->leap_61 = ptp->ports[i].annleap_61;
        ptp->leap_59 = ptp->ports[i].annleap_59;
        ptp->current_utc_offset_valid = ptp->ports[i].anncurrent_utc_offset_valid;
        ptp->time_traceable = ptp->ports[i].anntime_traceable;
        ptp->frequency_traceable = ptp->ports[i].annfrequency_traceable;
        ptp->current_utc_offset = ptp->ports[i].ann_current_utc_offset;
        ptp->time_source = ptp->ports[i].anntime_source;
        
        ptp->masterStepsRemoved = (VTSS_COMMON_UNALIGNED_GET_2B(ptp->ports[i].messagePriority.stepsRemoved) + 1);
      }
    }
  }
  print_str("Completed : GM Priority Vectors\r\n");



  /* Compute masterPriority vectors and assign port roles*/
  for (i = 0; i<ptp->numPorts; i++) {
    struct vtss_gptp_port_vars *pPort = &ptp->ports[i];

   print_str("Started :Master Priority  Vectors\r\n");
    /* masterPriority */
    memcpy(&pPort->masterPriority, ptp->gmPriority, sizeof(PtpPriorityVector));
    memcpy(pPort->masterPriority.sourcePortIdentity.clockIdentity,
           ptp->systemPriority.sourcePortIdentity.clockIdentity, sizeof(PtpClockIdentity));
    set_port_number(pPort->masterPriority.sourcePortIdentity.portNumber, (i+1));
    set_port_number(pPort->masterPriority.portNumber, (i+1));

    /* selectedRole */
    switch (pPort->infoIs) {
      default:
      case InfoIs_Disabled:
        pPort->selectedRole = PTP_DISABLED;
        break;

      case InfoIs_Aged:
        pPort->selectedRole = PTP_MASTER;
        pPort->updtInfo = TRUE;
        break;

      case InfoIs_Mine:
        pPort->selectedRole = PTP_MASTER;
        if ((pPort->portStepsRemoved != ptp->masterStepsRemoved) ||
            (IS_PRESENT_MASTER != bmca_comparison(&pPort->portPriority, &pPort->masterPriority))) {
          pPort->updtInfo = TRUE;
        }
        break;

      case InfoIs_Received:
        if (ptp->gmPriority == &pPort->gmPathPriority) {
          pPort->selectedRole = PTP_SLAVE;
          pPort->updtInfo = FALSE;
        } else if (REPLACE_PRESENT_MASTER == bmca_comparison(&pPort->portPriority, &pPort->masterPriority)) {
          pPort->selectedRole = PTP_MASTER;
          pPort->updtInfo = TRUE;
        } else {
          pPort->selectedRole = PTP_PASSIVE;
          pPort->updtInfo = FALSE;
        }
    }

  }

  /* Update gmPresent */
  ptp->gmPresent = (ptp->gmPriority->rootSystemIdentity.priority1 == 255) ? 0 : 1;

  /* TODO: Do we need selectedRole[0]? */

  /* Update pathTrace */
  print_str("Started: Update Path Trace\r\n");
  if (ptp->gmPriority == &ptp->systemPriority) {
    /* Init the path array with thisClock */
    ptp->pathTraceLength = 1;
    memcpy(&ptp->pathTrace[0], ptp->systemPriority.rootSystemIdentity.clockIdentity, sizeof(PtpClockIdentity));
    ptp->grand_master = 1;
    
  }
  else
  {
      ptp->grand_master = 0;
  }

  

  if (0 != memcmp(ptp->gmPriority, &ptp->lastGmPriority, sizeof(PtpPriorityVector))) {
    ptp->newMaster              = TRUE;


  }
}

static void setSelectedTree(struct gptpm *ptp)
{
  vars_uchar8 i;
  for (i = 0; i<ptp->numPorts; i++) {
    ptp->ports[i].selected = 1;
  }
}

/* 802.1AS PortRoleSelection state machine (10.3.12) transitions*/
void PortRoleSelection_StateMachine(struct gptpm *ptp)
{
  vars_uchar8 i;

  PortRoleSelection_State_t prevState;
  do
  {
    print_str("Entering PortRoleSelection_StateMachine\r\n");
    prevState = ptp->portRoleSelection_State;

    VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Port Role: Present  State %d\n", ptp->portRoleSelection_State));

    switch (ptp->portRoleSelection_State)
    {
      default:
      case PortRoleSelection_INIT_BRIDGE:
        updtRoleDisabledTree(ptp);
        ptp->portRoleSelection_State = PortRoleSelection_ROLE_SELECTION;
        break;

      case PortRoleSelection_ROLE_SELECTION:
        for (i = 0; i<ptp->numPorts; i++) {
          if (ptp->ports[i].reselect) {
            clearReselectTree(ptp);
            updtRolesTree(ptp);
            setSelectedTree(ptp);
            break;
          }
        }
        break;
    }
  } while (prevState != ptp->portRoleSelection_State);
  print_str("Exiting PortRoleSelection_StateMachine\r\n");
}




static void computePdelayRateRatio(struct gptpm *ptp, vars_uchar8 port)
{
    if (ptp->ports[port].initPdelayRespReceived == FALSE)
    {
        /* Capture the initial PDELAY response */
        ptp->ports[port].initPdelayRespReceived = TRUE;
        ptp->ports[port].pdelayRespTxTimestampI = ptp->ports[port].pdelayRespTxTimestamp;
        ptp->ports[port].pdelayRespRxTimestampI = ptp->ports[port].pdelayRespRxTimestamp;
    }
    else
    {
        vtss_ptp_time_t difference;
        vtss_ptp_time_t difference2;
        _UINT64 nsResponder;
        _UINT64 nsRequester;
        _UINT64 rateRatio;
        _UINT64 tempval;
        _UINT64 tempval1;
        _UINT64 tempval2;
        int shift;

        timestamp_difference(&ptp->ports[port].pdelayRespTxTimestamp, &ptp->ports[port].pdelayRespTxTimestampI, &difference);
        timestamp_difference(&ptp->ports[port].pdelayRespRxTimestamp, &ptp->ports[port].pdelayRespRxTimestampI, &difference2);

     /* Keep rolling the interval forward or we will react really slowly to changes in our
         * rate relative to our neighbor.
         */
        ptp->ports[port].pdelayRespTxTimestampI = ptp->ports[port].pdelayRespTxTimestamp;
        ptp->ports[port].pdelayRespRxTimestampI = ptp->ports[port].pdelayRespRxTimestamp;

        /* The raw differences have been computed; sanity-check the peer delay timestamps; if the 
         * initial Tx or Rx timestamp is later than the present one, the initial ones are bogus and
         * must be replaced.
         */ 

   
      if((difference.secondsUpper & 0x80000000) |
           (difference2.secondsUpper & 0x80000000)) {
          ptp->ports[port].initPdelayRespReceived = FALSE;
          ptp->ports[port].neighborRateRatioValid = FALSE;
        } else {

       
           Uint32ToUint64(tempval,difference.secondsLower);
           nsResponder = Uint64AddUint32(Uint64TimesUint32(tempval,1000000000),(unsigned long)(difference.nanoseconds));

           

           Uint32ToUint64(tempval,difference2.secondsLower);
           nsRequester = Uint64AddUint32(Uint64TimesUint32(tempval,1000000000),(unsigned long)difference2.nanoseconds);



          for (shift = 0; shift < 31; shift++)
          {
              if (nsResponder.HighPart & (1<<(31-shift))) break;
          }


          tempval = Uint64RShift(nsRequester,(31-shift));


          
          if(!Int64IsZero(tempval))
          {
              tempval1 = Uint64LShift(nsResponder,shift);
              tempval2 = Uint64RShift(nsRequester,(31-shift));
              rateRatio = Uint64DividedByUint64(tempval1,tempval2);

              ptp->ports[port].neighborRateRatio = Uint64ToUint32(rateRatio);

              ptp->ports[port].neighborRateRatioValid = TRUE;
              
              
          }


          VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Responder delta: %08X%08X.%08X (%llu ns)\n", difference.secondsUpper,
             difference.secondsLower, difference.nanoseconds, nsResponder));
          VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Requester delta: %08X%08X.%08X (%llu ns)\n", difference2.secondsUpper,
             difference2.secondsLower, difference2.nanoseconds, nsRequester));
          VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Rate ratio: %08X (shift %d)\n", ptp->ports[port].neighborRateRatio, shift));


    } /* if(differences are sane) */
  }
}

static void computePropTime(struct gptpm *ptp, vars_uchar8 port)
{
  if (ptp->ports[port].neighborRateRatioValid)
  {
    vtss_ptp_time_t difference;
    vtss_ptp_time_t difference2;
    _UINT64 nsResponder;
    _UINT64 nsRequester;
    _UINT64 tempval;

    timestamp_difference(&ptp->ports[port].pdelayRespTxTimestamp, &ptp->ports[port].pdelayReqRxTimestamp, &difference);
    timestamp_difference(&ptp->ports[port].pdelayRespRxTimestamp, &ptp->ports[port].pdelayReqTxTimestamp, &difference2);

    
    
    Uint32ToUint64(tempval,difference.secondsLower);
    nsResponder = Uint64AddUint32(Uint64TimesUint32(tempval,1000000000),(unsigned long)(difference.nanoseconds));
    
              
    
    Uint32ToUint64(tempval,difference2.secondsLower);
    nsRequester = Uint64AddUint32(Uint64TimesUint32(tempval,1000000000),(unsigned long)difference2.nanoseconds);

    tempval = Uint64TimesUint32(nsRequester,ptp->ports[port].neighborRateRatio);
    tempval = Uint64RShift(tempval,31);
    tempval = Int64ToUint64(Int64Subtract(Uint64ToInt64(tempval),Uint64ToInt64(nsResponder)));
    tempval = Int64ToUint64(tempval);


    ptp->ports[port].neighborPropDelay = Uint64ToUint32(Uint64RShift(tempval,1));  

   


    VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Responder delta: %08X%08X.%08X (%llu ns)\n", difference.secondsUpper,
      difference.secondsLower, difference.nanoseconds, nsResponder));
    VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Requester delta: %08X%08X.%08X (%llu ns)\n", difference2.secondsUpper,
      difference2.secondsLower, difference2.nanoseconds, nsRequester));
    VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Prop Delay: %08X\n", ptp->ports[port].neighborPropDelay));

  }
}

/* 802.1AS MDPdelayReq state machine (11.2.15.3) entry actions */
static void MDPdelayReq_StateMachine_SetState(struct gptpm *ptp, vars_uchar8 port, unsigned char newState)
{
  vars_uchar8 rxSourcePortId[PORT_ID_BYTES];

  ComMessageHdr *pComMesgHdr = NULL;

  PdelayRespFollowUpMessage *pPDelayResFupMsg = NULL;

  PdelayRespMessage *pDelayRespMsg = NULL;


  ptp->ports[port].mdPdelayReq_State = newState;

  vtss_printf("MDPdelayReq_StateMachine_SetState Port No %d and New state %d \r\n",(int)port,(int)ptp->ports[port].mdPdelayReq_State);

  switch (newState)
  {
    default:
    case MDPdelayReq_NOT_ENABLED:
      ptp->ports[port].asCapable = FALSE;

      /* Track consecutive multiple pdelay responses for AVnu_PTP-5 PICS */
      ptp->ports[port].multiplePdelayResponses = 0;
      break;

    case MDPdelayReq_INITIAL_SEND_PDELAY_REQ:
      ptp->ports[port].initPdelayRespReceived = FALSE;
      ptp->ports[port].neighborRateRatio = 0x80000000; /* 1.0 fixed point 1.31*/
      ptp->ports[port].rcvdMDTimestampReceive = FALSE;
      ptp->ports[port].pdelayReqSequenceId = 0x0000;/*Spec says random */
      ptp->ports[port].rcvdPdelayResp = FALSE;
      ptp->ports[port].rcvdPdelayRespFollowUp = FALSE;
      transmit_pdelay_request(ptp, port);
      ptp->ports[port].pdelayIntervalTimer = 0; /* currentTime ("now" is zero ticks)*/
      ptp->ports[port].lostResponses = 0;
      ptp->ports[port].isMeasuringDelay = FALSE;
      ptp->ports[port].asCapable = FALSE;
      ptp->ports[port].neighborRateRatioValid = FALSE;

      /* Track consecutive multiple pdelay responses for AVnu_PTP-5 PICS */
      ptp->ports[port].pdelayResponses = 0;
      ptp->ports[port].multiplePdelayResponses = 0;
      break;

    case MDPdelayReq_RESET:
      ptp->ports[port].initPdelayRespReceived = FALSE;
      ptp->ports[port].rcvdPdelayResp = FALSE;
      ptp->ports[port].rcvdPdelayRespFollowUp = FALSE;
      if (ptp->ports[port].lostResponses <= ptp->ports[port].allowedLostResponses)
      {
        ptp->ports[port].lostResponses++;
      }
      else
      {
        ptp->ports[port].isMeasuringDelay = FALSE;
        ptp->ports[port].asCapable = FALSE;
      }
      break;

    case MDPdelayReq_SEND_PDELAY_REQ:
      ptp->ports[port].pdelayReqSequenceId++;
      transmit_pdelay_request(ptp, port);
      ptp->ports[port].pdelayIntervalTimer = 0; /* currentTime ("now" is zero ticks)*/

      /* Track consecutive multiple pdelay responses for AVnu_PTP-5 PICS */
      if (ptp->ports[port].pdelayResponses == 1) {
        ptp->ports[port].multiplePdelayResponses = 0;
      } else if (ptp->ports[port].pdelayResponses > 1) {
        ptp->ports[port].multiplePdelayResponses++;
        if (ptp->ports[port].multiplePdelayResponses >= 3) {
          VTSS_GPTP_TRACE(VTSS_RSTP_TRLVL_DEBUG,("Disabling AS on port %d due to multiple pdelay responses (%d %d).\n",
            port+1, ptp->ports[port].pdelayResponses, ptp->ports[port].multiplePdelayResponses));
          ptp->ports[port].portEnabled = FALSE;
        }
      }
      ptp->ports[port].pdelayResponses = 0;
      break;

    case MDPdelayReq_WAITING_FOR_PDELAY_RESP:
      ptp->ports[port].rcvdMDTimestampReceive = FALSE;
      break;

    case MDPdelayReq_WAITING_FOR_PDELAY_RESP_FOLLOW_UP:
      ptp->ports[port].rcvdPdelayResp = FALSE;

      /* Obtain the peer delay request receive timestamp that our peer has just sent.
       * (Trsp2 - responder local clock) */

      pComMesgHdr = (ComMessageHdr *)(ptp->ports[port].rcvdPdelayRespPtr);

      pDelayRespMsg = (PdelayRespMessage*)((char *) pComMesgHdr+sizeof(ComMessageHdr));

      get_timestamp(ptp, port,0,pDelayRespMsg->requestReceiptTimestamp,&ptp->ports[port].pdelayReqRxTimestamp);     

  

      /* Capture the hardware timestamp at which we received this packet, and hang on to 
       * it for delay and rate calculation. (Trsp4 - our local clock) */
       /* The above is done when the pdelay response packet was received */
      
      break;

    case MDPdelayReq_WAITING_FOR_PDELAY_INTERVAL_TIMER:
      ptp->ports[port].rcvdPdelayRespFollowUp = FALSE;
 
      /* Obtain the follow up timestamp for delay and rate calculation.
       * (Trsp3 - responder local clock) */

      pComMesgHdr = (ComMessageHdr *)(ptp->ports[port].rcvdPdelayRespFollowUpPtr);
      pPDelayResFupMsg = (PdelayRespFollowUpMessage*)((char *) pComMesgHdr+sizeof(ComMessageHdr));
      get_timestamp(ptp, port,0, pPDelayResFupMsg->responseOriginTimestamp,&ptp->ports[port].pdelayRespTxTimestamp);

      if (ptp->ports[port].computeNeighborRateRatio)
      {
        computePdelayRateRatio(ptp, port);
      }
      if (ptp->ports[port].computeNeighborPropDelay)
      {
        computePropTime(ptp, port);
      }
      ptp->ports[port].lostResponses = 0;
      ptp->ports[port].isMeasuringDelay = TRUE;
  
      get_source_port_id(ptp, port, RECEIVED_PACKET, ptp->ports[port].rcvdPdelayRespPtr, rxSourcePortId);


      /* AS capable if the delay is low enough, the pdelay response is not from us, and we have a valid ratio */
      if ((ptp->ports[port].neighborPropDelay <= ptp->ports[port].neighborPropDelayThresh) &&
          (compare_clock_identity(rxSourcePortId, ptp->properties.grandmasterIdentity) != 0) &&
          ptp->ports[port].neighborRateRatioValid)
      {
        ptp->ports[port].asCapable = TRUE;
      }
      else
      {
        if (ptp->ports[port].asCapable && (ptp->ports[port].neighborPropDelay > ptp->ports[port].neighborPropDelayThresh)) {
          /* Log something here */
        }
        ptp->ports[port].asCapable = FALSE;
      }
      break;
  } 
}


int compare_port_ids(const vars_uchar8 *portIdA, const vars_uchar8 *portIdB) {
  vars_ulong32 byteIndex;
  int comparisonResult = 0;

  for(byteIndex = 0; byteIndex < PORT_ID_BYTES; byteIndex++) {
    if(portIdA[byteIndex] < portIdB[byteIndex]) {
      comparisonResult = -1;
      break;
    } else if(portIdA[byteIndex] > portIdB[byteIndex]) {
      comparisonResult = 1;
      break;
    }
  }
  return(comparisonResult);
}


/* 802.1AS MDPdelayReq state machine (11.2.15.3) transitions */
void MDPdelayReq_StateMachine(struct gptpm *ptp, vars_uchar8  port)
{
  ComMessageHdr *pMesgHdr = NULL;


  vtss_printf("MDP Delay Req_StateMachine for Port %d\r\n",(int)port);


  if(ptp->properties.delayMechanism != PTP_DELAY_MECHANISM_P2P) {
    /* The PDELAY state machine should only be active in P2P mode */
    return;
  }


  if (!ptp->ports[port].portEnabled || !ptp->ports[port].pttPortEnabled)
  {
    if (ptp->ports[port].mdPdelayReq_State != MDPdelayReq_NOT_ENABLED)
    {
      /* Disabling the port immediately forces the state machine into the disabled state */
	  print_str("MDPdelayReq_StateMachine Putting Port into Delay Request not enabled\r\n");
      MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_NOT_ENABLED);
    }
  }
  else
  {
    vars_uchar8 rxRequestingPortId[PORT_ID_BYTES];
    vars_uchar8 txRequestingPortId[PORT_ID_BYTES];
    vars_ulong32 rxSequenceId = 0;
    vars_ulong32 txSequenceId = 0;
    vars_uchar8 rxFUPRequestingPortId[PORT_ID_BYTES];
    vars_uchar8 txFUPRequestingPortId[PORT_ID_BYTES];
    vars_ulong32 rxFUPSequenceId = 0;
    vars_ulong32 txFUPSequenceId = 0;
    MDPdelayReq_State_t prevState;
    

    memset(rxRequestingPortId, 0, PORT_ID_BYTES);
    memset(txRequestingPortId, 0, PORT_ID_BYTES);
    memset(rxFUPRequestingPortId, 0, PORT_ID_BYTES);
    memset(txFUPRequestingPortId, 0, PORT_ID_BYTES);

    /* Grab some information needed for comparisons if we got a PDelay Response */
    if (ptp->ports[port].rcvdPdelayResp)
    {


      
      get_receiving_port_id(ptp, port, ptp->ports[port].rcvdPdelayRespPtr, rxRequestingPortId);      
      get_source_port_id_from_port(ptp, port,txRequestingPortId);
      pMesgHdr = (ComMessageHdr*)(ptp->ports[port].rcvdPdelayRespPtr);
      rxSequenceId = VTSS_COMMON_UNALIGNED_GET_4B(pMesgHdr->sequenceId);
      txSequenceId = ptp->ports[port].pdelayReqSequenceId;
    }
    if (ptp->ports[port].rcvdPdelayRespFollowUp)
    {
      get_receiving_port_id(ptp, port, ptp->ports[port].rcvdPdelayRespFollowUpPtr, rxFUPRequestingPortId);      
      get_source_port_id_from_port(ptp, port,txFUPRequestingPortId);
      pMesgHdr = (ComMessageHdr*)(ptp->ports[port].rcvdPdelayRespFollowUpPtr);
      rxFUPSequenceId = VTSS_COMMON_UNALIGNED_GET_4B(pMesgHdr->sequenceId);
      txFUPSequenceId = ptp->ports[port].pdelayReqSequenceId;
    }

    do
    {
      prevState = ptp->ports[port].mdPdelayReq_State;

	  vtss_printf("MDPdelayReq_StateMachine Previous state %d on port %d\r\n",(int)prevState,(int)port);

      switch (ptp->ports[port].mdPdelayReq_State)
      {
        default:
        case MDPdelayReq_NOT_ENABLED:
          if (ptp->ports[port].portEnabled && ptp->ports[port].pttPortEnabled)
          {
      
            VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Port index %d enabled\n", port));


            /* Port (and timesync on it) became enabled */
			print_str("MDPdelayReq StateMachine Sending Initial Delay Request\r\n");
			vtss_printf("Value of MDPdelayReq_INITIAL_SEND_PDELAY_REQ is %d\r\n",(int)MDPdelayReq_INITIAL_SEND_PDELAY_REQ);
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_INITIAL_SEND_PDELAY_REQ);
          }
          else
          {
            /* Don't time when this port is not enabled */
            ptp->ports[port].pdelayIntervalTimer = 0;
          }
          break;

        case MDPdelayReq_RESET:

          VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Resetting port index %d\n", port));
          MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_SEND_PDELAY_REQ);
          break;

        case MDPdelayReq_INITIAL_SEND_PDELAY_REQ:
        case MDPdelayReq_SEND_PDELAY_REQ:
          if (ptp->ports[port].rcvdMDTimestampReceive)
          {

            VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("PDelay Request Tx Timestamp available (port index %d).\n", port));

            /* The transmit timestamp for the request is available */
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_WAITING_FOR_PDELAY_RESP);
          }
          else if (ptp->ports[port].pdelayIntervalTimer >= PDELAY_REQ_INTERVAL_TICKS(ptp, port))
          {
            /* We didn't see a timestamp for some reason (this can happen on startup sometimes) */
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_RESET);
          }
          break;

        case MDPdelayReq_WAITING_FOR_PDELAY_RESP:
          if ((ptp->ports[port].pdelayIntervalTimer >= PDELAY_REQ_INTERVAL_TICKS(ptp, port)) ||
              (ptp->ports[port].rcvdPdelayResp &&
               ((compare_port_ids(rxRequestingPortId, txRequestingPortId) != 0) ||
                (rxSequenceId != txSequenceId))))
          {
            vars_uchar8 i;
            VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("Resetting %d: intervalTimer %d, reqInterval %d, rcvdPdelayResp %d, rcvdPdelayRespPtr %d, rxSequence %d, txSequence %d\n",
              port, ptp->ports[port].pdelayIntervalTimer, PDELAY_REQ_INTERVAL_TICKS(ptp, port), ptp->ports[port].rcvdPdelayResp,
              ptp->ports[port].rcvdPdelayRespPtr, rxSequenceId, txSequenceId));
            VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("rxRequestingPortID:"));
            for (i=0; i<PORT_ID_BYTES; i++) VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("%02X", rxRequestingPortId[i]));
            VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("\n"));
            VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("txRequestingPortID:"));
            for (i=0; i<PORT_ID_BYTES; i++) VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("%02X", txRequestingPortId[i]));
            VTSS_GPTP_TRACE(VTSS_GPTP_TRLVL_DEBUG,("\n"));
 
            /* Timeout or a non-matching response was received */
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_RESET);
          }
          else if (ptp->ports[port].rcvdPdelayResp &&
                   (rxSequenceId == txSequenceId) &&
                   (compare_port_ids(rxRequestingPortId, txRequestingPortId) == 0))
          {
            /* A matching response was received */
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_WAITING_FOR_PDELAY_RESP_FOLLOW_UP);
          }
          break;

        case MDPdelayReq_WAITING_FOR_PDELAY_RESP_FOLLOW_UP:
          if ((ptp->ports[port].pdelayIntervalTimer >= PDELAY_REQ_INTERVAL_TICKS(ptp, port)) ||
              (ptp->ports[port].rcvdPdelayResp &&
               (rxSequenceId == txSequenceId)))
          {
            /* Timeout or another response was received while waiting for the follow-up */
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_RESET);
          }
          else if (ptp->ports[port].rcvdPdelayRespFollowUp &&
                   (rxFUPSequenceId == txFUPSequenceId) &&
                   (compare_port_ids(rxFUPRequestingPortId, txFUPRequestingPortId) == 0))
          {
            /* Matching follow-up received */
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_WAITING_FOR_PDELAY_INTERVAL_TIMER);
          }
          break;

        case MDPdelayReq_WAITING_FOR_PDELAY_INTERVAL_TIMER:
          if (ptp->ports[port].pdelayIntervalTimer >= PDELAY_REQ_INTERVAL_TICKS(ptp, port))
          {
            /* Request interval timer expired */
            MDPdelayReq_StateMachine_SetState(ptp, port, MDPdelayReq_SEND_PDELAY_REQ);
          }
          break;
      }

    } while (prevState != ptp->ports[port].mdPdelayReq_State);
  }
}

/* 802.1AS LinkDelaySyncIntervalSetting state machine (11.2.17.2) entry actions */
static void LinkDelaySyncIntervalSetting_StateMachine_SetState(struct gptpm *ptp, vars_uchar8 port, unsigned char newState)
{
  
  ComMessageHdr *pComMesgHdr = (ComMessageHdr *)ptp->ports[port].rcvdSignalingPtr ;
  SignalingMessage_t *SignalMesg = (AnnounceMessage *) ((vars_uchar8 *) pComMesgHdr+sizeof(ComMessageHdr));
  
  char *pVariables = ((vars_uchar8 *) pComMesgHdr+sizeof(ComMessageHdr)+sizeof(SignalingMessage_t)); 


  ptp->ports[port].linkDelaySyncIntervalSetting_State = newState;

  switch (newState)
  {
    default:
    case LinkDelaySyncIntervalSetting_NOT_ENABLED:
      break;

    case LinkDelaySyncIntervalSetting_INITIALIZE:
      ptp->ports[port].currentLogPdelayReqInterval = ptp->ports[port].initialLogPdelayReqInterval;
      ptp->ports[port].currentLogSyncInterval = ptp->ports[port].initialLogSyncInterval;
      ptp->ports[port].computeNeighborRateRatio = TRUE;
      ptp->ports[port].computeNeighborPropDelay = TRUE;
      ptp->ports[port].rcvdSignalingMsg1 = FALSE;
      break;

    case LinkDelaySyncIntervalSetting_SET_INTERVALS:
    {
      
      vars_char8 linkDelayInterval = pVariables[0];
      vars_char8 timeSyncInterval = pVariables[1];
      vars_char8 announceInterval = pVariables[2];
      vars_char8 flags = pVariables[3];
      
      switch (linkDelayInterval)
      {
        case (-128): /* don't change the interval */
          break;
        case 126: /* set interval to initial value */
          ptp->ports[port].currentLogPdelayReqInterval = ptp->ports[port].initialLogPdelayReqInterval;
        default: /* use the indicated value */
          ptp->ports[port].currentLogPdelayReqInterval = linkDelayInterval;
          break;
      }

      switch (timeSyncInterval)
      {
        case (-128): /* don't change the interval */
          break;
        case 126: /* set interval to initial value */
          ptp->ports[port].currentLogSyncInterval = ptp->ports[port].initialLogSyncInterval;
          break;
        default: /* use indicated value */
          ptp->ports[port].currentLogSyncInterval = timeSyncInterval;
          break;
      }

      ptp->ports[port].computeNeighborRateRatio = flags & (1<<0);
      ptp->ports[port].computeNeighborPropDelay = flags & (1<<1);

      ptp->ports[port].rcvdSignalingMsg1 = FALSE;
      break;
    }
  }
}

/* 802.1AS LinkDelaySyncIntervalSetting state machine (11.2.17.2) transitions */
void LinkDelaySyncIntervalSetting_StateMachine(struct gptpm *ptp, vars_uchar8 port)
{
  if (!ptp->ports[port].portEnabled || !ptp->ports[port].pttPortEnabled)
  {
    if (ptp->ports[port].linkDelaySyncIntervalSetting_State != LinkDelaySyncIntervalSetting_NOT_ENABLED)
    {
      /* Disabling the port immediately forces the state machine into the disabled state */
      LinkDelaySyncIntervalSetting_StateMachine_SetState(ptp, port, LinkDelaySyncIntervalSetting_NOT_ENABLED);
    }
  }
  else
  {
    LinkDelaySyncIntervalSetting_State_t prevState;
    do
    {
      prevState = ptp->ports[port].linkDelaySyncIntervalSetting_State;

      switch (ptp->ports[port].linkDelaySyncIntervalSetting_State)
      {
        default:
        case LinkDelaySyncIntervalSetting_NOT_ENABLED:
          if (ptp->ports[port].portEnabled && ptp->ports[port].pttPortEnabled)
          {
            /* Port (and timesync on it) became enabled */
            LinkDelaySyncIntervalSetting_StateMachine_SetState(ptp, port, LinkDelaySyncIntervalSetting_INITIALIZE);
          }
          break;

        case LinkDelaySyncIntervalSetting_INITIALIZE:
        case LinkDelaySyncIntervalSetting_SET_INTERVALS:
          if (ptp->ports[port].rcvdSignalingMsg1)
          {
            LinkDelaySyncIntervalSetting_StateMachine_SetState(ptp, port, LinkDelaySyncIntervalSetting_SET_INTERVALS);
          }
          break;
      }

    } while (prevState != ptp->ports[port].linkDelaySyncIntervalSetting_State);
  }
}





#endif /* !VTSS_GPTP_NOT_WANTED */
