/*

Author: Reza Toghraee

 Copyright (c) 2002-2008 Vitesse Semiconductor Corporation "Vitesse". All
 Rights Reserved.
 
 Unpublished rights reserved under the copyright laws of the United States of
 America, other countries and international treaties. Permission to use, copy,
 store and modify, the software and its source code is granted. Permission to
 integrate into other products, disclose, transmit and distribute the software
 in an absolute machine readable format (e.g. HEX file) is also granted.  The
 source code of the software may not be disclosed, transmitted or distributed
 without the written permission of Vitesse. The software and its source code
 may only be used in products utilizing the Vitesse switch products.
 
 This copyright notice must appear in any copy, modification, disclosure,
 transmission or distribution of the software. Vitesse retains all ownership,
 copyright, trade secret and proprietary rights in the software.
 
 THIS SOFTWARE HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY
 INCLUDING, WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR USE AND NON-INFRINGEMENT.

*/

#ifndef _VTSS_MSRP_PRIVATE_H_
#define _VTSS_MSRP_PRIVATE_H_ 1

typedef unsigned char vtss_msrp_tcount_t; /* tick counter */



typedef unsigned long vtss_msrp_time_interval_t;




#include "vtss_msrp_os.h"
#include "msrp_pdu.h"
#include "vtss_msrp.h"

/*
#include "gptp_bpdu.h"
#include "gptpvector.h"
#include "gptpstatmch.h"
#include "times.h"
*/



#define MRP_END_MARK                0x0000

#define MRP_END_MARK_LEN            2   /* octets (IEEE Corrigendum 1 to MRP) */
#define MRP_PROTOCOL_VERSION_LEN    1   /* octets */
#define MRP_MSG_HDR_LEN             2   /* octets */
#define MRP_ATTR_HDR_LEN            2   /* octets */


#define MAX_THREE_PACKED_EVENT_VAL  ((((5 * 6) + 5) * 6) + 5)

#define VLAN_TAG_LEN                4  /* octets */

/* VLAN tagged frames */
#define VLAN_TAG_LEN        4

/* Each Stream can have maximum 10 attributes, we have 8 ports, 
 * per port 1 stream 
 */


/* Support 10 streams, with each stream 
 * supporting 3 attributes for each participant of the port
 * 8 participants(8 ports) hence 240 attributes.*/

#define MRP_MAX_ATTRS 240

#define AVB_DEFAULT_VLAN                   (2)



#if RAVE_FEATURE_MSRP
#define MRP_SEND_BUFFER_SIZE (1518)
#else
#define MRP_SEND_BUFFER_SIZE 0
#endif




//static mad_machine [MRP_MAX_ATTRS];





struct stream_info {
  unsigned id[2];
  unsigned vlan;
  char addr[6];
  unsigned int maxframesize;
  unsigned int maxintframes;
  unsigned int dataFramePrio;
  unsigned char tspec;
  unsigned int accumulated_latency;
  char failed_bridge_id[8];
  unsigned char failure_code;
  char used;
};

struct vtss_mrp_port;
struct vtss_timers_context;

typedef struct reservations_info {
unsigned id[2];
unsigned char direction;
unsigned int decl_type;
unsigned int accumulated_latency;
//char failed_bridge_id[6];
unsigned int failure_code;
unsigned int streams_age;
struct vtss_mrp_port *pPort;
int four_packed_event;
int used;
}reservations_info ;


typedef struct vtss_timers_context {
    vars_uchar8 running;
    int timeout;
}vtss_timers_context_t;

/* MRP port */
typedef  struct vtss_mrp_port {
    vars_uchar8          port_number;
    vtss_common_macaddr_t       port_macaddr; // HW MAC address of the port

    vtss_msrp_port_config_t      port_config;

    vars_uchar8 point_to_point;                 // 1 = point to point
                                        // 0 = shared medium
    vars_ulong32 periodic_timeout;   // Periodic timer expiration time
    enum mrp_periodic_state periodic_state;

    struct vtss_mrp_application *app;        // mrp application component
    struct vtss_mrp_participant * participants; // list of attached participants
    struct vtss_mrp_port *next;          // item in list of application ports

    vars_uchar8 is_enabled;
    int reg_failures;                   // number of registration failures
    vars_uchar8 last_pdu_origin[6];  // MAC addr of originator of the last
                                        // MRPDU that caused a change in the
                                        // Registrar state machine
    vars_ushort16 SPVID;//Default VLAN ID for streams on this port
    
    vars_uchar8 maxPortLatency;//Latency for all classes is 1 by default.

    reservations_info *pReservations;

    struct vtss_timers_context port_periodic;

    
}vtss_mrp_port_t;



typedef enum {
  MSRP_TALKER_ADVERTISE,
  MSRP_TALKER_FAILED,
  MSRP_LISTENER,
  MSRP_DOMAIN_VECTOR,  
  MRP_NUM_ATTRIBUTE_TYPES
} mrp_attribute_type;

#define FIRST_VALUE_LENGTHS \
{   sizeof(srp_talker_first_value), \
    sizeof(srp_talker_failed_first_value),\
    sizeof(srp_listener_first_value),\
    sizeof(srp_domain_first_value)\
}

#define AVB_SRP_ATTRIBUTE_TYPE_TALKER_ADVERTISE 1
#define AVB_SRP_ATTRIBUTE_TYPE_TALKER_FAILED 2
#define AVB_SRP_ATTRIBUTE_TYPE_LISTENER 3
#define AVB_SRP_ATTRIBUTE_TYPE_DOMAIN 4

#define AVB_SRP_FOUR_PACKED_EVENT_IGNORE 0
#define AVB_SRP_FOUR_PACKED_EVENT_ASKING_FAILED 1
#define AVB_SRP_FOUR_PACKED_EVENT_READY 2
#define AVB_SRP_FOUR_PACKED_EVENT_READY_FAILED 3






//Lengths of the first values for each attribute type
extern struct stream_info stream_info_vars[];

extern struct reservations_info reservations_info_ptr[];

extern int first_value_lengths[];





/* Holds per-attribute applicant and registrar state machines.*/
typedef  struct mad_machine {
    enum mrp_applicant_state app_state;
    enum mrp_registrar_state reg_state;

    unsigned char attribute_type;

    enum mrp_applicant_mgt app_mgt;
    enum mrp_registrar_mgt reg_mgt;

    /* Denotes if this is a four vector event */
    char is_four_vector;

    char is_domain_attr;

    unsigned char SRclassID;
    unsigned char SRclassPriority;
    unsigned char SRclassVID[2];

    /* Contains the four packed event */
    int  four_vector_parameter;

    enum mrp_attribute_direction attr_dir;

    struct mad_machine *next; /* Pointer to next attribute */

    void *attribute_info;

    vars_uchar8 used;

    unsigned int StreamID[2];   

    
}mad_machine_t;


struct vtss_mrp_application;



/* MRP participant */
typedef  struct vtss_mrp_participant {
    struct vtss_mrp_port *port;              // mrp port

    struct mad_machine machines[24];       // per-attribute state machines
    enum mrp_leaveall_state leaveall_state; // per-part leaveall state machine

    struct mad_machine domain_attr_sent; //Domain attribute sent on port
    struct mad_machine domain_attr_recv;//Domain Attribute received on port

    

    struct vtss_timers_context join_timer_running;             // 1 = running, 0 = not running
    struct vtss_timers_context leave_timer_running;            // 1 = running, 0 = not running
    struct vtss_timers_context leaveall_timer;                       // 1 = leaveall timer expired
    int leaveall;

    vars_ulong32  tx_window[3];       // Tx rate control

    char send_buf[MRP_SEND_BUFFER_SIZE];                   // used to encode MRP PDU

    /* When Participant is initialized the send pointer points to the send_buf above*/
    /* &send_buf[0] + sizeof(mrp_ethernet_hdr) + sizeof(mrp_header);*/


    char *send_ptr;//Used to determine the end of the message 

    vars_uchar8 next_to_process;                // next attribute to start processing
                                        // with, in the following tx opportunity    

    
}vtss_mrp_participant_t;

typedef   struct vtss_mrp_proto {
    vars_uchar8 version;                    // MRP Application Protocol Version
    vars_ushort16 ethertype;
    vars_uchar8 address[6];    
    vars_uchar8 tagged;                         // 1 = vlan aware protocol
                                        // 0 = not vlan aware
    vars_uchar8 enabled;
}vtss_mrp_proto_t;

/* MRP application configuration */
typedef  struct vtss_mrp_application {

    vtss_mrp_system_config_t  sys_config;// System level configuration.

	unsigned char initialized;

    struct vtss_mrp_port ports[VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS];             // List of (all) ports

	struct vtss_mrp_participant participants[VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS];
    

}vtss_mrp_application_t;

/* Service primitives */
void mrp_join_req(struct vtss_mrp_participant *p, struct mad_machine *mid, int is_new);
void mrp_leave_req(struct vtss_mrp_participant *p, struct mad_machine *mid);

void mrp_flush(struct vtss_mrp_participant *p);
void mrp_redeclare(struct vtss_mrp_participant *p);

/* MRP Configuration */
int mrp_register_application(struct vtss_mrp_application *app);
int mrp_unregister_application(struct vtss_mrp_application *app);

void mrp_init_port(struct vtss_mrp_port *port);
void mrp_uninit_port(struct vtss_mrp_port *port);

struct vtss_mrp_participant *mrp_create_participant(struct vtss_mrp_port *port);

void mrp_destroy_participant(struct vtss_mrp_participant *p);

struct map_context *map_context_create(int cid, struct vtss_mrp_application *app);
void map_context_destroy(struct vtss_map_context *c, struct vtss_mrp_application *app);

struct mrp_port *mrp_find_port(struct vtss_mrp_application *app, int hw_index);
struct map_context *mrp_find_context(struct vtss_mrp_application *app, int cid);
struct mrp_participant *mrp_find_participant(struct vtss_mrp_port *port,
                                             struct vtss_map_context *c);

int map_context_add_port(struct vtss_map_context *c, struct vtss_mrp_port *port);
void map_context_remove_port(struct vtss_map_context *c, struct vtss_mrp_port *port);

int map_context_add_participant(struct vtss_map_context *c,
                                struct vtss_mrp_participant *p);

void map_context_remove_participant(struct vtss_map_context *c,
                                    struct vtss_mrp_participant *p);

/* MRP Protocol */
int mrp_init(void);
void mrp_protocol(struct vtss_mrp_application *app);

int mad_attr_event(struct vtss_mrp_participant *p,
                    struct mad_machine *mid,
                    enum mrp_event event);
void mad_participant_event(struct vtss_mrp_participant *p, enum mrp_event event);

/* MRP Management */
void mrp_set_registrar_control(struct vtss_mrp_participant *p,
                               struct mad_machine *mid,
                               enum mrp_registrar_mgt control);

void mrp_set_applicant_control(struct vtss_mrp_participant *p,
                               struct mad_machine *mid,
                               enum mrp_applicant_mgt control);

/* MRP PDU */

//void mrp_pdu_init(struct mrpdu *pdu, int tagged);
//int mrp_pdu_rcv(struct vtss_mrp_application *app);
//void mrp_pdu_send(struct vtss_mrp_participant *part);

//int mrp_pdu_empty(struct mrpdu *pdu);

/*int mrp_pdu_append_attr(struct vtss_mrp_participant *part,
                        struct mad_machine *mid,
                        enum mrp_attr_event event);*/

//int mrp_pdu_append_endmark(struct mrpdu *pdu);

/* MRP Attribute Operations */





/* Define the attribute operations here */


extern struct vtss_mrp_application  vtss_mrp_vars; /* The *only* global variable */
#define MSRP    (&vtss_mrp_vars) /* Defined as pointer to global area */




#endif /* _VTSS_MSRP_PRIVATE_H_ */
