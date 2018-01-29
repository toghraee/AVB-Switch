
/*
 * This file contains the API definitions used between the GPTP protocol
 * module and the operating environment.
 */
#ifndef _VTSS_GPTP_H_
#define _VTSS_GPTP_H_ 1


#include "vtss_gptp_os.h"


/* Global Definitions Begin */

/* Mac address space length */
#define MAC_ADDRESS_LENGTH  (6)

static unsigned char DEFAULT_SOURCE_MAC[MAC_ADDRESS_LENGTH] = {
  0x00, 0x0A, 0x35, 0x00, 0x22, 0xFF
};

#define PTP_CLOCK_IDENTITY_BYTES  (8)


#define RESET_THRESHOLD_NS  (100000)

/*
typedef unsigned char vars_uchar8;
typedef signed char vars_char8;
typedef unsigned short vars_ushort16;
typedef short vars_short16;
typedef long           vars_long32;
typedef unsigned long  vars_ulong32;*/



#define VTSS_GPTP_IS_AGGR(POAG) ((POAG) > VTSS_GPTP_MAX_PORTS)
#define VTSS_GPTP_IS_PORT(POAG) ((POAG) <= VTSS_GPTP_MAX_PORTS)
#define VTSS_GPTP_IS_POAG(POAG) ((POAG) > 0 && (POAG) <= VTSS_GPTP_MAX_PORTS + VTSS_GPTP_MAX_PORTS)


typedef unsigned short vtss_gptp_clock_vars_t; /* Time intervals in 1 sec */
#define VTSS_GPTP_TICKS_PER_SEC         (1000)

#define DELAY_REQ_INTERVAL        (1000)

#define VTSS_GPTP_CLOCK_TYPE           ((vtss_gptp_clock_vars_t)1)

#define VTSS_GPTP_DEFAULT_CLOCK_ACCURACY    ((vtss_gptp_clock_vars_t)254)
#define VTSS_GPTP_DEFAULT_CLOCK_CLASS   ((vtss_gptp_clock_vars_t)248)
#define VTSS_GPTP_DEFAULT_OFFSET_LOG_VAR ((vtss_gptp_clock_vars_t)1)
#define VTSS_GPTP_DEFAULT_CURRENT_UTC_OFFSET ((vtss_gptp_clock_vars_t)33)
#define VTSS_GPTP_DEFAULT_CURRENT_UTC_OFFSET_VALID ((vtss_gptp_clock_vars_t)1)
#define VTSS_GPTP_DEFAULT_LEAP61 ((vtss_gptp_clock_vars_t)0)
#define VTSS_GPTP_DEFAULT_LEAP59 ((vtss_gptp_clock_vars_t)1)
#define VTSS_GPTP_DEFAULT_TIME_TRACEABLE ((vtss_gptp_clock_vars_t)0)
#define VTSS_GPTP_DEFAULT_FREQ_TRACEABLE ((vtss_gptp_clock_vars_t)0)
#define VTSS_GPTP_TIME_SOURCE ((vtss_gptp_clock_vars_t)0xA0)

#define PTP_TIMESCALE (1)




typedef unsigned char vtss_gptp_prio_dom_en;

#define VTSS_GPTP_DEFAULT_PRIO1 ((vtss_gptp_prio_dom_en)1)
#define VTSS_GPTP_DEFAULT_PRIO2 ((vtss_gptp_prio_dom_en)1)
#define VTSS_GPTP_DEFAULT_DOMAIN_ID ((vtss_gptp_prio_dom_en)0)
#define VTSS_GPTP_DEFAULT_ENABLE ((vtss_gptp_prio_dom_en)1)
#define VTSS_GPTP_DEFAULT_GM_CAPABLE ((vtss_gptp_prio_dom_en)1)

/* Global Default Definitions are completed */

typedef struct tdINT64          /* size is 8 */
{
    int  HighPart;
    unsigned long LowPart;
}_INT64;

typedef struct tdUINT64         /* size is 8 */
{
    unsigned long HighPart;
    unsigned long LowPart;
}_UINT64;


#define Int64Initializer(_hi32_,_lo32_) { _hi32_, _lo32_ }


#define Int64Less(_a_,_b_) \
    (((_a_).HighPart < (_b_).HighPart) || \
     (((_a_).HighPart == (_b_).HighPart) && ((_a_).LowPart < (_b_).LowPart)))
#define Uint64Less(_a_,_b_) Int64Less((_a_), (_b_))




/* 64-bit, add and subtract routines */
#define carry(a,b) (((unsigned long)(a) + (unsigned long)(b)) < (a))
#define borrow(a,b) ((a) < (b))

#define MUX(a,b,c)     ((a) ? (b) : (c))
#define iabs(a) ((int)(a) < 0 ? -(int)(a) : (a))
#define izero(a,b) ((a) ? (b) : 0)



#define Uint32TimesUint32(_a_,_b_) ((_a_) * (_b_))


#define Int32ToInt64(_i64_,_i32_) \
    { \
      (_i64_).LowPart = (unsigned long) _i32_; \
      (_i64_).HighPart = ((_i32_) < 0)  ? -1 : 0; \
    }

#define Uint32ToUint64(_u64_,_u32_) \
    { \
      (_u64_).LowPart = _u32_; \
      (_u64_).HighPart = 0; \
    }

#define Int64ToInt32(_i64_) (int)((_i64_).LowPart)
#define Uint64ToUint32(_u64_) ((_u64_).LowPart)
#define Int64ToUint64(_x_) (*(_UINT64*)&(_x_))
#define Uint64ToInt64(_x_) (*(_INT64*)&(_x_))



#define Int64IsZero(_a_) \
    (((_a_).HighPart == 0) && ((_a_).LowPart == 0))





/* Start the port level defintions */

typedef unsigned char vtss_gptp_port_level;

#define VTSS_GPTP_PORT_ENABLED_DEFAULT ((vtss_gptp_port_level)1)

typedef signed char vtss_gptp_port_data_schar;

#define VTSS_GPTP_PORT_INIT_LOG_ANNOUNCE_INT_DEFAULT ((vtss_gptp_port_data_schar)0)
#define VTSS_GPTP_PORT_CURR_LOG_ANNOUNCE_INT_DEFAULT ((vtss_gptp_port_data_schar)0)
#define VTSS_GPTP_PORT_INIT_LOG_PDELAY_REQ_INT_DEFAULT ((vtss_gptp_port_data_schar)0)
#define VTSS_GPTP_PORT_CURR_PDELAY_REQ_INT_DEFAULT   ((vtss_gptp_port_data_schar)0)

typedef unsigned int vtss_gptp_port_config_data;

#define VTSS_GPTP_PORT_NBR_THRESHOLD_DELAY_DEFAULT ((vtss_gptp_port_config_data)1200)
#define VTSS_GPTP_PORT_DEFAULT_DELAY_ASSYMETRY     ((vtss_gptp_port_config_data)0)
#define VTSS_GPTP_PORT_ANNOUNCE_RX_TIMEOUT_DEFAULT ((vtss_gptp_port_level)3)
#define VTSS_GPTP_PORT_SYNC_RECEIPT_TIMEOUT_DEFAULT ((vtss_gptp_port_level)3)
#define VTSS_GPTP_PORT_ALLOWED_LOST_RESPONSES_DEFAULT    ((vtss_gptp_port_config_data)3)


/* PTP data types and constant definitions */
typedef struct vtss_gptp_clock_qual_vars {
  vars_uchar8  uc_clockClass;
  vars_uchar8  uc_clockAccuracy;
  vars_ushort16 us_offsetScaledLogVariance;
} vtss_gptp_clock_qual_vars_t;


typedef struct vtss_gptp_port_properties {
  /* Port number to get/set */
  vars_ushort16 us_portNumber;

  /* Source MAC address of the interface the PTP hardware uses */
  vtss_common_macaddr_t   port_macaddr;

  /* Steps removed from the master */
  vars_ushort16 us_stepsRemoved;

} vtss_gptp_port_properties_t;


/* Stores the Time in seconds , nanoseconds */
typedef struct vtss_ptp_time {
  vars_long32 secondsUpper;
  vars_ulong32 secondsLower;
  vars_long32 nanoseconds;
} vtss_ptp_time_t;

typedef vars_uchar8 PtpClockIdentity[PTP_CLOCK_IDENTITY_BYTES];





typedef  struct vtss_gptp_system_properties {
  /* Various PTP-defined properties */
  vars_uchar8          domainNumber;
  vars_short16          currentUtcOffset;
  /* Note that this is our local systemIdentity, but is kept as separate
     fields (instead of as a PtpSystemIdentity) for backwards compatability */
  vars_uchar8          grandmasterPriority1;    /* 8.6.2.1 */
  vtss_gptp_clock_qual_vars_t   grandmasterClockQuality; /* 8.6.2.2 - 8.6.2.4 */
  vars_uchar8          grandmasterPriority2;    /* 8.6.2.5 */
  PtpClockIdentity grandmasterIdentity;     /* 8.6.2.6 */
  vars_uchar8          timeSource;              /* 8.6.2.7 */
  vars_uchar8          delayMechanism;
  vars_ulong32         lockRangeNsec;
  vars_ulong32         lockTimeMsec;
  vars_ulong32         unlockThreshNsec;
  vars_ulong32         unlockTimeMsec;
  vars_ushort16        masterStepsRemoved;
  PtpClockIdentity     pLocalClockIdentity;
} vtss_gptp_system_properties_t;

typedef struct RtcIncrement {
  unsigned long mantissa;
  unsigned long fraction;
}RtcIncrement;



/* Finish Port level definitions*/



/* ------------------------------------------------------------------------------------------- */

/*
 * Functions provided by the vtss_gptp protocol module.
 */

/**
 * vtss_gptp_init - Initialize internal data, obtain HW MAC adresses and start the GPTP protocol.
 */
extern void vtss_gptp_init(void);


/* New Prototypes */
static void set_ptp_ethernet_hdr(unsigned char *buf,vars_uchar8 port_no);

void get_rtc_time(struct gptpm *ptp,vtss_ptp_time_t *time);

static void put_timestamp(struct gptpm *ptp, vars_ulong32 port, vars_uchar8 *rxBuffer, vtss_ptp_time_t *ts);

void ptpex_os_tx_frame (vars_uchar8  port_no, vars_uchar8 xdata * frame, unsigned short len);

void timestamp_copy(vtss_ptp_time_t *destination, vtss_ptp_time_t *source);

void timestamp_difference(vtss_ptp_time_t *minuend, vtss_ptp_time_t *subtrahend, vtss_ptp_time_t *difference);
void timestamp_sum(vtss_ptp_time_t *addend, vtss_ptp_time_t *augend, vtss_ptp_time_t *sum);

static void normalize_timestamp(vtss_ptp_time_t *operand);

void timestamp_abs(vtss_ptp_time_t *operand, vtss_ptp_time_t *result);

void get_32_into_timestamp(vars_uchar8 *value,vtss_ptp_time_t *timestamp);

void set_rtc_increment(struct gptpm *ptp, RtcIncrement *value);

static void gptp_sw_init(void);

extern void get_timestamp(struct gptpm *ptp, vars_ulong32 port,int bufferDirection, vars_uchar8 *rxBuffer,vtss_ptp_time_t *ts);

int compare_clock_identity(const vars_uchar8 *clockIdentityA, const vars_uchar8 *clockIdentityB);

void transmit_pdelay_request(struct gptpm *ptp, vars_uchar8 port);



_UINT64 Uint64AddUint32(_UINT64 x, unsigned long y);

_UINT64 Uint64TimesUint32(_UINT64 x, int y);

_UINT64 Uint64RShift(_UINT64 x, unsigned long n);


_UINT64 Uint64LShift(_UINT64 x, int n);

_UINT64 Uint64DividedByUint64(_UINT64 x, _UINT64 y);


_INT64 Int64Subtract(_INT64 x, _INT64 y);
























/**
 * vtss_gptp_deinit - Initialize internal data back to default. Await new vtss_gptp_init() call.
 */
extern void vtss_gptp_deinit(void);

/* Global parameters */
typedef struct {
    vtss_gptp_prio_dom_en prio1;
    vtss_gptp_prio_dom_en prio2;
    vtss_gptp_prio_dom_en gptp_enable;
    vtss_gptp_prio_dom_en gm_capable;
} vtss_gptp_system_config_t;

/**
 * vtss_gptp_set_config - Set global parameters.
 * Note: Can be called before vtss_gptp_init().
 */
extern void vtss_gptp_set_config(const vtss_gptp_system_config_t VTSS_COMMON_PTR_ATTRIB *system_config);

/**
 * vtss_gptp_get_config - Get global parameters.
 * Note: Can be called before vtss_gptp_init().
 */
extern void vtss_gptp_get_config(vtss_gptp_system_config_t VTSS_COMMON_PTR_ATTRIB *system_config);

/* Port parameters */
typedef struct {
    vtss_gptp_port_config_data nbrPropDelayThresh;
    vtss_gptp_port_level    initialLogAnnounceInterval;
    vtss_gptp_port_data_schar announceReceiptTimeout;
    vtss_gptp_port_data_schar initialLogSyncInterval;
    vtss_gptp_port_level  syncReceiptTimeOut;
    vtss_gptp_port_data_schar initialLogPdelayReqInterval;
    vtss_common_bool_t enable_gptp;
} vtss_gptp_port_config_t;

/**
 * vtss_gptp_set_portconfig - Set port-specific parameters.
 * Note: Can be called before vtss_gptp_init().
 */
extern void vtss_gptp_set_portconfig(vtss_common_port_t portno,
                                     const vtss_gptp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config);


/**
 * vtss_gptp_get_portconfig - Get port-specific parameters.
 * Note: Can be called before vtss_gptp_init().
 */
extern void vtss_gptp_get_portconfig(vtss_common_port_t portno,
                                     vtss_gptp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config);

/**
 * vtss_gptp_tick() - Timer tick event.
 * Must be called from OS once every second.
 */
extern void vtss_gptp_tick(void);

void vtss_gptp_port_timers_tick(void);


/**
 * vtss_gptp_more_work() - Called when idle.
 * Must be called to perform remaining tasks.
 */
extern void vtss_gptp_more_work(void);

/**
 * vtss_gptp_receive - Deliver a frame from the OS to the GPTP protocol.
 * Must be called from OS when a frame with destination
 * MAC address VTSS_GPTP_MULTICAST_MACADDR and ethertype
 * of VTSS_GPTP_ETHTYPE is received
 */
extern void vtss_gptp_receive(vtss_common_port_t from_port,
			      const vtss_common_octet_t VTSS_COMMON_BUFMEM_ATTRIB *frame,
			      vtss_common_framelen_t len);


/*
 * Functions to be provided by OS specifically for the vtss_gptp module.
 * There are other functions defined in vtss_common_os.h
 */

extern void vtss_gptp_get_new_master(vtss_gptp_system_properties_t *pMasterData);

extern void ptp_print_port_role(vars_ushort16 port_no);

#ifndef VTSS_GPTP_NDEBUG
extern void vtss_gptp_dump_port(vtss_common_port_t portno);
extern void vtss_gptp_dump(void);
#endif /* !VTSS_GPTP_NDEBUG */




// Bit mask for PTP Message Type
#define PTP_MESSAGE_TYPE_MASK             (0xF)

#define PTP_TRANSPORT_SPECIFIC_HDR     (0x1 << 4)


#define PTP_LOG_MIN_PDELAY_REQ_INTERVAL            (0)


// PTP Message type definations
#define PTP_SYNC_MESG                     (0x0)
#define PTP_DELAY_REQ_MESG                (0x1)
#define PTP_PDELAY_REQ_MESG               (0x2)
#define PTP_PDELAY_RESP_MESG              (0x3)
#define PTP_FOLLOW_UP_MESG                (0x8)
#define PTP_DELAY_RESP_MESG               (0x9)
#define PTP_PDELAY_RESP_FOLLOW_UP_MESG    (0xA)
#define PTP_ANNOUNCE_MESG                 (0xB)
#define PTP_SIGNALING_MESG                (0xC)
#define PTP_MANAGEMENT_MESG               (0xD)

#define PTP_ANNOUNCE_TLV_TYPE (0x8)


// PTP Control Field ENUM
#define PTP_CTL_FIELD_SYNC                (0x0)
#define PTP_CTL_FIELD_DELAY_REQ           (0x1)
#define PTP_CTL_FIELD_FOLLOW_UP           (0x2)
#define PTP_CTL_FIELD_DELAY_RESP          (0x3)
#define PTP_CTL_FIELD_MANAGEMENT          (0x4)
#define PTP_CTL_FIELD_OTHERS              (0x5)

// PTP common message header length
#define PTP_COMMON_MESG_HDR_LENGTH        (34)



/* Useful types for network packet processing */


/* Host data types - little endian */
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef long  u64_t;  


typedef struct { unsigned char dataa[10]; } u80_t;
typedef struct { unsigned int dataa[3]; } u96_t;

/* Network dataa types - big endian  */
typedef unsigned char n8_t;
typedef struct { unsigned char dataa[2]; } n16_t;
typedef struct { unsigned char dataa[4]; } n32_t;



typedef struct { unsigned char dataa[8]; } p64_t;
typedef struct { unsigned char dataa[10]; } n80_t;
typedef struct { unsigned char dataa[12]; } n96_t;

/*
n16_t hton16(u16_t x) {
  n16_t ret;
  ret.dataa[0] = x >> 8;
  ret.dataa[1] = (x & 0xff);
  return ret;
}

u16_t ntoh16(n16_t x) {
  return ((x.dataa[0] << 8) | x.dataa[1]);
}

u32_t ntoh32(n32_t x) {
  return ((x.dataa[0] << 24) | x.dataa[1] << 16 | x.dataa[2] << 8 | x.dataa[1]);
}




n32_t hton32(u32_t x) {
  n32_t ret;

  ret.dataa[0] = ((x >> 24) & 0xff);
  ret.dataa[1] = ((x >> 16) & 0xff);
  ret.dataa[2] = ((x >>  8) & 0xff);
  ret.dataa[3] = ((x >>  0) & 0xff);
  return ret;
}

n80_t hton80(u80_t x) {
  n80_t ret;
  int i=0;
  for (i=0;i<10;i++) 
    ret.dataa[i] = x.dataa[9-i];
  return ret;
}
*/

/* Ethernet headers */
typedef struct ethernet_hdr_t {
  unsigned char dest_addr[6];
  unsigned char src_addr[6];
  unsigned char ethertype[2];
} ethernet_hdr_t;

typedef struct tagged_ethernet_hdr_t {
  unsigned char dest_addr[6];
  unsigned char src_addr[6];
  unsigned char qtag[2];
  unsigned char ethertype[2];
} tagged_ethernet_hdr_t;


// PTP Common Message Header format
typedef  struct ComMessageHdr
{
  vtss_common_octet_t transportSpecific_messageType;
  vtss_common_octet_t versionPTP;
  vtss_common_octet_t messageLength[2];   
  vtss_common_octet_t domainNumber;
  vtss_common_octet_t Resv0;
  vtss_common_octet_t flagField[2];
  vtss_common_octet_t correctionField[8];
  vtss_common_octet_t Resv1[4];   
  vtss_common_octet_t sourcePortIdentity[10];
  vtss_common_octet_t sequenceId[2];
  vtss_common_octet_t controlField;
  vtss_common_octet_t logMessageInterval;
} ComMessageHdr;


// PTP Announce message
typedef  struct AnnounceMessage
{
  vtss_common_octet_t originTimestamp[10];
  vtss_common_octet_t currentUtcOffset[2];
  vtss_common_octet_t Resv1;
  vtss_common_octet_t grandmasterPriority1;
  vtss_common_octet_t clockClass;
  vtss_common_octet_t clockAccuracy;
  vtss_common_octet_t clockOffsetScaledLogVariance[2];
  vtss_common_octet_t grandmasterPriority2;
  vtss_common_octet_t grandmasterIdentity[8];
  vtss_common_octet_t stepsRemoved[2];
  vtss_common_octet_t timeSource;  
  vtss_common_octet_t tlvType[2];
  vtss_common_octet_t tlvLength[2];
  /*vtss_common_octet_t pathSequence[8];*/
} AnnounceMessage;

// PTP Sync & Delay_Req message
typedef  struct SyncMessage
{
  vtss_common_octet_t originTimestamp[10];
} SyncMessage;

// PTP Follow_Up message
typedef  struct FollowUpMessage
{
  vtss_common_octet_t preciseOriginTimestamp[10];
  vtss_common_octet_t tlvType[2];
  vtss_common_octet_t lengthField[2];
  vtss_common_octet_t  organizationId[3];
  vtss_common_octet_t  organizationSubType[3];
  vtss_common_octet_t cumulativeScaledRateOffset[4];
  vtss_common_octet_t gmTimeBaseIndicator[2];
  vtss_common_octet_t lastGmPhaseChange[12];
  vtss_common_octet_t scaledLastGmFreqChange[4];
} FollowUpMessage;

// PTP Peer delay request message
typedef  struct PdelayReqMessage
{
  vtss_common_octet_t originTimestamp[10];
  vtss_common_octet_t Resv0[10];
} PdelayReqMessage;

// PTP Peer delay response message
typedef  struct PdelayRespMessage
{
  vtss_common_octet_t requestReceiptTimestamp[10];
  vtss_common_octet_t requestingPortIdentity[10];
} PdelayRespMessage;

// PTP Peer delay response follow up message
typedef  struct PdelayRespFollowUpMessage
{
  vtss_common_octet_t responseOriginTimestamp[10];
  vtss_common_octet_t requestingPortIdentity[10];
} PdelayRespFollowUpMessage;


/* PTP Signaling Message */

typedef struct SignalingMessage
{
    vtss_common_octet_t targetPortIdentity[10];
    vtss_common_octet_t tlvType[2];
    vtss_common_octet_t tlvLength[2];
    vtss_common_octet_t  organizationId[3];
    vtss_common_octet_t  organizationSubType[3];   

}SignalingMessage_t;

// Macro to evaluate flagField(s) in PTP message
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



#endif /* _VTSS_GPTP_H_ */
