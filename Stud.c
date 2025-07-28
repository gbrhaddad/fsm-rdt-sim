/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
// Labbgrupp:15 Daniel Meladi

#include "Stud.h"
#include <stdbool.h>
#include <string.h>
#define STOP 18
#define STARTAR_TIMER 0 // startar timern
#define TIMER_INTERVALL 1000 // timer intervall

int checker = 0, seq = 0, ack = 0;
struct pkt glob_paket_a;
bool glob_flagga_a, glob_flagga_b;
struct msg msg1;


int checksum(struct pkt paket) { // checksum detects bit errors in a transmitted packet
	int sum = 0, i=0;
	sum = sum + paket.acknum + paket.seqnum;
	while(i < sizeof(paket.payload)){
		sum += paket.payload[i];
		i++;
	}
	return sum;
}

void make_packet(int seq, struct msg message){ // skapar ett paket
	glob_paket_a.seqnum = seq;
	strcpy(glob_paket_a.payload, message.data);
	//printf("A: Sending packet: %s", paket.payload);
	glob_paket_a.checksum = checksum(glob_paket_a);
	tolayer3(0, glob_paket_a);
	starttimer(STARTAR_TIMER, TIMER_INTERVALL);
}

void make_ack(int ack){ // skapar en positiv aknowledgement
    struct pkt paket_b;
	paket_b.acknum = ack;
	paket_b.checksum = checksum(paket_b);
	tolayer3(1, paket_b);
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{	
	glob_flagga_a = false;
	if(checker != 0){
		//printf("\n Dropped message: %s \n", message.data);
	}else{
		checker = 1;
		if(glob_flagga_a){
		make_ack(seq);
		}else{
		make_packet(seq,message);
		}
	}
}

void B_output(struct msg message)  /* need be completed only for extra credit */
{}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt paket){
	if(paket.acknum != seq){
		//printf("Incorrect ack sequence number");
	}else if(!(paket.checksum == checksum(paket))){
		//printf("Corrupted packet");
	}else{
		checker = 0;
		stoptimer(0); 
		seq = 1 - seq;
	}
}

/* called when A's timer goes off */
void A_timerinterrupt(){
	starttimer(STARTAR_TIMER, TIMER_INTERVALL);
	tolayer3(0, glob_paket_a);
	//printf("Resending the last sent packet: %s.\n", paket.payload);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt paket)
{
	glob_flagga_b = true;

	if (paket.seqnum != ack){
		//printf("  B:sending ACK for previous packet.\n");
		make_ack(1-ack);
		if(!glob_flagga_b){
		make_packet(1-ack,msg1);
		}
		return;
	}
	if (checksum(paket) != paket.checksum){
		//printf(" B:The packet is corrupted.\n");
		if(glob_flagga_b){
		make_ack(1-ack);
		}else{
		make_packet(1-ack,msg1);
		}
		return;
	}
	else
	{
		//printf(" B_input: The packet is received: %s\n", paket.payload);
		if(glob_flagga_b){
			make_ack(ack);
		}else{
			make_packet(ack, msg1);
		}
		tolayer5(1, paket.payload);
        printf("%s\n", paket.payload);
        
		ack = 1-ack;
	}
}

/* called when B's timer goes off */
void B_timerinterrupt()
{}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{}
