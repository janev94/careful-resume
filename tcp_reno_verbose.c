/*
* Based on TCP Reno (https://github.com/torvalds/linux/blob/v5.4/net/ipv4/tcp.c). This version of TCP Reno logs to the kern.log whenver IN_ACK_EVENT or congestion window (CWND) related event occurs. 
*
* ---------------------------------------------------------------------
* Copyright (C) 2022 Mihail Yanev
* This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; version 2.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
* 
* ---------------------------------------------------------------------
* New-CWV implementation for Linux: draft-ietf-tcpm-newcwv-05.txt
*/

#include <linux/module.h>
#include <net/tcp.h>
#include <linux/vmalloc.h>



void tcp_vreno_in_ack_event(struct sock *sk, u32 flags)
{
	const struct tcp_sock *tp = tcp_sk(sk);
	const struct inet_sock *isock = inet_sk(sk);

	uint16_t sport = ntohs(isock->inet_sport);
	uint16_t dport = ntohs(isock->inet_dport);

	if(sport == 80 || dport == 80) { // HTTP server doing 
		printk(KERN_INFO "ACK Received. sourcep: %u dstp: %u proto%u send window: %u recv window %u\n",
				sport, dport, sk->sk_protocol, tp->snd_cwnd, tp->rcv_wnd);
	}
}

void clamp_init(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	tp->snd_cwnd_clamp=42;
}


void vreno_set_state(struct sock *sk, u8 new_state)
{
	printk(KERN_INFO "Entering new state: %u", new_state);

}


void vreno_cwnd_event(struct sock *sk, enum tcp_ca_event ev)
{

	printk(KERN_INFO "Congestion window event occurred: %u", ev);
	if(ev == CA_EVENT_CWND_RESTART)
	{
		// Something has reset our CWND, possibly CWND validation. For now ignore that and bring the CWND back up
		struct tcp_sock *tp = tcp_sk(sk);
		u32 reset_cwnd = tp->snd_cwnd;
		tp->snd_cwnd = tp->snd_cwnd_clamp; // bounce back the CWND value to the clampped value
		printk(KERN_INFO "CWND validation attempted to bring cwnd down to: %u. It was bumped back to %u", reset_cwnd, tp->snd_cwnd);
	}
	
}

u32 vreno_undo_cwnd(struct sock *sk) 
{
	u32 new_cwnd = tcp_reno_undo_cwnd(sk);
	printk(KERN_INFO "Loss occurred, new CWND value is: %u", new_cwnd);
	return new_cwnd;
}

struct tcp_congestion_ops tcp_reno_verbose = {
	.init		= clamp_init,
	.flags		= TCP_CONG_NON_RESTRICTED,
	.name		= "reno_verbose",
	.owner		= THIS_MODULE,
	.ssthresh	= tcp_reno_ssthresh,
	.cong_avoid	= tcp_reno_cong_avoid,
	.set_state  = vreno_set_state,
	.cwnd_event = vreno_cwnd_event,
	.undo_cwnd	= vreno_undo_cwnd,
	.in_ack_event = tcp_vreno_in_ack_event,
};


static int __init tcp_reno_verbose_register(void)
{
    printk(KERN_INFO "Verbose Reno Going Up5");
	return tcp_register_congestion_control(&tcp_reno_verbose);
}


static void __exit tcp_reno_verbose_unregister(void)
{
    printk(KERN_INFO "Verbose Reno Shutting Down5");
    tcp_unregister_congestion_control(&tcp_reno_verbose);
}



module_init(tcp_reno_verbose_register);
module_exit(tcp_reno_verbose_unregister);

MODULE_AUTHOR("Yanev");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Verbose reno");
