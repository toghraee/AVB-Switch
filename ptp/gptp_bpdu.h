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
 * along with GPTP library; see the file COPYING.  If not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA. 
 **********************************************************************/

/* GPTP formats*/
 
#ifndef _GPTP_BPDU_H__
#define _GPTP_BPDU_H__

#if 0

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

#endif

#endif /* _GPTP_BPDU_H__ */

