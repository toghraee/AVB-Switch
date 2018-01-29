/************************************************************************ 
 * MSRP library - 
 * Author: Reza Toghraee
 * 
 * This file is part of MSRP library. 
 * 
 * MSRP library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; version 2.1 
 * 
 * MSRP library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with MSRP library; see the file COPYING.  If not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA. 
 **********************************************************************/

/* MSRP PDU formats: */
 
#ifndef _MSRP_PDU_H__
#define _MSRP_PDU_H__			   



#define MAX_MRP_MSG_SIZE (sizeof(mrp_msg_header) + sizeof(srp_talker_first_value) + 1 /* for event vector */ + sizeof(mrp_msg_footer))

// The size of the send buffer - currently a full ethernet frame
#define MRP_SEND_BUFFER_SIZE (1518)

//! Lengths of the first values for each attribute type
//static int first_value_lengths[MRP_NUM_ATTRIBUTE_TYPES] = FIRST_VALUE_LENGTHS;


//#define SRP_VERSION_5 1


/* Default Bandwidth Parameters */
#define AVB_SRP_MAX_INTERVAL_FRAMES_DEFAULT 1
#define AVB_SRP_TSPEC_RANK_DEFAULT 1
#define AVB_SRP_TSPEC_PRIORITY_DEFAULT 3
#define AVB_SRP_TSPEC_RESERVED_VALUE 0

/* Used in the domain advertisements */
#define AVB_SRP_SRCLASS_DEFAULT 6


/* Initial guess at 150us Accumulated Latency Delay */
#define AVB_SRP_ACCUMULATED_LATENCY_DEFAULT (150000U)

/* SRP Ethernet Types and Mac addresses */

#define AVB_SRP_ETHERTYPE (0x22ea) 

#define AVB_SRP_MACADDR { 0x01, 0x80, 0xc2, 0x00, 0x00, 0xe }
#define AVB_SRP_LEGACY_MACADDR { 0x01, 0x00, 0x5e, 0x0, 1, 129 }

#define MAX_AVB_SRP_PDU_SIZE (64)

static unsigned char srp_proper_dest_mac[6] = AVB_SRP_MACADDR;
static unsigned char srp_legacy_dest_mac[6] = AVB_SRP_LEGACY_MACADDR;



typedef  struct mrp_eth_hdr_struct {
  vtss_common_octet_t dest_addr[6];
  vtss_common_octet_t src_addr[6];
  vtss_common_octet_t ethertype[2];
} mrp_ethernet_hdr;


typedef  struct mrp_header_struct {
  vtss_common_octet_t ProtocolVersion;
} mrp_header;

typedef  struct mrp_msg_header_struct {
  vtss_common_octet_t AttributeType;
  vtss_common_octet_t AttributeLength;
  vtss_common_octet_t AttributeListLength[2];
} mrp_msg_header;

typedef  struct mrp_vector_header_struct {
  vtss_common_octet_t LeaveAllEventNumberOfValuesHigh;
  vtss_common_octet_t NumberOfValuesLow;
} mrp_vector_header;

typedef  struct mrp_footer_struct {
  vtss_common_octet_t EndMark[2];
} mrp_footer; 

typedef  struct mrp_msg_footer_struct {
  vtss_common_octet_t EndMark[2];
} mrp_msg_footer; 


typedef  struct srp_talker_first_value {
  vtss_common_octet_t StreamId[8];
  vtss_common_octet_t DestMacAddr[6];
#ifndef SRP_VERSION_5
  vtss_common_octet_t VlanID[2];
#endif
  vtss_common_octet_t TSpecMaxFrameSize[2];
  vtss_common_octet_t TSpecMaxIntervalFrames[2];
  vtss_common_octet_t TSpec;
  vtss_common_octet_t AccumulatedLatency[4];
} srp_talker_first_value;

typedef struct srp_talker_failed_first_value {
  vtss_common_octet_t StreamId[8];
  vtss_common_octet_t DestMacAddr[6];
#ifndef SRP_VERSION_5
  vtss_common_octet_t VlanID[2];
#endif
  vtss_common_octet_t TSpecMaxFrameSize[2];
  vtss_common_octet_t TSpecMaxIntervalFrames[2];
  vtss_common_octet_t TSpec;
  vtss_common_octet_t AccumulatedLatency[4];
  vtss_common_octet_t FailureBridgeId[8];
  vtss_common_octet_t FailureCode;
} srp_talker_failed_first_value;



typedef struct srp_listener_first_value {
  vtss_common_octet_t StreamId[8];
} srp_listener_first_value;



typedef struct srp_domain_first_value {
  vtss_common_octet_t SRclassID;
  vtss_common_octet_t SRclassPriority;
  vtss_common_octet_t SRclassVID[2];
} srp_domain_first_value;




#endif /* _MSRP_PDU_H__ */

