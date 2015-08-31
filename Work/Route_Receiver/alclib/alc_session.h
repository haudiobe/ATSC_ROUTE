/** \file alc_session.h \brief ALC session
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.94 $
 *
 *  MAD-ALCLIB: Implementation of ALC/LCT protocols, Compact No-Code FEC,
 *  Simple XOR FEC, Reed-Solomon FEC, and RLC Congestion Control protocol.
 *  Copyright (c) 2003-2007 TUT - Tampere University of Technology
 *  main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  In addition, as a special exception, TUT - Tampere University of Technology
 *  gives permission to link the code of this program with the OpenSSL library (or
 *  with modified versions of OpenSSL that use the same license as OpenSSL), and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify this file, you may extend this exception to your version
 *  of the file, but you are not obligated to do so. If you do not wish to do so,
 *  delete this exception statement from your version.
 */

#ifndef _ALC_SESSION_H_
#define _ALC_SESSION_H_

#ifdef _MSC_VER
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "defines.h"
#include "mad.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ALC session states.
 * @enum alc_session_states
 */

enum alc_session_states {
	SError = -1,	/**< error in the session */  
	SActive,		/**< session is active */ 
	SExiting,		/**< ctrl^c pressed */ 
	STxStopped,		/**< sender has stopped sending and receiving lists are empty */ 
	SClosed,		/**< session is closed */ 
	SAFlagReceived,	/**< A flag is received */
	SPaused			/**< session is paused */
};

/**
 * Information for object which is wanted to receive.
 * @struct wanted_obj
 */

typedef struct wanted_obj {
	
  struct wanted_obj *prev;				/**< previous item */
  struct wanted_obj *next;				/**< next item */
  
  unsigned long long toi;				/**< transport object identifier */
  unsigned long long transfer_len;		/**< transfer length */
  
  unsigned short es_len;				/**< encoding symbol length */
  unsigned int max_sb_len;				/**< maximum source block length */
  int fec_inst_id;						/**< FEC instance id */
  short fec_enc_id;						/**< FEC encoding id */
  unsigned short max_nb_of_es;			/**< maximum number of encoding symbols*/
  unsigned char content_enc_algo;		/**< content encoding algorithm */
  
  unsigned char finite_field;			/**< finite field parameter with new RS FEC */
  unsigned char nb_of_es_per_group;		/**< number of encoding symbols in packet with new RS FEC */  
} wanted_obj_t;
   
/**
 * Structure which stores information for received FDT instance.
 * @struct rx_fdt_instance
 */

typedef struct rx_fdt_instance {
        
    struct rx_fdt_instance *prev;		/**< previous item */
    struct rx_fdt_instance *next;		/**< next item */
    unsigned int fdt_instance_id;		/**< FDT instance id*/
} rx_fdt_instance_t;

/**
 * Structure for the FLUTE packet when tx_thread() function is used.
 * @struct tx_queue_struct
 */

typedef struct tx_queue_struct {
	unsigned char *data;				/**< data to be sent */
	unsigned int datalen;				/**< length of data to be sent */
	unsigned int nb_tx_ch;				/**< number of channel where data is sent */
	struct tx_queue_struct *next;		/**< next item */
} tx_queue_t;

/**
 * Structure for the ALC session.
 * @struct alc_session
 */

typedef struct alc_session {
  int s_id;								/**< ALC session identifier */
  int mode;								/**< mode for the ALC session (sender or receiver) */
  
  unsigned long long tsi;				/**< transport session identifier */
  unsigned long long starttime;			/**< start time of the session */
  unsigned long long stoptime;			/**< stop time of the session */
  double ftimestarttime;
  
  struct alc_channel *ch_list[MAX_CHANNELS_IN_SESSION];	/**< channels in the session */
  int nb_channel;						/**< number of channels in the session */
  int max_channel;				       /**< number of maximum channels in the session */
  
  int nb_sending_channel;			/**< number of sending channels */
  int nb_ready_channel;				/**< number of ready channels */
  
  enum alc_session_states state;	/**< state of the session */
  
  int addr_family;					/**< address family */
  int addr_type;					/**< address type, multicast (0) or unicast (1) */
  
  int fdt_instance_id;				/**< current FDT instance */
  
  struct mad_rlc *rlc;				/**< RLC congestion control */
    
  int def_ttl;						/**< time to live */
  int def_tx_rate;					/**< transmission rate in kbit/s */
  unsigned short def_eslen;			/**< encoding symbol length */
  unsigned int def_mxnbes;			/**< maximum number of encoding symbols
									that can be generated from one source block */
  unsigned int def_max_sblen;		/**< maximum source block length */
  BOOL simul_losses;				/**< simulate packet losses, TRUE = yes, FALSE = no */
  double loss_ratio1;				/**< packet loss ratio one */
  double loss_ratio2;				/**< packet loss ratio two */
  int def_fec_ratio;				/**< FEC ratio percent */
  
  unsigned char def_fec_enc_id;		/**< FEC encoding id */
  unsigned short def_fec_inst_id;	/**< FEC instance id  */
  int cc_id;						/**< used congestion control, 0 = NULL, 1 = RLC */
  int use_fec_oti_ext_hdr;			/**< use FEC OTI extension header */
  
  int rx_memory_mode;				/**< used memory mode in the receiver */
  int verbosity;					/**< verbosity level */
  
  unsigned long long sent_bytes;		/**< bytes sent in the session */
  unsigned long long obj_sent_bytes;	/**< bytes sent for object */
  unsigned long long obj_start_time;	/**< start time for the transport object which sender is currently sending */
  unsigned long long tx_toi;			/**< transport object which sender is currently sending */
  
  double last_print_tx_percent;		/**< last printed transmission percent */
  
  int a_flag;						/**< send A flag in the session */

  tx_queue_t *tx_queue_begin;		/**< first item in transmission queue */
  tx_queue_t *tx_queue_end;			/**< last item in transmission queue */
  unsigned int tx_queue_size;		/**< size of transmission queue */
  
  BOOL first_unit_in_loop;			/**< first unit in the session sent to be sent? */
  int encode_content;				/**< encode content, 0 = no, 1 = ZLIB FDT, 2 = ZLIB FDT and GZIP files, 3 = PAD files */
  BOOL half_word;					/**< use half word flag */
  BOOL optimize_tx_rate;			/**< optimize transmission rate (use more CPU) */
  BOOL calculate_session_size;      /**< TRUE if the transmission is simulated and the sent_bytes is calculated */
  
#ifdef SSM
  BOOL ssm;							/**< use source specific multicast */
#endif

  char base_dir[MAX_PATH_LENGTH];			/**< Base directory for downloaded/sent files */
  char src_addr[40];						/**< source address for the session in the receiver */
	
  unsigned int rx_objs;						/**< number of objects received in this session */
  struct trans_obj *obj_list;				/**< pointer to first object */
  struct trans_obj *fdt_list;				/**< pointer to first FDT instance */
  wanted_obj_t *wanted_obj_list;			/**< pointer to first wanted object */ 
  rx_fdt_instance_t *rx_fdt_instance_list;	/**< pointer to first FDT instance information structure */

#ifdef _MSC_VER
  HANDLE handle_rx_thread;					/**< handle to thread which receives packets from the session */
  unsigned int rx_thread_id;				/**< identifier for thread which receives packets from the session */
  HANDLE handle_tx_thread;					/**< handle to thread which sents packets to the session */
  unsigned int tx_thread_id;				/**< identifier for thread which sents packets to the session */
#else
  pthread_t rx_thread_id;					/**< identifier for thread which receives packets from the session */
  pthread_t tx_thread_id;					/**< identifier for thread which sents packets to the session */
#endif

  unsigned int lost_packets;				/**< how many packets is lost*/
  int accept_expired_fdt_inst;				/**< accept expired FDT instances */

#ifdef USE_RETRIEVE_UNIT
  struct trans_unit_container *unit_pool;  /**< list of preallocated units */
  struct trans_unit_container *last_given; /**< last unit returned */
#endif

  BOOL waiting_fdt_instance;				/**< FDT instance is in parsing state */ 

} alc_session_t;

/**
 * This function creates and initializes new session.
 *
 * @param a ALC specific commandline arguments
 *
 * @return identifier for the created session in success, -1 otherwise
 *
 */

int open_alc_session(alc_arguments_t *a);

/**
 * This function closes the session.
 *
 * @param s_id session identifier
 *
 */

void close_alc_session(int s_id);

/**
 * This function returns session from the alc_session_list.
 *
 * @param s_id session identifier
 *
 * @return pointer to session or NULL if session does not exist
 *
 */

alc_session_t* get_alc_session(int s_id);

/**
 * This function adds channel to the session.
 * @param s_id session identifier
 * @param port pointer to the port string
 * @param addr pointer to the address string
 * @param intface pointer to the interface string
 * @param intface_name pointer to the interface name string
 *
 * @return identifier for the created channel in success, -1 in error cases
 *
 */

int add_alc_channel(int s_id, const char *port, const char *addr, const char *intface, const char *intface_name);

/**
 * This function removes channels from session.
 *
 * @param s_id session identifier
 *
 */

void remove_alc_channels(int s_id);

/**
 * This function returns session object list.
 *
 * @param s_id session identifier
 *
 * @return pointer to the object list, NULL in error cases
 *
 */

struct trans_obj* get_session_obj_list(int s_id);

/**
 * This function returns session fdt list.
 *
 * @param s_id session identifier
 *
 * @return pointer to the fdt list, NULL in error cases
 *
 */

struct trans_obj* get_session_fdt_list(int s_id);

/**
 * This function returns session wanted object list.
 *
 * @param s_id session identifier
 *
 * @return pointer to the wanted object list, NULL in error cases
 *
 */

wanted_obj_t* get_session_wanted_obj_list(int s_id);

/**
 * This function returns the state of session.
 *
 * @param s_id session identifier
 *
 * @return the state of the session, -1 if the session does not exist anymore
 *
 */

int get_session_state(int s_id);

/**
 * This function sets state of session.
 *
 * @param s_id session identifier
 * @param state new state for the session
 *
 */

void set_session_state(int s_id, enum alc_session_states state);

/**
 * This function sets state for all sessions.
 *
 * @param state new state for all sessions
 *
 */

void set_all_sessions_state(enum alc_session_states state);

/**
 * This function returns the status of A flag usage in the session.
 *
 * @param s_id session identifier
 *
 * @return the state of A flag usage (0 = no, 1 = yes)
 *
 */

int get_session_a_flag_usage(int s_id);

/**
 * This function sets the A flag usage in session.
 *
 * @param s_id session identifier
 *
 */

void set_session_a_flag_usage(int s_id);

/**
 * This function returns session's current FDT instance id.
 *
 * @param s_id session identifier
 *
 * @return session's FDT instance id
 *
 */

unsigned int get_fdt_instance_id(int s_id);

/**
 * This function sets session's FDT instance id.
 *
 * @param s_id session identifier
 * @param instance_id FDT instance id to be set
 *
 */

void set_fdt_instance_id(int s_id, unsigned int instance_id);

/**
 * This function sets FDT instance parsed.
 *
 * @param s_id session identifier
 *
 */

void set_fdt_instance_parsed(int s_id);

/**
 * This function returns the number of bytes sent in the session.
 *
 * @param s_id session identifier
 *
 * @return number of bytes sent in the session since latest zeroing
 *
 */

unsigned long long get_session_sent_bytes(int s_id);

/**
 * This function sets the number of bytes sent in the session.
 *
 * @param s_id session identifier
 * @param sent_bytes bytes sent
 *
 */

void set_session_sent_bytes(int s_id, unsigned long long sent_bytes);

/**
 * This function adds sent bytes in session.
 *
 * @param s_id session identifier
 * @param sent_bytes bytes sent
 *
 */

void add_session_sent_bytes(int s_id, unsigned int sent_bytes);

/**
 * This function returns the number of bytes sent for the object.
 *
 * @param s_id session identifier
 *
 * @return number of bytes sent for the object since latest zeroing
 *
 */

unsigned long long get_object_sent_bytes(int s_id);

/**
 * This function sets number of bytes sent for the current object.
 *
 * @param s_id session identifier
 * @param sent_bytes bytes sent
 *
 */

void set_object_sent_bytes(int s_id, unsigned long long sent_bytes);

/**
 * This function adds sent bytes for the object.
 *
 * @param s_id session identifier
 * @param sent_bytes bytes sent
 *
 */

void add_object_sent_bytes(int s_id, unsigned int sent_bytes);

/**
 * This function returns last printed percent for the object.
 *
 * @param s_id session identifier
 *
 * @return last printed percent
 *
 */

double get_object_last_print_tx_percent(int s_id);

/**
 * This function sets last printed percent for the object.
 *
 * @param s_id session identifier
 * @param last_print_tx_percent printed percent
 *
 */

void set_object_last_print_tx_percent(int s_id, double last_print_tx_percent);

/**
 * This function returns identifier for the object which sender is currently sending.
 *
 * @param s_id session identifier
 *
 * @return transport object identifier for the object which sender is currently sending
 *
 */

unsigned long long get_session_tx_toi(int s_id);

/**
 * This function sets transport object which sender is currently sending.
 *
 * @param s_id session identifier
 * @param toi transport object identifier for the object which sender is currently sending
 *
 */

void set_session_tx_toi(int s_id, unsigned long long toi);

/**
 * This function sets state of session.
 *
 * @param s_id session identifier
 * @param base_tx_rate new base tx rate for the session
 *
 */

void update_session_tx_rate(int s_id, int base_tx_rate);

/**
 * This function checks if object identified with TOI is wanted and returns pointer to the wanted object structure.
 *
 * @param s pointer to the session
 * @param toi transport object identifier to be checked
 *
 * @return pointer to wanted object structure in success, NULL otherwise
 *
 */

wanted_obj_t* get_wanted_object(alc_session_t *s, unsigned long long toi);

/**
 * This function set object wanted from the session.
 *
 * @param s_id session identifier
 * @param toi transport object identifier
 * @param transfer_len transport object length
 * @param es_len encoding symbol length
 * @param max_sb_len maximum source block length
 * @param fec_inst_id FEC instance id
 * @param fec_enc_id FEC encoding id
 * @param max_nb_of_enc_symb maximum number of encoding symbols
 * @param content_enc_algo content encoding algorithm
 * @param finite_field finite field parameter with new RS FEC
 * @param nb_of_es_per_group number of encoding symbols per FLUTE packet
 * @param content_len the size of file corresponding this transport object
 * @param file_uri file URI for the file corresponding this transport object
 *
 * @return 0 in success, -1 otherwise
 *
 */

int set_wanted_object(int s_id,
		      unsigned long long toi,
		      unsigned long long transfer_len,
		      unsigned short es_len, unsigned int max_sb_len, int fec_inst_id,
		      short fec_enc_id, unsigned short max_nb_of_enc_symb,
		      unsigned char content_enc_algo, unsigned char finite_field,
			  unsigned char nb_of_es_per_group
		      );

/**
 * This function removes wanted object identified with toi from the session.
 *
 * @param s_id session identifier
 * @param toi transport object identifier
 *
 */

void remove_wanted_object(int s_id, unsigned long long toi);

/**
 * This function checks if FDT Instance is already received
 *
 * @param s pointer to the session
 * @param fdt_instance_id FDT instance id to be checked
 *
 * @return TRUE if FDT instance is already received, FALSE otherwise
 *
 */

BOOL is_received_instance(alc_session_t *s, unsigned int fdt_instance_id);

/**
 * This function sets FDT Instance to the received list.
 *
 * @param s pointer to the session
 * @param fdt_instance_id FDT instance id to be set
 *
 * @return 0 in success, -1 otherwise
 *
 */

int set_received_instance(alc_session_t *s, unsigned int fdt_instance_id);

/**
 * This function returns session's base directory.
 *
 * @param s_id session identifier
 *
 * @return pointer to base directory
 *
 */

char* get_session_basedir(int s_id);

/**
 * This function initializes session handler.
 *
 */

void initialize_session_handler();

/**
 * This function releases session handler.
 *
 */

void release_session_handler();

#ifdef __cplusplus
}; //extern "C"
#endif

#endif /* _ALC_SESSION_H_ */

