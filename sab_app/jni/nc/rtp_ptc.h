
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __RTP_H__
#define __RTP_H__

#include "tool_type.h"
#include "tool_sock.h"

#define RTP_PKT_SIZE			2048

typedef enum
{
	RTP_PKT_TYPE_SPS	= 7,
	RTP_PKT_TYPE_PPS	= 8,	
	RTP_PKT_TYPE_FUA	= 28,
	RTP_PKT_TYPE_FUB	= 29,
}RTP_PKT_TYPE_E;

typedef enum
{
	RTP_PT_PCMU			= 0,	
	RTP_PT_PCMA			= 8,
	RTP_PT_H264			= 96,
	RTP_PT_PCMA_OLD		= 97,
	RTP_PT_PCMU_OLD		= 98,
}RTP_PT_E;

typedef struct
{
	TOOL_UINT8	flag;			/* $=0x24 */
	TOOL_UINT8	channel;
	TOOL_UINT16	len;
}rtp_tcp_head_t;

typedef struct
{
	TOOL_UINT8	type:5;
	TOOL_UINT8	nri:2;
	TOOL_UINT8	f:1;
}rtp_fu_indicator_t;

typedef struct
{
	TOOL_UINT8	type:5;
	TOOL_UINT8	r:1;
	TOOL_UINT8	e:1;
	TOOL_UINT8	s:1;
}rtp_fu_head_t;

typedef struct __rtp_head_t
{
	TOOL_UINT8	cc:4;			/* CSRC count */
	TOOL_UINT8	x:1;			/* header extension flag */
	TOOL_UINT8	p:1;			/* padding flag */
	TOOL_UINT8	v:2;			/* protocol version */

	TOOL_UINT8	pt:7;			/* payload type */
	TOOL_UINT8	m:1;			/* marker bit */
	
	TOOL_UINT16	seq;			/* sequence number */
	TOOL_UINT32	ts;				/* timestamp */
	TOOL_UINT32	ssrc;			/* synchronization source */
//	u_int32 csrc[1];		/* optional CSRC list */
}rtp_head_t;

/*	sps:
 *	
 *	[0]=0x67 
 *	[1]=AVCProfileIndication: 66=baseline, 77=main, 100=hign, 110=hign10, 122=hign422, high444=244
 *	[2]=profile_compatibility: 0-5=constraint_set_flags, 6-7=reserve
 *	[3]=AVCLevlIndication: 9=1b, 10=1.0, 20=2.0, 20=3.0
 *	[4-7]=sps_id
 */
typedef struct
{
	TOOL_INT8	flag;
	TOOL_INT8	AVCProfileIndication;
	TOOL_INT8	profile_compatibility;
	TOOL_INT8	AVCLevlIndication;
}rtp_sps_t;


/*	pps:
 *	
 *	[0]=0x68 
 */


#endif

#if defined __cplusplus
}
#endif

