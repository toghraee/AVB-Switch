
/**
 * This file contains an implementation of the MSRP protocol 
 *
 * It is written generic to be adapted into different operating environment.
 * The file vtss_gptp.h defines the API between this protocol module and
 * the operating environment.
 */

#define VTSS_MSRP_PROTOCOL 1    /* This is the protocol module */




#include "vtss_msrp.h"
#include "H2.h"
#ifdef VTSS_MSRP_NOT_WANTED
#if defined(__CX51__) || defined(__C51__)
const char *vtss_msrp_coseg = "This creates the CO segment of vtss_msrp";
#endif /* __CX51__ || __C51__ */
#else


#include "vtss_msrp_private.h"

#if defined(__CX51__) || defined(__C51__)
#include "print.h"
#endif

#ifndef VTSS_MSRP_TRACE
#include <stdio.h>
#pragma warning "Falling back on printf for trace"
#define VTSS_MSRP_TRACE(lvl, fmt, ...)     printf(fmt, ##__VA_ARGS__)
#endif /* VTSS_MSRP_TRACE */





#define MSRP_IS_UNINITED (!MSRP->initialized)
#define MSRP_INIT       do { if (MSRP_IS_UNINITED) msrp_sw_init(); } while (0)


int first_value_lengths[MRP_NUM_ATTRIBUTE_TYPES] = FIRST_VALUE_LENGTHS;


struct stream_info stream_info_vars[5];


struct reservations_info reservations_info_ptr[30];


struct vtss_mrp_application vtss_mrp_vars;



code mrp_state_trans  mrp_applicant_state_table[MRP_APPLICANT_MAX][MRP_EVENT_MAX] = {                      
                {0},                                                                             
                {                                                                                
				          {0},                                                                           
                  {MRP_APPLICANT_VN, MRP_ACTION_REQ_TX },                                        
                  {MRP_APPLICANT_VP, MRP_ACTION_REQ_TX},                                         
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_AO},                                                            
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                         
                  {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                         
                  {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                         
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                         
                  {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX}                                          
               },                                                                                
			        {                                                                                  
				          {0},                                                                           
                  {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX },                                         
                  {MRP_APPLICANT_VP},                                                            
                  {MRP_APPLICANT_VO},                                                            
                  {MRP_APPLICANT_VP},                                                            
                  {MRP_APPLICANT_AP,MRP_ACTION_REQ_TX},                                          
                  { MRP_APPLICANT_VP},                                                           
                  { MRP_APPLICANT_VP},                                                           
                  { MRP_APPLICANT_VP},                                                           
                  { MRP_APPLICANT_VP},                                                           
                  { MRP_APPLICANT_VP},                                                           
                  { MRP_APPLICANT_VP},                                                           
                  { MRP_APPLICANT_VP},                                                           
                  { MRP_APPLICANT_AA, MRP_ACTION_S_JOIN | MRP_ACTION_REQ_TX},                    
                  { MRP_APPLICANT_AA, MRP_ACTION_S |MRP_ACTION_REQ_TX},                          
                  {MRP_APPLICANT_VP, MRP_ACTION_REQ_TX}                                          
               },                                                                                
               {                                                                                 
                  {0},                                                                           
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_LA,MRP_ACTION_REQ_TX},                                          
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_VN},                                                            
                  {MRP_APPLICANT_AN,MRP_ACTION_S_NEW | MRP_ACTION_REQ_TX},                       
                  {MRP_APPLICANT_AN,MRP_ACTION_S_NEW | MRP_ACTION_REQ_TX},                       
                  {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX}                                           
                },                                                                               
                {                                                                                
                  {0},                                                                           
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_LA,MRP_ACTION_REQ_TX},                                 
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX},                                 
                  {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX},                                 
                  {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX},                                 
                  {MRP_APPLICANT_AN},                                                   
                  {MRP_APPLICANT_QA,MRP_ACTION_S_NEW},                                  
                  {MRP_APPLICANT_QA,MRP_ACTION_S_NEW},                                  
                  {MRP_APPLICANT_VN, MRP_ACTION_REQ_TX}                                 
                                                                                                 
                },                                                                               
                {                                                                                
                  {0},                                                                           
                  {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX},                                          
                  {MRP_APPLICANT_AA},                                                            
                  {MRP_APPLICANT_LA,MRP_ACTION_REQ_TX},                                          
                  {MRP_APPLICANT_AA},                                                            
                  {MRP_APPLICANT_QA},                                                            
                  {MRP_APPLICANT_QA},                                                            
                  {MRP_APPLICANT_AA},                                                            
                  {MRP_APPLICANT_AA},                                                            
                  {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                          
                  {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                          
                  {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                          
                  {MRP_APPLICANT_AA},                                                            
                  {MRP_APPLICANT_QA,MRP_ACTION_S_JOIN},                                          
                  {MRP_APPLICANT_QA,MRP_ACTION_S_JOIN},                                          
                  {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX}                                           
                                                                                                 
                },                                                                               
                {                                                                                
                {0},                                                                             
                {MRP_APPLICANT_VN, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_QA},                                                              
                {MRP_APPLICANT_LA, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_QA},                                                              
                {MRP_APPLICANT_QA},                                                              
                {MRP_APPLICANT_QA},                                                              
                {MRP_APPLICANT_AA,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_AA,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_AA,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_QA},                                                              
                {MRP_APPLICANT_QA,MRP_ACTION_S_JOIN},                                            
                {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX}                                             
                },                                                                               
                {                                                                                
                {0},                                                                             
                {MRP_APPLICANT_VN, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_AA, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_LA},                                                              
                {MRP_APPLICANT_VO,MRP_ACTION_S_LV},                                              
                {MRP_APPLICANT_LO,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_LO,MRP_ACTION_REQ_TX}                                             
                                                                                                 
                },                                                                               
                {                                                                                
                {0},                                                                             
                {MRP_APPLICANT_VN, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_AP, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_AO},                                                              
                {MRP_APPLICANT_AO},                                                              
                {MRP_APPLICANT_QO},                                                              
                {MRP_APPLICANT_AO},                                                              
                {MRP_APPLICANT_AO},                                                              
                {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                           
                {MRP_APPLICANT_AO},                                                              
                {MRP_APPLICANT_AO},                                                              
                {MRP_APPLICANT_LO,MRP_ACTION_REQ_TX},                                            
                {MRP_APPLICANT_LO,MRP_ACTION_REQ_TX}                                             
              },                                                                                 
              {                                                                                  
              {0},                                                                               
              {MRP_APPLICANT_VN, MRP_ACTION_REQ_TX},                                             
              {MRP_APPLICANT_QP},                                                                
              {MRP_APPLICANT_QO},                                                                
              {MRP_APPLICANT_QO},                                                                
              {MRP_APPLICANT_QO},                                                                
              {MRP_APPLICANT_QO},                                                                
              {MRP_APPLICANT_AO},                                                                
              {MRP_APPLICANT_AO},                                                                
              {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                             
              {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                             
              {MRP_APPLICANT_LO, MRP_ACTION_REQ_TX},                                             
              {MRP_APPLICANT_QO},                                                                
              {MRP_APPLICANT_QO},                                                                
              {MRP_APPLICANT_LO,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_LO,MRP_ACTION_REQ_TX}                                               
              },                                                                                 
              {                                                                                  
              {0},                                                                               
              {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_AP},                                                                
              {MRP_APPLICANT_AO},                                                                
              {MRP_APPLICANT_AP},                                                                
              {MRP_APPLICANT_QP},                                                                
              {MRP_APPLICANT_AP},                                                                
              {MRP_APPLICANT_AP},                                                                
              {MRP_APPLICANT_AP},                                                                
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_AP},                                                                
              {MRP_APPLICANT_QA,MRP_ACTION_S_JOIN},                                              
              {MRP_APPLICANT_QA,MRP_ACTION_S_JOIN},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX}                                               
              },                                                                                 
              {                                                                                  
              {0},                                                                               
              {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_QP},                                                                
              {MRP_APPLICANT_QO},                                                                
              {MRP_APPLICANT_QP},                                                                
              {MRP_APPLICANT_QP},                                                                
              {MRP_APPLICANT_QP},                                                                
              {MRP_APPLICANT_AP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_AP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_AP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_QP},                                                                
              {MRP_APPLICANT_QA,MRP_ACTION_S_JOIN},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX}                                               
              },                                                                                 
              {                                                                                  
              {0},                                                                               
              {MRP_APPLICANT_VN,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_VP,MRP_ACTION_REQ_TX},                                              
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_VO},                                                                
              {MRP_APPLICANT_VO},                                                                
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_VO,MRP_ACTION_S},                                                   
              {MRP_APPLICANT_LO},                                                                
              {MRP_APPLICANT_LO}                                                                 
              }                                                                                  
};                                                                                               


code  struct mrp_state_trans  mrp_registrar_state_table[MRP_REGISTRAR_MAX][MRP_EVENT_MAX] = {
        {0},
        {
            {0},
            {MRP_REGISTRAR_IN, MRP_ACTION_NEW},
            {MRP_REGISTRAR_IN},
            {MRP_REGISTRAR_IN},
            {MRP_REGISTRAR_LV,MRP_ACTION_START_LEAVE_TIMER},
            {MRP_REGISTRAR_LV,MRP_ACTION_START_LEAVE_TIMER},
            {MRP_REGISTRAR_LV,MRP_ACTION_START_LEAVE_TIMER},
            {MRP_REGISTRAR_LV,MRP_ACTION_START_LEAVE_TIMER},
            {MRP_REGISTRAR_MT},
            {MRP_REGISTRAR_IN}
        },

        { 
            {0},
            {MRP_REGISTRAR_IN,MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_LV},
            {MRP_REGISTRAR_LV},
            {MRP_REGISTRAR_LV},
            {MRP_REGISTRAR_LV},
            {MRP_REGISTRAR_MT,MRP_ACTION_LV},
            {MRP_REGISTRAR_L3,MRP_ACTION_START_LEAVE_TIMER}
        },
        {
            {0},
            {MRP_REGISTRAR_IN,MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_L3},
            {MRP_REGISTRAR_L3},
            {MRP_REGISTRAR_L3},
            {MRP_REGISTRAR_L3},
            {MRP_REGISTRAR_MT,MRP_ACTION_LV},
            {MRP_REGISTRAR_L2,MRP_ACTION_START_LEAVE_TIMER}
        },
        {
            {0},
            {MRP_REGISTRAR_IN,MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_L2},
            {MRP_REGISTRAR_L2},
            {MRP_REGISTRAR_L2},
            {MRP_REGISTRAR_L2},
            {MRP_REGISTRAR_MT,MRP_ACTION_LV},
            {MRP_REGISTRAR_L1,MRP_ACTION_START_LEAVE_TIMER}
        },
        {
            {0},
            {MRP_REGISTRAR_IN,MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_IN,MRP_ACTION_STOP_LEAVE_TIMER},
            {MRP_REGISTRAR_L1},
            {MRP_REGISTRAR_L1},
            {MRP_REGISTRAR_L1},
            {MRP_REGISTRAR_L1},
            {MRP_REGISTRAR_MT,MRP_ACTION_LV},
            {MRP_REGISTRAR_MT,MRP_ACTION_LV}
        },
        {
        {0},
           {MRP_REGISTRAR_IN,MRP_ACTION_NEW},
           {MRP_REGISTRAR_IN,MRP_ACTION_JOIN},
           {MRP_REGISTRAR_IN,MRP_ACTION_JOIN},
           {MRP_REGISTRAR_MT},
           {MRP_REGISTRAR_MT},
           {MRP_REGISTRAR_MT},
           {MRP_REGISTRAR_MT},
           {MRP_REGISTRAR_MT},
           {MRP_REGISTRAR_MT}
        }
    };



code struct mrp_state_trans  mrp_leaveall_state_table[MRP_LEAVEALL_MAX][MRP_EVENT_MAX] = {
                           {0},
        {
        {0},
            {MRP_LEAVEALL_P, MRP_ACTION_S_LA},
            {MRP_LEAVEALL_P, MRP_ACTION_START_LEAVEALL_TIMER},
            {MRP_LEAVEALL_A, MRP_ACTION_START_LEAVEALL_TIMER |  MRP_ACTION_REQ_TX}
        },
        {
          {0},
          {MRP_LEAVEALL_P},
          {MRP_LEAVEALL_P,MRP_ACTION_START_LEAVEALL_TIMER},
          {MRP_LEAVEALL_A,MRP_ACTION_START_LEAVEALL_TIMER | MRP_ACTION_REQ_TX}
        }
    };

/* Full-Participant Applicant State Table (See 802.1ak-2007 Table 10.3)
   Note: encoding optimization not handled in this version.
   Note: Whenever a state machine transitions to a state that requires
   transmission of a message, a transmit opportunity is requested (if one is not
   already pending.) */
#if 0
struct mrp_state_trans mrp_applicant_state_table[MRP_APPLICANT_MAX][MRP_EVENT_MAX];
/* Registrar State Table (See 802.1ak-2007 Table 10.4) */
struct mrp_state_trans  mrp_registrar_state_table[MRP_REGISTRAR_MAX][MRP_EVENT_MAX];

struct mrp_state_trans  mrp_leaveall_state_table[MRP_LEAVEALL_MAX][MRP_EVENT_MAX] ;




void init_app_state_table1()
{

    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_NEW].action = MRP_ACTION_REQ_TX;    
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_JOIN].state = MRP_APPLICANT_VP;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_JOIN].action = MRP_ACTION_REQ_TX;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_LV].state = MRP_APPLICANT_VO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_NEW].state = MRP_APPLICANT_VO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_JOIN_IN].state = MRP_APPLICANT_AO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_IN].state = MRP_APPLICANT_VO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_JOIN_MT].state = MRP_APPLICANT_VO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_MT].state = MRP_APPLICANT_VO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_LV].state = MRP_APPLICANT_LO;    
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_LV].action = MRP_ACTION_REQ_TX;    
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_LA].state  = MRP_APPLICANT_LO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_R_LA].action  = MRP_ACTION_REQ_TX;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_REDECLARE].state  = MRP_APPLICANT_LO;       
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_REDECLARE].action  = MRP_ACTION_REQ_TX;  
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_PERIODIC].state  = MRP_APPLICANT_VO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_TX].state  = MRP_APPLICANT_VO;    
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_TX_LA].state  = MRP_APPLICANT_LO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_TX_LA].action  = MRP_ACTION_REQ_TX;    
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_TX_LAF].state  = MRP_APPLICANT_LO;
    mrp_applicant_state_table[MRP_APPLICANT_VO][MRP_EVENT_TX_LAF].action  = MRP_ACTION_REQ_TX;




     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_NEW].action = MRP_ACTION_REQ_TX;    
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_LV].state  =   MRP_APPLICANT_VO;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_AP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_R_JOIN_IN].action  =   MRP_ACTION_REQ_TX;

     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_TX].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_TX].action  =  MRP_ACTION_S_JOIN | MRP_ACTION_REQ_TX;

     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_AA;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_TX_LA].action  =  MRP_ACTION_S | MRP_ACTION_REQ_TX;
            
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_VP;
     mrp_applicant_state_table[MRP_APPLICANT_VP][MRP_EVENT_TX_LAF].action  =  MRP_ACTION_REQ_TX;

     
    

}

void init_app_table2()
{

  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_LV].state  =   MRP_APPLICANT_LA;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_LV].action  =   MRP_ACTION_REQ_TX;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_TX].state  =   MRP_APPLICANT_AN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_TX].action  =   MRP_ACTION_S_NEW | MRP_ACTION_REQ_TX;

  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_AN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_TX_LA].action  =   MRP_ACTION_S_NEW | MRP_ACTION_REQ_TX;

  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_VN][MRP_EVENT_TX_LAF].action  =  MRP_ACTION_REQ_TX;


}


void init_app_table3()
{
    
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_NEW].state  =   MRP_APPLICANT_AN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_AN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_LV].state  =   MRP_APPLICANT_LA;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_LV].action  =   MRP_ACTION_REQ_TX;            

    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_AN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_AN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_AN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_AN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_AN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_VN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_LV].action  =   MRP_ACTION_REQ_TX;

    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_VN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_R_LA].action  =   MRP_ACTION_REQ_TX;

    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_VN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_REDECLARE].action  =   MRP_ACTION_REQ_TX;

    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_AN;

                
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_TX].state  =   MRP_APPLICANT_QA;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_TX].action  =   MRP_ACTION_S_NEW;

    
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_QA;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_TX_LA].action  =   MRP_ACTION_S_NEW;


    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_VN;
    mrp_applicant_state_table[MRP_APPLICANT_AN][MRP_EVENT_TX_LAF].action  =   MRP_ACTION_REQ_TX;
               

}

void init_app_table4()
{


    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_NEW].action =  MRP_ACTION_REQ_TX;

    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_AA;

    
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_LV].state  =   MRP_APPLICANT_LA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_LV].action =  MRP_ACTION_REQ_TX;

    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_AA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_QA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_QA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_AA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_AA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_VP;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_LV].action = MRP_ACTION_REQ_TX;

    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_VP;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_R_LA].action = MRP_ACTION_REQ_TX;


    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_VP;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_REDECLARE].action = MRP_ACTION_REQ_TX;

    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_AA;
    
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_TX].state  =   MRP_APPLICANT_QA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_TX].action  =   MRP_ACTION_S_JOIN;

    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_QA;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_TX_LA].action  =   MRP_ACTION_S_JOIN;

    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_VP;
    mrp_applicant_state_table[MRP_APPLICANT_AA][MRP_EVENT_TX_LAF].action  =   MRP_ACTION_REQ_TX;                    
            
            
         

}

void init_app_table5()
{

     
  mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;
  mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_NEW].action  =   MRP_ACTION_REQ_TX;
                                      
     
  mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_QA;                
                
  
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_LV].state  =   MRP_APPLICANT_LA;
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_LV].action  =   MRP_ACTION_REQ_TX;                                      

            
                                      
  
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_QA;
  
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_QA;               
  
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_QA;         
            
  
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_AA;
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_JOIN_MT].action  =   MRP_ACTION_REQ_TX;

  
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_AA;
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_MT].action  =   MRP_ACTION_REQ_TX;
                                      
  
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_VP;
 mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_LV].action  =   MRP_ACTION_REQ_TX;
                                      
  
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_VP;
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_R_LA].action  =   MRP_ACTION_REQ_TX;                                      
                                      
  
                                      
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_VP;
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_REDECLARE].action  =   MRP_ACTION_REQ_TX; 

  
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_AA;
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_PERIODIC].action  =   MRP_ACTION_REQ_TX; 

                                      
  
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_TX].state  =   MRP_APPLICANT_QA;            
  
  
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_QA;
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_TX_LA].action  =   MRP_ACTION_S_JOIN;                                       

            
  
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_VP;
mrp_applicant_state_table[MRP_APPLICANT_QA][MRP_EVENT_TX_LAF].action  =   MRP_ACTION_REQ_TX;

}


void init_app_table6()
{
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_NEW].action  =   MRP_ACTION_REQ_TX;
                                        
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_AA;
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_JOIN].action  =   MRP_ACTION_REQ_TX;                                        
                                        
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_LV].state  =   MRP_APPLICANT_LA;                
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_LA;                               
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_LA;                                         
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_LA;            
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_LA; 
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_LA;                 
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_LA;
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_LA;
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_LA;                
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_LA;                 
              
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_TX].state  =   MRP_APPLICANT_VO;                                         
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_TX].action  =  MRP_ACTION_S_LV;              
                                        
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_LO;                                         
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_TX_LA].action  =  MRP_ACTION_REQ_TX;                                          
    
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_LO;                                         
    mrp_applicant_state_table[MRP_APPLICANT_LA][MRP_EVENT_TX_LAF].action  =  MRP_ACTION_REQ_TX;
        

}

void init_app_table7()
{


mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;                                         
    mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_NEW].action  =  MRP_ACTION_REQ_TX;                                      
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_AP;                                         
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_JOIN].action  =  MRP_ACTION_REQ_TX;

mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_LV].state  =   MRP_APPLICANT_AO;                  

mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_AO;                  

mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_QO;                  

mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_AO;                  

mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_AO;                  
            
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_MT].action  =  MRP_ACTION_REQ_TX;                                      
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_LV].action  =  MRP_ACTION_REQ_TX;                                      
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_R_LA].action  =  MRP_ACTION_REQ_TX;                                      
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_REDECLARE].action  =  MRP_ACTION_REQ_TX;                                      

mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_AO;                  

mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_TX].state  =   MRP_APPLICANT_AO;                  
            
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_TX_LA].action  =  MRP_ACTION_REQ_TX;                                      
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_AO][MRP_EVENT_TX_LAF].action  =  MRP_ACTION_REQ_TX;  


}

init_app_table8()
{         

mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;                                         
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_NEW].action  =  MRP_ACTION_REQ_TX;                                           
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_QP;                 
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_LV].state  =   MRP_APPLICANT_QO;                 
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_QO;                 
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_QO;                 
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_QO;                 
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_AO;                 
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_AO;                 
               
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_LV].action  =  MRP_ACTION_REQ_TX;                                           
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_R_LA].action  =  MRP_ACTION_REQ_TX;                                           
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_REDECLARE].action  =  MRP_ACTION_REQ_TX;                                           
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_QO;                 
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_TX].state  =   MRP_APPLICANT_QO;                 
               
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_TX_LA].action  =  MRP_ACTION_REQ_TX;                                           
          
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_LO;                                         
mrp_applicant_state_table[MRP_APPLICANT_QO][MRP_EVENT_TX_LAF].action  =  MRP_ACTION_REQ_TX;                 
          
}

init_app_table9()
{
    


 mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;                                         
 mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_NEW].action  =  MRP_ACTION_REQ_TX;                                    
                                    

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_AP;             
          

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_LV].state  =   MRP_APPLICANT_AO;             
          

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_AP;             
          

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_QP;             
          

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_AP;             
          

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_AP;             
          

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_AP;             
          
 mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_VP;                                         
 mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_LV].action  =  MRP_ACTION_REQ_TX;                                    
                                    
                                    
         
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_R_LA].action  =  MRP_ACTION_REQ_TX;                                    
                                    
                                    
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_REDECLARE].action  =  MRP_ACTION_REQ_TX;                                    
                                    
                                    

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_AP;             
          
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_TX].state  =   MRP_APPLICANT_QA;                                         
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_TX].action  =  MRP_ACTION_S_JOIN;                                    
                                    
                                    
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_QA;                                         
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_TX_LA].action  =  MRP_ACTION_S_JOIN;                                    
                                    
                                    

mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_AP][MRP_EVENT_TX_LAF].action  =  MRP_ACTION_REQ_TX;                                    
          
}


void init_app_table10()
{



mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_NEW].action  =  MRP_ACTION_REQ_TX;                                     
                                     

mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_QP;             
           

mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_LV].state  =   MRP_APPLICANT_QO;             
           

mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_QP;             
           

mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_QP;             
           

mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_QP;             

mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_AP;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_JOIN_MT].action  =  MRP_ACTION_REQ_TX;                                     
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_AP;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_MT].action  =  MRP_ACTION_REQ_TX;                                     
                                     
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_LV].action  =  MRP_ACTION_REQ_TX;                                     
                                     
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_R_LA].action  =  MRP_ACTION_REQ_TX;                                     
                                     
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_REDECLARE].action  =  MRP_ACTION_REQ_TX;                                     
                                     
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_AP;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_PERIODIC].action  =  MRP_ACTION_REQ_TX;                                     
                                     

mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_TX].state  =   MRP_APPLICANT_QP;            
           
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_QA;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_TX_LA].action  =  MRP_ACTION_S_JOIN;                                     
                                     
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_QP][MRP_EVENT_TX_LAF].action  =  MRP_ACTION_REQ_TX;           


}

void init_app_table11()
{


 
 
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_NEW].state  =   MRP_APPLICANT_VN;                                         
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_NEW].action  =  MRP_ACTION_REQ_TX;                                        
 
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_JOIN].state  =   MRP_APPLICANT_VP;                                         
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_JOIN].action  =  MRP_ACTION_REQ_TX;                                        

mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_LV].state  =   MRP_APPLICANT_LO;                                                         
                

mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_R_NEW].state  =   MRP_APPLICANT_LO;                                                         
              

mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_R_JOIN_IN].state  =   MRP_APPLICANT_LO;                                                         
              

mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_R_IN].state  =   MRP_APPLICANT_LO;                                                         
              
            
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_R_JOIN_MT].state  =   MRP_APPLICANT_VO;                                                         
              
            
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_R_MT].state  =   MRP_APPLICANT_VO;                                                         
              
            
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_R_LV].state  =   MRP_APPLICANT_LO;                                                         
              
            
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_R_LA].state  =   MRP_APPLICANT_LO;                                                         
              
            
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_REDECLARE].state  =   MRP_APPLICANT_LO;                                                         
              
          
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_PERIODIC].state  =   MRP_APPLICANT_LO;                                                         
              
          
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_TX].state  =   MRP_APPLICANT_VO;                                         
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_TX].action  =  MRP_ACTION_S;                                        
                                        
          
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_TX_LA].state  =   MRP_APPLICANT_LO;                                                         
              
          
mrp_applicant_state_table[MRP_APPLICANT_LO][MRP_EVENT_TX_LAF].state  =   MRP_APPLICANT_LO;                                                       

}




void init_registrar_table1()
{
   
 mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_NEW].state =  MRP_REGISTRAR_IN;
 mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_NEW].action =  MRP_ACTION_NEW; 
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_JOIN_IN].state =  MRP_REGISTRAR_IN;                
                
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_JOIN_MT].state =  MRP_REGISTRAR_IN;                
            
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_LV].state =  MRP_REGISTRAR_LV;
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_LV].action =  MRP_ACTION_START_LEAVE_TIMER;                                      
                                      
                                      
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_LA].state =  MRP_REGISTRAR_LV;
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_R_LA].action =  MRP_ACTION_START_LEAVE_TIMER;                                      
                                      
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_TX_LA].state =  MRP_REGISTRAR_LV;
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_TX_LA].action =  MRP_ACTION_START_LEAVE_TIMER;                                      
                                      
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_REDECLARE].state =  MRP_REGISTRAR_LV;
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_REDECLARE].action =  MRP_ACTION_START_LEAVE_TIMER;                                      
                                      
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_FLUSH].state =  MRP_REGISTRAR_MT;                
            
   
mrp_registrar_state_table[MRP_REGISTRAR_IN][MRP_EVENT_LEAVE_TIMER].state =  MRP_REGISTRAR_IN;            


/* Second Part */

mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_NEW].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_NEW].action =  MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER;
                                               
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_JOIN_IN].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_JOIN_IN].action =  MRP_ACTION_STOP_LEAVE_TIMER;                                           
                                     
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_JOIN_MT].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_JOIN_MT].action =  MRP_ACTION_STOP_LEAVE_TIMER;                                           
                                     

mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_LV].state =  MRP_REGISTRAR_LV;            
           

mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_R_LA].state =  MRP_REGISTRAR_LV;            
           

mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_TX_LA].state =  MRP_REGISTRAR_LV;            
           

mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_REDECLARE].state =  MRP_REGISTRAR_LV;            
           
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_FLUSH].state =  MRP_REGISTRAR_MT;
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_FLUSH].action =  MRP_ACTION_LV;                                           
                                     
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_LEAVE_TIMER].state =  MRP_REGISTRAR_L3;
mrp_registrar_state_table[MRP_REGISTRAR_LV][MRP_EVENT_LEAVE_TIMER].action =  MRP_ACTION_START_LEAVE_TIMER;                 


/*3rd Part  */
mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_NEW].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_NEW].action =  MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER;            
                                                
mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_JOIN_IN].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_JOIN_IN].action =  MRP_ACTION_STOP_LEAVE_TIMER;            
                                      
                                      
mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_JOIN_MT].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_JOIN_MT].action =   MRP_ACTION_STOP_LEAVE_TIMER;            
                                      
                                      

mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_LV].state =  MRP_REGISTRAR_L3;                
            

mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_R_LA].state =  MRP_REGISTRAR_L3;                
            

mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_TX_LA].state =  MRP_REGISTRAR_L3;                
            

mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_REDECLARE].state =  MRP_REGISTRAR_L3;                
            

 mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_FLUSH].state =  MRP_REGISTRAR_MT;
 mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_FLUSH].action =  MRP_ACTION_LV;            
                                      
            
 mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_LEAVE_TIMER].state =  MRP_REGISTRAR_L2;
 mrp_registrar_state_table[MRP_REGISTRAR_L3][MRP_EVENT_LEAVE_TIMER].action =  MRP_ACTION_START_LEAVE_TIMER;     



mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_NEW].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_NEW].action =  MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER;
                                                
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_JOIN_IN].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_JOIN_IN].action =  MRP_ACTION_STOP_LEAVE_TIMER;                                       
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_JOIN_MT].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_JOIN_MT].action =  MRP_ACTION_STOP_LEAVE_TIMER;                                       

mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_LV].state =  MRP_REGISTRAR_L2;                
                

mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_R_LA].state =  MRP_REGISTRAR_L2;                
            

mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_TX_LA].state =  MRP_REGISTRAR_L2;                
            

mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_REDECLARE].state =  MRP_REGISTRAR_L2;                
            
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_FLUSH].state =  MRP_REGISTRAR_MT;
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_FLUSH].action =  MRP_ACTION_LV;                                        
                                      
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_LEAVE_TIMER].state =  MRP_REGISTRAR_L1;
mrp_registrar_state_table[MRP_REGISTRAR_L2][MRP_EVENT_LEAVE_TIMER].action =  MRP_ACTION_START_LEAVE_TIMER;              
    
 
/* part 5 */

mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_NEW].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_NEW].action =  MRP_ACTION_NEW | MRP_ACTION_STOP_LEAVE_TIMER;                                                
                                                
        
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_JOIN_IN].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_JOIN_IN].action =   MRP_ACTION_STOP_LEAVE_TIMER;                                          
                                      
 
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_JOIN_MT].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_JOIN_MT].action =   MRP_ACTION_STOP_LEAVE_TIMER;                                          
                                      
            
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_LV].state =  MRP_REGISTRAR_L1;                
            
           
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_R_LA].state =  MRP_REGISTRAR_L1;                
            
           
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_TX_LA].state =  MRP_REGISTRAR_L1;                
            
            
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_REDECLARE].state =  MRP_REGISTRAR_L1;                
            
            
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_FLUSH].state =  MRP_REGISTRAR_MT;
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_FLUSH].action =  MRP_ACTION_LV;                                          
                                      
           
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_LEAVE_TIMER].state =  MRP_REGISTRAR_MT;
mrp_registrar_state_table[MRP_REGISTRAR_L1][MRP_EVENT_LEAVE_TIMER].action =  MRP_ACTION_LV;    


/* Part 6 */



mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_NEW].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_NEW].action =  MRP_ACTION_NEW;                                     
                                   

mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_JOIN_IN].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_JOIN_IN].action =  MRP_ACTION_JOIN;                                     
                                   

mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_JOIN_MT].state =  MRP_REGISTRAR_IN;
mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_JOIN_MT].action =  MRP_ACTION_JOIN;                                     
                                   

mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_LV].state =  MRP_REGISTRAR_MT;            
         
 
mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_R_LA].state =  MRP_REGISTRAR_MT;             
         

mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_TX_LA].state =  MRP_REGISTRAR_MT;             
         
    
mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_REDECLARE].state =  MRP_REGISTRAR_MT;             
         
   
mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_FLUSH].state =  MRP_REGISTRAR_MT;             
         
        
mrp_registrar_state_table[MRP_REGISTRAR_MT][MRP_EVENT_LEAVE_TIMER].state =  MRP_REGISTRAR_MT;          
         

}




void init_leave_all_table()
{


 
mrp_leaveall_state_table[MRP_LEAVEALL_A][MRP_EVENT_TX].state = MRP_LEAVEALL_P;
mrp_leaveall_state_table[MRP_LEAVEALL_A][MRP_EVENT_TX].action =  MRP_ACTION_S_LA;                                          
                                         
 
mrp_leaveall_state_table[MRP_LEAVEALL_A][MRP_EVENT_R_LA].state = MRP_LEAVEALL_P;
mrp_leaveall_state_table[MRP_LEAVEALL_A][MRP_EVENT_R_LA].action =  MRP_ACTION_START_LEAVEALL_TIMER;                                          
                                         
mrp_leaveall_state_table[MRP_LEAVEALL_A][MRP_EVENT_LEAVEALL_TIMER].state = MRP_LEAVEALL_A;
mrp_leaveall_state_table[MRP_LEAVEALL_A][MRP_EVENT_LEAVEALL_TIMER].action =  MRP_ACTION_START_LEAVEALL_TIMER |  MRP_ACTION_REQ_TX;           
            
mrp_leaveall_state_table[MRP_LEAVEALL_P][MRP_EVENT_TX].state = MRP_LEAVEALL_P;                
                
mrp_leaveall_state_table[MRP_LEAVEALL_P][MRP_EVENT_R_LA].state = MRP_LEAVEALL_P;
mrp_leaveall_state_table[MRP_LEAVEALL_P][MRP_EVENT_R_LA].action =  MRP_ACTION_START_LEAVEALL_TIMER;                                          
                                         
mrp_leaveall_state_table[MRP_LEAVEALL_P][MRP_EVENT_LEAVEALL_TIMER].state = MRP_LEAVEALL_A;
mrp_leaveall_state_table[MRP_LEAVEALL_P][MRP_EVENT_LEAVEALL_TIMER].action =  MRP_ACTION_START_LEAVEALL_TIMER | MRP_ACTION_REQ_TX ;             

}


struct mrp_state_trans   mrp_periodic_state_table[MRP_PERIODIC_MAX][MRP_EVENT_MAX];


/* Periodic State Table (See 802.1ak-2007 Table 10.6) */

void init_periodic_state_table()
{

mrp_periodic_state_table[MRP_PERIODIC_A][MRP_EVENT_PERIODIC_ENABLED].state =  MRP_PERIODIC_A;
           
mrp_periodic_state_table[MRP_PERIODIC_A][MRP_EVENT_PERIODIC_DISABLED].state =  MRP_PERIODIC_P;            
           
mrp_periodic_state_table[MRP_PERIODIC_A][MRP_EVENT_PERIODIC_TIMER].state = MRP_LEAVEALL_A;
mrp_periodic_state_table[MRP_PERIODIC_A][MRP_EVENT_PERIODIC_TIMER].action = MRP_ACTION_PERIODIC | MRP_ACTION_START_PERIODIC_TIMER;
          
       
mrp_periodic_state_table[MRP_PERIODIC_P][MRP_EVENT_PERIODIC_ENABLED].state = MRP_PERIODIC_A;
mrp_periodic_state_table[MRP_PERIODIC_P][MRP_EVENT_PERIODIC_ENABLED].action = MRP_ACTION_START_PERIODIC_TIMER;                                           
                                           
       
mrp_periodic_state_table[MRP_PERIODIC_P][MRP_EVENT_PERIODIC_DISABLED].state =  MRP_PERIODIC_P;              
       
mrp_periodic_state_table[MRP_PERIODIC_P][MRP_EVENT_PERIODIC_TIMER].state =  MRP_PERIODIC_P;             
       
}
#endif

/*
 *  Perform the next task needed. But only for one port.
 *   This leaves CPU for all the other protocols and tasks.
 */

/* Hold on to the CPU and finish up any outstanding work tasks now */
static void finish_msrp_work(void)
{
   vtss_msrp_more_work();
}


static void configure_send_buffer(vtss_mrp_participant_t * part,unsigned char* addr, short etype) {
  mrp_ethernet_hdr* hdr = (mrp_ethernet_hdr *) &part->send_buf[0];
  memcpy(&hdr->dest_addr, addr, 6);
  memcpy(&hdr->src_addr,part->port->port_macaddr.macaddr,6);
  hdr->ethertype[0] = (etype >> 8);
  hdr->ethertype[1] = etype & 0xff; 
}




static void initialize_participant (vtss_mrp_participant_t *part)
{
    /* Join Timer is initialized */
    part->join_timer_running.running = 0;
    part->leave_timer_running.running = 0;

    /* Leavall Event is not recorded */
    part->leaveall = 0;
    /* Next attribute to process is from start*/
    part->next_to_process = 0;

    part->leaveall_state = MRP_LEAVEALL_P;

    /* Initialize PDU */
	configure_send_buffer(part,srp_proper_dest_mac,AVB_SRP_ETHERTYPE);

	/* Start the Leave All Timer */
	part->leaveall_timer.timeout = MRP_LEAVEALL_TIMER_PERIOD_CENTISECONDS;


	/* Prepare the Domain Vector */
	part->domain_attr_sent.attribute_type = MSRP_DOMAIN_VECTOR;
    part->domain_attr_sent.is_domain_attr = 1;
	part->domain_attr_sent.app_state = MRP_APPLICANT_VO;
	part->domain_attr_sent.attr_dir = MRP_DIR_DEC;


	/* Send the mad_join_req */

	msrp_mad_join_req(part,&part->domain_attr_sent,1);
	



}



static void msrp_sw_init(void)
{
    vtss_common_port_t pix;
    vtss_mrp_port_t  *pp;

    vtss_mrp_participant_t *participant = NULL;

    memset(MSRP, 0, sizeof(*MSRP));
    MSRP->initialized = VTSS_COMMON_BOOL_TRUE;

    MSRP->sys_config.ethertype = AVB_SRP_ETHERTYPE;
    memcpy(MSRP->sys_config.address,srp_proper_dest_mac,6);
    MSRP->sys_config.tagged = 1;
    MSRP->sys_config.enabled = 1;


    /* Initialize all (physical + virtual) ports */
    pp = &MSRP->ports[0];
    for (pix = 0; pix < VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS; pp++) {       
        pp->port_number = pix+1;
        pp->participants = &(MSRP->participants[pix]);
        MSRP->participants->port = pp;       
        pp->port_config.enable_msrp = VTSS_COMMON_BOOL_TRUE;
		initialize_participant(&(MSRP->participants[pix]));
		pix++;
    }

    /* Initialize all the participants */

    participant = &MSRP->participants[0];
#if 0
    for(pix = 0; pix < VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS; pix++)
    {
        initialize_participant(participant);
    }
#endif
}


static void msrp_hw_init(void)
{
    vtss_common_port_t pix;
    vtss_mrp_port_t *pp;
    vtss_common_linkstate_t linkup;

    pp = &MSRP->ports[0];
    for (pix = 1; pix <= VTSS_MSRP_MAX_PORTS; pix++, pp++) {
        VTSS_MSRP_ASSERT(pp->port_number == pix);
        //linkup = vtss_aggr_get_linkstate(pix);
        if(linkup)
            vtss_os_get_portmac(pix, &pp->port_macaddr);

        /* Get port address up front - unconditionally */
        vtss_os_get_portmac(pix, &pp->port_macaddr);

        vtss_os_set_fwdstate(pix, VTSS_COMMON_FWDSTATE_ENABLED);
        if (pp->port_config.enable_msrp)
        pp->is_enabled = VTSS_COMMON_BOOL_TRUE;
        else
        pp->is_enabled = VTSS_COMMON_BOOL_FALSE;

        
    }
}

void vtss_msrp_init(void)
{
    MSRP_INIT;
    msrp_hw_init();
}

void vtss_msrp_deinit(void)
{
    msrp_sw_init();
}

void vtss_msrp_set_config(const vtss_mrp_system_config_t VTSS_COMMON_PTR_ATTRIB *system_config)
{
    MSRP_INIT;
    if (memcmp(system_config, &MSRP->sys_config, sizeof(*system_config)) == 0)
        return;
    MSRP->sys_config = *system_config;
}

static void msrp_set_portconfig(struct vtss_mrp_port *pp, 
                                const vtss_msrp_port_config_t *port_config)
{
    if (memcmp(&pp->port_config, port_config, sizeof(pp->port_config)) == 0)
        return;
    finish_msrp_work();
    VTSS_MSRP_TRACE(VTSS_MSRP_TRLVL_DEBUG, ("Setting port %u config\n",
                    (unsigned)pp->port_number));
    pp->port_config = *port_config;
}



void vtss_msrp_set_portconfig(vtss_common_port_t portno,
                              const vtss_msrp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config)
{
    struct vtss_mrp_port *pp;

    VTSS_MSRP_ASSERT(portno == 0)
    MSRP_INIT;
    finish_msrp_work();
    if (portno == 0) {
        for (pp = &MSRP->ports[VTSS_MSRP_MAX_PORTS]; pp < &MSRP->ports[VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS]; pp++)
            msrp_set_portconfig(pp, port_config);
    }
    else {
        pp = &MSRP->ports[portno - 1];
        msrp_set_portconfig(pp, port_config);
    }
}



void vtss_msrp_get_config(vtss_mrp_system_config_t VTSS_COMMON_PTR_ATTRIB *system_config)
{
    MSRP_INIT;
    *system_config = MSRP->sys_config;
}

void vtss_msrp_get_portconfig(vtss_common_port_t portno,
                              vtss_msrp_port_config_t VTSS_COMMON_PTR_ATTRIB *port_config)
{
    VTSS_MSRP_ASSERT(portno == 0 || VTSS_MSRP_IS_PORT(portno));
    MSRP_INIT;
    *port_config = MSRP->ports[ portno - 1 ].port_config;
}


static int decode_attr_type(int atype) {
  switch (atype) 
  {
   case AVB_SRP_ATTRIBUTE_TYPE_TALKER_ADVERTISE:
        return MSRP_TALKER_ADVERTISE;
   case AVB_SRP_ATTRIBUTE_TYPE_TALKER_FAILED:
        return MSRP_TALKER_FAILED;
   case AVB_SRP_ATTRIBUTE_TYPE_LISTENER:
        return MSRP_LISTENER;
   case AVB_SRP_ATTRIBUTE_TYPE_DOMAIN:
        return MSRP_DOMAIN_VECTOR;   
  }  
  return -1;
}


void mrp_attribute_init(struct mad_machine *st,
                        mrp_attribute_type t,
                        int four_packed_event,
                        void *info)
{

  struct stream_info *pInfo = NULL;


  st->attribute_type = t;
  st->attribute_info = info;
  st->four_vector_parameter = four_packed_event;

  if(four_packed_event != 0)
  {
      st->is_four_vector = 1;
  }

  pInfo = (struct stream_info*)info;

  st->StreamID[0] = pInfo->id[0];
  st->StreamID[1] = pInfo->id[1];
  return;
}



struct mad_machine *find_matching_dec_attribute_by_stream_info(struct stream_info *info, struct vtss_mrp_participant *pParticipant,int attribute_type)
{

  int i = 0;
  attribute_type = attribute_type;
  for(i = 0; i < 24;i ++)
  {
        
      if((pParticipant->machines[i].StreamID[0] == info->id[0]) &&
         (pParticipant->machines[i].StreamID[1] == info->id[1]) &&
         (pParticipant->machines[i].attr_dir == MRP_DIR_DEC))
      {
          return &pParticipant->machines[i];
      }

  }

  return NULL;



}


struct mad_machine *find_attribute_by_existing_stream_info(struct stream_info*info,struct vtss_mrp_participant *pParticipant,int attribute_type)
{

  int i = 0;
  for(i = 0; i < 24;i ++)
  {
       if(&pParticipant->machines[i].attribute_type == attribute_type)
       {
          
           if(pParticipant->machines[i].StreamID[0] == info->id[0] &&
              pParticipant->machines[i].StreamID[1] == info->id[1] )

            return &pParticipant->machines[i];


 
       }

  }
  

}

struct stream_info * vtss_msrp_get_free_stream(void)
{
   int i = 0;

    while(i <= 9)
    {
      if(stream_info_vars[i].used != 1)
      {
          stream_info_vars[i].used = 1;
          return &stream_info_vars[i];
      }
      i++;
    }
   return NULL;
}

struct stream_info * vtss_msrp_find_stream(char *fv,int num)
{

   unsigned int high_val = 0;
   unsigned int low_val = 0;
   char *val = NULL;

   int i = 0;
   val = (char*)fv;
   for(i=0;i<8;i++)
   {
       if(i < 4)
       {
           high_val = (high_val << 8) + val[i];

       }
       else
       {
           low_val = (low_val << 8 ) + val[i];
       }
    }

    low_val = low_val+num;


    for(i = 0 ; i < 10 ; i++)
    {
       if(((stream_info_vars[i].id[0] == high_val) && stream_info_vars[i].id[1] == low_val))
       return &stream_info_vars[i];
       
    }
    return NULL;

}


struct mad_machine *get_attribute_app(struct vtss_mrp_participant *pParticipant)
{

  int i = 0;

  for(i=0;i<MRP_MAX_ATTRS;i++)
  {
    if (pParticipant->machines[i].used == 0)
    {      
        pParticipant->machines[i].app_state = MRP_APPLICANT_VO;
        pParticipant->machines[i].attr_dir = MRP_DIR_DEC;
        pParticipant->machines[i].used = 1;
        return &pParticipant->machines[i];
    }
  }
  return NULL;
}


struct mad_machine *get_attribute_reg(struct vtss_mrp_participant *pParticipant)
{
   int i = 0;
   for(i = 0 ; i < MRP_MAX_ATTRS;i++)
   {
       if(pParticipant->machines[i].used == 0)
       {
           pParticipant->machines[i].reg_state = MRP_REGISTRAR_MT;
           pParticipant->machines[i].attr_dir = MRP_DIR_REG;
           pParticipant->machines[i].used = 1;
           return &pParticipant->machines[i];
       }
   }
   return NULL;

}




struct stream_info *vtss_msrp_allocate_stream(int attr_type,char *fv,int num)
{


      unsigned int high_val = 0;
      unsigned int low_val = 0;
      char *val = NULL;
      int i = 0;
      struct stream_info *pStream = NULL;

      srp_talker_first_value *pTalker_ok = NULL;

      srp_talker_failed_first_value *pTalkerFail = NULL;
           


      val = (char*)fv;
     
       /* Get the Stream ID */

       for(i=0;i<8;i++)
       {
           if(i<4)
           {
               high_val = (high_val << 8) + val[i];
           }
           else
           {
               low_val = (low_val << 8 ) + val[i];
           }

       }

       low_val = low_val + num;

       /* Get Free Stream from Pool */

      pStream = vtss_msrp_get_free_stream();
  
      pStream->id[0] = high_val;
      pStream->id[1] = low_val;

      switch(attr_type)
      {

          case MSRP_TALKER_ADVERTISE:
          case MSRP_TALKER_FAILED:   
  


               /* copy the relevant data from stream into local stream structure */

               pTalker_ok = (srp_talker_first_value *) fv;


               pStream->vlan = UNAL_NET2HOSTS(pTalker_ok->VlanID);

               memcpy(pStream->addr,pTalker_ok->DestMacAddr,6);

               pStream->maxframesize = UNAL_NET2HOSTL(pTalker_ok->TSpecMaxFrameSize);
               pStream->maxintframes = UNAL_NET2HOSTL(pTalker_ok->TSpecMaxIntervalFrames);
               pStream->tspec        = pTalker_ok->TSpec;
               pStream->accumulated_latency = UNAL_NET2HOSTL(pTalker_ok->AccumulatedLatency);
               if(attr_type == MSRP_TALKER_FAILED)
               {
                   pTalkerFail = (srp_talker_failed_first_value*)fv;
                   memcpy(pStream->failed_bridge_id,pTalkerFail->FailureBridgeId,8);
                   pStream->failure_code = pTalkerFail->FailureCode;

               }
               

               /* Till here the stream is allocated */

          break;

           
      }
               
    return pStream;   



}


struct reservations_info *msrp_get_free_reservation()
{
   int i;
   for(i = 0 ;i < 30 ;i++)
   {
       if(reservations_info_ptr[i].used == 0)
       {
             memset(&reservations_info_ptr[i],0,sizeof(reservations_info));
             reservations_info_ptr[i].used = 1;
             return &reservations_info_ptr[i];
       }
   }


}


struct reservations_info *msrp_make_reservation(struct stream_info *pStreamInfo,struct vtss_mrp_participant *part,struct mad_machine *pMadMachine,int attr_type,int four_packed_event)
{
        
    struct reservations_info *pResvInfo = NULL;  
 


    pMadMachine = pMadMachine;


    pResvInfo = msrp_get_free_reservation();

    if(NULL != pResvInfo)
    {
        if(attr_type == MSRP_TALKER_ADVERTISE)
        {
            pResvInfo->pPort = part->port;

            pResvInfo->id[0] = pStreamInfo->id[0];
            pResvInfo->id[1] = pStreamInfo->id[1];

            pResvInfo->direction = 0;

            pResvInfo->decl_type = attr_type;
            pResvInfo->accumulated_latency = pStreamInfo->accumulated_latency;

            pResvInfo->streams_age = 0;            

         
        }
        else if(attr_type == MSRP_TALKER_FAILED)
        {
             
            pResvInfo->pPort = part->port;
  
            pResvInfo->id[0] = pStreamInfo->id[0];
            pResvInfo->id[1] = pStreamInfo->id[1];

            pResvInfo->direction = 0;

            pResvInfo->decl_type = attr_type;
            pResvInfo->accumulated_latency = pStreamInfo->accumulated_latency;
            

            //memcpy(pResvInfo->failed_bridge_id,pStreamInfo->failed_bridge_id,8);

            pResvInfo->failure_code = pStreamInfo->failure_code;

            pResvInfo->streams_age = 0;  
        }
        else
        {
            if(attr_type == MSRP_LISTENER)
            {
                pResvInfo->pPort = part->port;
  
                pResvInfo->id[0] = pStreamInfo->id[0];
                pResvInfo->id[1] = pStreamInfo->id[1];

                pResvInfo->direction = 1;

                pResvInfo->decl_type = attr_type;
                pResvInfo->four_packed_event = four_packed_event;
                pResvInfo->accumulated_latency = pStreamInfo->accumulated_latency;
            }

        }


    }
         
    return pResvInfo;
}


struct reservations_info *msrp_find_reservation_by_declaration(struct stream_info *pStreamInfo,struct vtss_mrp_port *port, int attr_sub_type)
{

   vars_uchar8 i;
   for(i = 0 ;i < 30 ;i++)
   {
       if(reservations_info_ptr[i].used == 1)
       {
             if((reservations_info_ptr[i].id[0] == pStreamInfo->id[0]) && (reservations_info_ptr[i].id[1] == pStreamInfo->id[1]) &&
                (reservations_info_ptr[i].decl_type == attr_sub_type) && (reservations_info_ptr[i].pPort == port))
             return &reservations_info_ptr[i];
       }
   }
   
   return NULL;   

}



int msrp_mad_join_ind(struct vtss_mrp_participant *part,struct mad_machine *pMadMachine,int new)
{

    struct reservations_info *pReservationsInfo = NULL;

    int attr_type = 0;

    struct stream_info *pStreamInfo = NULL;

    int four_packed_event = 0;

    attr_type = pMadMachine->attribute_type;    

    pStreamInfo = (struct stream_info*)(pMadMachine->attribute_info);

	new = new;



    if(pMadMachine->attribute_type == MSRP_DOMAIN_VECTOR)
    {
        /* If attribute is MSRP_DOMAIN_VECTOR
               * Add Port into VID membership: TODO
               */
        return 1;
    }


    if(NULL == pStreamInfo)
    {
        /* Log that Stream was not found in the attribute */
        return -1;
    }



    four_packed_event = pMadMachine->four_vector_parameter;
   

    /* Find the reservations if available or make some */    


    if(attr_type == MSRP_TALKER_ADVERTISE || MSRP_TALKER_FAILED)
    {
        pReservationsInfo = msrp_find_reservation_by_declaration(pStreamInfo,part->port,attr_type);

        if(NULL == pReservationsInfo)
        {
            pReservationsInfo = msrp_make_reservation(pStreamInfo,part,pMadMachine,attr_type,four_packed_event);                          

        }
              
    }
    else if(attr_type == MSRP_LISTENER)
    {
        pReservationsInfo = msrp_find_reservation_by_declaration(pStreamInfo,part->port,attr_type);

        if(NULL == pReservationsInfo)
        {
             pReservationsInfo = msrp_make_reservation(pStreamInfo,part,pMadMachine,attr_type,four_packed_event);                          
        }      

    }
    else
    {
        if(attr_type == MSRP_DOMAIN_VECTOR)
        {
           /* Check with local declaration and mark port as boundary port if necessary */
        }

    }


    if(NULL == pReservationsInfo)
    return -1;

    return 1;
   

}



int msrp_mad_leave_indication (struct vtss_mrp_participant *part,struct mad_machine *pMadMachine)
{
    
    int attr_type = 0;
    struct stream_info *pStreamInfo = NULL;

    struct reservations_info *pReservation = NULL;

    pStreamInfo = (struct stream_info*)pMadMachine->attribute_info;

    attr_type = pMadMachine->attribute_type;


    if(attr_type == MSRP_DOMAIN_VECTOR)
    {
        /* Remove the MSRP VLAN-ID from the Domain Vector TODO */

       return 1;

    }
    

    if((attr_type == MSRP_TALKER_ADVERTISE) || (attr_type == MSRP_TALKER_FAILED ))
    {

        /* Release the reservation of this attribute on this participant */

        pReservation =  msrp_find_reservation_by_declaration(pStreamInfo,part->port,attr_type);


    }
    else
    {
         if(attr_type == MSRP_LISTENER)
         {
             pReservation = msrp_find_reservation_by_declaration(pStreamInfo,part->port,pMadMachine->four_vector_parameter);
         }

    }
    
      
    if(NULL != pReservation)
    {

           /* Release the reservation */
       pReservation->used = 0;
    }

    return 1;

}


int msrp_mad_leave_propagation( struct vtss_mrp_participant *part,struct mad_machine *pMadMachine)
{

    /* Propagate to all ports except self */

    struct  vtss_mrp_port *pMSRPAppPorts = NULL; 
    /* Participant port */
    struct vtss_mrp_port *pSourcePort = NULL;

    struct vtss_mrp_port *pFirst = NULL;

    struct mad_machine *pAttribute = NULL;

    int i = 0;

    struct reservations_info *pReservation = NULL;

    struct stream_info *pStreamInfo = NULL;

    int attr_type = pMadMachine->attribute_type;


    pStreamInfo = (struct stream_info*)pMadMachine->attribute_info;


    if(attr_type == MSRP_DOMAIN_VECTOR)
    {

        /* Need not propagate domain vectors */
        return 1;
    }


    if(NULL == pStreamInfo)
    {
       /* Log that stream info is NULL or not available */
       return -1;
    }

     
    /* Process Talker advertise Propagations */

  
    pMSRPAppPorts = &MSRP->ports[0];
        
    pSourcePort = part->port;


   if(attr_type == MSRP_TALKER_ADVERTISE || MSRP_TALKER_FAILED)
   {

       /* Check if there are listener attributes on source port */

       pAttribute  = find_matching_dec_attribute_by_stream_info(pStreamInfo,pSourcePort->participants,attr_type);

       if(NULL != pAttribute)
       {
           msrp_mad_leave_req(pSourcePort->participants,pAttribute);
       }
           
   }



   for(i = 0 ; i < VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS;i++,pMSRPAppPorts++)
   {

       if(pSourcePort == pMSRPAppPorts)
       continue;


             
       pAttribute = find_matching_dec_attribute_by_stream_info(pStreamInfo,pMSRPAppPorts->participants,attr_type);

       if(NULL != pAttribute)
       {

           msrp_mad_leave_req(pMSRPAppPorts->participants,pAttribute);

       }

   }
   return 1;

}






int msrp_set_port_bandwidth(struct vtss_mrp_port *pPort,struct stream_info *pStreamInfo)
{
    /*Set the port bandwidth here */
    h2_set_max_frame(pPort->port_number,(vars_ushort16)pStreamInfo->maxframesize);
}



int msrp_check_port_bandwidth(struct vtss_mrp_port *pPort)
{
    pPort = pPort;
    return 1;

}


int msrp_map_join_propagate(struct vtss_mrp_participant *part,struct mad_machine *pMadMachine,int new)
{
    /* Propagate to all ports except self */

    struct vtss_mrp_port *pMSRPAppPorts = NULL; 
    /* Participant port */
    struct vtss_mrp_port *pSourcePort = NULL;

    struct vtss_mrp_port *pFirst = NULL;

    struct mad_machine *pAttribute = NULL;

    int i = 0;

    struct reservations_info *pReservation = NULL;

    struct stream_info *pStreamInfo = NULL;

    int attr_type = pMadMachine->attribute_type;


    pStreamInfo = (struct stream_info*)pMadMachine->attribute_info;

    if(attr_type == MSRP_DOMAIN_VECTOR)
    {

        return 1;
    }


    if(NULL == pStreamInfo)
    {
       /* Log that stream info is NULL or not available */
       return -1;
    }

     
    /* Process Talker advertise Propagations */

  
    pMSRPAppPorts = &MSRP->ports[0];
        
    pSourcePort = part->port;



    for(i = 0 ; i < VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS;i++,pMSRPAppPorts++)
    {

        if(pSourcePort == pMSRPAppPorts)
        continue;


        if(attr_type == MSRP_LISTENER)
        {

            pReservation = msrp_find_reservation_by_declaration(pStreamInfo,pMSRPAppPorts,MSRP_TALKER_ADVERTISE);
            if(NULL == pReservation)
            continue;
        }
        pAttribute = get_attribute_app(pMSRPAppPorts->participants);


        if(NULL == pAttribute)
        return -1;

        if(attr_type == MSRP_LISTENER)
        {
           msrp_set_port_bandwidth(pMSRPAppPorts,pStreamInfo);
        }


        
        if(attr_type == MSRP_TALKER_ADVERTISE)
        {
            /* For Each Port not including self, allocate declare attributes */


            if(msrp_check_port_bandwidth(pMSRPAppPorts) > 0)
            {
                mrp_attribute_init(pAttribute ,attr_type,0,pStreamInfo);
            }
            else
            {
                mrp_attribute_init(pAttribute,MSRP_TALKER_FAILED,0,pStreamInfo);

            }
             

        }
        else if(attr_type == MSRP_TALKER_FAILED)
        {

            mrp_attribute_init(pAttribute ,attr_type,0,pStreamInfo);

        }
        else
        {

            if(attr_type == MSRP_LISTENER)
            {
                mrp_attribute_init(pAttribute ,attr_type,pMadMachine->four_vector_parameter,pStreamInfo);
            }

        }
        msrp_mad_join_req(pMSRPAppPorts->participants,pAttribute,new);

      }
     
    return 0;
}


int msrp_mad_join_req(struct vtss_mrp_participant *part,struct mad_machine *pMadMachine,int new)
{
         
    mad_attr_event(part,pMadMachine,new ? MRP_EVENT_NEW:MRP_EVENT_JOIN);
    return 1;
}


int msrp_mad_leave_req(struct vtss_mrp_participant *part,struct mad_machine *pMadMachine)
{

    mad_attr_event(part,pMadMachine,MRP_EVENT_LV);
    return 1;
}


static int check_domain_merge(char *buf) {
	// We never both to merge domain attribute together
	buf = buf;
	return 0;
}

static int merge_domain_message(struct vtss_mrp_participant *p,char *buf,
                                struct mad_machine *st,
                                int vector)
{
  mrp_msg_header *mrp_hdr = (mrp_msg_header *) buf;
  mrp_vector_header *hdr = 
    (mrp_vector_header *) (buf + sizeof(mrp_msg_header));  
  int merge = 0;
  int num_values;


  if (mrp_hdr->AttributeType != AVB_SRP_ATTRIBUTE_TYPE_DOMAIN)
    return 0;


  num_values = hdr->NumberOfValuesLow;
                           
  if (num_values == 0) 
    merge = 1;
  else 
    merge = check_domain_merge(buf);

  if (merge) { 
    srp_domain_first_value *first_value = 
      (srp_domain_first_value *) (buf + sizeof(mrp_msg_header) + sizeof(mrp_vector_header));    

    first_value->SRclassID = AVB_SRP_SRCLASS_DEFAULT;
    first_value->SRclassPriority = AVB_SRP_TSPEC_PRIORITY_DEFAULT;
    first_value->SRclassVID[0] = (AVB_DEFAULT_VLAN>>8)&0xff;
    first_value->SRclassVID[1] = (AVB_DEFAULT_VLAN&0xff);

    mrp_encode_three_packed_event(p,buf, vector, st->attribute_type);    

    hdr->NumberOfValuesLow = num_values+1;
  }    
  
  return merge;
}


static int encode_attr_type(mrp_attribute_type attr)
{
  switch (attr) {
  case MSRP_TALKER_ADVERTISE:
    return AVB_SRP_ATTRIBUTE_TYPE_TALKER_ADVERTISE;
    break;
  case MSRP_TALKER_FAILED:
    return AVB_SRP_ATTRIBUTE_TYPE_TALKER_FAILED;
    break;
  case MSRP_LISTENER:
    return AVB_SRP_ATTRIBUTE_TYPE_LISTENER;
    break;
  case MSRP_DOMAIN_VECTOR:
    return AVB_SRP_ATTRIBUTE_TYPE_DOMAIN;
    break;
  default:
    return 0;
  }
}







static void create_empty_msg(struct vtss_mrp_participant *p,int attr, int leave_all) {
  mrp_msg_header *hdr = (mrp_msg_header *)p->send_ptr;  
  mrp_vector_header *vector_hdr = (mrp_vector_header *) (p->send_ptr + sizeof(mrp_msg_header));
  int hdr_length = sizeof(mrp_msg_header);
  int vector_length = 0;
  int first_value_length =  first_value_lengths[attr];
  int attr_list_length = first_value_length + sizeof(mrp_vector_header)  + vector_length + sizeof(mrp_footer);
  int msg_length = hdr_length + attr_list_length;

  // clear message
  memset((char *)hdr, 0, msg_length);

  // Set the relevant fields
  hdr->AttributeType = encode_attr_type(attr);
  hdr->AttributeLength = first_value_length;
  VTSS_COMMON_UNALIGNED_PUT_2B(hdr->AttributeListLength, attr_list_length);
  
  vector_hdr->LeaveAllEventNumberOfValuesHigh = leave_all << 5;
  vector_hdr->NumberOfValuesLow = 0;

  p->send_ptr += msg_length;
}


static int check_listener_merge(char *buf, 
                                struct stream_info *pStreamInfo)
{
  int i = 0;
  mrp_vector_header *hdr = (mrp_vector_header *) (buf + sizeof(mrp_msg_header));  
  int num_values = hdr->NumberOfValuesLow;
  unsigned int stream_id_high =0, stream_id_low =0;
  srp_listener_first_value *first_value = 
    (srp_listener_first_value *) (buf + sizeof(mrp_msg_header) + sizeof(mrp_vector_header));

  

  for (i=0;i<8;i++) {
    if( i < 4 )
    {
        stream_id_high = (stream_id_high << 8) + first_value->StreamId[i];
    }
    else
    {
        stream_id_low = (stream_id_low << 8) + first_value->StreamId[i];
    }
  }
  
  stream_id_low += num_values;



  if((stream_id_high == pStreamInfo->id[0]) && (stream_id_low == pStreamInfo->id[1]))
  return 1;
  
  return 0;

}



static int encode_four_packed(int event, int i, int vector)
{
  int j = 0;
  for (j=0;j<(3-i);j++)
    event *= 4;
  return (vector + event);
}


void mrp_encode_four_packed_event(struct vtss_mrp_participant *p,char *buf,
                                  int event,
                                  mrp_attribute_type attr)
{
  mrp_msg_header *hdr = (mrp_msg_header *) buf;  
  mrp_vector_header *vector_hdr = (mrp_vector_header *) (buf + sizeof(mrp_msg_header));  
  int num_values = vector_hdr->NumberOfValuesLow;
  int first_value_length =  first_value_lengths[attr];  
  char *vector = buf + sizeof(mrp_msg_header) + sizeof(mrp_vector_header) + first_value_length + (num_values+3)/3 + num_values/4 ;
  int shift_required = (num_values % 4 == 0);
  unsigned attr_list_length = attribute_length_length(hdr);



  if (shift_required)  {
    char *endmark;
    if (p->send_ptr - vector > 0) 
      mymemmove(vector+1, vector, p->send_ptr - vector);
    *vector = 0;
    attr_list_length++;
    p->send_ptr++;
    VTSS_COMMON_UNALIGNED_PUT_2B(hdr->AttributeListLength, attr_list_length);
    endmark = buf + sizeof(mrp_msg_header) + attr_list_length - 2;
    *endmark = 0;
    *(endmark+1) = 0;
  }
  
  *vector = encode_four_packed(event, num_values % 4, *vector);
  return;
}


static int merge_listener_message(struct vtss_mrp_participant *p,struct stream_info *pStreamInfo,char *buf,
                                  struct mad_machine *st,
                                  int vector)
{
  mrp_msg_header *mrp_hdr = (mrp_msg_header *) buf;
  mrp_vector_header *hdr = 
    (mrp_vector_header *) (buf + sizeof(mrp_msg_header));  
  int merge = 0;
  

  int num_values = 0;

  if (mrp_hdr->AttributeType != AVB_SRP_ATTRIBUTE_TYPE_LISTENER)
    return 0;

  num_values = hdr->NumberOfValuesLow;
                           
  if (num_values == 0) 
    merge = 1;
  else 
    merge = check_listener_merge(buf, pStreamInfo);


  if (merge) {
    srp_listener_first_value *first_value = 
      (srp_listener_first_value *) (buf + sizeof(mrp_msg_header) + sizeof(mrp_vector_header));
    

    if (num_values == 0) {
     
      first_value->StreamId[0] = (unsigned char) (pStreamInfo->id[0] << 0);
      first_value->StreamId[1] = (unsigned char) (pStreamInfo->id[0] << 8);
      first_value->StreamId[2] = (unsigned char) (pStreamInfo->id[0] << 16);
      first_value->StreamId[3] = (unsigned char) (pStreamInfo->id[0] << 24);
      first_value->StreamId[4] = (unsigned char) (pStreamInfo->id[1] << 0);
      first_value->StreamId[5] = (unsigned char) (pStreamInfo->id[1] << 8);
      first_value->StreamId[6] = (unsigned char) (pStreamInfo->id[1] << 16);
      first_value->StreamId[7] = (unsigned char) (pStreamInfo->id[1] << 24);
    }
    
    mrp_encode_three_packed_event(p,buf, vector, st->attribute_type);    
    mrp_encode_four_packed_event(p,buf, AVB_SRP_FOUR_PACKED_EVENT_READY, st->attribute_type);

    hdr->NumberOfValuesLow = num_values+1;    
    
  }

  return merge;
}






/* Check if we can merge the talker declarations */

static int check_talker_merge(struct stream_info *pStreamInfo, char *buf)
{
  mrp_vector_header *hdr = (mrp_vector_header *) (buf + sizeof(mrp_msg_header));  
  int num_values = hdr->NumberOfValuesLow;
  unsigned int my_dest_addr_high=0,my_dest_addr_low = 0;
  unsigned int my_streamid_addr_high,my_streamid_addr_low = 0;
  unsigned int dest_addr_high =0, dest_addr_low =0;
  unsigned int streamid_addr_high,streamid_addr_low = 0;
  int framesize=0, my_framesize=0;
  int vlan, my_vlan, i;
  srp_talker_first_value *first_value = 
    (srp_talker_first_value *) (buf + sizeof(mrp_msg_header) + sizeof(mrp_vector_header));

  // check if we can merge

  for (i=0;i<6;i++) {

    if(i < 4)
    {
        my_dest_addr_high = (my_dest_addr_high << 8) + pStreamInfo->addr[i];
        dest_addr_high = (dest_addr_high << 8) + first_value->DestMacAddr[i];
    }
    else
    {
        my_dest_addr_low = (my_dest_addr_low << 8) + pStreamInfo->addr[i];
        dest_addr_low = (dest_addr_low << 8) + first_value->DestMacAddr[i];
    }

  
  }
  
  dest_addr_low += num_values;

  if((dest_addr_high == my_dest_addr_high) && (dest_addr_low == my_dest_addr_low))
  return 1;


  // check if we can merge
  my_streamid_addr_high  = pStreamInfo->id[0];
  my_streamid_addr_low  =  pStreamInfo->id[1];

  for (i=0;i<8;i++) {
    if(i < 4)
    {
        streamid_addr_high = (streamid_addr_high << 8) + first_value->StreamId[i];
    }
    else
    {
        streamid_addr_low = (streamid_addr_low << 8) + first_value->StreamId[i];
    }
  }

  streamid_addr_low += num_values;
  /* To check if the stream id's are same */

  if((my_streamid_addr_high == streamid_addr_high) && 
      (my_streamid_addr_low == streamid_addr_low))
  return 1;
  
  

  vlan =    VTSS_COMMON_UNALIGNED_GET_2B(first_value->VlanID);
  my_vlan = pStreamInfo->vlan;

  if (vlan == my_vlan)
    return 1;
  
  my_framesize = pStreamInfo->maxframesize;


  framesize = VTSS_COMMON_UNALIGNED_GET_2B(first_value->TSpecMaxFrameSize);  

  if (framesize == my_framesize)
    return 1;
  
  
  return 0;

}



void * mymemmove(void * dest, const void * src, int n)
{
    char *s1 = (char *)dest;
    char *s2 = (char *)src;
    if( (s1 > s2) && (s1 < (s2 + n))) 
    {
        s1 += n-1;
        s2 += n-1;
        while( n-- )
        *s1-- = *s2--;
    } 
    else
    {
        while(n--)
        *s1++ = *s2++;
    }
    return dest;
} 


int encode_three_packed(int event, int i, int vector)
{
  int j;
  for (j=0;j<(2-i);j++)
    event *= 6;
  return (vector + event);
}

void mrp_encode_three_packed_event(struct vtss_mrp_participant *p,char *buf,
                                   int event,
                                   int attr)
{
  mrp_msg_header *hdr = (mrp_msg_header *) buf;  
  mrp_vector_header *vector_hdr = (mrp_vector_header *) (buf + sizeof(mrp_msg_header));  
  int num_values = vector_hdr->NumberOfValuesLow;
  int first_value_length =  first_value_lengths[attr];  
  char *vector = buf + sizeof(mrp_msg_header) + sizeof(mrp_vector_header) + first_value_length + num_values/3;
  int shift_required = (num_values % 3 == 0);
  unsigned attr_list_length = attribute_length_length(hdr);


  if (shift_required) {
    char *endmark;
    if (p->send_ptr - vector > 0) 
      mymemmove(vector+1, vector, p->send_ptr - vector);
    p->send_ptr++;
    *vector = 0;
    attr_list_length++;
    VTSS_COMMON_UNALIGNED_PUT_2B(hdr->AttributeListLength, attr_list_length);           
    endmark = buf + sizeof(mrp_msg_header) + attr_list_length - 2;
    *endmark = 0;
    *(endmark+1) = 0;
  }
  
  *vector = encode_three_packed(event, num_values % 3, *vector);
  return;
}




static int merge_talker_message(struct vtss_mrp_participant *p,struct stream_info *pStreamInfo,char *buf,
                                struct mad_machine *st,
                                int vector)
{
  mrp_msg_header *mrp_hdr = (mrp_msg_header *) buf;
  mrp_vector_header *hdr =  (mrp_vector_header *) (buf + sizeof(mrp_msg_header));  
  int merge = 0; 
  int num_values = 0;
  int accumulated_latency = 0;
  int i = 0;


  if (mrp_hdr->AttributeType != AVB_SRP_ATTRIBUTE_TYPE_TALKER_ADVERTISE)
    return 0;

  num_values = hdr->NumberOfValuesLow;
                           
  if (num_values == 0) 
    merge = 1;
  else 
    merge = check_talker_merge(pStreamInfo,buf);
  
  

  if (merge) {
    srp_talker_first_value *first_value = 
      (srp_talker_first_value *) (buf + sizeof(mrp_msg_header) + sizeof(mrp_vector_header));
    

    // The SRP layer
   
    if (num_values == 0) {
      for (i=0;i<6;i++) {
        first_value->DestMacAddr[i] = pStreamInfo->addr[i];
      }

      

      first_value->StreamId[0] = (unsigned char) (pStreamInfo->id[0] << 0);
      first_value->StreamId[1] = (unsigned char) (pStreamInfo->id[0] << 8);
      first_value->StreamId[2] = (unsigned char) (pStreamInfo->id[0] << 16);
      first_value->StreamId[3] = (unsigned char) (pStreamInfo->id[0] << 24);
      first_value->StreamId[4] = (unsigned char) (pStreamInfo->id[1] << 0);
      first_value->StreamId[5] = (unsigned char) (pStreamInfo->id[1] << 8);
      first_value->StreamId[6] = (unsigned char) (pStreamInfo->id[1] << 16);
      first_value->StreamId[7] = (unsigned char) (pStreamInfo->id[1] << 24);
      

#ifndef SRP_VERSION_5


      VTSS_COMMON_UNALIGNED_PUT_2B(first_value->VlanID,pStreamInfo->vlan);
      
#endif
 

      first_value->TSpec = pStreamInfo->tspec;
      
 

      VTSS_COMMON_UNALIGNED_PUT_2B(first_value->TSpecMaxFrameSize, pStreamInfo->maxframesize);
      VTSS_COMMON_UNALIGNED_PUT_2B(first_value->TSpecMaxIntervalFrames,pStreamInfo->maxintframes);
      accumulated_latency = pStreamInfo->accumulated_latency + 1;
      VTSS_COMMON_UNALIGNED_PUT_4B(first_value->AccumulatedLatency,accumulated_latency);
    }

    mrp_encode_three_packed_event(p,buf, vector, st->attribute_type);    

    hdr->NumberOfValuesLow = num_values+1;

  }

  return merge;   
}




int avb_srp_merge_message(struct vtss_mrp_participant *p,struct stream_info *pStreamInfo,char *buf,struct mad_machine *st,int vector)
{
  switch (st->attribute_type) {
  case MSRP_TALKER_ADVERTISE:
    return merge_talker_message(p,pStreamInfo,buf, st, vector);
    break;
  case MSRP_LISTENER:
    return merge_listener_message(p,pStreamInfo,buf, st, vector);
    break;
  case MSRP_DOMAIN_VECTOR:
    return merge_domain_message(p,buf, st, vector);
    break;

  default:
    break;
  }
  return 0;
}




static int merge_msg(struct vtss_mrp_participant *p,struct stream_info *pStreamInfo,char *msg, struct mad_machine* st, int vector)
{
  switch (st->attribute_type) 
    {
    case MSRP_TALKER_ADVERTISE: 
    case MSRP_TALKER_FAILED: 
    case MSRP_LISTENER:
    case MSRP_DOMAIN_VECTOR:             
      return avb_srp_merge_message(p,pStreamInfo,msg, st, vector);
      break;

  }
  return 0;
}

static int send(struct vtss_mrp_participant *p)
{
  // Send only when the buffer is full
  if (p->send_buf + MRP_SEND_BUFFER_SIZE < p->send_ptr + MAX_MRP_MSG_SIZE + sizeof(mrp_footer)) {
    return -1;
  }
}

unsigned attribute_length_length(mrp_msg_header* hdr)
{
    return VTSS_COMMON_UNALIGNED_GET_2B(hdr->AttributeListLength);
}


int doTx(struct vtss_mrp_participant *p,struct mad_machine *st,int vector)
{  
  int merged = 0;
  char *msg = &(p->send_buf[0])+sizeof(mrp_ethernet_hdr)+sizeof(mrp_header);
  char *end = p->send_ptr;

  while (!merged &&
         msg < end && 
         (*msg != 0 || *(msg+1) != 0)) {      
    mrp_msg_header *hdr = (mrp_msg_header *) &msg[0];

    merged = merge_msg(p,(struct stream_info*)(st->attribute_info),msg, st, vector);
    
    msg = msg + sizeof(mrp_msg_header) + attribute_length_length(hdr);
  }   

  if (!merged) {
    create_empty_msg(p,st->attribute_type, 0);
    (void) merge_msg(p,(struct stream_info*)(st->attribute_info),msg, st, vector);
  }

  send(p);
}


int mrp_tx_rate_exceeded(struct vtss_mrp_participant *p)
{
    p = p;
    return 0;
}



static int mrp_tx_opportunity(struct vtss_mrp_participant *p)
{
    if (p->join_timer_running.running == 1) {
        if ((p->join_timer_running.timeout == 0)) {
            p->join_timer_running.running = 0;
            return p->port->point_to_point ? !mrp_tx_rate_exceeded:1;
        }
    } else {
        if (!p->port->point_to_point) {
            p->join_timer_running.timeout = MRP_JOINTIMER_PERIOD_CENTISECONDS; 
            p->join_timer_running.running = 1;
        }
    }
    return 0;
}

static void mrp_req_tx_opportunity(struct vtss_mrp_participant *p)
{
    if (p->join_timer_running.running == 0) {
        p->join_timer_running.timeout = MRP_JOINTIMER_PERIOD_CENTISECONDS; 
        p->join_timer_running.running = 1;
    }
}



int mad_attr_event(struct vtss_mrp_participant *p,struct mad_machine *machine,enum mrp_event event)
{ 
    vars_uchar8 state, action;
    int is_new = 0;     //  is_new = 1 for new attribute declarations
    int in;             //  in = 1 when the registrar is in IN state
    struct vtss_mrp_port *port = p->port;
    struct vtss_mrp_application *app = port->app;
    
   
    
//registrar:

    /* Fixed    : Registrar ignores all MRP messages, and remains IN
       Forbidden: Registrar ignores all MRP messages, and remains MT */
    if (machine->attr_dir == MRP_DIR_DEC)
        goto applicant;

    /* Update registrar state _before_ computing applicant state */
    state  = mrp_registrar_state_table[machine->reg_state][event].state;
    if (state == MRP_REGISTRAR_INVALID)
        goto applicant;

    action = mrp_registrar_state_table[machine->reg_state][event].action;
    switch (action & MRP_ACTION_MASK) {
    case MRP_ACTION_NEW:
        is_new = 1;
        /* fall through */
    case MRP_ACTION_JOIN:
        if (msrp_mad_join_ind(p,machine,is_new) < 0) {
            port->reg_failures++;
            break;
        }
        msrp_map_join_propagate(p,machine,is_new);
        break;
    case MRP_ACTION_LV:
        msrp_mad_leave_indication(p,machine);
        msrp_mad_leave_propagation(p, machine);
        break;
    }

    switch (action & MRP_TIMER_ACTION_MASK) {
    case MRP_ACTION_START_LEAVE_TIMER:
        if (0 == p->leave_timer_running.running) {
            p->leaveall_timer.timeout = MRP_LEAVETIMER_PERIOD_CENTISECONDS;
            p->leave_timer_running.running = 1;
        }
        break;
    case MRP_ACTION_STOP_LEAVE_TIMER:;
        // leave timer handled at participant level
        break;
    }

    machine->reg_state = state;

applicant:
    in = (machine->reg_state == MRP_REGISTRAR_IN);

    /* applicant state machine */
    state  = mrp_applicant_state_table[machine->app_state][event].state;
    if (state == MRP_APPLICANT_INVALID)
        return 0;

    action = mrp_applicant_state_table[machine->app_state][event].action;

    /* some state transitions depend on additional conditions (as defined in
       notes attached to the applicant state table in 802.1ak) */
    switch (machine->app_state) {
    case MRP_APPLICANT_VO:
    case MRP_APPLICANT_VP:
        /* Ignore transitions to states AO and AP when operPointToPointMAC
           is TRUE (802.1ak 2007 Table 10.3 Note 3) */
        if ((event == MRP_EVENT_R_JOIN_IN) && (port->point_to_point))
            return 0;
        break;
    case MRP_APPLICANT_AA:
        /* Transition from AA to QA due to rIn! is Ignored (no transition) if
           operPointToPointMAC is FALSE (802.1ak 2007 Table 10.3 Note 5)*/
        if ((event == MRP_EVENT_R_IN) && (!port->point_to_point))
            return 0;
        break;
    case MRP_APPLICANT_AN:
        /* Upon tx! event, transition from AN to QA if the Registrar
           is IN, and AA otherwise (802.1ak 2007 Table 10.3 Note 8) */
        if ((event == MRP_EVENT_TX) && !in) {
            state = MRP_APPLICANT_AA;
            action |= MRP_ACTION_REQ_TX;
        }
        break;
    default:;
    }

    /* The Applicant Administrative Control, determines whether or not the
       Applicant state machine participates in MRP protocol exchanges.*/
    if (machine->app_mgt == MRP_NON_PARTICIPANT)
        goto non_participant;

//normal_participant:

    /* Note 7: If the PDU is full, state remains unchanged. This way the
       message will be sent in the next tx opportunity (which is requested) */

    /* If Registration Fixed or Forbidden, In and JoinIn messages are
       sent rather than Empty or JoinEmpty messages */
    in |= (machine->reg_mgt != MRP_NORMAL_REGISTRATION);
    switch (action & MRP_ACTION_MASK) {
    case MRP_ACTION_S_JOIN:
           if (doTx(p,machine,in ? MRP_JOIN_IN:MRP_JOIN_MT) < 0)
           {
             mrp_req_tx_opportunity(p);
             return -1;
           }
           break;
           
    case MRP_ACTION_S_NEW:
           if (doTx(p,machine,MRP_NEW) < 0)
           {
               mrp_req_tx_opportunity(p);
               return -1;
           }
        break;
    case MRP_ACTION_S_LV:
           if (doTx(p,machine,MRP_LV) < 0)
           {
               mrp_req_tx_opportunity(p);
               return -1; 
           }
        break;
    case MRP_ACTION_S:
           if (doTx(p,machine,in ? MRP_IN:MRP_MT) < 0)
           {
              mrp_req_tx_opportunity(p);
              return -1;
           }
        break;
    }

    /* 802.1ak 2007 Table 10.3 Note 6.- Request opportunity to transmit on entry
       to VN, AN, AA, LA, VP, AP, and LO states. */
    if (action & MRP_REQ_TX_MASK)
        mrp_req_tx_opportunity(p);

non_participant:
    machine->app_state = state;
    return 0;
}



static int has_fourpacked_events(int attr) {
  return (attr == MSRP_LISTENER) ? 1 : 0;
}

static int decode_fourpacked(int vector, int i)
{
  int j ;
  for (j=0;j<(3-i);j++)
    vector /= 4;
  return (vector % 4);
}


static int decode_threepacked(int vector, int i)
{
  int j;
  for (j=0;j<(2-i);j++)
    vector /= 6;
  return (vector % 6);
}




void vtss_msrp_receive(vtss_common_port_t from_port,
              const vtss_common_octet_t VTSS_COMMON_BUFMEM_ATTRIB *rxframe,
               vtss_common_framelen_t len)
{

  /* Storage for the port Participant */

  struct vtss_mrp_participant *part = NULL;

  struct stream_info *pStreamInfo = NULL;

  struct mad_machine *pAttribute = NULL;

  struct srp_domain_first_value *first_value_domain = NULL;

  char *end = (char *) &rxframe[0] + len;
  char *msg = (char *) &rxframe[0] + sizeof(mrp_header);

  int i = 0;

  struct vtss_mrp_port *pPort = NULL;

  pPort = &MSRP->ports[from_port];

  part = pPort->participants;

  while (msg < end && (msg[0]!=0 || msg[1]!=0))
  {
    mrp_msg_header *hdr = (mrp_msg_header *) &msg[0];     

    unsigned first_value_len = hdr->AttributeLength;
    int attr_type = decode_attr_type(hdr->AttributeType);
    if (attr_type==-1) return;
    

    msg = msg + sizeof(mrp_msg_header);

    
    
    while (msg < end && (msg[0]!=0 || msg[1]!=0))
    {
      mrp_vector_header *vector_hdr = (mrp_vector_header *) msg;
      char *first_value = msg + sizeof(mrp_vector_header);
      int numvalues = 
        ((vector_hdr->LeaveAllEventNumberOfValuesHigh & 0x1f)<<8) +
        (vector_hdr->NumberOfValuesLow);
      int leave_all = (vector_hdr->LeaveAllEventNumberOfValuesHigh & 0xe0)>>5;
      int threepacked_len = (numvalues+2)/3;
      int fourpacked_len = has_fourpacked_events(attr_type)?(numvalues+3)/4:0;
      int len = sizeof(mrp_vector_header) + first_value_len + threepacked_len + fourpacked_len;
      
      // Check to see that it isn't asking us to overrun the buffer
      if (msg + len > end) return;


      
      if (leave_all)
      {
          mad_attrtype_event(part, attr_type, MRP_EVENT_R_LA);
          mad_participant_event(part, MRP_EVENT_R_LA);
      }
      
      for (i=0;i<numvalues;i++)
      {

          // Get the three packed data out of the vector
          int three_packed_event = decode_threepacked(*(first_value + first_value_len + i/3), i%3);

          // Get the four packed data out of the vector
          int four_packed_event = has_fourpacked_events(attr_type) ?
          decode_fourpacked(*(first_value + first_value_len + threepacked_len + i/4),i%4) : 0;

          // This allows various modules to snoop on the individual message
          // process(attr_type, first_value, i, three_packed_event, four_packed_event);



           /* Handle Domain Advertise here directly */
           if(attr_type == MSRP_DOMAIN_VECTOR)
           {
               /* Copy the values from first value into the attribute */
               first_value_domain = (srp_domain_first_value *)first_value;
               part->domain_attr_recv.is_domain_attr = 1;
               part->domain_attr_recv.SRclassID = first_value_domain->SRclassID;
               part->domain_attr_recv.SRclassPriority = first_value_domain->SRclassPriority;
               part->domain_attr_recv.SRclassVID[0] = first_value_domain->SRclassVID[0];
               part->domain_attr_recv.SRclassVID[1] = first_value_domain->SRclassVID[1];
               part->domain_attr_recv.reg_state = MRP_REGISTRAR_MT;
               part->domain_attr_recv.attr_dir = MRP_DIR_REG;
               mad_attr_event(part,pAttribute,three_packed_event);
               continue;    
           }


           pStreamInfo = vtss_msrp_find_stream(first_value,i);
  
           if(NULL != pStreamInfo) /* Existing Stream */
           {
                /* Get a Stream from pool and add values into it */

                /* Find the attributes of streams to be initialized and execute state machines on them  */

               pAttribute = find_attribute_by_existing_stream_info(pStreamInfo,part,attr_type);
               
               if(NULL == pAttribute)
               {
                 /* Log here that no such attribute was been found */
                 continue;
               }
            
           }
           else
           {
               pStreamInfo = vtss_msrp_allocate_stream(attr_type,first_value,i);

               if(NULL == pStreamInfo)
               {
                    /* log that we have exceeded registration limits and continue */
                    continue;
               } 
               else 
               {
                   /* Allocate registrant attributes for StreamID */
                    pAttribute = get_attribute_reg(pPort->participants);
                    mrp_attribute_init(pAttribute,attr_type,four_packed_event,pStreamInfo);
               }

            }
            /* Execute the MRP IN State Machines for the attributes just received */
            mad_attr_event(part,pAttribute,three_packed_event);

        }            
      
      msg = msg + len;
      }
    msg += 2;  
    }
    
  
  return;
}



void vtss_msrp_tick(void)
{

    struct vtss_mrp_port *pp;

    finish_msrp_work();

    for (pp = &MSRP->ports[0]; pp < &MSRP->ports[VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS]; pp++)
    {



        if(pp->participants->join_timer_running.running == 1)
        {
            if(pp->participants->join_timer_running.timeout > 0)
            pp->participants->join_timer_running.timeout--;

           
        }

        if(pp->participants->leave_timer_running.running == 1)
        {
            if(pp->participants->leave_timer_running.timeout > 0)
            pp->participants->leave_timer_running.timeout--;

            if(pp->participants->leave_timer_running.timeout == 0)
            pp->participants->leave_timer_running.running = 0;
        }
        

        if(pp->participants->leaveall_timer.running == 1)
        {
            if(pp->participants->leaveall_timer.timeout > 0)
            pp->participants->leaveall_timer.timeout--;

            if(pp->participants->leaveall_timer.timeout == 0)
            pp->participants->leaveall_timer.running = 0;
            
        }


     }

    

}



void mad_participant_event(struct vtss_mrp_participant *p, enum mrp_event event)
{
    vars_uchar8 state;
    vars_uchar8 action;

    
    state = mrp_leaveall_state_table[p->leaveall_state][event].state;
    if (state == MRP_LEAVEALL_INVALID)
        return;

    action = mrp_leaveall_state_table[p->leaveall_state][event].action;
    switch (action & MRP_ACTION_MASK) {
    case MRP_ACTION_S_LA:
        /* The LeaveAll event value specified by the transmission action is
           encoded in the NumberOfValues field of the VectorAttribute as
           specified. The sLA action also gives rise to a rLA! event against all
           instances of the Applicant state machine, or the Registrar state
           machine, associated with the MRP participant. */
        p->leaveall = 1; /* signal that the leaveall event ocurred */
        mad_event(p, MRP_EVENT_R_LA);
    }
    switch (action & MRP_TIMER_ACTION_MASK) {
    case MRP_ACTION_START_LEAVEALL_TIMER:
        /* 'The Leave All Period Timer is set to a random value in the
           range LeaveAllTime < T < 1.5*LeaveAllTime when it is started' */
        p->leaveall_timer.running = 1;
        p->leaveall_timer.timeout = MRP_LEAVEALL_TIMER_PERIOD_CENTISECONDS;
    }

    if (action & MRP_REQ_TX_MASK)
        mrp_req_tx_opportunity(p);


    p->leaveall_state = state;
}


int mrp_pdu_full(struct vtss_mrp_participant *p)
{

    if ((p->send_buf + MRP_SEND_BUFFER_SIZE) < (p->send_ptr + MAX_MRP_MSG_SIZE + sizeof(mrp_footer)))
    {
        return 1;
    }
    else
    {
        return 0;
    }

}
static int mad_machine_active(struct vtss_mrp_participant *p, struct mad_machine *pAttribute)
{
    p = p;
    if(pAttribute->attr_dir == MRP_DIR_REG)
    {
        if(pAttribute->reg_state != MRP_MT && pAttribute->used != 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if(pAttribute->attr_dir == MRP_DIR_DEC)
    {
        if((pAttribute->app_state != MRP_APPLICANT_VO) && (pAttribute->used != 0))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;    
    }
 
}


void mad_attrtype_event(struct vtss_mrp_participant *p,vars_uchar8 attrtype,enum mrp_event event)
{
    struct mad_machine *pMachine = NULL;
    
    int i;

    for (i = p->next_to_process ; i < 24 ; i++) {
        pMachine = &p->machines[i];
        if (mad_machine_active(p, pMachine)&& (pMachine->attribute_type == attrtype)) {
            if (mad_attr_event(p, pMachine, event) < 0) {
                /* PDU is full */
                p->next_to_process = i;
                return;
            }
        }
    }
}


static void mad_event(struct vtss_mrp_participant *p, enum mrp_event event)
{
    int i = 0;
    struct mad_machine *pMachine = NULL;

    /* Handle attr event considering a circular state machine list */
    for (i = p->next_to_process ; i < 24 ; i++) {
        pMachine = &p->machines[i];
        if (mad_machine_active(p, pMachine)) {
            if (mad_attr_event(p, pMachine, event) < 0) {
                /* PDU is full */
                p->next_to_process = i;
                return;
            }
        }
    }
}


int mrp_pdu_empty(struct vtss_mrp_participant *p)
{
  char *msg = &(p->send_buf[0])+sizeof(mrp_ethernet_hdr)+sizeof(mrp_header);
  char *end = p->send_ptr;

  if(end == msg)
  return 1;


  return 0;
  
}

int mrp_pdu_check_send(int port_no, char *buf, int len)
{

    buf[len++] = 0x55;
    buf[len++] = 0x55;
    buf[len++] = 0x55;
    buf[len++] = 0x55;
    h2_send_frame(port2int(port_no),buf,len);



}

int mrp_pdu_ex_send(struct vtss_mrp_participant *part)
{

  char *buf = &part->send_buf[0];
  char *ptr = part->send_ptr;
  char *end = NULL;
  char *p = NULL;

  // Strip out attribute length fields for MMRP and MVRP
 //  strip_attribute_list_length_fields();

  if (ptr != buf+sizeof(mrp_ethernet_hdr)+sizeof(mrp_header)) {

   // Check that the buffer is long enough for a valid ethernet packet
    char *end = ptr + 4;
    if (end < buf + 64) end = buf + 64;

    // Pad with zero if necessary
    for (p = ptr;p<end;p++) *p = 0;

    // Transmit
     mrp_pdu_check_send(part->port->port_number,buf, ((end - buf)));
  }
  part->send_ptr = buf+sizeof(mrp_ethernet_hdr)+sizeof(mrp_header);

  return 0;

}


static void mrp_tx(struct vtss_mrp_participant *p)
{
    enum mrp_event event;

    /* Trigger per-participant tx! event */
    mad_participant_event(p, MRP_EVENT_TX);

    /* Determine the type of transmission event */
    event = p->leaveall ?
        (mrp_pdu_full(p) ? MRP_EVENT_TX_LAF:MRP_EVENT_TX_LA):
        MRP_EVENT_TX;

    /* Trigger per-attribute tx! event */
    mad_event(p, event);

    if (!mrp_pdu_empty(p)) {
        /* Send MRPDU */
        mrp_pdu_ex_send(p);
        
    }
}


void vtss_msrp_more_work(void)
{
    /* Check the State of the timers for participants and then schedule the events for it */
    struct vtss_mrp_port *pp;
    
    for (pp = &MSRP->ports[0]; pp < &MSRP->ports[VTSS_MSRP_MAX_PORTS + VTSS_MSRP_MAX_APORTS]; pp++)
    {
        if(pp->participants->leave_timer_running.running == 1)
        {
            if(pp->participants->leave_timer_running.timeout == 0)
            {
                pp->participants->leave_timer_running.running = 0;
                mad_event(pp->participants, MRP_EVENT_LEAVE_TIMER);
            }
        }

        if(pp->participants->leaveall_timer.running  == 1)
        {
             if(pp->participants->leaveall_timer.timeout == 0)
             {
                 mad_participant_event(pp->participants, MRP_EVENT_LEAVEALL_TIMER); 
             }
        }

        if(mrp_tx_opportunity(pp->participants))
         mrp_tx(pp->participants);

    }


}

#endif

