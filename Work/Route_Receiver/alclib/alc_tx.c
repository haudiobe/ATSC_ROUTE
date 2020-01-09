/** \file alc_tx.c \brief ALC level sending
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.114 $
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/timeb.h>

#ifdef _MSC_VER
#include <process.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "alc_tx.h"
#include "alc_channel.h"
#include "alc_session.h"
#include "transport.h"
#include "lct_hdr.h"
#include "null_fec.h"
#include "xor_fec.h"
#include "rs_fec.h"
#include "alc_hdr.h"
#include "mad_rlc.h"
#include "fec.h"

//Malek El Khatib 01.08.2014
unsigned short nb_of_symbols;	/*This is used to configure how many encoding symbols to send per packet. Only tested with Compact-No-Code-FEC*/
									/*IMPORTANT: es_len*nb_of_symbols SHOULD NOT lead to packet sizes exceeding maximum transmission unit*/
//End

/** 
 * This is a private function which adds packet to transmission queue when TX_THREAD mode is used.
 * 
 * @param s pointer to the session
 * @param sendbuf pointer to data to be added to transmission queue
 * @param sendlen lenght of data
 *
 * @return > 0 in success, 0 or -1 in error cases.
 *
 */

int add_pkt_to_tx_queue(alc_session_t *s, unsigned char *sendbuf,  unsigned int sendlen) {
	int retval = 0;
	tx_queue_t *pkt;
	int i;
	alc_channel_t *ch;

	if(s->tx_queue_size == MAX_TX_QUEUE_SIZE) {
		return retval;
	}

	/* Allocate memory for pkt */
	if(!(pkt = (tx_queue_t*)calloc(1, sizeof(tx_queue_t)))) {
		printf("Could not alloc memory for tx_queue pkt!\n");
		return -1;
	}

	pkt->nb_tx_ch = 0;
	pkt->next = NULL;
	pkt->datalen = sendlen;
	pkt->data = sendbuf;

	if(s->tx_queue_begin == NULL) {

		s->tx_queue_begin = pkt;
		s->tx_queue_end = pkt;

		for(i = 0; i < s->nb_channel; i++) {
			ch = s->ch_list[i];
			ch->ready = FALSE;
			ch->queue_ptr = s->tx_queue_begin;

			if(s->cc_id == RLC) {
				if(ch->ch_id != 0) {
					ch->wait_after_sp = RLC_WAIT_AFTER_SP;	
					ch->start_sending = FALSE;
				}
			}
		}

		s->nb_ready_channel = 0;

		if(s->cc_id == RLC) {
			s->nb_sending_channel = 1;
		}
	}
	else {
		s->tx_queue_end->next = pkt;
		s->tx_queue_end = pkt;

		for(i = 0; i < s->nb_channel; i++) {
			ch = s->ch_list[i];
			
			if(ch->queue_ptr == NULL) {
				ch->queue_ptr = s->tx_queue_end;
			}
		}
	}

	s->tx_queue_size++;

	return sendlen;
}

/**
 * This function a private function which sends one unit to channel or tx_queue.
 *
 * @param tr_unit pointer to transport unit to be sent
 * @param tr_block pointer to transport block that this units belongs
 * @param s pointer to session
 * @param ch pointer to channel
 * @param toi transport object identifier
 * @param transfer_len length of transport object
 * @param max_sb_len maximum source block length
 * @param es_len encoding symbol length
 * @param fec_enc_id FEC encoding id
 * @param fec_inst_id FEC instance id
 *
 * @return 0 in success, -1 or -2  in error cases/stopping situations
 *
 */

int send_unit(trans_unit_t *tr_unit, trans_block_t *tr_block, alc_session_t *s,
	      alc_channel_t *ch, unsigned long long toi, unsigned long long transfer_len,
	      unsigned int max_sb_len, unsigned short es_len,
	      unsigned char fec_enc_id, unsigned short fec_inst_id) {

  int hdrlen;		/* length of total ALC header */
  
  unsigned char pkt[MAX_PACKET_LENGTH];
  int sendlen = 0;
  unsigned char *sendbuf;
  
  def_lct_hdr_t *def_lct_hdr;
  
  unsigned short half_word1;
  unsigned short half_word2;
  unsigned int word;
  
  unsigned short max_16bits = 0xFFFF;
  unsigned int max_32bits = 0xFFFFFFFF;
  
  unsigned long long max_48bits = 0xFFFFFFFFFFFFULL;
  unsigned long long  max_64bits = 0xFFFFFFFFFFFFFFFFULL;
  
  hdrlen = sizeof(def_lct_hdr_t);
  
  memset(pkt, 0, MAX_PACKET_LENGTH);
  def_lct_hdr = (def_lct_hdr_t*)pkt;
  
  def_lct_hdr->version = ALC_VERSION;
  def_lct_hdr->flag_c = 0;
  def_lct_hdr->reserved = 0;
  def_lct_hdr->flag_t = 0;
  def_lct_hdr->flag_r = 0;
  
  def_lct_hdr->flag_a = 0;
  def_lct_hdr->flag_b = 0; /* TODO */
  
  def_lct_hdr->codepoint = fec_enc_id;

  def_lct_hdr->cci = htonl(0);
  
  if(s->half_word) {
		if(((s->tsi <= max_16bits) && (toi <= max_16bits))) {
			def_lct_hdr->flag_s = 0;
			def_lct_hdr->flag_o = 0;
			def_lct_hdr->flag_h = 1;

			half_word1 = (unsigned short)s->tsi;
			half_word2 = (unsigned short)toi;
			
			word = ((half_word1 << 16) | (half_word2 & 0xFFFF));
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;
		}
		else if(((((s->tsi <= max_16bits) && (((toi > max_16bits) && (toi <= max_32bits))))) ||
			 (((toi <= max_16bits) && (((s->tsi > max_16bits) && (s->tsi <= max_32bits))))))) {
			def_lct_hdr->flag_s = 1;
			def_lct_hdr->flag_o = 1;
			def_lct_hdr->flag_h = 0;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)s->tsi);
			hdrlen += 4;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;	
		}
		else if((((s->tsi > max_16bits) && (s->tsi <= max_32bits))) &&
			 (((toi > max_16bits) && (toi <= max_32bits)))) {			
			def_lct_hdr->flag_s = 1;
			def_lct_hdr->flag_o = 1;
			def_lct_hdr->flag_h = 0;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)s->tsi);
			hdrlen += 4;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;
		}
		else if(((s->tsi <= max_16bits) && (((toi > max_32bits) && (toi <= max_48bits))))) {		
			def_lct_hdr->flag_s = 0;
			def_lct_hdr->flag_o = 1;
			def_lct_hdr->flag_h = 1;

			half_word1 = (unsigned short)s->tsi;
			half_word2 = (unsigned short)((toi >> 32) & 0x0000FFFF);
			word = ((half_word1 << 16) | (half_word2 & 0xFFFF));
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;
			
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;
		}
		else if(((toi < max_16bits) && (((s->tsi > max_32bits) && (s->tsi <= max_48bits))))) {

			def_lct_hdr->flag_s = 1;
			def_lct_hdr->flag_o = 0;
			def_lct_hdr->flag_h = 1;

			word = (unsigned int)((s->tsi >> 16) & 0xFFFFFFFF);
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;
		
			half_word1 = (unsigned short)(s->tsi & 0x0000FFFF);
			half_word2 = (unsigned short)toi;

			word = ((half_word1 << 16) | (half_word2 & 0xFFFF));
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;
		}
		else if((((s->tsi > max_32bits) && (s->tsi <= max_48bits))) &&
			 (((toi > max_32bits) && (toi <= max_48bits)))) {		
			def_lct_hdr->flag_s = 1;
			def_lct_hdr->flag_o = 1;
			def_lct_hdr->flag_h = 1;

			word = (unsigned int)((s->tsi >> 16) & 0xFFFFFFFF);
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;
		
			half_word1 = (unsigned short)(s->tsi & 0x0000FFFF);
			half_word2 = (unsigned short)((toi >> 32) & 0x0000FFFF);
			word = ((half_word1 << 16) | (half_word2 & 0xFFFF));
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;
			
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;
		}
		else if((((s->tsi > max_16bits) && (s->tsi <= max_32bits))) &&
			 (((toi > max_32bits) && (toi <= max_64bits)))) {
			def_lct_hdr->flag_s = 1;
			def_lct_hdr->flag_o = 2;
			def_lct_hdr->flag_h = 0;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)s->tsi);
			hdrlen += 4;

			word = (unsigned int)((toi >> 32) & 0xFFFFFFFF);
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;
		}
		else if(((s->tsi < max_16bits) && ((toi > max_48bits) && (toi <= max_64bits)))) {
			def_lct_hdr->flag_s = 1;
			def_lct_hdr->flag_o = 2;
			def_lct_hdr->flag_h = 0;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)s->tsi);
			hdrlen += 4;

			word = (unsigned int)((toi >> 32) & 0xFFFFFFFF);
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;

			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;
		}
		else {
			printf("TSI & TOI field combination not supported!\n");
			fflush(stdout);
			return -1;
		}
	}
	else {
		def_lct_hdr->flag_s = 1;
		def_lct_hdr->flag_h = 0;
	
		*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)s->tsi);
		hdrlen += 4;

		if(toi <= max_32bits) {
			def_lct_hdr->flag_o = 1;
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;
		}
		else if(((toi > max_32bits) && (toi <= max_64bits))) {
			def_lct_hdr->flag_o = 2;
		
			word = (unsigned int)((toi >> 32) & 0xFFFFFFFF);
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
			hdrlen += 4;
		
			*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)toi);
			hdrlen += 4;
		}
		else {
			printf("TSI & TOI field combination not supported!\n");
			fflush(stdout);
			return -1;
		}
	}

	if(toi == FDT_TOI) {
	  hdrlen += add_fdt_lct_he(def_lct_hdr, hdrlen, s->fdt_instance_id);
		
#ifdef USE_ZLIB
	  if(s->encode_content == ZLIB_FDT || s->encode_content == ZLIB_FDT_AND_GZIP_FILES ) {
	    hdrlen += add_cenc_lct_he(def_lct_hdr, hdrlen, (unsigned char)ZLIB);
	  }	
#endif
	  
	}
	
	if(((toi == FDT_TOI) || (s->use_fec_oti_ext_hdr == 1))) {
	  
	  if(fec_enc_id == COM_NO_C_FEC_ENC_ID) {
	    
	    hdrlen += add_fti_0_2_128_130_lct_he(def_lct_hdr, hdrlen, transfer_len, 0,
						 es_len, max_sb_len);
	  }
	  else if(fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
	    
	    hdrlen += add_fti_0_2_128_130_lct_he(def_lct_hdr, hdrlen, transfer_len, 0,
						 es_len, max_sb_len);
	  }
	  else if(fec_enc_id == RS_FEC_ENC_ID) {
	    
	    hdrlen += add_fti_3_lct_he(def_lct_hdr, hdrlen, transfer_len, (unsigned char)GF_BITS,
				       (unsigned char)1, es_len,
				       (unsigned short)tr_block->max_k, (unsigned short)tr_block->max_n);
	  }
	  else if((fec_enc_id == COM_FEC_ENC_ID) || (fec_enc_id == SB_LB_E_FEC_ENC_ID)) {
	    
	    hdrlen += add_fti_0_2_128_130_lct_he(def_lct_hdr, hdrlen, transfer_len, fec_inst_id,
						 es_len, max_sb_len);	
	  }
	  
	  else if(fec_enc_id == SB_SYS_FEC_ENC_ID) {
	    
	    hdrlen += add_fti_129_lct_he(def_lct_hdr, hdrlen, transfer_len, fec_inst_id,
					 es_len, (unsigned short)tr_block->max_k, (unsigned short)tr_block->max_n);
	  }
	}
	
	/* TODO: add other LCT header extensions here */
	/*if(nop) {
	  hdrlen += add_nop_lct_he();
	  }
	  if(auth) {
	  hdrlen += add_auth_lct_he();
	  }
	  if(time) {
	  hdrlen += add_time_lct_he();
	  }
	*/
	
	def_lct_hdr->hdr_len = hdrlen >> 2; /* Header length in 32-bit words */	
	*(unsigned short*)def_lct_hdr = htons(*(unsigned short*)def_lct_hdr);

	/* FEC Payload ID */
	//Malek El Khatib
	printf("What is the next encoding symbol id: %u\n",tr_unit->esi);
	//End
	if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID))) {
	  
	  //Malek El Khatib 04.08.2014
	  //Use unsigned int for esi
	  //hdrlen += add_alc_fpi_0_130(def_lct_hdr, hdrlen, (unsigned short)tr_block->sbn,
		//		      (unsigned short)tr_unit->esi);
	  hdrlen += add_alc_fpi_0_130(def_lct_hdr, hdrlen, (unsigned short)tr_block->sbn,tr_unit->esi);
	  //End
	}
	else if(fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
	  
	  hdrlen += add_alc_fpi_2_128(def_lct_hdr, hdrlen, tr_block->sbn, tr_unit->esi);
	}
	else if(fec_enc_id == RS_FEC_ENC_ID) {
	  
	  hdrlen += add_alc_fpi_3(def_lct_hdr, hdrlen, tr_block->sbn,
				  tr_unit->esi, GF_BITS);
	}
	else if(fec_enc_id == SB_LB_E_FEC_ENC_ID) {
	  
	  hdrlen += add_alc_fpi_2_128(def_lct_hdr, hdrlen, tr_block->sbn, tr_unit->esi);
	}
	else if(fec_enc_id == SB_SYS_FEC_ENC_ID) {
	  
	  hdrlen += add_alc_fpi_129(def_lct_hdr, hdrlen, tr_block->sbn,
				    (unsigned short)tr_block->k,
				    (unsigned short)tr_unit->esi);
	}

	//Malek El Khatib 01.08.2014
	//Insert a number of encoding symbols equal to nb_of_symbols into the payload
	//memcpy(pkt + hdrlen, tr_unit->data, tr_unit->len);
	//sendlen = hdrlen + tr_unit->len;
	//unsigned short shift =0;
	int j;
	int shift = hdrlen;
	sendlen = hdrlen;
	for ( j=0; j < nb_of_symbols; j++)
	{
			memcpy(pkt + shift, (tr_unit+j)->data, (tr_unit+j)->len);
			shift += (int)(tr_unit+j)->len;
			sendlen += (tr_unit+j)->len;
	}
	//End

	if(ch != NULL) {

		if (ch->s->calculate_session_size == FALSE) {

			/*if(toi == 0 && tr_block->sbn == 0 && tr_unit->esi == 0) {
			}
			else {*/
		  sendto(ch->tx_sock, (char *)pkt, sendlen, 0,		
		    ch->addrinfo->ai_addr, ch->addrinfo->ai_addrlen);
			/*}*/
		}
		
		add_session_sent_bytes(s->s_id, sendlen);

		/* Should we take UDP/IP headers into account? */
		/* UDP */
		add_session_sent_bytes(s->s_id, 8);
		/* IPv4 or IPv6 */
		if(s->addr_family == PF_INET) {
		  add_session_sent_bytes(s->s_id, 20);
		}
		else {
		  add_session_sent_bytes(s->s_id, 40);
		}
	}
	else {
		if(!(sendbuf = (unsigned char*)calloc((sendlen + 1), sizeof(unsigned char)))) {
			printf("Could not alloc memory for tx queue packet!\n");
			fflush(stdout);
			return -1;
		}

		memcpy(sendbuf, pkt, sendlen);

		while(1) {

			if(s->state == SExiting) {
				return -2;
			}

			if(add_pkt_to_tx_queue(s, sendbuf, sendlen) > 0) {
				break;
			}
			else {
#ifdef _MSC_VER
				Sleep(10);
#else
				usleep(10000);
#endif
			}
		}
	}
	  
	return sendlen;
}

/**  
 * This is a private function which calculates packet length used in session.
 * 
 * @param s pointer to the session		
 *
 * @return packet length for the session
 *
 */

int calculate_packet_length(alc_session_t *s) {

	int packet_length = 0;

	if(s->addr_family == PF_INET) {

		if(s->use_fec_oti_ext_hdr == 1) {

			/* eslen + DEF_LCT_HDR + TSI + TOI + EXT_FTI + FEC_PL_ID + UDP + IP */

			if(((s->def_fec_enc_id == COM_NO_C_FEC_ENC_ID) ||
				(s->def_fec_enc_id == COM_FEC_ENC_ID) ||
				(s->def_fec_enc_id == RS_FEC_ENC_ID))) {	 	
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 4 + 8 + 20); 
			}
			else if(((s->def_fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_SYS_FEC_ENC_ID))) {
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 8 + 8 + 20); 
			}
		}
		else if(s->use_fec_oti_ext_hdr == 0) {
			/* eslen + DEF_LCT_HDR + TSI + TOI + FEC_PL_ID + UDP + IP */ 

			if(((s->def_fec_enc_id == COM_NO_C_FEC_ENC_ID) ||
				(s->def_fec_enc_id == COM_FEC_ENC_ID) ||
				(s->def_fec_enc_id == RS_FEC_ENC_ID))) {	
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 8 + 20); 
			}
			else if(((s->def_fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_SYS_FEC_ENC_ID))) {
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 8 + 8 + 20); 
			}
		}
	}
	else if(s->addr_family == PF_INET6) {
		if(s->use_fec_oti_ext_hdr == 1) {
			/* eslen + DEF_LCT_HDR + TSI + TOI + EXT_FTI + FEC_PL_ID + UDP + IP */

			if(((s->def_fec_enc_id == COM_NO_C_FEC_ENC_ID) ||
				(s->def_fec_enc_id == COM_FEC_ENC_ID) ||
				(s->def_fec_enc_id == RS_FEC_ENC_ID))) {	 	
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 4 + 8 + 40); 
			}
			else if(((s->def_fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_SYS_FEC_ENC_ID))) {
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 8 + 8 + 40); 
			}
		}
		else if(s->use_fec_oti_ext_hdr == 0) {
			/* eslen + DEF_LCT_HDR + TSI + TOI + FEC_PL_ID + UDP + IP */

			if(((s->def_fec_enc_id == COM_NO_C_FEC_ENC_ID) ||
				(s->def_fec_enc_id == COM_FEC_ENC_ID) ||
				(s->def_fec_enc_id == RS_FEC_ENC_ID))) {	 	
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 8 + 20); 
			}
			else if(((s->def_fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
				 (s->def_fec_enc_id == SB_SYS_FEC_ENC_ID))) {
				packet_length =  s->def_eslen + (sizeof(def_lct_hdr_t) + 4 + 4 + 8 + 8 + 20); 
			}
		}
	}

	return packet_length;	
}

/**  
 * This is a private function which calculates actual bitrate for the session.
 * 
 * @param s pointer to the session		
 *
 * @return actual bitrate for the session
 *
 */

float session_kbit_rate(const alc_session_t *s) {

    double curr_time;
    float actual_kbit_rate = (float)s->def_tx_rate;
    struct timeb timeb_current_time;

    ftime(&timeb_current_time);

    curr_time = timeb_current_time.time+timeb_current_time.millitm/1000.0;

    if(s->sent_bytes > 0 && curr_time > s->ftimestarttime) {
        actual_kbit_rate = (float)((s->sent_bytes*8/1024.0) / (curr_time - s->ftimestarttime));
    }

    /*printf("\n%f", curr_time-s->ftimestarttime);
    printf(" %f", actual_kbit_rate);*/
    return actual_kbit_rate;
}

int alc_send(int s_id, int tx_mode, char *buf, int buf_len,
			 unsigned long long toi, unsigned long long transfer_len,
			 unsigned short es_len, unsigned int max_sb_len, unsigned int sbn,
			 unsigned char fec_enc_id, unsigned short fec_inst_id) {

	trans_block_t *tr_block = NULL;
	trans_unit_t *tr_unit = NULL;
	unsigned int i = 0;

	alc_channel_t *ch = NULL;
	alc_session_t *s = NULL;

	int sent = 0;
	int tb_data_left = 0;

	int packet_length = 0;
	int addr_family = 0;
	int use_fec_oti_ext_hdr = 0;
	double loss_prob = 0;
	int retval = 0;
	double tx_percent = 0;

	
    double packetpersec = 0;
    double interval = 0;
    double block_interval = 0;
    long sleep_interval = 0;
    float diff_kbit_rate = 0;
    double relative_error = 0;
    double correction = 0;

	s = get_alc_session(s_id);

	if(tx_mode != TX_THREAD) { /* We have only the base channel in this case */
		ch = s->ch_list[0]; /* Let's take the base channel */
		addr_family = s->addr_family;
		use_fec_oti_ext_hdr = s->use_fec_oti_ext_hdr;

		//Malek El Khatib 01.08.2014
		nb_of_symbols = 1;
		if (toi !=FDT_TOI)
		{
			if (numEncSymbPerPacket > 0)
				nb_of_symbols = numEncSymbPerPacket;	/*<This is used to reset value to initial value. It could also get its values from an external file>*/
			else
				printf("For sending process, number of encoding symbols per packet (-Y:) must be greater than zero\n");


			if (addr_family == PF_INET && fec_enc_id == COM_NO_C_FEC_ENC_ID)
			{
				if (es_len*nb_of_symbols > MAX_SYMB_LENGTH_IPv4_FEC_ID_0_3_130) //Fix The condition
					nb_of_symbols = (unsigned short)floor((double)(MAX_SYMB_LENGTH_IPv4_FEC_ID_0_3_130) / es_len);
			}
			else if (addr_family == PF_INET6 && fec_enc_id == COM_NO_C_FEC_ENC_ID) //NOT TESTED
			{
				if (es_len*nb_of_symbols > MAX_SYMB_LENGTH_IPv6_FEC_ID_0_3_130)
					nb_of_symbols = (unsigned short)floor((double)(MAX_SYMB_LENGTH_IPv6_FEC_ID_0_3_130) / es_len);
			}
		}
		printf("What is the number of symbols in a packet %u\n",nb_of_symbols);
		//END

		//Malek El Khatib 01.08.2014// To calculate sizes, we multiply es_length by nb_of_symbols to determine how much data is in the payload
		if(toi == FDT_TOI) { 

			/* eslen + DEF_LCT_HDR + TSI + TOI + EXT_FDT + EXT_CENC + EXT_FTI + FEC_PL_ID + UDP + IP */ 

			if(addr_family == PF_INET) {

				if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID)
					|| (fec_enc_id == RS_FEC_ENC_ID))) {
						packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 4 + 16 + 4 + 8 + 20); 
				}
				else if(((fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) || (fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
					(fec_enc_id == SB_SYS_FEC_ENC_ID))) {
						packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 4 + 16 + 8 + 8 + 20); 
				}
			}
			else if(addr_family == PF_INET6) {

				if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID)
					|| (fec_enc_id == RS_FEC_ENC_ID))) {
						packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 4 + 16 + 4 + 8 + 40); 
				}
				else if(((fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) || (fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
					(fec_enc_id == SB_SYS_FEC_ENC_ID))) {
						packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 4 + 16 + 8 + 8 + 40); 
				}
			}
		}
		else { 
			if(addr_family == PF_INET) {

				if(use_fec_oti_ext_hdr == 1) {
					/* eslen + DEF_LCT_HDR + TSI + TOI + EXT_FTI + FEC_PL_ID + UDP + IP */ 

					if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID)
						|| (fec_enc_id == RS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 4 + 8 + 20); 
					}
					else if(((fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) || (fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
						(fec_enc_id == SB_SYS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 8 + 8 + 20); 
					}
				}
				else if(use_fec_oti_ext_hdr == 0) {

					/* eslen + DEF_LCT_HDR + TSI + TOI + FEC_PL_ID + UDP + IP */ 

					if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID)
						|| (fec_enc_id == RS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 8 + 20); 
					}
					else if(((fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) || (fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
						(fec_enc_id == SB_SYS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 8 + 8 + 20); 
					}
				}
			}
			else if(addr_family == PF_INET6) {

				if(use_fec_oti_ext_hdr == 1) {
					/* eslen + DEF_LCT_HDR + TSI + TOI + EXT_FTI + FEC_PL_ID + UDP + IP */ 

					if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID)
						|| (fec_enc_id == RS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 4 + 8 + 40); 
					}
					else if(((fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) || (fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
						(fec_enc_id == SB_SYS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 16 + 8 + 8 + 40); 
					}
				}
				else if(use_fec_oti_ext_hdr == 0) {

					/* eslen + DEF_LCT_HDR + TSI + TOI + FEC_PL_ID + UDP + IP */ 

					if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID)
						|| (fec_enc_id == RS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 4 + 8 + 40); 
					}
					else if(((fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) || (fec_enc_id == SB_LB_E_FEC_ENC_ID) ||
						(fec_enc_id == SB_SYS_FEC_ENC_ID))) {
							packet_length =  es_len*nb_of_symbols + (sizeof(def_lct_hdr_t) + 4 + 4 + 8 + 8 + 40); 
					}
				}
			}
		}
	}

	//Malek El Khatib
	printf("What is the size of the packet: %d %ld \n",packet_length, sizeof(def_lct_hdr_t));
	printf("The buffer legnth is: %d\n",buf_len);
	//packet_length -= es_len;
	//packet_length += 1428;
	//End

	if(fec_enc_id == COM_NO_C_FEC_ENC_ID) {
		tr_block = null_fec_encode_src_block(buf, buf_len, sbn, es_len);
	}
	else if(fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
		tr_block = xor_fec_encode_src_block(buf, buf_len, sbn, es_len);
	}
	else if(fec_enc_id == RS_FEC_ENC_ID) {
		tr_block = rs_fec_encode_src_block(buf, buf_len, sbn, es_len, s->def_fec_ratio,
			max_sb_len);
	}
	else if(((fec_enc_id == SB_SYS_FEC_ENC_ID) && (fec_inst_id == REED_SOL_FEC_INST_ID))) {
		tr_block = rs_fec_encode_src_block(buf, buf_len, sbn, es_len, s->def_fec_ratio,
			max_sb_len);
	}
	else {
		if(fec_enc_id < 128) {
			printf("FEC Encoding %i is not supported!\n", fec_enc_id);
		}
		else {
			printf("FEC Encoding %i/%i is not supported!\n", fec_enc_id, fec_inst_id);
		}

		return -1; 
	}

	if(tr_block == NULL) {
		return -1;
	}

	if(tx_mode != TX_THREAD) {

		tb_data_left = es_len * tr_block->n;
		tr_unit = tr_block->unit_list; 

		//Malek El Khatib 01.08.2014
		//Make sure that there are enough encodedsymbols to begin with
		//if (nb_of_symbols > tr_block->n)
		//	nb_of_symbols = tr_block->n;
		int symbolsSent=0;
		int numOfPackets=0;
		//Increment i by nb_of_symbols
		//for(i = 0; i < tr_block->n; i++) {
		printf("The total number of encoding symbols for this object is: %u\n",tr_block->n);
		for(i = 0; i < tr_block->n; i+=nb_of_symbols) { 
			
			//Send multiple consecutive encoding symbols per packet. However, esi=0 should be sent alone at the end 
			if (i == (tr_block->n -1))
				nb_of_symbols = 1;
			else if ((symbolsSent + nb_of_symbols) > (tr_block->n -1))
				nb_of_symbols = tr_block->n -1 - symbolsSent;
			//END
			if(s->state == SPaused) {
				while(1) {
					if(s->state == SExiting) {
						return -2;
					}
			  
					if(s->state == SActive) {
						break;
					}
#ifdef _MSC_VER
					Sleep(1000);
#else
					usleep(1000000);
#endif
				}
			}

			if(s->state == SExiting) {
				while(1) {
					free(tr_unit->data);

					if(tr_unit->esi == (tr_block->n - 1)) {
						break;
					}
					tr_unit++;
				}

				free(tr_block->unit_list);
				free(tr_block);

				return -2;
			}

			if (ch->s->calculate_session_size == FALSE) {
				packetpersec =
					((double)(ch->tx_rate * 1000) / (double)(packet_length * 8));   
				interval = ((double)1 / packetpersec);
				block_interval = /*tr_block->n*/ 1*interval;
				sleep_interval = (long)(block_interval*1000);
				diff_kbit_rate = session_kbit_rate(ch->s)-ch->s->def_tx_rate;
				relative_error = (double)diff_kbit_rate/(double)ch->s->def_tx_rate;
				correction = 1;

				//printf("\n%f ", relative_error);
				if(relative_error > 0.001) {
					correction = 1.0 + (relative_error*50.0);
				}
				else if(relative_error < -0.001) {
					relative_error = -relative_error;
					correction = 1.0 - (relative_error*50.0);
				}

				//printf("%f ", correction);
				//printf("%d ", sleep_interval);

				// Adjust the sleep interval according the diff_bit_rate value.
				sleep_interval = sleep_interval*correction;
				//printf("%d", sleep_interval);

				if(sleep_interval > 0) {
#ifdef _MSC_VER
					Sleep(sleep_interval);
#else
					usleep(sleep_interval*1000);
#endif
				}
			}

			loss_prob = 0;

			if(s->simul_losses) {
				if(ch->previous_lost == TRUE) {
					loss_prob = s->loss_ratio2;
				}
				else {
					loss_prob = s->loss_ratio1;
				}
			}

			if(!randomloss(loss_prob)) {

				retval = send_unit(tr_unit, tr_block, s, ch, toi, transfer_len, max_sb_len, es_len,
					fec_enc_id, fec_inst_id);

				if(retval < 0) {

					while(1) {

						free(tr_unit->data);

						if(tr_unit->esi == (tr_block->n - 1)) {
							break;
						}

						tr_unit++;
					}

					free(tr_block->unit_list);
					free(tr_block);

					return -1;
				}

				//Malek El Khatib 01.08.2014
				//Adjust the value of sent bytes
				//sent += tr_unit->len;

				//add_object_sent_bytes(s_id, tb_data_left < tr_unit->len ?
				//	(unsigned int)tb_data_left : tr_unit->len);

				//tb_data_left -= tb_data_left < tr_unit->len ? tb_data_left : tr_unit->len;

				int j =0;
				trans_unit_t *tr_unit_tmp = tr_unit;

				while(j<nb_of_symbols)
				{
					sent += tr_unit_tmp->len;

					add_object_sent_bytes(s_id, tb_data_left < tr_unit_tmp->len ?
						(unsigned int)tb_data_left : tr_unit_tmp->len);

					tb_data_left -= tb_data_left < tr_unit_tmp->len ? tb_data_left : tr_unit_tmp->len;

					tr_unit_tmp++;
					j++;
				}
				//End

				if(((toi == FDT_TOI && s->verbosity == 4) || (toi != FDT_TOI && s->verbosity > 1))) {

					tx_percent = (double)((double)(100 *
						((double)(long long)get_object_sent_bytes(s_id)/
						(double)(long long)transfer_len)));

					if(((tx_percent >= (get_object_last_print_tx_percent(s_id) + 1))
						|| (tx_percent == 100))) {
							set_object_last_print_tx_percent(s_id, tx_percent);
							printf("%.2f%% of object sent (TOI=%llu)\n", tx_percent, toi);
					}

					fflush(stdout);
				}

				ch->previous_lost = FALSE;
			}
			else { 
				ch->previous_lost = TRUE;
			}


			free(tr_unit->data);
			//Malek El Khatib 01.08.2014
			//Start
			//tr_unit++;
			tr_unit += nb_of_symbols;
			symbolsSent +=nb_of_symbols;
			numOfPackets++;
			//End
		}

		//Malek El Khatib 11.08.2014
		printf("The number of symbols and packets sent are: %d %d\n",symbolsSent,numOfPackets);
		printf("The number of symbols and packets sent are: %d %d\n",symbolsSent,numOfPackets);

		free(tr_block->unit_list);
		free(tr_block);
	}
	else {
		tr_unit = tr_block->unit_list;

		for(i = 0; i < tr_block->n; i++) {

			retval = send_unit(tr_unit, tr_block, s, NULL, toi, transfer_len,
				max_sb_len, es_len, fec_enc_id, fec_inst_id);

			if(retval == -1) {
				i--;
				continue;
			}
			else if(retval == -2) {

				while(1) {

					free(tr_unit->data);

					if(tr_unit->esi == (tr_block->n - 1)) {
						break;
					}

					tr_unit++;
				}

				free(tr_block->unit_list);
				free(tr_block);

				return retval;		
			}

			free(tr_unit->data);
			tr_unit++;
		}

		free(tr_block->unit_list);
		free(tr_block);
	}

	return sent;
}

int send_session_close_packet(int s_id) {

	int	sendlen;			     /* number of bytes to send */
	int	hdrlen;					/* length of total ALC header */
	int retval = 0;

	alc_channel_t *ch;

	unsigned char sendbuf[MAX_PACKET_LENGTH];	/* TU (hdrs+data) cannot be larger */

	def_lct_hdr_t *def_lct_hdr;
	
	unsigned short half_word1;
	unsigned short half_word2;
	unsigned int word;

	assert(get_alc_session(s_id)!=NULL);
	ch = get_alc_session(s_id)->ch_list[0]; /* We send the close packet only to the base channel */
	
	memset(sendbuf, 0, MAX_PACKET_LENGTH);
	def_lct_hdr = (def_lct_hdr_t*)sendbuf;

	def_lct_hdr->version = ALC_VERSION;
	def_lct_hdr->flag_c = 0;
	def_lct_hdr->reserved = 0;

	def_lct_hdr->flag_t = 0;
	def_lct_hdr->flag_r = 0;
	def_lct_hdr->flag_a = 1;
	def_lct_hdr->flag_b = 0;
	def_lct_hdr->codepoint = ch->s->def_fec_enc_id; 

	def_lct_hdr->cci = htonl(0);

	hdrlen = sizeof(def_lct_hdr_t);
	
	if(ch->s->tsi <= 0xFFFFFFFF) {
		
		def_lct_hdr->flag_s = 1;
		def_lct_hdr->flag_o = 0;
		def_lct_hdr->flag_h = 0;
		
		*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl((unsigned int)ch->s->tsi);
		hdrlen += 4;
	}
	else {
		/* TSI field length 48 bits not possible without TOI field */

		def_lct_hdr->flag_s = 1;
		def_lct_hdr->flag_o = 0;
		def_lct_hdr->flag_h = 1;

		word = (unsigned int)((ch->s->tsi >> 16) & 0xFFFFFFFF);
		*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
		hdrlen += 4;
	
		half_word1 = (unsigned short)(ch->s->tsi & 0x0000FFFF);
		half_word2 = (unsigned short)0;

		word = ((half_word1 << 16) | (half_word2 & 0xFFFF));
		*(unsigned int*)((unsigned char*)def_lct_hdr + hdrlen) = htonl(word);
		hdrlen += 4;
	}

	def_lct_hdr->hdr_len = hdrlen >> 2; /* Header length in 32-bit words */	
	*(unsigned short*)def_lct_hdr = htons(*(unsigned short*)def_lct_hdr);

	sendlen = hdrlen;

	if(ch->s->cc_id == RLC) {
		retval = mad_rlc_fill_header(ch->s, (rlc_hdr_t*)(sendbuf + 4), ch->ch_id);
	}
                	
	retval = sendto(ch->tx_sock, (char *)sendbuf, sendlen, 0,				
		ch->addrinfo->ai_addr, ch->addrinfo->ai_addrlen);

	add_session_sent_bytes(s_id, sendlen);

        /* Should we take UDP/IP headers into account? */
        /* UDP */
        add_session_sent_bytes(s_id, 8);
        /* IPv4 or IPv6 */
        if(ch->s->addr_family == PF_INET) {
          add_session_sent_bytes(s_id, 20);
        }
        else {
          add_session_sent_bytes(s_id, 40);
        }

	return retval;
}

void* tx_thread(void *s) {

  alc_session_t *session;
  alc_channel_t *channel;
  tx_queue_t *tmp_ptr;
  
  tx_queue_t *next_pkt;
  tx_queue_t *pkt;
  
  int i, j;
  int retcode;
  int packet_length;
  
  double interval;
  double packetpersec;
  double currenttime;
  double lasttime;
  
  double loss_prob;
  
  session = (alc_session_t *)s;
  
  packet_length = calculate_packet_length(session);
  
  packetpersec = ((double)(session->def_tx_rate * 1000) / (double)(packet_length * 8));   
  interval = ((double)1 / packetpersec);
  
  /* interval is too small for FDT Instance, because in packet length calculation FDT Instance's
     extra header fields are not counted */
  
  while(session->state == SActive) {	
    
    if(session->tx_queue_begin == NULL) {
#ifdef _MSC_VER
      Sleep(1);
#else
      usleep(1000);
#endif		
      continue;
    }
    else {
      break;
    }
  }
  
  lasttime = sec();
  
  while(session->state == SActive) {
    
    currenttime = sec();
    
	if(session->calculate_session_size == FALSE) {

		while(currenttime < (lasttime + interval)) {
      
			if(session->optimize_tx_rate) {
#ifdef _MSC_VER
				Sleep(0);
#else
				usleep(0);
#endif
			}
			else {
#ifdef _MSC_VER
				Sleep(1);
#else
				usleep(1000);
#endif
			}
			currenttime = sec();
		}
	}    
    for(i = 0; i < session->nb_channel; i++) {
      
      channel = session->ch_list[i];
      
      if(session->cc_id == RLC) {
	
	if(channel->start_sending == FALSE) {
	  continue;
	}
	
	if(channel->wait_after_sp > 0) {
	  channel->wait_after_sp--;
	  continue;
	}
      }
      
      for(j = 0; j < channel->nb_tx_units; j++) {
	
	if(channel->queue_ptr == NULL) {
	  
	  if(channel->ready == FALSE) {
	    session->nb_ready_channel++;
	    channel->ready = TRUE;
	  }
	  
	  if(session->nb_ready_channel == session->nb_sending_channel) {
	    
	    pkt = session->tx_queue_begin;
	    
	    while(pkt != NULL) {
	      next_pkt = pkt->next;
	      free(pkt->data);
	      free(pkt);
	      pkt = next_pkt;
	    }	
	    
	    session->tx_queue_begin = NULL;
	    session->tx_queue_size = 0;
	  }
	  
	  break;
	}
	
	if(session->first_unit_in_loop) {
	  
	  if(session->cc_id == RLC) {
	    mad_rlc_reset_tx_sp(session);
	  }
	  
	  session->first_unit_in_loop = FALSE;
	}
	if(session->cc_id == RLC) {
	  
	  retcode = mad_rlc_fill_header(session, (rlc_hdr_t*)(channel->queue_ptr->data + 4),
					channel->ch_id);
	  
	  if(retcode < 0) {
	  }
	}
	
	loss_prob = 0;
	
	if(session->simul_losses) {
	  if(channel->previous_lost == TRUE) {
	    loss_prob = channel->s->loss_ratio2; /*P_LOSS_WHEN_LOSS;*/
	  }
	  else {
	    loss_prob = channel->s->loss_ratio1; /*P_LOSS_WHEN_OK;*/
	  }
	}
	
	if(!randomloss(loss_prob)) {
  	
	  if(session->calculate_session_size == FALSE) {
	    retcode = sendto(channel->tx_sock, (char*)channel->queue_ptr->data,	
			     channel->queue_ptr->datalen, 0, channel->addrinfo->ai_addr,
			     channel->addrinfo->ai_addrlen);
	    
	    if(retcode < 0) {
	      
#ifdef _MSC_VER
	      printf("sendto failed with: %d\n", WSAGetLastError());
#else
	      printf("sendto failed with: %d\n", errno);
#endif
	      break;
	    }
	  }
	  
	  add_session_sent_bytes(session->s_id, channel->queue_ptr->datalen);
	  /* Should we take UDP/IP headers into account? */
	  /* UDP */
	  add_session_sent_bytes(session->s_id, 8);
	  /* IPv4 or IPv6 */
	  if(session->addr_family == PF_INET) {
	    add_session_sent_bytes(session->s_id, 20);
	  }
	  else {
	    add_session_sent_bytes(session->s_id, 40);
	  }
	  
	  channel->previous_lost = FALSE;
	}
	else {
	  channel->previous_lost = TRUE;
	}
	
	channel->queue_ptr->nb_tx_ch++;
	
	if(channel->queue_ptr->nb_tx_ch == (unsigned int)session->nb_channel) {
	  
	  tmp_ptr = channel->queue_ptr->next;
	  free(channel->queue_ptr->data);
	  free(channel->queue_ptr);
	  
	  channel->queue_ptr = tmp_ptr;
	  session->tx_queue_begin = tmp_ptr;
	  session->tx_queue_size--;
	}
	else {
	  channel->queue_ptr = channel->queue_ptr->next;
	}
      }
    }
    
    lasttime += interval;
  }

#ifdef _MSC_VER
  _endthread();
#else
  pthread_exit(0);
#endif

  return NULL;
}
