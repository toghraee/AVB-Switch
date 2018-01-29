/*

  Author: Reza Toghraee

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef _VTSS_GPTP_PRIVATE_H_
#define _VTSS_GPTP_PRIVATE_H_ 1

#include "gptpstatmch.h"

typedef unsigned char vtss_gptp_tcount_t; /* tick counter */

typedef struct { unsigned char time_data[8]; } n64_t;

typedef unsigned long vtss_gptp_time_interval_t;


#include "vtss_gptp_os.h"


/*#include "gptp_bpdu.h"*/
/*
#include "gptpvector.h"
#include "gptpstatmch.h"
#include "times.h"
*/




/* Number of fractional nanosecond bits for correction field */
#define CORRECTION_FRACTION_BITS  (16)

/* Enumeration for RTC lock state */
#define PTP_RTC_UNLOCKED (0)
#define PTP_RTC_LOCKED   (1)

/* Enumeration for RTC acquisition */
#define PTP_RTC_ACQUIRED  (0)
#define PTP_RTC_ACQUIRING (1)

/* Enumeration for flagging valid RTC offsets */
#define PTP_RTC_OFFSET_INVALID (0)
#define PTP_RTC_OFFSET_VALID   (1)


/* Threshold and purely-proportional coefficient to use when in phase
 * acquisition mode
 */
#define ACQUIRE_THRESHOLD (10000)
#define ACQUIRE_COEFF_P   ((int)0xE0000000)

/* RTC increment constants */

#define INCREMENT_ONE         ((int) 0x08000000)
#define INCREMENT_NEG_ONE     ((int) 0xF8000000)
#define INCREMENT_HALF        ((int) 0x04000000)
#define INCREMENT_NEG_HALF    ((int) 0xFC000000)
#define INCREMENT_QUARTER     ((int) 0x02000000)
#define INCREMENT_NEG_QUARTER ((int) 0xFE000000)
#define INCREMENT_EIGHTH      ((int) 0x01000000)
#define INCREMENT_NEG_EIGHTH  ((int) 0xFF000000)
#define INCREMENT_DELTA_MAX   (INCREMENT_EIGHTH)
#define INCREMENT_DELTA_MIN   (INCREMENT_NEG_EIGHTH)


/* Bits to shift to convert a coefficient product */

#define COEFF_PRODUCT_SHIFT  (28)




#define RTC_MANTISSA_MASK   (0x0000000F)
#define RTC_MANTISSA_SHIFT  (27)
#define RTC_FRACTION_MASK   (0x07FFFFFF)



/* Rate ratio limits that are considered to be reasonable */
#define RATE_RATIO_MAX ((unsigned long)0x80100000)
#define RATE_RATIO_MIN ((unsigned long)0x7FF00000)

/* Saturation range limit for the integrator */
#define INTEGRAL_MAX_ABS  (100000)





/* timeSource enumeration */
#define PTP_SOURCE_ATOMIC_CLOCK         (0x10)
#define PTP_SOURCE_GPS                  (0x20)
#define PTP_SOURCE_TERRESTRIAL_RADIO    (0x30)
#define PTP_SOURCE_PTP                  (0x40)
#define PTP_SOURCE_NTP                  (0x50)
#define PTP_SOURCE_HAND_SET             (0x60)
#define PTP_SOURCE_OTHER                (0x90)
#define PTP_SOURCE_INTERNAL_OSCILLATOR  (0xA0)

/* delayMechanism enumeration */
#define PTP_DELAY_MECHANISM_E2E       (0x01)
#define PTP_DELAY_MECHANISM_P2P       (0x02)
#define PTP_DELAY_MECHANISM_DISABLED  (0xFE)





/* Number of bytes in a PTP port ID */
#define PORT_ID_BYTES  (10)



#define USE_BITFIELDS 1

#define PTP_VERSION_NUMBER       (2)




#define PTP_8021AS_DEST_ADDR { 0x01, 0x80, 0xc2, 0x0, 0x0, 0xe }

#define PTP_8021AS_LEGACY_ADDR { 0x01, 0x00, 0x5e, 0x0, 1, 129 }

#define PTP_802AS_SYSTEM_MACADDR{0x01, 0x99, 0x99, 0x99, 0x99, 0x99}

#define PTP_USE_8021AS_MULTICAST

// constants base on 802.1as specification.
#ifdef PTP_USE_8021AS_MULTICAST 
#define PTP_DEFAULT_DEST_ADDR PTP_8021AS_DEST_ADDR
#else
#define PTP_DEFAULT_DEST_ADDR PTP_8021AS_LEGACY_ADDR
#endif



#define PTP_ETHERTYPE                 (0x88f7)



/* Enumerated type identifying PTP roles. Defined to match 802.1AS Table 14-5 */
typedef enum {
  PTP_MASTER   = 6,
  PTP_SLAVE    = 9,
  PTP_PASSIVE  = 7,
  PTP_DISABLED = 3
    
} PtpRole;




/* 802.1AS PortRoleSelection state machine states */
typedef enum { PortRoleSelection_INIT_BRIDGE, PortRoleSelection_ROLE_SELECTION
} PortRoleSelection_State_t;


/* 802.1AS MDPdelayReq state machine states */
typedef enum { MDPdelayReq_NOT_ENABLED, MDPdelayReq_INITIAL_SEND_PDELAY_REQ,
  MDPdelayReq_RESET, MDPdelayReq_SEND_PDELAY_REQ, MDPdelayReq_WAITING_FOR_PDELAY_RESP,
  MDPdelayReq_WAITING_FOR_PDELAY_RESP_FOLLOW_UP, MDPdelayReq_WAITING_FOR_PDELAY_INTERVAL_TIMER
} MDPdelayReq_State_t;

/* 802.1AS LinkDelaySyncIntervalSettings state machine states */
typedef enum { LinkDelaySyncIntervalSetting_NOT_ENABLED, LinkDelaySyncIntervalSetting_INITIALIZE,
  LinkDelaySyncIntervalSetting_SET_INTERVALS
} LinkDelaySyncIntervalSetting_State_t;


/* 802.1AS PortAnnounceInformation state machine states */
typedef enum { PortAnnounceInformation_BEGIN,
  PortAnnounceInformation_DISABLED, PortAnnounceInformation_AGED,
  PortAnnounceInformation_UPDATE, PortAnnounceInformation_SUPERIOR_MASTER_PORT,
  PortAnnounceInformation_REPEATED_MASTER_PORT, PortAnnounceInformation_INFERIOR_MASTER_OR_OTHER_PORT,
  PortAnnounceInformation_CURRENT, PortAnnounceInformation_RECEIVE
} PortAnnounceInformation_State_t;



typedef enum {
  TRANSMITTED_PACKET,
  RECEIVED_PACKET
}PacketDirection;


/* Definitions and macros for manipulating port numbers */
#define PTP_PORT_NUMBER_BYTES sizeof(vars_ushort16)
typedef vars_uchar8 PtpPortNumber[PTP_PORT_NUMBER_BYTES];

typedef  struct vtss_ptp_port_identity {
  PtpClockIdentity clockIdentity;
  PtpPortNumber    portNumber;
} PtpPortIdentity; /* 8.5.2 */

/* Definitions and macros for manipulating offset scaled log variance */
#define PTP_OFFSET_VARIANCE_BYTES sizeof(vars_ushort16)
typedef vars_uchar8 PtpOffsetVariance[PTP_OFFSET_VARIANCE_BYTES];

typedef  struct vtss_ptp_system_identity {
  vars_uchar8       priority1;
  vars_uchar8       clockClass;
  vars_uchar8       clockAccuracy;
  PtpOffsetVariance offsetScaledLogVariance;
  vars_uchar8           priority2;
  PtpClockIdentity  clockIdentity;
} PtpSystemIdentity; /* 10.3.2 */

/* Definitions and macros for tracking steps removed */
#define PTP_STEPS_REMOVED_BYTES sizeof(vars_ushort16)
typedef vars_uchar8 PtpStepsRemoved[PTP_STEPS_REMOVED_BYTES];

/* NOTE: All fields in the priority vector should be BIG ENDIAN for proper
   byte-wise comparison of the UInteger224 */
typedef  struct PtpPriorityVector {
  PtpSystemIdentity rootSystemIdentity;
  PtpStepsRemoved   stepsRemoved;
  PtpPortIdentity   sourcePortIdentity; /* Port identity of the transmitting port */
  PtpPortNumber     portNumber;         /* Port number of the receiving port */
} PtpPriorityVector; /* 10.3.4 */


typedef enum {
  SuperiorMasterInfo,
  RepeatedMasterInfo,
  InferiorMasterInfo,
  OtherInfo
} AnnounceReceiveInfo;



/* Type used to represent RTC increments as well as permissible bounds to the
 * nominal increment.  This limits the RTC clock range to [100, 250] MHz, which
 * is a reasonable limitation for good performance without excessive precision.
 */
#define PTP_RTC_MANTISSA_BITS   (4)
#define PTP_RTC_INC_MIN ( 4)
#define PTP_RTC_INC_MAX (10)
#define PTP_RTC_FRACTION_BITS  (27)




/* Number of bits in the timer prescaler and divider, respectively */
#define PTP_TIMER_PRESCALER_BITS  (12)
#define PTP_TIMER_DIVIDER_BITS    (10)


typedef struct {
  int P;
  int I;
  int D;
} PtpCoefficients;


/* Platform data structure for configuring an instance with board-specific
 * software parameters.  The RTC coefficients are signed, fully-fractional values;
 * that is, a full-scale -1.0 value is 0x80000000.
 * Proportional coefficients must be negative to converge.
 */
typedef VTSS_COMMON_DATA_ATTRIB struct PtpPlatformData {
  /* Number of PTP ports attached to this instance */
  vars_ulong32 numPorts;


  /* Parameters for the RTC servo */
  RtcIncrement    nominalIncrement;
  PtpCoefficients coefficients;
  
  /* Parameters for the MAC/PHY delay */
  vtss_ptp_time_t  rxPhyMacDelay;
  vtss_ptp_time_t  txPhyMacDelay;

} PtpPlatformData;



typedef enum { InfoIs_Disabled, InfoIs_Received, InfoIs_Aged, InfoIs_Mine
} vtss_ptp_infois_enum;


typedef  struct {
  vars_ulong32 index; /* Port index to read */

  vars_ulong32 rxSyncCount;                             /* 14.7.2 */
  vars_ulong32 rxFollowupCount;                         /* 14.7.3 */
  vars_ulong32 rxPDelayRequestCount;                    /* 14.7.4 */
  vars_ulong32 rxPDelayResponseCount;                   /* 14.7.5 */
  vars_ulong32 rxPDelayResponseFollowupCount;           /* 14.7.6 */
  vars_ulong32 rxAnnounceCount;                         /* 14.7.7 */
  vars_ulong32 rxPTPPacketDiscardCount;                 /* 14.7.8 */
  vars_ulong32 syncReceiptTimeoutCount;                 /* 14.7.9 */
  vars_ulong32 announceReceiptTimeoutCount;             /* 14.7.10 */
  vars_ulong32 pDelayAllowedLostResponsesExceededCount; /* 14.7.11 */
  vars_ulong32 txSyncCount;                             /* 14.7.12 */
  vars_ulong32 txFollowupCount;                         /* 14.7.13 */
  vars_ulong32 txPDelayRequestCount;                    /* 14.7.14 */
  vars_ulong32 txPDelayResponseCount;                   /* 14.7.15 */
  vars_ulong32 txPDelayResponseFollowupCount;           /* 14.7.16 */
  vars_ulong32 txAnnounceCount;                         /* 14.7.17 */

} PtpAsPortStatistics;


/* Variables associated with each Port (from IEEE 802.1 AS standard) */
typedef VTSS_COMMON_DATA_ATTRIB struct vtss_gptp_port_vars {   

    vtss_common_port_t          port_number;
    vtss_gptp_port_config_t     port_config;
    vtss_gptp_port_properties_t port_properties;

   /* Timing parameters; these consist of raw timestamps for the slave as
       * well as the derived and filtered delay measurements.
       */
       
   /* End to End Delay Mechanism timing parameters */
    vtss_ptp_time_t syncRxTimestampTemp;
    vtss_ptp_time_t syncRxTimestamp;
    vtss_ptp_time_t syncTxTimestamp;
    vars_ulong32 syncTimestampsValid;
    vtss_ptp_time_t delayReqTxTimestampTemp;
    vtss_ptp_time_t delayReqTxLocalTimestampTemp;
    vtss_ptp_time_t delayReqTxTimestamp;
    vtss_ptp_time_t delayReqTxLocalTimestamp;
    vtss_ptp_time_t delayReqRxTimestamp;
    vars_ulong32 delayReqTimestampsValid;



   /* Configured delay for the PHY/MAC to where timestamping actually happens
       * Here we can have a predetermined delay factor.
      */
    vtss_ptp_time_t rxPhyMacDelay;
    vtss_ptp_time_t txPhyMacDelay;



    /* 802.1AS per-port variables (10.2.3 - 10.2.4) - Time sync state machines */
    PtpRole  selectedRole; /* 10.2.3.20 */
    vars_ulong32 asCapable;
    unsigned long   currentLogSyncInterval;
    unsigned long   initialLogSyncInterval;
    vars_ulong32 neighborRateRatio;
    vars_ulong32 neighborPropDelay;
    vars_ulong32 computeNeighborRateRatio;
    vars_ulong32 computeNeighborPropDelay;
    vars_ulong32 portEnabled;
    vars_ulong32 pttPortEnabled;



    /* 802.1AS per-port variables (10.3.8 - 10.3.9) - BMCA/Announce state machines */
    vars_char8            reselect;                   /* 10.3.8.1 (bool) */
    vars_char8            selected;                   /* 10.3.8.2 (bool) */
    vtss_ptp_infois_enum  infoIs;                     /* 10.3.9.2 */
    PtpPriorityVector     masterPriority;             /* 10.3.9.3 */
    vars_char8            currentLogAnnounceInterval; /* 10.3.9.4 */
    vars_char8            initialLogAnnounceInterval; /* 10.3.9.5 */
    vars_char8            newInfo;                    /* 10.3.9.8 (bool) */
    PtpPriorityVector     portPriority;               /* 10.3.9.9 */
    PtpPriorityVector     gmPathPriority;
    vars_ushort16         portStepsRemoved;           /* 10.3.9.10 */
    vars_uchar8          *rcvdAnnouncePtr;            /* 10.3.9.11 */
    vars_char8            rcvdMsg;                    /* 10.3.9.12 (bool) */
    vars_char8            updtInfo;                   /* 10.3.9.13 (bool) */


    /* per Port state machines */


    /* 802.1AS PortAnnounceInformation state machine variables */
    PortAnnounceInformation_State_t portAnnounceInformation_State;
    vars_ulong32        announceTimeoutCounter;/* 10.3.11.1.1 equivalent (Ticks since last received announce) */
    PtpPriorityVector   messagePriority;/* 10.3.11.1.2 */
    AnnounceReceiveInfo rcvdInfo; /* 10.3.11.1.3 */



      
    /* 802.1AS timeouts (10.6.3) */
    vars_char8 syncReceiptTimeout;
    vars_char8 announceReceiptTimeout;
  
    /* 802.1AS MD entity variables (11.2.12) */
    vars_char8 currentLogPdelayReqInterval;
    vars_char8 initialLogPdelayReqInterval;
    vars_ulong32 allowedLostResponses;
    vars_ulong32 isMeasuringDelay;
    vars_ulong32 neighborPropDelayThresh;



   /* 802.1AS Peer-to-peer delay mechanism variables (11.2.15.1) */
    MDPdelayReq_State_t mdPdelayReq_State;

    vars_ulong32 pdelayIntervalTimer;
    vars_ulong32 rcvdPdelayResp;
    vars_uchar8 *rcvdPdelayRespPtr;
    vars_ulong32 rcvdPdelayRespFollowUp;
    vars_uchar8 *rcvdPdelayRespFollowUpPtr;
    vars_ulong32 rcvdMDTimestampReceive;
    vars_ushort16 pdelayReqSequenceId;
    vars_ulong32 initPdelayRespReceived;
    vars_ulong32 lostResponses;
    vars_ulong32 neighborRateRatioValid;

    /* AVnu_PTP-5 PICS */
    vars_ulong32 pdelayResponses;
    vars_ulong32 multiplePdelayResponses;

    /* 802.1AS LinkDelaySyncIntervalSetting variables (11.2.17.1) */
    LinkDelaySyncIntervalSetting_State_t linkDelaySyncIntervalSetting_State;
    vars_ulong32 rcvdSignalingMsg1;
    vars_uchar8 *rcvdSignalingPtr;

    /* 802.1AS Follow_Up information TLV variables (11.4.4.3)*/
    vars_ulong32 cumulativeScaledRateOffset;

    /* Current PDelay Request/Response timestamps */
    vtss_ptp_time_t pdelayReqTxTimestamp;  // pdelayReqEventEgressTimestamp (Treq1)
    vtss_ptp_time_t pdelayReqRxTimestamp;  // pdelayReqEventIngressTimestamp (Trsp2)
    vtss_ptp_time_t pdelayRespTxTimestamp; // pdelayRespEventEgressTimestamp (Trsp3)
    vtss_ptp_time_t pdelayRespRxTimestamp; // pdelayRespEventIngressTimestamp (Treq4)

    /* First PDelay Response timestamps (after the last enable/reset) */
    vtss_ptp_time_t pdelayRespTxTimestampI; // pdelayRespEventEgressTimestamp (Trsp3)
    vtss_ptp_time_t pdelayRespRxTimestampI; // pdelayRespEventIngressTimestamp (Treq4)

    /* pdelay response variables */
    vars_uchar8 lastPeerRequestPortId[PORT_ID_BYTES];

    /* Timer state space */
    vars_ulong32 announceCounter;
    vars_ushort16 announceSequenceId;
    vars_ulong32 syncTimeoutCounter;
    vars_ulong32 syncCounter;
    vars_ushort16 syncSequenceId;
    vars_ulong32 delayReqCounter;
    vars_ulong32 delayReqSequenceId;

    /* Packet Rx state space */
    vars_ulong32 lastRxBuffer;
    vars_ulong32 syncSequenceIdValid;


    /* Announce Information */

    vars_ushort16     ann_current_utc_offset;     
    vars_uchar8       annleap_59;                 
    vars_uchar8       annleap_61;                  
    vars_uchar8       anncurrent_utc_offset_valid;
    vars_uchar8       anntime_traceable;
    vars_uchar8       annfrequency_traceable;
  
    vars_ushort16     anntime_source; 

    /* Packet statistics */
    PtpAsPortStatistics stats;

    /*Waiting for FOLLOWUP Message */

    vars_uchar8      waiting_for_fup;
    

    struct gptpm                *owner; /* Bridge, that this port belongs to */

  
} vtss_gptp_port_vars_t;



/* Structure definition to hold the RTC Clock and Servo Variables */

typedef struct vtss_rtc_clock_vars{
    _UINT64 rtc_adj;
    _UINT64 rtc_nominal;
    _UINT64 rtc_fraction;


}vtss_rtc_clock_vars_t;

#define GPTP_MAX_PATH_TRACE 50

typedef  struct gptpm {

    vtss_gptp_port_vars_t       ports[VTSS_GPTP_MAX_PORTS]; /* List of ports in this VLAN/Bridge */


    vars_uchar8 numPorts; /* (8.6.2.8) */


    vtss_common_bool_t          initialized;

    vtss_common_bool_t          admin_enabled;
    vtss_gptp_system_config_t   system_config;



    /* 802.1AS Time aware system attributes (8.6.2) */
    vtss_gptp_system_properties_t properties;

    vtss_gptp_system_properties_t new_master;

    /* 802.1AS Time aware system global variables (10.2.3) */
    vars_uchar8 gmPresent; /* 10.2.3.13 (bool) */

    /* 802.1AS Time aware system global variables (10.3.8) */
    vars_ushort16    masterStepsRemoved; /* 10.3.8.3 */
    PtpPriorityVector  systemPriority; /* Priority vector for this system (10.3.8.18) */
    PtpPriorityVector *gmPriority;  /* Priority vector for the current grandmaster (10.3.8.19) */
    PtpPriorityVector  lastGmPriority; /* Previous grandmaster priority vector (10.3.8.20) */
    vars_ulong32 pathTraceLength;/* Number of paths listed in the pathTrace array */
    PtpClockIdentity   pathTrace[GPTP_MAX_PATH_TRACE]; /* 10.3.8.21 */

    /* The only "per bridge" state machine */

    /* Present role and delay mechanism for the endpoint */
    PortRoleSelection_State_t       portRoleSelection_State;
    vars_ulong32 timerTicks;
     
    
    /* New master event needs to be transmitted */
    vars_ulong32 newMaster;


    /* System Wide Time Variables */
    vars_ushort16     current_utc_offset;     
    vars_uchar8       leap_59;                 
    vars_uchar8       leap_61;                  
    vars_uchar8       current_utc_offset_valid;
    vars_uchar8       time_traceable;
    vars_uchar8       frequency_traceable;
  
    vars_ushort16     time_source; 


    
    /* RTC control loop constants */
     RtcIncrement    nominalIncrement;
     PtpCoefficients coefficients;
     unsigned long masterRateRatio;
     unsigned long masterRateRatioValid;
    
     /* RTC control loop persistent values */
     _UINT64  integral;
     int  derivative;
     int  previousOffset;
     unsigned long rtcChangesAllowed;
     int  rtcLastOffset;
     unsigned long rtcLastOffsetValid;
     unsigned long rtcLastLockState;
     unsigned long acquiring;
     unsigned long rtcLockState;
     unsigned long rtcLockCounter;
     unsigned long rtcLockTicks;
     unsigned long rtcUnlockTicks;
     RtcIncrement currentIncrement;


     vtss_rtc_clock_vars_t rtc_clock_vars;

     /*Clock Master and Non-Master variables for sync */

     vtss_ptp_time_t upSTreamTxTime;
     vtss_ptp_time_t rxPreciseOriginTimeStamp;/* From SyncFollowup*/
     vtss_ptp_time_t rxCorrectionField;/* From SyncFollowup Received and Updated. */
     vars_ulong32 rateRatio; /* Received and computed Rate Ratio */
     PtpPortIdentity rxSourcePortIdentity; /* Sync Source port identity */

     /*Grandmaster */

     vars_uchar8 grand_master;

     
     
     
     

   
} gptpm_t;



extern gptpm_t vtss_gptp_vars; /* The *only* global variable */
#define GPTP    (&vtss_gptp_vars) /* Defined as pointer to global area */





#endif /* _VTSS_GPTP_PRIVATE_H_ */
