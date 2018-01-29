
/*
 * This file contains the API definitions used between the MSRP protocol
 * module and the operating environment.
 */
#ifndef _VTSS_MSRP_H_
#define _VTSS_MSRP_H_ 1




#include "vtss_msrp_os.h"
#include "msrp_pdu.h"
#include "h2txrx.h"



/* ------------------------------------------------------------------------------------------- */

#define VTSS_MSRP_TICKS_PER_SEC         (100)

#define MRP_ACTION_MASK         0x0f
#define MRP_TIMER_ACTION_MASK   0x70
#define MRP_REQ_TX_MASK         0x80



/* MRP State Transition */
typedef  struct mrp_state_trans {
    vars_uchar8 state;
    vars_uchar8 action;
}mrp_state_trans;


/* Multiple actions can be combined considering the following syntax:
   bit 0-3: user and tx actions
   bit 4-6: timer actions
   bit 7:   flag for requesting tx opportunity */



enum mrp_action {
    MRP_ACTION_NONE  = 0x00,
    MRP_ACTION_NEW   = 0x01,         // Send New indication to MAP and MRP app.
    MRP_ACTION_JOIN  = 0x02,         // Send Join indication to MAP and MRP app.
    MRP_ACTION_LV    = 0x03,         // Send Leave indication to MAP and MRP app.

    MRP_ACTION_S_NEW  = 0x04,        // Send a New message
    MRP_ACTION_S_JOIN = 0x05,        // Send a JoinIn or JoinEmpty message
    MRP_ACTION_S_LV   = 0x06,        // Send a Leave message
    MRP_ACTION_S      = 0x07,        // Send an In or Empty message
    MRP_ACTION_S_LA   = 0x08,        // Send a LeaveAll message

    MRP_ACTION_PERIODIC = 0x09,      // Causes periodic event

    MRP_ACTION_START_LEAVE_TIMER     = 0x10,    // Start leave timer
    MRP_ACTION_STOP_LEAVE_TIMER      = 0x20,    // Stop leave timer
    MRP_ACTION_START_LEAVEALL_TIMER  = 0x30,    // Start leave all timer
    MRP_ACTION_START_PERIODIC_TIMER  = 0x40,    // Start periodic timer

    MRP_ACTION_REQ_TX = 0x80        // Request transmission opportunity
};



enum mrp_applicant_mgt {
    MRP_NORMAL_PARTICIPANT,
    MRP_NON_PARTICIPANT
};

enum mrp_registrar_mgt {
    MRP_NORMAL_REGISTRATION,
    MRP_FIXED_REGISTRATION,
    MRP_FORBIDDEN_REGISTRATION
};

enum mrp_attribute_direction{
     MRP_DIR_REG, // Incoming to a bridge participant
     MRP_DIR_DEC //  Outgoing to a bridge participant

};

enum mrp_applicant_state {
    MRP_APPLICANT_INVALID,
    MRP_APPLICANT_VO,           // Very anxious observer
    MRP_APPLICANT_VP,           // Very anxious passive
    MRP_APPLICANT_VN,           // Very anxious new
    MRP_APPLICANT_AN,           // Anxious      new
    MRP_APPLICANT_AA,           // Anxious      active
    MRP_APPLICANT_QA,           // Quiet        active
    MRP_APPLICANT_LA,           // Leaving      active
    MRP_APPLICANT_AO,           // Anxious      observer
    MRP_APPLICANT_QO,           // Quiet        observer
    MRP_APPLICANT_AP,           // Anxious      passive
    MRP_APPLICANT_QP,           // Quiet        passive
    MRP_APPLICANT_LO,           // Leaving      observer
    MRP_APPLICANT_MAX
};

enum mrp_registrar_state {
    MRP_REGISTRAR_INVALID,
    MRP_REGISTRAR_IN,           // In
    MRP_REGISTRAR_LV,           //'The accuracy required for the leavetimer is
    MRP_REGISTRAR_L3,           // sufficiently coarse as to permit the use of
    MRP_REGISTRAR_L2,           // a single operating system timer per Participant
    MRP_REGISTRAR_L1,           // with 2 bits of state for each Registrar'
    MRP_REGISTRAR_MT,           // Empty
    MRP_REGISTRAR_MAX
};

enum mrp_leaveall_state {
    MRP_LEAVEALL_INVALID,
    MRP_LEAVEALL_A,             // Active
    MRP_LEAVEALL_P,             // Passive
    MRP_LEAVEALL_MAX
};

enum mrp_periodic_state {
    MRP_PERIODIC_INVALID,
    MRP_PERIODIC_A,             // Active
    MRP_PERIODIC_P,             // Passive
    MRP_PERIODIC_MAX
};

enum mrp_attr_event {
    MRP_NEW         = 0,        // New
    MRP_JOIN_IN     = 1,        // JoinIn
    MRP_IN          = 2,        // In
    MRP_JOIN_MT     = 3,        // JoinEmpty
    MRP_MT          = 4,        // Empty
    MRP_LV          = 5         // Leave
};


enum mrp_event {
    MRP_EVENT_UNKOWN,

    MRP_EVENT_NEW,              // New attribute declaration
    MRP_EVENT_JOIN,             // Attribute declaration (no new registration)
    MRP_EVENT_LV,               // Withdraw attribute declaration

    MRP_EVENT_R_NEW,            // Receive New message
    MRP_EVENT_R_JOIN_IN,        // Receive JoinIn message
    MRP_EVENT_R_IN,             // Receive In message
    MRP_EVENT_R_JOIN_MT,        // Receive JoinEmpty message
    MRP_EVENT_R_MT,             // Receive Empty message
    MRP_EVENT_R_LV,             // Receive Leave message
    MRP_EVENT_R_LA,             // Receive LeaveAll message


    MRP_EVENT_TX,               // Tx opportunity without LeaveAll
    MRP_EVENT_TX_LA,            // Tx opportunity with a LeaveAll.
    MRP_EVENT_TX_LAF,           // Tx opportunity with a LeaveAll. (PDU full)

    MRP_EVENT_FLUSH,            // Root or Alternate port changed to Designated
    MRP_EVENT_REDECLARE,        // Designated port changed to Root or Alternate
    MRP_EVENT_PERIODIC,         // Periodic transmission event
    MRP_EVENT_PERIODIC_TIMER,   // Periodic timer expired
    MRP_EVENT_PERIODIC_ENABLED, // Periodic transmission enabled by management
    MRP_EVENT_PERIODIC_DISABLED,// Periodic transmission disabled by management

    MRP_EVENT_LEAVE_TIMER,      // Leave timer expired
    MRP_EVENT_LEAVEALL_TIMER,   // Leave All timer expired
    MRP_EVENT_MAX
};

#define MRP_JOINTIMER_PERIOD_CENTISECONDS 20

#define MRP_LEAVETIMER_PERIOD_CENTISECONDS 80

#define MRP_LEAVEALL_TIMER_PERIOD_CENTISECONDS 1000
#define MRP_LEAVEALL_TIMER_MULTIPLIER 100

#define MRP_PERIODIC_TIMER_PERIOD_CENTISECONDS 100
#define MRP_PERIODIC_TIMER_MULTIPLIER 10

#define VTSS_MSRP_IS_AGGR(POAG) ((POAG) > VTSS_MSRP_MAX_PORTS)
#define VTSS_MSRP_IS_PORT(POAG) ((POAG) <= VTSS_MSRP_MAX_PORTS)
#define VTSS_MSRP_IS_POAG(POAG) ((POAG) > 0 && (POAG) <= VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_PORTS)



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
void mad_attrtype_event(struct vtss_mrp_participant *p,vars_uchar8 attrtype, enum mrp_event event);

void mad_participant_event(struct vtss_mrp_participant *p, enum mrp_event event);

/* MRP Management */
void mrp_set_registrar_control(struct vtss_mrp_participant *p,
                               struct mad_machine *mid,
                               enum mrp_registrar_mgt control);

void mrp_set_applicant_control(struct vtss_mrp_participant *p,
                               struct mad_machine *mid,
                               enum mrp_applicant_mgt control);

/* MRP PDU */


int mrp_pdu_rcv(struct vtss_mrp_application *app);
int mrp_pdu_ex_send(struct vtss_mrp_participant *part);
int mrp_pdu_full(struct vtss_mrp_participant *p);

int mrp_pdu_empty(struct vtss_mrp_participant *p);

int mrp_pdu_check_send(int port_no, char *buf, int len);





int mrp_pdu_append_attr(struct vtss_mrp_participant *part,
                        struct mad_machine *mid,
                        enum mrp_attr_event event);

int mrp_pdu_append_endmark(struct mrpdu *pdu);

/* MRP Attribute Operations */

/* Implement MSRP operations here */

/*
 * Functions provided by the vtss_msrp protocol module.
 */

/**
 * vtss_msrp_init - Initialize internal data, obtain HW MAC adresses and start the GPTP protocol.
 */
extern void vtss_msrp_init(void);

/**
 * vtss_msrp_deinit - Initialize internal data back to default. Await new vtss_gptp_init() call.
 */
extern void vtss_msrp_deinit(void);

/* Global parameters */
/* MRP protocol configuration */
typedef struct vtss_mrp_system_config {
    vars_uchar8 version;/* MSRP Application Protocol version */
    vars_ushort16 ethertype; /* MSRP Application Ethernet Type */
    vars_uchar8 address[6];  /* Application Destination Mac Address */  
    vars_uchar8 tagged;/* If vlan aware or not 1 is aware */
    vtss_common_bool_t enabled;/* if MSRP  is enabled on the system*/
}vtss_mrp_system_config_t;

/* Port parameters */
typedef struct vtss_msrp_port_config {    
	vtss_common_bool_t enable_msrp;
} vtss_msrp_port_config_t;




/**
 * vtss_msrp_set_config - Set global parameters.
 * Note: Can be called before vtss_msrp_init().
 */
extern void vtss_msrp_set_config(const vtss_mrp_system_config_t VTSS_COMMON_PTR_ATTRIB *vtss_mrp_sys_cfg);

/**
 * vtss_gptp_get_config - Get global parameters.
 * Note: Can be called before vtss_gptp_init().
 */
extern void vtss_msrp_get_config(vtss_mrp_system_config_t VTSS_COMMON_PTR_ATTRIB *vtss_mrp_sys_cfg);


/**
 * vtss_msrp_set_portconfig - Set port-specific parameters.
 * Note: Can be called before vtss_gptp_init().
 */
extern void vtss_msrp_set_portconfig(vtss_common_port_t portno,
                                     const vtss_msrp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config);


/**
 * vtss_msrp_get_portconfig - Get port-specific parameters.
 * Note: Can be called before vtss_msrp_init().
 */
extern void vtss_msrp_get_portconfig(vtss_common_port_t portno,
                                     vtss_msrp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config);

/**
 * vtss_msrp_tick() - Timer tick event.
 * Must be called from OS once every second.
 */
extern void vtss_msrp_tick(void);

/**
 * vtss_msrp_more_work() - Called when idle.
 * Must be called to perform remaining tasks.
 */
extern void vtss_msrp_more_work(void);

/**
 * vtss_gptp_receive - Deliver a frame from the OS to the GPTP protocol.
 * Must be called from OS when a frame with destination
 * MAC address VTSS_GPTP_MULTICAST_MACADDR and ethertype
 * of VTSS_GPTP_ETHTYPE is received
 */
extern void vtss_msrp_receive(vtss_common_port_t from_port,
			      const vtss_common_octet_t VTSS_COMMON_BUFMEM_ATTRIB *frame,
			      vtss_common_framelen_t len);


/*
 * Functions to be provided by OS specifically for the vtss_msrp module.
 * There are other functions defined in vtss_common_os.h
 */

int msrp_mad_join_req(struct vtss_mrp_participant *part,struct mad_machine *pMadMachine,int new);
int msrp_mad_leave_req(struct vtss_mrp_participant *part,struct mad_machine *pMadMachine);

static int check_domain_merge(char *buf) ;

void mrp_encode_three_packed_event(struct vtss_mrp_participant *p,char *buf,int event,int attr);

unsigned attribute_length_length(mrp_msg_header* hdr);


void * mymemmove(void * dest, const void * src, int n);


static int has_fourpacked_events(int attr);

static int decode_fourpacked(int vector, int i);

static int decode_threepacked(int vector, int i);

static void mad_event(struct vtss_mrp_participant *p, enum mrp_event event);






#ifndef VTSS_GPTP_NDEBUG
extern void vtss_msrp_dump_port(vtss_common_port_t portno);
extern void vtss_msrp_dump(void);
#endif /* !VTSS_GPTP_NDEBUG */

#endif /* _VTSS_GPTP_H_ */
