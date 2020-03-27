

#if defined __cplusplus
extern "C"
{
#endif

#include "tool_ptc.h"
#include "tool_type.h"
#include "qh_ptc.h"
#include "tool_sock.h"
#include "tool_log.h"
#include "tool_sysf.h"
#include "tool_json.h"
#include "tool_md5.h"
#include "tool_base64.h"
#include "rtp_ptc.h"
#include <stdio.h>

TOOL_INT32 tool_ptc_qh_sendMsg(TOOL_VOID* sock_item, ptc_msg_t* msg)
{
	msg->head.flag = PTC_FLAG;
	msg->head.ver = PTC_VERSION;
	msg->head.pack_count = 1;
	msg->head.pack_no = 1;
	msg->head.pack_len = msg->head.len;
	TOOL_INT32 len = sizeof(ptc_head_t)+msg->head.len;
	if (tool_sock_item_send((tool_sock_item_t *)sock_item, msg, len) != len)
		return -1;
//	log_debug("send message(0x%08x) len(%d)", msg->head.cmd, msg->head.len);
	return 0;
}

TOOL_INT32 tool_ptc_qh_recvMsg(TOOL_VOID* sock_item, ptc_msg_t* msg)
{
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, &msg->head, sizeof(ptc_head_t)) != sizeof(ptc_head_t))
		return -1;
	if (msg->head.flag != PTC_FLAG)
	{
		log_error("msg->head.flag(0x%08x)", msg->head.flag);
		return -1;
	}
	if (msg->head.len >= PTC_MSG_SIZE)
	{
		log_error("msg->head.len(%d) cmd(0x%08x)", msg->head.len, msg->head.cmd);
		return -1;
	}
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, msg->data, msg->head.len) != msg->head.len)
		return -1;
	msg->data[msg->head.len] = 0;
//	log_debug("recv message(0x%08x) len(%d)", msg->head.cmd, msg->head.len);
	return 0;
}

TOOL_INT32 tool_ptc_qh_sendtoMsg(TOOL_VOID* sock_item, ptc_msg_t* msg)
{
	msg->head.flag = PTC_FLAG;
	msg->head.ver = PTC_VERSION;
	msg->head.pack_count = 1;
	msg->head.pack_no = 1;
	msg->head.pack_len = msg->head.len;
	TOOL_INT32 len = sizeof(ptc_head_t)+msg->head.len;
	if (tool_sock_item_sendto((tool_sock_item_t *)sock_item, msg, len) != len)
		return -1;
//	log_debug("send message(0x%08x) len(%d)", msg->head.cmd, msg->head.len);
	return 0;	
}

TOOL_INT32 tool_ptc_qh_recvfromMsg(TOOL_VOID* sock_item, ptc_msg_t* msg)
{
	TOOL_INT32 ret = tool_sock_item_recvfrom((tool_sock_item_t *)sock_item, msg, sizeof(ptc_msg_t));
	TOOL_INT32 head_len = sizeof(ptc_head_t);
	if (ret < head_len)
		return -1;
	if (msg->head.flag != PTC_FLAG)
	{
		log_error("msg->head.flag(0x%08x)", msg->head.flag);
		return -1;
	}
	if (msg->head.len > PTC_MSG_SIZE)
	{
		log_error("msg->head.len(%d)", msg->head.len);
		return -1;
	}
//	log_debug("recv message(0x%08x) len(%d)", msg->head.cmd, msg->head.len);
	return 0;	
}

TOOL_INT32 tool_ptc_qh_sendFrame(TOOL_VOID* sock_item, ptc_frame_t* frame)
{
	frame->head.flag = PTC_FLAG;
	frame->head.ver = PTC_VERSION;
	if (frame->head.len == 0)
		frame->head.pack_count = 1;
	else if (frame->head.len % PTC_PACK_SIZE == 0)
		frame->head.pack_count = frame->head.len / PTC_PACK_SIZE;
	else
		frame->head.pack_count = frame->head.len / PTC_PACK_SIZE + 1;
	TOOL_INT32 i = 0;
	for (i = 0; i < frame->head.pack_count; i++)
	{
		frame->head.pack_no = i+1;
		if (frame->head.pack_no < frame->head.pack_count)
			frame->head.pack_len = PTC_PACK_SIZE;
		else
			frame->head.pack_len = frame->head.len-(i*PTC_PACK_SIZE);
		if (tool_sock_item_send((tool_sock_item_t *)sock_item, &frame->head, sizeof(ptc_head_t)) != sizeof(ptc_head_t) ||
			tool_sock_item_send((tool_sock_item_t *)sock_item, frame->data+i*PTC_PACK_SIZE, frame->head.pack_len) != frame->head.pack_len)
			return -1;
	}
//	log_debug("send Frame(0x%08x) len(%d)", frame->head.cmd, frame->head.len);
	return 0;
}


TOOL_INT32 tool_ptc_qh_recvFrame(TOOL_VOID* sock_item, ptc_frame_t* frame)
{
	TOOL_UINT32 len = 0;
	while (1)
	{
		if (tool_sock_item_recv((tool_sock_item_t *)sock_item, &frame->head, sizeof(ptc_head_t)) != sizeof(ptc_head_t))
		{
			log_error("tool_sock_item_recv");
			return -1;
		}
//		log_debug("frame->head.cmd(0x%08x)", frame->head.cmd);
		if (frame->head.flag != PTC_FLAG)
		{
			log_error("frame->head.flag(0x%08x)", frame->head.flag);
			return -1;
		}
		if (len + frame->head.pack_len >= PTC_VIDEO_FRAME_SIZE)
		{
			log_error("len(%d), frame->head.pack_len(%d)", len, frame->head.pack_len);
			return -1;
		}
		if (tool_sock_item_recv((tool_sock_item_t *)sock_item, frame->data+len, frame->head.pack_len) != frame->head.pack_len)
		{
			log_error("tool_sock_item_recv");
			return -1;
		}
		len += frame->head.pack_len;
//		log_debug("len(%d %d) pack_count(%d) pack_no(%d)", frame->head.len, len, frame->head.pack_count, frame->head.pack_no);

		if (frame->head.pack_count == frame->head.pack_no)
			break;
	}
	if (len != frame->head.len)
	{
		log_error("len(%d), frame->head.len(%d)", len, frame->head.len);
		return -1;
	} 
	frame->data[len] = 0;
	
//	ptc_frame_head_t* frame_head = (ptc_frame_head_t*)frame->data;
//	frame_head->flag = PTC_FRAME_FLAG;
//	log_debug("recv Frame(0x%08x) len(%d)", frame->head.cmd, frame->head.len);
	return 0;
}

TOOL_INT32 tool_ptc_qh_sendFrame_v2(TOOL_VOID* sock_item, ptc_frame_t* frame, TOOL_INT32 seed)
{
	frame->head.flag = PTC_FLAG;
	frame->head.ver = PTC_VERSION;
	if (frame->head.len == 0)
		frame->head.pack_count = 1;
	else if (frame->head.len % PTC_PACK_SIZE == 0)
		frame->head.pack_count = frame->head.len / PTC_PACK_SIZE;
	else
		frame->head.pack_count = frame->head.len / PTC_PACK_SIZE + 1;

	if (frame->head.pack_count > PTC_PACK_NUM)
	{
		log_error("pack_count(%d) PTC_PACK_SIZE(%d) len(%d)", frame->head.pack_count, PTC_PACK_SIZE, frame->head.len);
		return -1;
	}
	TOOL_INT32 array[PTC_PACK_NUM] = {0};
	tool_ran_wash(seed, array, frame->head.pack_count);
	
	TOOL_INT32 i = 0;
	for (i = 0; i < frame->head.pack_count; i++)
	{
		frame->head.pack_no = i+1;
		if (frame->head.pack_no < frame->head.pack_count)
			frame->head.pack_len = PTC_PACK_SIZE;
		else
			frame->head.pack_len = frame->head.len-(i*PTC_PACK_SIZE);
		if (tool_sock_item_send((tool_sock_item_t *)sock_item, &frame->head, sizeof(ptc_head_t)) != sizeof(ptc_head_t) ||
			tool_sock_item_send((tool_sock_item_t *)sock_item, frame->data+array[i]*PTC_PACK_SIZE, frame->head.pack_len) != frame->head.pack_len)
			return -1;
	}
//	log_debug("send Frame(0x%08x) len(%d)", frame->head.cmd, frame->head.len);
	return 0;
}

TOOL_INT32 tool_ptc_qh_recvFrame_v2(TOOL_VOID* sock_item, ptc_frame_t* frame, TOOL_INT32 seed)
{
	TOOL_UINT32 len = 0;
	TOOL_INT32 array[PTC_PACK_NUM] = {0};
	tool_ran_wash(seed, array, frame->head.pack_count);
	
	while (1)
	{
		if (tool_sock_item_recv((tool_sock_item_t *)sock_item, &frame->head, sizeof(ptc_head_t)) != sizeof(ptc_head_t))
		{
			log_error("tool_sock_item_recv");
			return -1;
		}
		if (frame->head.flag != PTC_FLAG)
		{
			log_error("frame->head.flag(0x%08x)", frame->head.flag);
			return -1;
		}
		if (len + frame->head.pack_len > PTC_VIDEO_FRAME_SIZE)
		{
			log_error("len(%d), frame->head.pack_len(%d)", len, frame->head.pack_len);
			return -1;
		}
		if (tool_sock_item_recv((tool_sock_item_t *)sock_item, frame->data+len, frame->head.pack_len) != frame->head.pack_len)
		{
			log_error("tool_sock_item_recv");
			return -1;
		}
		len += frame->head.pack_len;
//		log_debug("len(%d %d) pack_count(%d) pack_no(%d)", frame->head.len, len, frame->head.pack_count, frame->head.pack_no);

		if (frame->head.pack_count == frame->head.pack_no)
			break;
	}
	if (len != frame->head.len)
	{
		log_error("len(%d), frame->head.len(%d)", len, frame->head.len);
		return -1;
	} 
//	ptc_frame_head_t* frame_head = (ptc_frame_head_t*)frame->data;
//	frame_head->flag = PTC_FRAME_FLAG;
//	log_debug("recv Frame(0x%08x) len(%d)", frame->head.cmd, frame->head.len);
	return 0;
}

TOOL_INT8* tool_ptc_qh_buildRect(ptc_rect_t* pst_rect, TOOL_INT8* buf, TOOL_INT32 len)
{
	snprintf(buf, len, "(%f,%f);(%f,%f);(%f,%f);(%f,%f);", 
		pst_rect->right_down.x, pst_rect->right_down.y,
		pst_rect->left_down.x, pst_rect->left_down.y,
		pst_rect->left_up.x, pst_rect->left_up.y,
		pst_rect->right_up.x, pst_rect->right_up.y);
	return buf;
}

TOOL_INT32 tool_ptc_qh_parseRect(ptc_rect_t* pst_rect, TOOL_INT8* buf)
{
	TOOL_INT32 ret = sscanf(buf, "(%f,%f);(%f,%f);(%f,%f);(%f,%f);", 
		&pst_rect->right_down.x, &pst_rect->right_down.y,
		&pst_rect->left_down.x,  &pst_rect->left_down.y,
		&pst_rect->left_up.x,    &pst_rect->left_up.y,
		&pst_rect->right_up.x,   &pst_rect->right_up.y);
/*
	log_debug("(%f,%f);(%f,%f);(%f,%f);(%f,%f);", 
		pst_rect->right_down.x, pst_rect->right_down.y,
		pst_rect->left_down.x,  pst_rect->left_down.y,
		pst_rect->left_up.x,    pst_rect->left_up.y,
		pst_rect->right_up.x,   pst_rect->right_up.y);
*/
	return ret;
}

TOOL_INT8* tool_ptc_qh_buildIPv4(TOOL_UINT8* pch_ip, TOOL_INT8* buf, TOOL_INT32 len)
{
	snprintf(buf, len, "%d.%d.%d.%d", pch_ip[0], pch_ip[1], pch_ip[2], pch_ip[3]);
	return buf;
}

TOOL_INT32 tool_ptc_qh_parseIPv4(TOOL_UINT8* pch_ip, TOOL_INT8* buf)
{
	TOOL_INT32 ip0 = 0;
	TOOL_INT32 ip1 = 0;
	TOOL_INT32 ip2 = 0;
	TOOL_INT32 ip3 = 0;
	
	TOOL_INT32 ret = sscanf(buf, "%d.%d.%d.%d", &ip0, &ip1, &ip2, &ip3);
	pch_ip[0] = ip0;
	pch_ip[1] = ip1;
	pch_ip[2] = ip2;
	pch_ip[3] = ip3;
	return ret;
}

TOOL_INT8* tool_ptc_qh_encodeNonce(TOOL_INT32 nonce, TOOL_INT8* buf)
{
	TOOL_INT8 nonce_str[64] = {0};
	TOOL_INT32 nonce_len = snprintf(nonce_str, sizeof(nonce_str), "hdcctv7com_%d", nonce);

	TOOL_UINT8 nonce_byte[16] = {0};
	tool_md5_calc((TOOL_UINT8*)nonce_str, nonce_len, nonce_byte);
	tool_str_byte2str(nonce_byte, 16, buf);
//	log_state("nonce_md5(%s) nonce(%d)", buf, nonce);
	return buf;	
}

//////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_ctrlPtz(ptc_msg_t* pst_msg, TOOL_INT32 channel, PTC_PTZ_CMD_E ptz, TOOL_INT32 param)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"channel\":\"%d\","
			"\"ptz\":\"%d\","
			"\"param\":\"%d\""
		"}",
		PTC_CMD_CFG2_CTRL_PTZ, channel, ptz, param);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_ctrlPtz(ptc_msg_t* pst_msg, TOOL_INT32* pi_channel, PTC_PTZ_CMD_E* pe_ptz, TOOL_INT32* pi_param)
{
	*pi_channel = 0;
	*pe_ptz = (PTC_PTZ_CMD_E)0;
	*pi_param = 0;
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "channel", &tmp) == 0)
		*pi_channel = tmp;
	if (tool_json2_getNumber(pst_msg->data, "ptz", &tmp) == 0)
		*pe_ptz = (PTC_PTZ_CMD_E)tmp;
	if (tool_json2_getNumber(pst_msg->data, "param", &tmp) == 0)
		*pi_param = tmp;
	return 0;
}

TOOL_INT32 tool_ptc_qh_c2d_encode_syncTime(ptc_msg_t* pst_msg, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"utc_sec\":\"%d\","
			"\"zone\":\"%d\","
			"\"dst_hour\":\"%d\""
		"}",
		PTC_CMD_CFG2_SYNC_TIME, utc_sec, zone, dst_hour);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_syncTime(ptc_msg_t* pst_msg, TOOL_UINT32* pu_utc_sec, TOOL_INT32* pi_zone, TOOL_INT32* pi_dst_hour)
{
	*pu_utc_sec = 0;
	*pi_zone = 0;
	*pi_dst_hour = 0;
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "utc_sec", &tmp) == 0)
		*pu_utc_sec = tmp;
	if (tool_json2_getNumber(pst_msg->data, "zone", &tmp) == 0)
		*pi_zone = tmp;
	if (tool_json2_getNumber(pst_msg->data, "dst_hour", &tmp) == 0)
		*pi_dst_hour = tmp;
	return 0;
}

TOOL_INT32 tool_ptc_qh_c2d_encode_getAbility(ptc_frame_t* pst_frame)
{
	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"cmd\":\"%d\""
		"}",
		PTC_CMD_SEARCH2_ABILITY);
	return 0;	
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getAbility(ptc_frame_t* pst_frame){return 0;}
TOOL_INT32 tool_ptc_qh_d2c_encode_getAbility(ptc_frame_t* pst_frame, ptc_cfg_ability_t* pst_cfg_ability)
{
	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"result\":\"%d\","
			"\"base.device_type\":\"%d\","
			"\"base.video_input_num\":\"%d\","
			"\"base.audio_input_num\":\"%d\","
			"\"base.alarm_input_num\":\"%d\","
			"\"base.alarm_output_num\":\"%d\","
			"\"base.dev_id\":\"%s\","
			"\"audio_ability.enable\":\"%d\","
			"\"audio_ability.sample_rate\":\"%d\","
			"\"audio_ability.bit_width\":\"%d\","
			"\"audio_ability.encode_type\":\"%d\","
			"\"audio_ability.frame_size\":\"%d\""
		"}", 
		PTC_OK, pst_cfg_ability->base.device_type, pst_cfg_ability->base.video_input_num, pst_cfg_ability->base.audio_input_num,
		pst_cfg_ability->base.alarm_input_num, pst_cfg_ability->base.alarm_output_num, pst_cfg_ability->base.dev_id,
		pst_cfg_ability->audio_ability.enable, pst_cfg_ability->audio_ability.sample_rate, pst_cfg_ability->audio_ability.bit_width,
		pst_cfg_ability->audio_ability.encode_type, pst_cfg_ability->audio_ability.frame_size);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getAbility(ptc_frame_t* pst_frame, ptc_cfg_ability_t* pst_cfg_ability)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_frame->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return tmp*(-1);
	}
	
	tool_mem_memset(pst_cfg_ability, sizeof(ptc_cfg_ability_t));

	if (tool_json2_getNumber(pst_frame->data, "base.device_type", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->base.device_type = tmp;

	if (tool_json2_getNumber(pst_frame->data, "base.video_input_num", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->base.video_input_num = tmp;

	if (tool_json2_getNumber(pst_frame->data, "base.audio_input_num", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->base.audio_input_num = tmp;

	if (tool_json2_getNumber(pst_frame->data, "base.alarm_input_num", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->base.alarm_input_num = tmp;

	if (tool_json2_getNumber(pst_frame->data, "base.alarm_output_num", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->base.alarm_output_num = tmp;

	if (tool_json2_getValue(pst_frame->data, "base.dev_id", pst_cfg_ability->base.dev_id, sizeof(pst_cfg_ability->base.dev_id)) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}

	if (tool_json2_getNumber(pst_frame->data, "audio_ability.enable", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->audio_ability.enable = tmp;

	if (tool_json2_getNumber(pst_frame->data, "audio_ability.sample_rate", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->audio_ability.sample_rate = tmp;

	if (tool_json2_getNumber(pst_frame->data, "audio_ability.bit_width", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->audio_ability.bit_width = tmp;

	if (tool_json2_getNumber(pst_frame->data, "audio_ability.encode_type", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->audio_ability.encode_type = tmp;

	if (tool_json2_getNumber(pst_frame->data, "audio_ability.frame_size", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_frame->data);
		return -8;
	}
	pst_cfg_ability->audio_ability.frame_size = tmp;

	pst_cfg_ability->video_input_num = pst_cfg_ability->base.video_input_num;
	pst_cfg_ability->audio_input_num = pst_cfg_ability->base.audio_input_num;
	pst_cfg_ability->alarm_input_num = pst_cfg_ability->base.alarm_input_num;
	pst_cfg_ability->alarm_output_num = pst_cfg_ability->base.alarm_output_num;
	
	return 0;
}

TOOL_INT32 tool_ptc_qh_c2d_encode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2)
{
	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"start_time\":\"%d\","
			"\"end_time\":\"%d\""
		"}",
		PTC_CMD_SEARCH2_LOGS, pst_logs_v2->start_time, pst_logs_v2->end_time);
	return 0;	
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_frame->data, "start_time", &tmp) < 0)
		return -8;
	pst_logs_v2->start_time = tmp;
	if (tool_json2_getNumber(pst_frame->data, "end_time", &tmp) < 0)
		return -8;
	pst_logs_v2->end_time = tmp;
	return 0;	
}
TOOL_INT32 tool_ptc_qh_d2c_encode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2)
{
	if (pst_logs_v2->num > PTC_LOGS_SIZE)
	{
		log_error("pst_logs_v2->num(%d)", pst_logs_v2->num);
		return -1;
	}

	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"result\":\"%d\","
			"\"num\":\"%d\","
			"\"log\":"
			"[", 
		PTC_OK, pst_logs_v2->num);

	TOOL_INT8 tmp_buf[128] = {0};
	TOOL_INT32 tmp_len = 0 ;
	TOOL_INT32 i = 0;
	for (i = 0; i < pst_logs_v2->num; i++)
	{
		if (i != 0)
		{
			tool_str_strcat((TOOL_INT8*)pst_frame->data, ",");
			pst_frame->head.len ++;
		}
		
		tmp_len = snprintf(tmp_buf, sizeof(tmp_buf), 
			"{"
				"\"time\":\"%d\","
				"\"type\":\"%d\","
				"\"sub_type\":\"%d\","
				"\"channel\":\"%d\","
				"\"ch_type\":\"%d\","
				"\"sub_type2\":\"%d\","
				"\"ip\":\"%d.%d.%d.%d\","
				"\"data\":\"%s\""
			"}", 
			pst_logs_v2->log_v2[i].time, pst_logs_v2->log_v2[i].type, pst_logs_v2->log_v2[i].sub_type,
			pst_logs_v2->log_v2[i].channel, pst_logs_v2->log_v2[i].ch_type, pst_logs_v2->log_v2[i].sub_type2,
			pst_logs_v2->log_v2[i].ip[0], pst_logs_v2->log_v2[i].ip[1], pst_logs_v2->log_v2[i].ip[2], pst_logs_v2->log_v2[i].ip[3],
			pst_logs_v2->log_v2[i].data);
		tool_str_strcat((TOOL_INT8*)pst_frame->data, tmp_buf);
		pst_frame->head.len += tmp_len;
		if (pst_frame->head.len > PTC_VIDEO_FRAME_SIZE-1024)
			break;
	}

	tool_str_strcat((TOOL_INT8*)pst_frame->data, "]}");
	pst_frame->head.len += 2;
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getLogs(ptc_frame_t* pst_frame, ptc_logs_v2_t* pst_logs_v2)
{
	TOOL_INT32 ret = 0;
	if (tool_json2_getNumber(pst_frame->data, "result", &ret) < 0)
		return -8;
	if (ret != 0)
		return ret*(-1);
	
	TOOL_INT32 x_size = PTC_LOGS_SIZE;
	TOOL_INT32 y_size = 512;
	TOOL_INT8* pch_array_value = (TOOL_INT8*)tool_mem_malloc(x_size*y_size, 0);
	TOOL_INT32 i = 0;
	TOOL_INT32 num = 0;
	TOOL_INT32 tmp = 0;
	pst_logs_v2->num = 0;
	ret = 0;
	do
	{
		num = tool_json2_getArray(pst_frame->data, "log", pch_array_value, x_size, y_size);
		if (num < 0)
		{
			ret = -8;
			break;
		}
		for (i = 0; i < num; i++)
		{
			if (pst_logs_v2->num >= PTC_LOGS_SIZE)
				break;
			tool_mem_memset(&pst_logs_v2->log_v2[i], sizeof(ptc_log_v2_t));
			if (tool_json2_getNumber(&pch_array_value[i*y_size], "time", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_logs_v2->log_v2[i].time = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "type", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_logs_v2->log_v2[i].type = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "sub_type", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_logs_v2->log_v2[i].sub_type = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "channel", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_logs_v2->log_v2[i].channel = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "ch_type", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_logs_v2->log_v2[i].ch_type = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "sub_type2", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_logs_v2->log_v2[i].sub_type2 = tmp;

			if (tool_json2_getIpv4(&pch_array_value[i*y_size], "ip", pst_logs_v2->log_v2[i].ip) < 0)
			{
				ret = -8;
				break;
			}

			if (tool_json2_getValue(&pch_array_value[i*y_size], "data", pst_logs_v2->log_v2[i].data, 64) < 0)
			{
				ret = -8;
				break;
			}

			pst_logs_v2->num ++;
		}
	
	} while (0);
	tool_mem_free(pch_array_value);
	pch_array_value = NULL;
	return 0;		
}

TOOL_INT32 tool_ptc_qh_c2d_encode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records)
{
	TOOL_INT32 channel = 0;
	TOOL_INT32 i = 0;
	for (i = 0; i < 64; i++)
	{
		if (pst_records->channel & (1<<i))
		{
			channel = i;
			break;
		}
	}

	
	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"start_time\":\"%d\","
			"\"end_time\":\"%d\","
			"\"channel\":\"%d\","
			"\"type\":\"%d\","
			"\"sub_type\":\"%d\""
		"}",
		PTC_CMD_SEARCH2_RECORDS, pst_records->start_time, pst_records->end_time, 
		channel, pst_records->type, pst_records->sub_type);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_frame->data, "start_time", &tmp) < 0)
		return -8;
	pst_records->start_time = tmp;
	
	if (tool_json2_getNumber(pst_frame->data, "end_time", &tmp) < 0)
		return -8;
	pst_records->end_time = tmp;
	
	if (tool_json2_getNumber(pst_frame->data, "channel", &tmp) < 0)
		return -8;
	pst_records->channel = tmp;
	
	if (tool_json2_getNumber(pst_frame->data, "type", &tmp) < 0)
		return -8;
	pst_records->type = tmp;

	if (tool_json2_getNumber(pst_frame->data, "sub_type", &tmp) < 0)
		return -8;
	pst_records->sub_type = tmp;
	
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records)
{
	if (pst_records->num > PTC_RECORDS_SIZE)
	{
		log_error("pst_records->num(%d)", pst_records->num);
		return -1;
	}

	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"result\":\"%d\","
			"\"num\":\"%d\","
			"\"record\":"
			"[", 
		PTC_OK, pst_records->num);

	TOOL_INT8 tmp_buf[512] = {0};
	TOOL_INT32 tmp_len = 0 ;
	TOOL_INT8 base64_buf[256] = {0};

	TOOL_INT32 i = 0;
	for (i = 0; i < pst_records->num; i++)
	{
		if (i != 0)
		{
			tool_str_strcat((TOOL_INT8*)pst_frame->data, ",");
			pst_frame->head.len ++;
		}

		tool_mem_memset(base64_buf, sizeof(base64_buf));
		base64_encode(pst_records->record[i].file_info, base64_buf, sizeof(pst_records->record[i].file_info));
//		log_debug("base64_buf(%s)", base64_buf);

		tmp_len = snprintf(tmp_buf, sizeof(tmp_buf), 
			"{"
				"\"start_time\":\"%d\","
				"\"end_time\":\"%d\","
				"\"size\":\"%d\","
				"\"channel\":\"%d\","
				"\"type\":\"%d\","
				"\"sub_type\":\"%d\","
				"\"file_info_base64\":\"%s\""
			"}", 
			pst_records->record[i].start_time, pst_records->record[i].end_time, pst_records->record[i].size,
			pst_records->record[i].channel, pst_records->record[i].type, pst_records->record[i].sub_type,
			base64_buf);
		tool_str_strcat((TOOL_INT8*)pst_frame->data, tmp_buf);
		pst_frame->head.len += tmp_len;
		if (pst_frame->head.len > PTC_VIDEO_FRAME_SIZE-1024)
			break;
	}

	tool_str_strcat((TOOL_INT8*)pst_frame->data, "]}");
	pst_frame->head.len += 2;
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getRecords(ptc_frame_t* pst_frame, ptc_cfg_records_t* pst_records)
{
	TOOL_INT32 ret = 0;
	if (tool_json2_getNumber(pst_frame->data, "result", &ret) < 0)
		return -8;
	if (ret != 0)
		return ret*(-1);

	TOOL_INT32 x_size = PTC_RECORDS_SIZE;
	TOOL_INT32 y_size = 512;
	TOOL_INT8* pch_array_value = (TOOL_INT8*)tool_mem_malloc(x_size*y_size, 0);
	TOOL_INT32 i = 0;
	TOOL_INT32 num = 0;
	TOOL_INT32 tmp = 0;
	TOOL_INT8 file_info_base64[512] = {0};
	ret = 0;
	pst_records->num = 0;
	do
	{
		num = tool_json2_getArray(pst_frame->data, "record", pch_array_value, x_size, y_size);
		if (num < 0)
		{
			log_error("pst_frame->data(%s)", pst_frame->data);
			ret = -8;
			break;
		}
		for (i = 0; i < num; i++)
		{
			if (pst_records->num >= PTC_RECORDS_SIZE)
				break;
			tool_mem_memset(&pst_records->record[i], sizeof(ptc_cfg_record_t));
			if (tool_json2_getNumber(&pch_array_value[i*y_size], "start_time", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_records->record[i].start_time = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "end_time", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_records->record[i].end_time = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "size", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_records->record[i].size = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "channel", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_records->record[i].channel = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "type", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_records->record[i].type = tmp;

			if (tool_json2_getNumber(&pch_array_value[i*y_size], "sub_type", &tmp) < 0)
			{
				ret = -8;
				break;
			}
			pst_records->record[i].sub_type = tmp;

			if (tool_json2_getValue(&pch_array_value[i*y_size], "file_info_base64", file_info_base64, sizeof(file_info_base64)) < 0)
			{
				ret = -8;
				break;
			}
			base64_decode(file_info_base64, pst_records->record[i].file_info);

			pst_records->num ++;
		}
	
	} while (0);
	tool_mem_free(pch_array_value);
	pch_array_value = NULL;
	return 0;	
}



//////////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_getStatus(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\""
		"}",
		PTC_CMD_CFG2_GET_STATUS);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getStatus(ptc_msg_t* pst_msg){return 0;}
TOOL_INT32 tool_ptc_qh_d2c_encode_getStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\","
			"\"io_out\":\"%lld\""
		"}", 
		PTC_OK, pst_cfg_status->io_out);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
		return tmp*(-1);
	
	if (tool_json2_getNumber(pst_msg->data, "io_out", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	tool_mem_memset(pst_cfg_status, sizeof(ptc_cfg_status_t));
	pst_cfg_status->io_out = tmp;
	return 0;
}

/////////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_setStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"io_out\":\"%lld\""
		"}",
		PTC_CMD_CFG2_SET_STATUS, pst_cfg_status->io_out);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_setStatus(ptc_msg_t* pst_msg, ptc_cfg_status_t* pst_cfg_status)
{
	tool_mem_memset(pst_cfg_status, sizeof(ptc_cfg_status_t));
	TOOL_INT32 io_out = 0;
	if (tool_json2_getNumber((TOOL_INT8*)pst_msg->data, "io_out", &io_out) == 0)
		pst_cfg_status->io_out = io_out;
	return 0;	
}
TOOL_INT32 tool_ptc_qh_d2c_encode_setStatus(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\""
		"}", 
		PTC_OK);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_setStatus(ptc_msg_t* pst_msg)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
		return tmp*(-1);
	return 0;
}


//////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_getAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E alarm_type)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"alarm_type\":\"%d\""
		"}",
		PTC_CMD_CFG2_GET_ALARM, alarm_type);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E* pe_alarm_type)
{
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), "{\"result\":\"%d\"}", PTC_ERROR_JSON);
		return PTC_ERROR_JSON;
	}
	
	TOOL_INT8* index = tool_json_getValue(&st_json, "alarm_type");
	if (index == NULL)
	{
		pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), "{\"result\":\"%d\"}", PTC_ERROR_NO_ALARM_TYPE);
		return PTC_ERROR_JSON;
	}
	*pe_alarm_type = (PTC_ALARM_TYPE_E)tool_sysf_atoi(index);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_getAlarm(ptc_msg_t* pst_msg, ptc_cfg_alarm_t* pst_cfg_alarm)
{
	TOOL_INT8 str[256] = {0};
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\","
			"\"enable\":\"%d\","
			"\"rect\":\"%s\""
		"}", 
		PTC_OK, pst_cfg_alarm->enable, tool_ptc_qh_buildRect(&pst_cfg_alarm->rect, str, sizeof(str)));
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getAlarm(ptc_msg_t* pst_msg, ptc_cfg_alarm_t* pst_cfg_alarm)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
		return tmp*(-1);
	
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}

	tool_mem_memset(pst_cfg_alarm, sizeof(ptc_cfg_alarm_t));
	TOOL_INT8* index = tool_json_getValue(&st_json, "enable");
	if (index)
		pst_cfg_alarm->enable = tool_sysf_atoi(index);
	index = tool_json_getValue(&st_json, "rect");
	if (index)
		tool_ptc_qh_parseRect(&pst_cfg_alarm->rect, index);
	index = tool_json_getValue(&st_json, "drt");
	if (index)
		pst_cfg_alarm->drt = (PTC_VMF_LINE_DRT_E)tool_sysf_atoi(index);
	return 0;
}

//////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_setAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* pst_cfg_alarm)
{
	TOOL_INT8 str_rect[256] = {0};	
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"alarm_type\":\"%d\","
			"\"enable\":\"%d\","
			"\"rect\":\"%s\","
			"\"drt\":\"%d\""
		"}",
		PTC_CMD_CFG2_SET_ALARM, alarm_type, pst_cfg_alarm->enable, tool_ptc_qh_buildRect(&pst_cfg_alarm->rect, str_rect, sizeof(str_rect)),
		pst_cfg_alarm->drt);	
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_setAlarm(ptc_msg_t* pst_msg, PTC_ALARM_TYPE_E* pe_alarm_type, ptc_cfg_alarm_t* pst_cfg_alarm)
{
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), "{\"result\":\"%d\"}", PTC_ERROR_JSON);
		return PTC_ERROR_JSON;
	}
	
	TOOL_INT8* index = tool_json_getValue(&st_json, "alarm_type");
	if (index == NULL)
	{
		pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), "{\"result\":\"%d\"}", PTC_ERROR_NO_ALARM_TYPE);
		return PTC_ERROR_JSON;
	}
	*pe_alarm_type = (PTC_ALARM_TYPE_E)tool_sysf_atoi(index);
	tool_mem_memset(pst_cfg_alarm, sizeof(ptc_cfg_alarm_t));
	index = tool_json_getValue(&st_json, "enable");
	if (index)
		pst_cfg_alarm->enable = tool_sysf_atoi(index);
	index = tool_json_getValue(&st_json, "rect");
	if (index)
		tool_ptc_qh_parseRect(&pst_cfg_alarm->rect, index);	
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_setAlarm(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\""
		"}", 
		PTC_OK);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_setAlarm(ptc_msg_t* pst_msg)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
		return tmp*(-1);
	return 0;
}

//////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_getWorkMode(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\""
		"}",
		PTC_CMD_CFG2_GET_WORKMODE);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getWorkMode(ptc_msg_t* pst_msg){return 0;}
TOOL_INT32 tool_ptc_qh_d2c_encode_getWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\","
			"\"mode\":\"%d\""
		"}", 
		PTC_OK, pst_cfg_workMode->mode);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	tool_mem_memset(pst_cfg_workMode, sizeof(ptc_cfg_workMode_t));
	if (tool_json2_getNumber(pst_msg->data, "mode", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	pst_cfg_workMode->mode = (PTC_WORK_MODE_E)tmp;
	return 0;
}

//////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_setWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"mode\":\"%d\""
		"}",
		PTC_CMD_CFG2_SET_WORKMODE, pst_cfg_workMode->mode);
	return 0;
}

TOOL_INT32 tool_ptc_qh_c2d_decode_setWorkMode(ptc_msg_t* pst_msg, ptc_cfg_workMode_t* pst_cfg_workMode)
{
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), "{\"result\":\"%d\"}", PTC_ERROR_JSON);
		return PTC_ERROR_JSON;
	}
	tool_mem_memset(pst_cfg_workMode, sizeof(ptc_cfg_workMode_t));
	TOOL_INT8* index = tool_json_getValue(&st_json, "mode");
	if (index)
		pst_cfg_workMode->mode = (PTC_WORK_MODE_E)tool_sysf_atoi(index);
	return 0;
}

TOOL_INT32 tool_ptc_qh_d2c_encode_setWorkMode(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\""
		"}", 
		PTC_OK);
	return 0;
}

TOOL_INT32 tool_ptc_qh_d2c_decode_setWorkMode(ptc_msg_t* pst_msg)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_getMobile(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\""
		"}",
		PTC_CMD_CFG2_GET_MOBILE);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getMobile(ptc_msg_t* pst_msg){return 0;}
TOOL_INT32 tool_ptc_qh_d2c_encode_getMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\","
			"\"enable_3g\":\"%d\","
			"\"status_3g\":\"%d\""
		"}", 
		PTC_OK, pst_cfg_mobile->enable_3g, pst_cfg_mobile->status_3g);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	tool_mem_memset(pst_cfg_mobile, sizeof(ptc_cfg_mobile_t));
	if (tool_json2_getNumber(pst_msg->data, "enable_3g", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	pst_cfg_mobile->enable_3g = tmp;

	if (tool_json2_getNumber(pst_msg->data, "status_3g", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	pst_cfg_mobile->status_3g = tmp;
	return 0;
}

/////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_setMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"enable_3g\":\"%d\","
			"\"status_3g\":\"%d\""
		"}",
		PTC_CMD_CFG2_SET_MOBILE, pst_cfg_mobile->enable_3g, pst_cfg_mobile->status_3g);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_setMobile(ptc_msg_t* pst_msg, ptc_cfg_mobile_t* pst_cfg_mobile)
{
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	tool_mem_memset(pst_cfg_mobile, sizeof(ptc_cfg_mobile_t));
	TOOL_INT8* index = tool_json_getValue(&st_json, "enable_3g");
	if (index)
		pst_cfg_mobile->enable_3g = tool_sysf_atoi(index);
	index = tool_json_getValue(&st_json, "status_3g");
	if (index)
		pst_cfg_mobile->status_3g= tool_sysf_atoi(index);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_setMobile(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\""
		"}", 
		PTC_OK);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_setMobile(ptc_msg_t* pst_msg)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	return 0;
}

////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_getWifi(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\""
		"}",
		PTC_CMD_CFG2_GET_WIFI);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getWifi(ptc_msg_t* pst_msg){return 0;}
TOOL_INT32 tool_ptc_qh_d2c_encode_getWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\","
			"\"mode\":\"%d\","
			"\"client_ssid\":\"%s\","
			"\"client_pswd\":\"%s\","
			"\"client_level\":\"%d\","
			"\"client_dhcp\":\"%d\","
			"\"client_ip\":\"%d.%d.%d.%d\","
			"\"client_mask\":\"%d.%d.%d.%d\","
			"\"client_gate\":\"%d.%d.%d.%d\","
			"\"client_dns1\":\"%d.%d.%d.%d\","
			"\"client_dns2\":\"%d.%d.%d.%d\","
			"\"ap_ssid\":\"%s\","
			"\"ap_pswd\":\"%s\""
		"}", 
		PTC_OK, pst_cfg_wifi->mode, pst_cfg_wifi->client_ssid, pst_cfg_wifi->client_pswd, pst_cfg_wifi->client_level,
		pst_cfg_wifi->client_dhcp, 
		pst_cfg_wifi->client_ip[0], pst_cfg_wifi->client_ip[1], pst_cfg_wifi->client_ip[2], pst_cfg_wifi->client_ip[3],
		pst_cfg_wifi->client_mask[0], pst_cfg_wifi->client_mask[1], pst_cfg_wifi->client_mask[2], pst_cfg_wifi->client_mask[3],
		pst_cfg_wifi->client_gate[0], pst_cfg_wifi->client_gate[1], pst_cfg_wifi->client_gate[2], pst_cfg_wifi->client_gate[3],
		pst_cfg_wifi->client_dns1[0], pst_cfg_wifi->client_dns1[1], pst_cfg_wifi->client_dns1[2], pst_cfg_wifi->client_dns1[3],
		pst_cfg_wifi->client_dns2[0], pst_cfg_wifi->client_dns2[1], pst_cfg_wifi->client_dns2[2], pst_cfg_wifi->client_dns2[3],
		pst_cfg_wifi->ap_ssid, pst_cfg_wifi->ap_pswd);
	log_debug("pst_msg->data(%s)", pst_msg->data);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	TOOL_INT8* result = tool_json_getValue(&st_json, "result");
	if (result == NULL)
	{
		log_error("result(%d)", result);
		return -8;
	}
	if (tool_str_strncmp(result, "0", 1) != 0)
	{
		log_error("result(%s)", result);
		return -8;
	}
	
	tool_mem_memset(pst_cfg_wifi, sizeof(ptc_cfg_wifi_t));
	TOOL_INT8* index = tool_json_getValue(&st_json, "mode");
	if (index)
		pst_cfg_wifi->mode = (PTC_WiFi_MODE_E)tool_sysf_atoi(index);
	
	index = tool_json_getValue(&st_json, "client_ssid");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->client_ssid, index, sizeof(pst_cfg_wifi->client_ssid)-1);
	
	index = tool_json_getValue(&st_json, "client_pswd");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->client_pswd, index, sizeof(pst_cfg_wifi->client_pswd)-1);
	
	index = tool_json_getValue(&st_json, "client_level");
	if (index)
		pst_cfg_wifi->client_level = tool_sysf_atoi(index);

	index = tool_json_getValue(&st_json, "client_dhcp");
	if (index)
		pst_cfg_wifi->client_dhcp = tool_sysf_atoi(index);
	
	index = tool_json_getValue(&st_json, "client_ip");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_ip, index);
	
	index = tool_json_getValue(&st_json, "client_mask");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_mask, index);
	
	index = tool_json_getValue(&st_json, "client_gate");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_gate, index);

	index = tool_json_getValue(&st_json, "client_dns1");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_dns1, index);

	index = tool_json_getValue(&st_json, "client_dns2");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_dns2, index);
	
	index = tool_json_getValue(&st_json, "ap_ssid");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->ap_ssid, index, sizeof(pst_cfg_wifi->ap_ssid)-1);
	
	index = tool_json_getValue(&st_json, "ap_pswd");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->ap_pswd, index, sizeof(pst_cfg_wifi->ap_pswd)-1);
	return 0;
}

///////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_setWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
	"{"
		"\"cmd\":\"%d\","
		"\"mode\":\"%d\","
		"\"client_ssid\":\"%s\","
		"\"client_pswd\":\"%s\","
		"\"client_level\":\"%d\","
		"\"client_dhcp\":\"%d\","
		"\"client_ip\":\"%d.%d.%d.%d\","
		"\"client_mask\":\"%d.%d.%d.%d\","
		"\"client_gate\":\"%d.%d.%d.%d\","
		"\"client_dns1\":\"%d.%d.%d.%d\","
		"\"client_dns2\":\"%d.%d.%d.%d\","			
		"\"ap_ssid\":\"%s\","
		"\"ap_pswd\":\"%s\""
	"}",
	PTC_CMD_CFG2_SET_WIFI, pst_cfg_wifi->mode, pst_cfg_wifi->client_ssid, pst_cfg_wifi->client_pswd, pst_cfg_wifi->client_level, 
	pst_cfg_wifi->client_dhcp, 
	pst_cfg_wifi->client_ip[0], pst_cfg_wifi->client_ip[1], pst_cfg_wifi->client_ip[2], pst_cfg_wifi->client_ip[3],
	pst_cfg_wifi->client_mask[0], pst_cfg_wifi->client_mask[1], pst_cfg_wifi->client_mask[2], pst_cfg_wifi->client_mask[3],
	pst_cfg_wifi->client_gate[0], pst_cfg_wifi->client_gate[1], pst_cfg_wifi->client_gate[2], pst_cfg_wifi->client_gate[3],
	pst_cfg_wifi->client_dns1[0], pst_cfg_wifi->client_dns1[1], pst_cfg_wifi->client_dns1[2], pst_cfg_wifi->client_dns1[3],
	pst_cfg_wifi->client_dns2[0], pst_cfg_wifi->client_dns2[1], pst_cfg_wifi->client_dns2[2], pst_cfg_wifi->client_dns2[3],
	pst_cfg_wifi->ap_ssid, pst_cfg_wifi->ap_pswd);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_setWifi(ptc_msg_t* pst_msg, ptc_cfg_wifi_t* pst_cfg_wifi)
{
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	tool_mem_memset(pst_cfg_wifi, sizeof(ptc_cfg_wifi_t));
	TOOL_INT8* index = tool_json_getValue(&st_json, "mode");
	if (index)
		pst_cfg_wifi->mode = (PTC_WiFi_MODE_E)tool_sysf_atoi(index);
	
	index = tool_json_getValue(&st_json, "client_ssid");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->client_ssid, index, sizeof(pst_cfg_wifi->client_ssid)-1);
	
	index = tool_json_getValue(&st_json, "client_pswd");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->client_pswd, index, sizeof(pst_cfg_wifi->client_pswd)-1);
	
	index = tool_json_getValue(&st_json, "client_level");
	if (index)
		pst_cfg_wifi->client_level = tool_sysf_atoi(index);
	
	index = tool_json_getValue(&st_json, "client_dhcp");
	if (index)
		pst_cfg_wifi->client_dhcp = tool_sysf_atoi(index);
	
	index = tool_json_getValue(&st_json, "client_ip");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_ip, index);
	
	index = tool_json_getValue(&st_json, "client_mask");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_mask, index);
	
	index = tool_json_getValue(&st_json, "client_gate");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_gate, index);
	
	index = tool_json_getValue(&st_json, "client_dns1");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_dns1, index);
	
	index = tool_json_getValue(&st_json, "client_dns2");
	if (index)
		tool_ptc_qh_parseIPv4(pst_cfg_wifi->client_dns2, index);
	
	index = tool_json_getValue(&st_json, "ap_ssid");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->ap_ssid, index, sizeof(pst_cfg_wifi->ap_ssid)-1);
	
	index = tool_json_getValue(&st_json, "ap_pswd");
	if (index)
		tool_str_strncpy(pst_cfg_wifi->ap_pswd, index, sizeof(pst_cfg_wifi->ap_pswd)-1);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_setWifi(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\""
		"}", 
		PTC_OK);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_setWifi(ptc_msg_t* pst_msg)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	return 0;
}

//////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_getDevRecord(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\""
		"}",
		PTC_CMD_CFG2_GET_DEVRECORD);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getDevRecord(ptc_msg_t* pst_msg){return 0;}
TOOL_INT32 tool_ptc_qh_d2c_encode_getDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\","
			"\"enable\":\"%d\","
			"\"cover_type\":\"%d\","
			"\"state\":\"%d\","
			"\"total_size\":\"%d\","
			"\"left_size\":\"%d\""
		"}", 
		PTC_OK, pst_cfg_devRecord->enable, pst_cfg_devRecord->cover_type, pst_cfg_devRecord->state, 
		pst_cfg_devRecord->total_size, pst_cfg_devRecord->left_size);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}
	
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	TOOL_INT8* result = tool_json_getValue(&st_json, "result");
	if (result == NULL)
	{
		log_error("result(%d)", result);
		return -8;
	}
	if (tool_str_strncmp(result, "0", 1) != 0)
	{
		log_error("result(%s)", result);
		return -8;
	}
	
	tool_mem_memset(pst_cfg_devRecord, sizeof(ptc_cfg_devRecord_t));
	TOOL_INT8* index = tool_json_getValue(&st_json, "enable");
	if (index)
		pst_cfg_devRecord->enable = tool_sysf_atoi(index);

	index = tool_json_getValue(&st_json, "cover_type");
	if (index)
		pst_cfg_devRecord->cover_type = tool_sysf_atoi(index);

	index = tool_json_getValue(&st_json, "state");
	if (index)
		pst_cfg_devRecord->state = tool_sysf_atoi(index);

	index = tool_json_getValue(&st_json, "total_size");
	if (index)
		pst_cfg_devRecord->total_size = tool_sysf_atoi(index);

	index = tool_json_getValue(&st_json, "left_size");
	if (index)
		pst_cfg_devRecord->left_size = tool_sysf_atoi(index);
	return 0;
}

//////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_setDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"enable\":\"%d\","
			"\"cover_type\":\"%d\""
		"}",
		PTC_CMD_CFG2_SET_DEVRECORD, pst_cfg_devRecord->enable, pst_cfg_devRecord->cover_type);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_setDevRecord(ptc_msg_t* pst_msg, ptc_cfg_devRecord_t* pst_cfg_devRecord)
{
	tool_json_t st_json;
	if (tool_json_parse(&st_json, (TOOL_INT8*)pst_msg->data) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	tool_mem_memset(pst_cfg_devRecord, sizeof(ptc_cfg_devRecord_t));
	TOOL_INT8* index = tool_json_getValue(&st_json, "enable");
	if (index)
		pst_cfg_devRecord->enable = tool_sysf_atoi(index);

	index = tool_json_getValue(&st_json, "cover_type");
	if (index)
		pst_cfg_devRecord->cover_type = tool_sysf_atoi(index);
	
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_setDevRecord(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\""
		"}", 
		PTC_OK);
	return 0;
}	
TOOL_INT32 tool_ptc_qh_d2c_decode_setDevRecord(ptc_msg_t* pst_msg)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -8;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	return 0;
}


//////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_getAv(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\""
		"}",
		PTC_CMD_CFG2_GET_AV);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_getAv(ptc_msg_t* pst_msg){return 0;}
TOOL_INT32 tool_ptc_qh_d2c_encode_getAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\","
			"\"mask\":\"%d\""
		"}", 
		PTC_OK, pst_cfg_av->mask);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_getAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -1;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}

	if (tool_json2_getNumber(pst_msg->data, "mask", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -1;
	}
	pst_cfg_av->mask = tmp;
	
	return 0;
}

//////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_setAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"mask\":\"%d\""
		"}",
		PTC_CMD_CFG2_SET_AV, pst_cfg_av->mask);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_setAv(ptc_msg_t* pst_msg, ptc_cfg_av_t* pst_cfg_av)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "mask", &tmp) < 0)
	{
		pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), "{\"result\":\"%d\"}", PTC_ERROR_JSON);
		return PTC_ERROR_JSON;
	}
	pst_cfg_av->mask = tmp;
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_setAv(ptc_msg_t* pst_msg)
{
	pst_msg->head.len = snprintf((TOOL_INT8*)pst_msg->data, sizeof(pst_msg->data), 
		"{"
			"\"result\":\"%d\""
		"}", 
		PTC_OK);
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_decode_setAv(ptc_msg_t* pst_msg)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(pst_msg->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return -1;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", pst_msg->data);
		return tmp*(-1);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////
TOOL_INT32 tool_ptc_qh_c2d_encode_searchDates(ptc_frame_t* pst_frame, ptc_cfg_dates_t* pst_dates)
{
	TOOL_INT32 channel = 0;
	TOOL_INT32 i = 0;
	for (i = 0; i < 64; i++)
	{
		if (pst_dates->channel & (1<<i))
		{
			channel = i;
			break;
		}
	}
	
	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"cmd\":\"%d\","
			"\"channel\":\"%d\","
			"\"type\":\"%d\","
			"\"sub_type\":\"%d\""
		"}",
		PTC_CMD_SEARCH2_DATES, channel, pst_dates->type, pst_dates->sub_type);
	return 0;
}
TOOL_INT32 tool_ptc_qh_c2d_decode_searchDates(ptc_frame_t* pst_frame, ptc_cfg_dates_t* pst_dates)
{
	TOOL_INT32 tmp = 0;

	if (tool_json2_getNumber(pst_frame->data, "channel", &tmp) < 0)
		return -8;
	pst_dates->channel = tmp;
	
	if (tool_json2_getNumber(pst_frame->data, "type", &tmp) < 0)
		return -8;
	pst_dates->type = tmp;

	if (tool_json2_getNumber(pst_frame->data, "sub_type", &tmp) < 0)
		return -8;
	pst_dates->sub_type = tmp;
	
	return 0;
}
TOOL_INT32 tool_ptc_qh_d2c_encode_searchDates(ptc_frame_t* pst_frame, ptc_cfg_dates_t* pst_dates)
{
	if (pst_dates->num > PTC_DATES_SIZE)
	{
		log_error("pst_dates->num(%d)", pst_dates->num);
		return -1;
	}

	pst_frame->head.len = snprintf((TOOL_INT8*)pst_frame->data, sizeof(pst_frame->data), 
		"{"
			"\"result\":\"%d\","
			"\"num\":\"%d\","
			"\"date\":"
			"[", 
		PTC_OK, pst_dates->num);

	TOOL_INT8 tmp_buf[128] = {0};
	TOOL_INT32 tmp_len = 0 ;

	TOOL_INT32 i = 0;
	for (i = 0; i < pst_dates->num; i++)
	{
		if (i != 0)
		{
			tool_str_strcat((TOOL_INT8*)pst_frame->data, ",");
			pst_frame->head.len ++;
		}

		tmp_len = snprintf(tmp_buf, sizeof(tmp_buf), 
			"{"
				"\"year\":\"%d\","
				"\"month\":\"%d\","
				"\"day\":\"%d\""
			"}", 
			pst_dates->date[i].year, pst_dates->date[i].month, pst_dates->date[i].day);
		tool_str_strcat((TOOL_INT8*)pst_frame->data, tmp_buf);
		pst_frame->head.len += tmp_len;
		if (pst_frame->head.len > PTC_VIDEO_FRAME_SIZE-1024)
			break;
	}
	tool_str_strcat((TOOL_INT8*)pst_frame->data, "]}");
	pst_frame->head.len += 2;
	return 0;

}
TOOL_INT32 tool_ptc_qh_d2c_decode_searchDates(ptc_frame_t* ptc_frame_t, ptc_cfg_dates_t* pst_dates)
{
	TOOL_INT32 tmp = 0;
	if (tool_json2_getNumber(ptc_frame_t->data, "result", &tmp) < 0)
	{
		log_error("out_data(%s)", ptc_frame_t->data);
		return -1;
	}
	if (tmp != 0)
	{
		log_error("out_data(%s)", ptc_frame_t->data);
		return tmp*(-1);
	}

	TOOL_INT32 i = 0;
	TOOL_INT32 num = 0;
	TOOL_INT32 array_x = PTC_DATES_SIZE;
	TOOL_INT32 array_y = 256;
	TOOL_INT8* pch_array_value = (TOOL_INT8*)tool_mem_malloc(array_x*array_y, 0);
	TOOL_INT32 ret = 0;	
	do
	{
		num = tool_json2_getArray(ptc_frame_t->data, "date", pch_array_value, array_x, array_y);
		log_debug("num(%d)", num);
		if (num < 0)
		{
			log_error("ptc_frame_t->data(%s)", ptc_frame_t->data);
			ret = -8;
			break;
		}

		for (i = 0; i < num; i++)
		{
			if (pst_dates->num >= PTC_RECORDS_SIZE)
				break;
			
			if (tool_json2_getNumber(&pch_array_value[i*array_y], "year", (TOOL_INT32*)&pst_dates->date[pst_dates->num].year) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "month", (TOOL_INT32*)&pst_dates->date[pst_dates->num].month) < 0 ||
				tool_json2_getNumber(&pch_array_value[i*array_y], "day", (TOOL_INT32*)&pst_dates->date[pst_dates->num].day) < 0)
			{
				log_error("ptc_frame_t->data(%s)", ptc_frame_t->data);
				ret = -8;
				break;
			}
			pst_dates->num ++;
		}

	}while (0);
	tool_mem_free(pch_array_value);
	pch_array_value = NULL;
	return ret;
}



#include "hy_ptc.h"

TOOL_INT32 tool_ptc_hy_sendMsg(TOOL_VOID* sock_item, hy_msg_t* msg)
{
	TOOL_INT32 len = sizeof(hy_msg_head_t)+msg->head.len;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, msg, len) != len)
	{
		log_error("tool_ptc_hy_sendMsg");
		return -1;
	}
	return 0;
}

TOOL_INT32 tool_ptc_hy_recvMsg(TOOL_VOID* sock_item, hy_msg_t* msg)
{
	if (tool_sock_item_recv((tool_sock_item_t*)sock_item, &msg->head, sizeof(hy_msg_head_t)) != sizeof(hy_msg_head_t))
	{
	//	log_debug("");
		return -1;
	}
	if (msg->head.len > HY_MSG_SIZE)
	{
		log_error("msg->head.len(%d) cmd(0x%08x)", msg->head.len, msg->head.cmd);
		return -1;
	}
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, msg->data, msg->head.len) != msg->head.len)
	{
	//	log_debug("");
		return -1;
	}
	return 0;
}

TOOL_INT32 tool_ptc_hy_hy2qh(tool_sock_item_t* tmp_sock_item, TOOL_UINT8* data, ptc_frame_head_t** frame_head, TOOL_UINT32* frame_len)
{
	hy_iframe_head_t* iframe = NULL;
	hy_pframe_head_t* pframe = NULL;
	hy_aframe_head_t* aframe = NULL;
	ptc_frame_head_t ptc_frame_head;
	tool_mem_memset(&ptc_frame_head, sizeof(ptc_frame_head_t));
	hy_0frame_head_t* head0 = (hy_0frame_head_t*)data;
//	log_debug("data(%04d-%02d-%02d %02d:%02d:%02d) data-len(%d)", 
//		head0->ClockTimes.year+2000, head0->ClockTimes.month, head0->ClockTimes.day, head0->ClockTimes.hour, head0->ClockTimes.minute, head0->ClockTimes.second,
//		*len);
	if (data[4] == 0xFD)
	{
		iframe = (hy_iframe_head_t*)data;
		*frame_head = (ptc_frame_head_t*)(data + sizeof(hy_iframe_head_t) - sizeof(ptc_frame_head_t));
		ptc_frame_head.len = iframe->head0.avSliceLength-sizeof(hy_frame_tail_t)-sizeof(hy_iframe_head_t);
		ptc_frame_head.no = iframe->head0.avSequence;
		tool_time_Date2Time(head0->ClockTimes.year+2000-1900, head0->ClockTimes.month-1, head0->ClockTimes.day, head0->ClockTimes.hour, head0->ClockTimes.minute, head0->ClockTimes.second, &ptc_frame_head.sec);
		ptc_frame_head.width = iframe->PicWidth*8;
		ptc_frame_head.height = iframe->PicHeight*8;
		ptc_frame_head.frame_type = PTC_FRAME_I;
		ptc_frame_head.frame_sub_type = PTC_VIDEO_ENCODE_H264;
		ptc_frame_head.frame_rate = iframe->VFps;

		tmp_sock_item->width = ptc_frame_head.width;
		tmp_sock_item->height = ptc_frame_head.height;
		tmp_sock_item->fps_video = ptc_frame_head.frame_rate;	
	}
	else if (data[4] == 0xFC)
	{
		pframe = (hy_pframe_head_t*)data;
		*frame_head = (ptc_frame_head_t*)(data + sizeof(hy_pframe_head_t) - sizeof(ptc_frame_head_t));
		
		ptc_frame_head.len = pframe->head0.avSliceLength-sizeof(hy_frame_tail_t)-sizeof(hy_pframe_head_t);
		ptc_frame_head.no = pframe->head0.avSequence;	
		tool_time_Date2Time(head0->ClockTimes.year+2000-1900, head0->ClockTimes.month-1, head0->ClockTimes.day, head0->ClockTimes.hour, head0->ClockTimes.minute, head0->ClockTimes.second, &ptc_frame_head.sec);
		ptc_frame_head.width = tmp_sock_item->width;	
		ptc_frame_head.height = tmp_sock_item->height;
		ptc_frame_head.frame_type = PTC_FRAME_P;
		ptc_frame_head.frame_sub_type = PTC_VIDEO_ENCODE_H264;
		ptc_frame_head.frame_rate = tmp_sock_item->fps_video;		
	}
	else if (data[4] == 0xF0)
	{
		aframe = (hy_aframe_head_t*)data;
		*frame_head = (ptc_frame_head_t*)(data + sizeof(hy_aframe_head_t) - sizeof(ptc_frame_head_t));
		
		ptc_frame_head.len = aframe->head0.avSliceLength-sizeof(hy_frame_tail_t)-sizeof(hy_aframe_head_t);
		ptc_frame_head.no = aframe->head0.avSequence;
		tool_time_Date2Time(head0->ClockTimes.year+2000-1900, head0->ClockTimes.month-1, head0->ClockTimes.day, head0->ClockTimes.hour, head0->ClockTimes.minute, head0->ClockTimes.second, &ptc_frame_head.sec);
		ptc_frame_head.width = 0;
		ptc_frame_head.height = 0;
		ptc_frame_head.frame_type = PTC_FRAME_A;
		ptc_frame_head.frame_sub_type = PTC_AUDIO_ENCODE_PCM;
		ptc_frame_head.frame_rate = 8;
	}
	else if (data[4] == 0xFA)
	{
		return 0;
	}
	else
	{
		log_error("type(0x%02x)", data[4]);
		return -1;
	}

	tool_mem_memcpy(*frame_head, &ptc_frame_head, sizeof(ptc_frame_head_t));
	*frame_len = ptc_frame_head.len + sizeof(ptc_frame_head_t);
	ptc_frame_head.flag = PTC_FRAME_FLAG;
	return 1;
}

TOOL_INT32 tool_ptc_hy_recvRts(TOOL_VOID* sock_item, TOOL_UINT8* data, TOOL_UINT32 *size, ptc_frame_head_t** frame_head)
{
	tool_sock_item_t* tmp_sock_item = (tool_sock_item_t*)sock_item;
	hy_msg_head_t msg_head;
	if (tool_sock_item_recv(tmp_sock_item, &msg_head, sizeof(hy_msg_head_t)) != sizeof(hy_msg_head_t))
	{		
		log_error("tool_ptc_hy_recvRts hy_msg_head_t");
		return -1;
	}
	if (msg_head.cmd != HY_CMD_RTS_FRAME)
	{
		log_error("msg_head.cmd(0x%02x)", msg_head.cmd);
		return -1;
	}

	hy_0frame_head_t* head0 = (hy_0frame_head_t*)data;
	TOOL_UINT32 len = sizeof(hy_0frame_head_t);
	if (*size < len)
	{
		log_error("*size(%d) len(%d)", *size, len);
		return -1;
	}
	if (tool_sock_item_recv(tmp_sock_item, head0, len) != len)
	{
		log_error("hy_0frame_head_t");
		return -1;
	}
	if (head0->avHead != 0x56415948)
	{
		log_error("head0.avHead(0x%08x)", head0->avHead);
		return -1;
	}
	
	if (head0->avSliceLength > *size)
	{
		log_error("head0->avSliceLength(%d) *size(%d)", head0->avSliceLength, *size);
		return -1;
	}
	len = head0->avSliceLength-sizeof(hy_0frame_head_t);
	tmp_sock_item->timeout = 60;
	if (tool_sock_item_recv(tmp_sock_item, data+sizeof(hy_0frame_head_t), len) != len)
	{
		tmp_sock_item->timeout = TOOL_SOCK_TIMEOUT;
		log_error("data(%d)", len);
		return -1;
	}
	tmp_sock_item->timeout = TOOL_SOCK_TIMEOUT;
	*size = head0->avSliceLength;
	return tool_ptc_hy_hy2qh(tmp_sock_item, data, frame_head, size);	
}

/*
TOOL_INT32 tool_ptc_hy_recvPbs(TOOL_VOID* sock_item, TOOL_UINT8* data, TOOL_UINT32 *size, TOOL_UINT8* left_buf, TOOL_UINT32* left_len, ptc_frame_head_t** frame_head)
{
	TOOL_UINT32 len = 0;
	if (*left_len > 0)
	{
		if (*size < *left_len)
		{
			log_error("*size(%d) &left_len(%d)", *size, *left_len);
			return -1;
		}
		tool_mem_memcpy(data, left_buf, *left_len);
		len = *left_len;
		*left_len = 0;
	}

	TOOL_UINT32 i = 0;
	tool_sock_item_t* tmp_sock_item = (tool_sock_item_t*)sock_item;
	hy_msg_head_t msg_head;
	hy_0frame_head_t* head0 = (hy_0frame_head_t*)data;
	hy_0frame_head_t* head_tmp = NULL;
	while (1)
	{
//		log_debug("len(%d) head0->avSliceLength(%d) head(0x%08x) avSliceType(%02x)", len, head0->avSliceLength, head0->avHead, head0->avSliceType);		
		if (len > sizeof(hy_0frame_head_t))
		{
			//adjust 
			if (head0->avHead != 0x56415948)
			{
				log_debug("begin adjust head0->avHead(0x%08x) len(%d)", head0->avHead, len);
				for (i = 0; i < len; i++)
				{
					head_tmp = (hy_0frame_head_t*)(data+i);
					if (head_tmp->avHead == 0x56415948)
					{
						tool_mem_memmove(data, data+i, len-i);
						len = len-i;
						break;
					}
				}
				if (i == len)
				{
					log_debug("adjust error");
					*left_len = 0;
					return 0;
				}
				log_debug("end	adjust head0->avHead(0x%08x)", head0->avHead);
			}

			if (len > head0->avSliceLength)	
				break;
		}

		if (tool_sock_item_recv(tmp_sock_item, &msg_head, sizeof(hy_msg_head_t)) != sizeof(hy_msg_head_t))
		{		
			log_debug("msg_head");
			return -1;
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////
//		log_debug("cmd(0x%02x 0x%02x 0x%02x 0x%02x) len(%d)", msg_head.cmd, msg_head.reserve1[0], msg_head.reserve1[1], msg_head.reserve1[2], msg_head.len);
		if (msg_head.len == 0)
		{
			log_debug("cmd(%02x %02x %02x %02x)x len(%d)d", msg_head.cmd, msg_head.reserve1[0], msg_head.reserve1[1], msg_head.reserve1[2], msg_head.len);
			return 0;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		
		if (msg_head.len + len > *size)
		{
			log_error("msg_head.len(%d) len(%d) *size(%d)", msg_head.len, len, *size);
			return -1;
		}

		if (tool_sock_item_recv(tmp_sock_item, data+len, msg_head.len) != msg_head.len)
		{		
			log_debug("data");
			return -1;
		}
		len += msg_head.len;
	}

	*size = head0->avSliceLength;
	if (len - head0->avSliceLength > 32*1024)
	{
		log_error("len(%d) head0->avSliceLength(%d)", len, head0->avSliceLength);
		return -1;
	}
	tool_mem_memcpy(left_buf, data+head0->avSliceLength, len-head0->avSliceLength);
	*left_len = len-head0->avSliceLength;
	
	return tool_ptc_hy_hy2qh(tmp_sock_item, data, size, frame_head);
}
*/

TOOL_INT32 tool_ptc_hy_recvPbsBuf(TOOL_VOID* sock_item, TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_INT32 size)
{
	hy_msg_head_t msg_head;
	tool_sock_item_t* sock = (tool_sock_item_t*)sock_item;
	if (tool_sock_item_recv(sock, &msg_head, sizeof(hy_msg_head_t)) != sizeof(hy_msg_head_t))
	{		
		log_debug("msg_head");
		return -1;
	}
	if (msg_head.len == 0)
	{
		log_debug("cmd(%02x %02x %02x %02x)x len(%d)d", msg_head.cmd, msg_head.reserve1[0], msg_head.reserve1[1], msg_head.reserve1[2], msg_head.len);
		return 0;
	}
	if (msg_head.len + *len > size)
	{
		log_error("msg_head.len(%d) *len(%d) size(%d)", msg_head.len, *len, size);
		return -1;
	}

	if (tool_sock_item_recv(sock, buf+*len, msg_head.len) != msg_head.len)
	{		
		log_debug("data");
		return -1;
	}
	*len += msg_head.len;	
	return 0;	
}

TOOL_INT32 tool_ptc_hy_checkPbsBuf(TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_UINT8** left_buf, TOOL_UINT32* left_len)
{
	if (*len < sizeof(hy_0frame_head_t))
		return 0;
	
	TOOL_UINT32 i = 0;
	hy_0frame_head_t* head0 = (hy_0frame_head_t*)buf;
	hy_0frame_head_t* head_tmp = NULL;
	if (head0->avHead != 0x56415948)
	{
		log_debug("begin adjust head0->avHead(0x%08x) *len(%d)", head0->avHead, *len);
		for (i = 0; i < *len; i++)
		{
			head_tmp = (hy_0frame_head_t*)(buf+i);
			if (head_tmp->avHead == 0x56415948)
			{
				tool_mem_memmove(buf, buf+i, *len-i);
				*len = *len-i;
				break;
			}
		}
		if (i == *len)
		{
			log_debug("adjust error");
			*len = 0;
			return 0;
		}
		log_debug("end	adjust head0->avHead(0x%08x)", head0->avHead);
	}

	if (*len < head0->avSliceLength) 
		return 0;

	*left_buf = buf+head0->avSliceLength;
	*left_len = *len-head0->avSliceLength;
	return 1;
}

TOOL_INT32 tool_ptc_hy_recvPbs(TOOL_VOID* sock_item, TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_UINT32 size, 
	ptc_frame_head_t** frame_head, TOOL_UINT32* frame_len,
	TOOL_UINT8** left_buf, TOOL_UINT32* left_len)
{
	if (*left_len > 0)
	{
		tool_mem_memmove(buf, *left_buf, *left_len);
		*len = *left_len;
		*left_len = 0;
	}
	
	if (tool_ptc_hy_checkPbsBuf(buf, len, left_buf, left_len))
	{
		return tool_ptc_hy_hy2qh((tool_sock_item_t *)sock_item, buf, frame_head, frame_len);
	}

	TOOL_INT32 ret = tool_ptc_hy_recvPbsBuf(sock_item, buf, len, size);
	if (ret < 0)
		return ret;
	if (tool_ptc_hy_checkPbsBuf(buf, len, left_buf, left_len))
	{
		return tool_ptc_hy_hy2qh((tool_sock_item_t *)sock_item, buf, frame_head, frame_len);
	}
	return 0;
}

TOOL_INT32 tool_ptc_hy_sendTalk_msg(TOOL_VOID* sock_item, hy_msg_t* msg)
{
	TOOL_UINT8 buf[32] = {0};
	buf[0] = 0x1D;
	buf[8] = 0x02;
	buf[13] = 0x01;
	TOOL_UINT32* body_len = (TOOL_UINT32*)&buf[4];
	*body_len = msg->head.len;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, buf, 32) != 32)
	{
		log_error("head");
		return -1;
	}
	TOOL_INT32 len = sizeof(hy_msg_head_t)+msg->head.len;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, msg, len) != len)
	{
		log_error("body");
		return -1;
	}
	return 0;	
}

TOOL_INT32 tool_ptc_hy_sendTalk_frame(TOOL_VOID* sock_item, TOOL_UINT8* data, TOOL_UINT32 len)
{
/*
	TOOL_UINT8 buf[32] = {0};
	buf[0] = 0x1D;
	buf[4] = 0x2C;
	buf[5] = 0x04;
	buf[8] = 0x02;
	buf[13] = 0x01;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, buf, 32) != 32)
	{
		log_error("head");
		return -1;
	}

	log_debug("len(%d)", len);
	ptc_frame_head_t* qh_frame_head = (ptc_frame_head_t*)data;
	if (len != 2048)
	{
		log_error("len(%d)", len);
		return -1;
	}
	hy_tframe_head_t hy_tframe_head;
	tool_mem_memset(&hy_tframe_head, sizeof(hy_tframe_head_t));
	hy_tframe_head.head0.avHead = 0x56415948;
	hy_tframe_head.head0.avSliceType = 0xF0;
	hy_tframe_head.head0.avSequence = 0;
	hy_tframe_head.head0.avSliceLength = 0x042C;
	tool_date_t date;
	tool_sysf_dateLocal(&date, 0);
	hy_tframe_head.head0.ClockTimes.year = date.year-2000;
	hy_tframe_head.head0.ClockTimes.month = date.month;
	hy_tframe_head.head0.ClockTimes.day = date.day;
	hy_tframe_head.head0.ClockTimes.hour = date.hour;
	hy_tframe_head.head0.ClockTimes.minute = date.minute;
	hy_tframe_head.head0.ClockTimes.second = date.second;
	
	hy_tframe_head.head0.msTimes = 0;
	hy_tframe_head.AFomatMask = 0x83;
	hy_tframe_head.AenSoundmode = 0x01;
	hy_tframe_head.AenPayloadType = 0x10;		//wInBitsPerSample;	 2иж?и┤иж??ии		
	hy_tframe_head.AenSamplerate = 0x04;
	hy_tframe_head.reserve[0] = 0x88;
	hy_tframe_head.reserve[0] = 0xFF;
	hy_tframe_head.reserve[0] = 0xFF;
	hy_tframe_head.reserve[0] = 0xFF;
	hy_tframe_head.reserve[0] = 0xFF;
	hy_tframe_head.reserve[7] = 0x02;
	hy_frame_tail_t frame_tail;
	tool_mem_memset(&frame_tail, sizeof(hy_frame_tail_t));
	frame_tail.avTail = 0x56415948;
	frame_tail.avSliceLength = 0x042C;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &hy_tframe_head, sizeof(hy_tframe_head_t)) != sizeof(hy_tframe_head_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, data, 1024) != 1024)
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &frame_tail, sizeof(hy_frame_tail_t)) != sizeof(hy_frame_tail_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}

	hy_tframe_head.head0.avSequence = 0;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, buf, 32) != 32)
	{
		log_error("head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &hy_tframe_head, sizeof(hy_tframe_head_t)) != sizeof(hy_tframe_head_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, data+1024, 1024) != 1024)
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &frame_tail, sizeof(hy_frame_tail_t)) != sizeof(hy_frame_tail_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	return 0;
*/

	
	TOOL_UINT8 buf[32] = {0};
	buf[0] = 0x1D;
	buf[4] = 0x2C;
	buf[5] = 0x04;
	buf[8] = 0x02;
	buf[13] = 0x01;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, buf, 32) != 32)
	{
		log_error("head");
		return -1;
	}

	ptc_frame_head_t* qh_frame_head = (ptc_frame_head_t*)data;
	if (qh_frame_head->len != 2048)
	{
		log_error("qh_frame_head->len(%d)", qh_frame_head->len);
		return -1;
	}
	hy_tframe_head_t hy_tframe_head;
	tool_mem_memset(&hy_tframe_head, sizeof(hy_tframe_head_t));
	hy_tframe_head.head0.avHead = 0x56415948;
	hy_tframe_head.head0.avSliceType = 0xF0;
	hy_tframe_head.head0.avSequence = qh_frame_head->no*2;
	hy_tframe_head.head0.avSliceLength = 0x042C;
	tool_date_t date;
	tool_sysf_dateLocal(&date, qh_frame_head->sec);
	hy_tframe_head.head0.ClockTimes.year = date.year-2000;
	hy_tframe_head.head0.ClockTimes.month = date.month;
	hy_tframe_head.head0.ClockTimes.day = date.day;
	hy_tframe_head.head0.ClockTimes.hour = date.hour;
	hy_tframe_head.head0.ClockTimes.minute = date.minute;
	hy_tframe_head.head0.ClockTimes.second = date.second;
	
	hy_tframe_head.head0.msTimes = (qh_frame_head->sec % 1000)*1000 + (qh_frame_head->usec/1000);
	hy_tframe_head.AFomatMask = 0x83;
	hy_tframe_head.AenSoundmode = 0x01;
	hy_tframe_head.AenPayloadType = 0x10;		//wInBitsPerSample;	 2иж?и┤иж??ии		
	hy_tframe_head.AenSamplerate = 0x04;
	hy_tframe_head.reserve[0] = 0x88;
	hy_tframe_head.reserve[7] = 0x02;
	hy_frame_tail_t frame_tail;
	tool_mem_memset(&frame_tail, sizeof(hy_frame_tail_t));
	frame_tail.avTail = 0x56415948;
	frame_tail.avSliceLength = 0x042C;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &hy_tframe_head, sizeof(hy_tframe_head_t)) != sizeof(hy_tframe_head_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, data+sizeof(ptc_frame_head_t), 1024) != 1024)
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &frame_tail, sizeof(hy_frame_tail_t)) != sizeof(hy_frame_tail_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}

	hy_tframe_head.head0.avSequence = qh_frame_head->no*2+1;
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, buf, 32) != 32)
	{
		log_error("head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &hy_tframe_head, sizeof(hy_tframe_head_t)) != sizeof(hy_tframe_head_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, data+sizeof(ptc_frame_head_t)+1024, 1024) != 1024)
	{
		log_error("tool_sock_item_send head");
		return -1;
	}
	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &frame_tail, sizeof(hy_frame_tail_t)) != sizeof(hy_frame_tail_t))
	{
		log_error("tool_sock_item_send head");
		return -1;
	}

	return 0;
}

#ifdef __IS_SSL__

#include "tool_ssl.h"
#include <stdio.h>

//TOOL_INT32 tool_ptc_apple_sendQpns(TOOL_VOID* tssl, ptc_qpns_msg_s2c_t* msg)
TOOL_INT32 tool_ptc_apple_sendQpns(TOOL_VOID* tssl, hdcctv_qpns_alarm_t* pst_alarm, TOOL_INT8* alert, TOOL_INT32 load_badge)
{
	TOOL_INT8 buf[512] = {0};
	TOOL_INT32 len = 0;
	TOOL_INT8* cmd = &buf[len];
	*cmd = 1;
	len += 1;
	TOOL_INT32* identifier = (TOOL_INT32*)&buf[len];
	*identifier = pst_alarm->no;
	len += 4;

	tool_time_t tt;
	tool_sysf_time(&tt);
	TOOL_INT32* expiry = (TOOL_INT32*)&buf[len];
	*expiry = tt.sec+24*3600;;
	len += 4;
	
	TOOL_INT16* token_len = (TOOL_INT16*)&buf[len];
	*token_len = tool_sysf_htons(32);
	len += 2;

	TOOL_UINT8* token = (TOOL_UINT8*)&buf[len];
//	tool_mem_memcpy(token, msg->token, 32);
	tool_str_str2byte(pst_alarm->token, 64, token);
	len += 32;
	TOOL_INT16* load_len = (TOOL_INT16*)&buf[len];
	len += 2;
	TOOL_INT8* load = &buf[len];
	TOOL_INT32 ret = snprintf(load, 256, 
		"{\"aps\":{\"alert\":\"%s\",\"badge\":%d,\"sound\":\"load_sound\"},\"dev_id\":\"%s\",\"time\":\"%d\",\"channel\":\"%d\",\"type\":\"%d\"}", 
		alert, load_badge, pst_alarm->dev_id, pst_alarm->time, pst_alarm->channel, pst_alarm->type);
	*load_len = tool_sysf_htons(ret);
	len += ret;
//	log_debug("load(%s)", load);
	if (tool_ssl_write(tssl, buf, len) != len)
		return -1;
	return 0;
}

/*
 *   0 No errors encountered
 *   1 Processing error
 *   2 Missing device token
 *   3 Missing topic
 *   4 Missing payload
 *   5 Invalid token size
 *   6 Invalid topic size
 *   7 Invalid payload size
 *   8 Invalid token
 *  10 Shutdown
 * 255 None (unknown)
 */

TOOL_INT32 tool_ptc_apple_recvQpns(TOOL_VOID* tssl, ptc_qpns_msg_c2s_t* msg)
{
	TOOL_INT32 ret = tool_ssl_pend(tssl);
//	log_debug("tool_ssl_pend ret(%d)", ret);
	if (ret < 6)
		return 0;
	
	TOOL_INT8 buf[16] = {0};
	ret = tool_ssl_read(tssl, buf, 6);
	if (ret < 0)
		return ret;
	
	log_debug("%02x %02x %02x %02x %02x %02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	return ret;
}

#endif

#ifndef __NO_FFMPEG__

#include "lan_h264.h"

TOOL_VOID tool_ptc_rtp_check(tool_sock_item_t* sock, rtp_tcp_head_t* pst_tcp_head)
{
	if (pst_tcp_head->flag != 0x24 && pst_tcp_head->channel > 4)
		return;
	
	TOOL_INT32 pkt_len = tool_sysf_ntohs(pst_tcp_head->len);
	TOOL_UINT8 buf[RTP_PKT_SIZE] = {0};
	TOOL_INT32 len = 0;	

	len = tool_sock_item_recvPeek(sock, buf, sizeof(buf));
	if (len < 0)
		return ;

	rtp_tcp_head_t* pst_tcp_head2 = NULL;
	TOOL_INT32 pkt_len2 = 0;

	if (pkt_len + 4 <= len)
	{
		pst_tcp_head2 = (rtp_tcp_head_t*)&buf[pkt_len];
		pkt_len2 = tool_sysf_ntohs(pst_tcp_head2->len);
		if (pst_tcp_head2->flag == 0x24 && pst_tcp_head2->channel <= 4 && pkt_len2 > sizeof(rtp_head_t) && pkt_len2 < RTP_PKT_SIZE)
			return;
//		log_debug("fd(%d) (0x%0x2 0x%x 0x%04x)", sock->fd, pst_tcp_head2->flag, pst_tcp_head2->channel, pkt_len2);
	}

//	log_debug("pkt_len(%d) len(%d)", pkt_len, len);

	TOOL_INT32 i = 0;
/*	
	for (i = 0; i < len; i++)
	{
		printf("%02x ", buf[i]);
		if (i % 20 == 19)
			printf("\n");
	}
	printf("\n\n");
*/
	for (i = 0; i < len; i++)
	{
		pst_tcp_head2 = (rtp_tcp_head_t*)&buf[i];
		pkt_len2 = tool_sysf_ntohs(pst_tcp_head2->len);
		if (pst_tcp_head2->flag == 0x24 && pst_tcp_head2->channel <= 4 && pkt_len2 > sizeof(rtp_head_t) && pkt_len2 < RTP_PKT_SIZE)
		{
			pst_tcp_head->len = tool_sysf_htons(i);
			log_state("tool_ptc_rtp_check(%d)=>(%d)", pkt_len, i);
			return;
		}
	}
}

TOOL_INT32 tool_ptc_rtp_recvTcp_video(TOOL_VOID* sock_item, ptc_frame_video_t* frame_data)
{
	tool_sock_item_t* sock = (tool_sock_item_t*)sock_item;
	rtp_tcp_head_t tcp_head;
	tool_mem_memset(&tcp_head, sizeof(rtp_tcp_head_t));
	rtp_head_t rtp_head;
	tool_mem_memset(&rtp_head, sizeof(rtp_head_t));
	TOOL_UINT32 pkt_len = 0;
	TOOL_UINT32 data_len = 0;
	TOOL_UINT32 len = 0;
	TOOL_INT32 sps_len = 0;
	int last_ts = -1;

	while (1)
	{		
		if (tool_sock_item_recv(sock, &tcp_head, sizeof(rtp_tcp_head_t)) != sizeof(rtp_tcp_head_t))
		{
			log_error("tool_sysf_read tcp_head");
			return -1;
		}
		
	//	tool_ptc_rtp_check(sock, &tcp_head);

		pkt_len = tool_sysf_ntohs(tcp_head.len);
		
		if (tcp_head.flag != 0x24 || pkt_len < sizeof(rtp_head_t) || pkt_len > RTP_PKT_SIZE)
		{
			log_error("tcp_head.flag(%d) tcp_head.channel(%d) pkt_len(%d)", tcp_head.flag, tcp_head.channel, pkt_len);
			return -1;
		}
		if (tool_sock_item_recv(sock, &rtp_head, sizeof(rtp_head_t)) != sizeof(rtp_head_t))
		{
			log_error("tool_sysf_read rtp_head");
			return -1;
		}
		if (last_ts != rtp_head.ts)
		{
		    len = 0;
		    sps_len = 0;
		}
		data_len = pkt_len-sizeof(rtp_head_t);
		if (len + data_len + 4 > PTC_VIDEO_FRAME_SIZE)
		{
			log_error("len(%d) data_len(%d) > PTC_VIDEO_FRAME_SIZE(%d)", len, data_len, PTC_VIDEO_FRAME_SIZE);
			return -1;
		}
//		log_debug("pt(%d) seq(%d) ts(%d) ssrc(%d) m(%d)", 
//			rtp_head.pt, tool_sysf_ntohs(rtp_head.seq), tool_sysf_ntohl(rtp_head.ts), tool_sysf_ntohl(rtp_head.ssrc), rtp_head.m);

		if (rtp_head.pt == RTP_PT_H264 || rtp_head.pt == 99 || rtp_head.pt == 100 || rtp_head.pt == 98)
		{
			rtp_fu_indicator_t fu_indicator;
			if (tool_sock_item_recv(sock, &fu_indicator, 1) != 1)
			{
				log_error("tool_sysf_read fu_indicator");
				return -1;
			}
			data_len --;
//			log_debug("type(%d)", fu_indicator.type);
			if (fu_indicator.type == RTP_PKT_TYPE_FUA)
			{
				rtp_fu_head_t fu_head;
				if (tool_sock_item_recv(sock, &fu_head, 1) != 1)
				{
					log_error("tool_sysf_read fu_head");
					return -1;
				}
				data_len --;
				fu_indicator.type = fu_head.type;
				if (fu_head.s)
				{
					frame_data->frame_body[len]   = 0x0;
					frame_data->frame_body[len+1] = 0x0;
					frame_data->frame_body[len+2] = 0x0;
					frame_data->frame_body[len+3] = 0x1;
					len += 4;				
					tool_mem_memcpy(&frame_data->frame_body[len], &fu_indicator, 1);
					len ++;
				}
			}
			else
			{
				frame_data->frame_body[len]   = 0x0;
				frame_data->frame_body[len+1] = 0x0;
				frame_data->frame_body[len+2] = 0x0;
				frame_data->frame_body[len+3] = 0x1;
				len += 4;
				tool_mem_memcpy(&frame_data->frame_body[len], &fu_indicator, 1);
				len ++;
			}
			
			if (fu_indicator.nri == 3)
				frame_data->frame_head.frame_type = PTC_FRAME_I;
			else if (fu_indicator.nri == 2|| fu_indicator.nri == 0)
				frame_data->frame_head.frame_type = PTC_FRAME_P;
			else
				frame_data->frame_head.frame_type = PTC_FRAME_I;
			frame_data->frame_head.frame_sub_type = PTC_VIDEO_ENCODE_H264;
//			log_debug("nri(%d) type(%d)", fu_indicator->nri, fu_indicator->type);			
			
			//sps
			if (fu_indicator.type == RTP_PKT_TYPE_SPS)
				sps_len = data_len;
		}	
		else if (rtp_head.pt == RTP_PT_PCMU )
		{
			frame_data->frame_head.frame_type = PTC_FRAME_A;
			frame_data->frame_head.frame_sub_type = PTC_AUDIO_ENCODE_G711U;
		}
		else if (rtp_head.pt == RTP_PT_PCMA )
		{
			frame_data->frame_head.frame_type = PTC_FRAME_A;
			frame_data->frame_head.frame_sub_type = PTC_AUDIO_ENCODE_G711A;
		}
		else
		{
			frame_data->frame_head.frame_type = PTC_FRAME_NONE;
			frame_data->frame_head.frame_sub_type = PTC_AUDIO_ENCODE_NONE;
		}
		
		if (tool_sock_item_recv(sock, &frame_data->frame_body[len], data_len) != data_len)
		{
			log_error("tool_sysf_read data");
			return -1;
		}
		len += data_len;

		if (rtp_head.m == 1 || frame_data->frame_head.frame_type == PTC_FRAME_A)
			break;
		last_ts = rtp_head.ts;
	}
	
	frame_data->frame_head.len = len;
	if (sps_len)
	{		
//		log_debug("len(%d) sps: (%02x %02x %02x %02x) %02x %02x %02x %02x %02x %02x %02x %02x %02x", sps_len,
//			frame_data->frame_body[0], frame_data->frame_body[1], frame_data->frame_body[2], frame_data->frame_body[3],
//			frame_data->frame_body[4], frame_data->frame_body[5], frame_data->frame_body[6],  frame_data->frame_body[7], 
//			frame_data->frame_body[8], frame_data->frame_body[9], frame_data->frame_body[10], frame_data->frame_body[11],
//			frame_data->frame_body[12]);
		lan_H264Context h;
		lan_init_lan_get_bits(&h.gb, &frame_data->frame_body[5], sps_len*8);
		lan_ff_h264_decode_seq_parameter_set(&h, &sock->width, &sock->height);
		if (sock->width == 1920 && sock->height == 1088)
			sock->height = 1080;
	}
	frame_data->frame_head.width = sock->width;
	frame_data->frame_head.height = sock->height;
	//////////////////////////////////////////////////////

	tool_time_t tt;
	tool_sysf_time(&tt);
	if (sock->sec != tt.sec)
	{
		sock->sec = tt.sec;
		sock->count_video[9] = sock->count_video[8];
		sock->count_video[8] = sock->count_video[7];
		sock->count_video[7] = sock->count_video[6];
		sock->count_video[6] = sock->count_video[5];
		sock->count_video[5] = sock->count_video[4];
		sock->count_video[4] = sock->count_video[3];
		sock->count_video[3] = sock->count_video[2];
		sock->count_video[2] = sock->count_video[1];
		sock->count_video[1] = sock->count_video[0];
		sock->count_video[0] = 0;
		sock->fps_video = (sock->count_video[1]+sock->count_video[2]+sock->count_video[3]+sock->count_video[4]+sock->count_video[5]+sock->count_video[6]+sock->count_video[7]+sock->count_video[8]+sock->count_video[9])/9;
		sock->count_audio[9] = sock->count_audio[8];
		sock->count_audio[8] = sock->count_audio[7];
		sock->count_audio[7] = sock->count_audio[6];
		sock->count_audio[6] = sock->count_audio[5];
		sock->count_audio[5] = sock->count_audio[4];
		sock->count_audio[4] = sock->count_audio[3];
		sock->count_audio[3] = sock->count_audio[2];
		sock->count_audio[2] = sock->count_audio[1];
		sock->count_audio[1] = sock->count_audio[0];
		sock->count_audio[0] = 0;
		sock->fps_auido = (sock->count_audio[1]+sock->count_audio[2]+sock->count_audio[3]+sock->count_audio[4]+sock->count_audio[5]+sock->count_audio[6]+sock->count_audio[7]+sock->count_audio[8]+sock->count_audio[9])/9;
	}
	if (frame_data->frame_head.frame_type == PTC_FRAME_I || frame_data->frame_head.frame_type == PTC_FRAME_P)
	{
		sock->count_video[0] ++;
		frame_data->frame_head.frame_rate = sock->fps_video;
	}
	else if (frame_data->frame_head.frame_type == PTC_FRAME_A)
	{
		sock->count_audio[0] ++;
		frame_data->frame_head.frame_rate = sock->fps_auido;
	}
	frame_data->frame_head.sec  = tt.sec;
	frame_data->frame_head.usec = tt.usec;
/*
	if (frame_data->frame_head.frame_type == PTC_FRAME_I || frame_data->frame_head.frame_type == PTC_FRAME_P)
	{
		printf("len(%08x) ", frame_data->frame_head.len);
		TOOL_INT32 i = 0;
		for (i = 0; i < 40; i++)
			printf("%02x ", frame_data->frame_body[i]);
		printf("\n");
	}
*/
	return 0;
}

TOOL_INT32 tool_ptc_rtp_recvTcp_talk(TOOL_VOID* sock_item, ptc_frame_audio_t* frame_data)
{
	tool_sock_item_t* sock = (tool_sock_item_t*)sock_item;
	rtp_tcp_head_t tcp_head;
	tool_mem_memset(&tcp_head, sizeof(rtp_tcp_head_t));
	rtp_head_t rtp_head;
	tool_mem_memset(&rtp_head, sizeof(rtp_head_t));
	TOOL_UINT32 pkt_len = 0;
	TOOL_UINT32 data_len = 0;
	TOOL_UINT32 len = 0;
	while (1)
	{		
		if (tool_sock_item_recv(sock, &tcp_head, sizeof(rtp_tcp_head_t)) != sizeof(rtp_tcp_head_t))
		{
			log_error("tool_sysf_read tcp_head");
			return -1;
		}
		pkt_len = tool_sysf_ntohs(tcp_head.len);
		if (tcp_head.flag != 0x24 || pkt_len < sizeof(rtp_head_t) || pkt_len > RTP_PKT_SIZE)
		{
			log_error("pkt_len(%d)", pkt_len);
			return -1;
		}
		if (tool_sock_item_recv(sock, &rtp_head, sizeof(rtp_head_t)) != sizeof(rtp_head_t))
		{
			log_error("tool_sysf_read rtp_head");
			return -1;
		}
		data_len = pkt_len-sizeof(rtp_head_t);
		if (len + data_len > PTC_AUDIO_FRAME_SIZE)
		{
			log_error("len(%d) data_len(%d) > PTC_AUDIO_FRAME_SIZE(%d)", len, data_len, PTC_AUDIO_FRAME_SIZE);
			return -1;
		}
//		log_debug("pt(%d) seq(%d) ts(%d) ssrc(%d) m(%d)", 
//			rtp_head.pt, tool_sysf_ntohs(rtp_head.seq), tool_sysf_ntohl(rtp_head.ts), tool_sysf_ntohl(rtp_head.ssrc), rtp_head.m);

		if (rtp_head.pt == RTP_PT_PCMU)
		{
			frame_data->frame_head.frame_type = PTC_FRAME_A;
			frame_data->frame_head.frame_sub_type = PTC_AUDIO_ENCODE_G711U;
		}
		else if (rtp_head.pt == RTP_PT_PCMA)
		{
			frame_data->frame_head.frame_type = PTC_FRAME_A;
			frame_data->frame_head.frame_sub_type = PTC_AUDIO_ENCODE_G711A;
		}
		else
		{
			frame_data->frame_head.frame_type = PTC_FRAME_NONE;
			frame_data->frame_head.frame_sub_type = PTC_AUDIO_ENCODE_NONE;
		}
		
		if (tool_sock_item_recv(sock, &frame_data->frame_body[len], data_len) != data_len)
		{
			log_error("tool_sysf_read data");
			return -1;
		}
		len += data_len;

		if (rtp_head.m == 1)
			break;
	}
	
	frame_data->frame_head.len = len;

	tool_time_t tt;
	tool_sysf_time(&tt);
	frame_data->frame_head.frame_rate = 25;
	frame_data->frame_head.sec  = tt.sec;
	frame_data->frame_head.usec = tt.usec;

	return 0;
}

#endif


TOOL_INT32 tool_ptc_rtp_sendTcp_talk(TOOL_VOID* sock_item, ptc_frame_audio_t* frame_data)
{
	TOOL_INT8* tmp = (TOOL_INT8*)frame_data->frame_body;
	tmp -= (sizeof(rtp_tcp_head_t)+sizeof(rtp_head_t));
	
	tool_sock_item_t* sock = (tool_sock_item_t*)sock_item;
	rtp_tcp_head_t* tcp_head = (rtp_tcp_head_t*)tmp;
	tool_mem_memset(tcp_head, sizeof(rtp_tcp_head_t));
	tcp_head->flag = 0x24;
	tcp_head->len = tool_sysf_htons(YS_TALK_FRAME_SIZE+sizeof(rtp_head_t));
	
	rtp_head_t* rtp_head = (rtp_head_t*)(tmp+sizeof(rtp_tcp_head_t));
	tool_mem_memset(rtp_head, sizeof(rtp_head_t));
	rtp_head->v = 2;
	rtp_head->p = 0;
	rtp_head->x = 0;
	rtp_head->cc = 0;
	rtp_head->m = 1;
	rtp_head->pt = RTP_PT_PCMU;
	sock->seq ++;
	rtp_head->seq = tool_sysf_htons(sock->seq);
	rtp_head->ts = 0;
	rtp_head->ssrc = 0;

	TOOL_INT32 len = sizeof(rtp_tcp_head_t)+sizeof(rtp_head_t)+YS_TALK_FRAME_SIZE;
	if (tool_sock_item_send(sock, tmp, len) != len)
	{
		log_error("tool_ptc_rtp_sendTcp_audio");
		return -1;
	}
	return 0;
}

TOOL_VOID tool_ptc_ys_fillIeHead_login_c2d(ys_msg_login_c2d_t* login_c2d)
{
	login_c2d->user_simple.stIEHdr.u16IEID = YS_IE_ID_USER_SIMPLE_INFO;
	login_c2d->user_simple.stIEHdr.u16BodyLen = sizeof(ys_ie_user_simple_t)-sizeof(ys_ie_head_t);

	login_c2d->ip_address.stIEHdr.u16IEID = YS_IE_ID_IP_ADDRESS;
	login_c2d->ip_address.stIEHdr.u16BodyLen = sizeof(ys_ie_ip_address_t)-sizeof(ys_ie_head_t);

	login_c2d->user_log_type.stIEHdr.u16IEID = YS_IE_ID_USER_LOGIN_TYPE;
	login_c2d->user_log_type.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_type_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_logout_c2d(ys_msg_logout_c2d_t* logout_c2d)
{
	logout_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	logout_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	logout_c2d->user_log_type.stIEHdr.u16IEID = YS_IE_ID_USER_LOGIN_TYPE;
	logout_c2d->user_log_type.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_type_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_heartbeat_c2d(ys_msg_heartbeat_c2d_t* heartbeat_c2d)
{
	heartbeat_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	heartbeat_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_startrts_c2d(ys_msg_startrts_c2d_t* startrts_c2d)
{
	startrts_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	startrts_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	startrts_c2d->resource_code.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	startrts_c2d->resource_code.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	startrts_c2d->media_ip.stIEHdr.u16IEID = YS_IE_ID_MEDIA_IP;
	startrts_c2d->media_ip.stIEHdr.u16BodyLen = sizeof(ys_ie_media_ip_t)-sizeof(ys_ie_head_t);

	startrts_c2d->media_transport.stIEHdr.u16IEID = YS_IE_ID_MEDIA_TRANSPORT;
	startrts_c2d->media_transport.stIEHdr.u16BodyLen = sizeof(ys_ie_media_transport_t)-sizeof(ys_ie_head_t);

	startrts_c2d->video_index.stIEHdr.u16IEID = YS_IE_ID_MEDIA_VIDEO_STREAM_INDEX;
	startrts_c2d->video_index.stIEHdr.u16BodyLen = sizeof(ys_ie_video_index_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_stoprts_c2d(ys_msg_stoprts_c2d_t* stoprts_c2d)
{
	stoprts_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	stoprts_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	stoprts_c2d->task_no.stIEHdr.u16IEID = YS_IE_ID_TASK_NO;
	stoprts_c2d->task_no.stIEHdr.u16BodyLen = sizeof(ys_ie_task_no_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_ptz_c2d(ys_msg_ptz_c2d_t* ptz_c2d)
{
	ptz_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	ptz_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	ptz_c2d->resource_code.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	ptz_c2d->resource_code.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	ptz_c2d->ptz_command.stIEHdr.u16IEID = YS_IE_ID_PTZ_COMMAND;
	ptz_c2d->ptz_command.stIEHdr.u16BodyLen = sizeof(ys_ie_ptz_command_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_searchrecords_c2d(ys_msg_searchrecords_c2d_t* pst_searchrecords_c2d)
{
	pst_searchrecords_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_searchrecords_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_searchrecords_c2d->resource_code.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	pst_searchrecords_c2d->resource_code.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	pst_searchrecords_c2d->time_range.stIEHdr.u16IEID = YS_IE_ID_TIME_RANGE;
	pst_searchrecords_c2d->time_range.stIEHdr.u16BodyLen = sizeof(ys_ie_time_range_t)-sizeof(ys_ie_head_t);

//	pst_searchrecords_c2d->storage_type.stIEHdr.u16IEID = YS_IE_ID_STORAGE_TYPE;
//	pst_searchrecords_c2d->storage_type.stIEHdr.u16BodyLen = sizeof(ys_ie_storage_type_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_searchlogs_c2d(ys_msg_searchlogs_c2d_t* pst_searchlogs_c2d)
{
	pst_searchlogs_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_searchlogs_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_searchlogs_c2d->log_type.stIEHdr.u16IEID = YS_IE_ID_LOG_TYPE;
	pst_searchlogs_c2d->log_type.stIEHdr.u16BodyLen = sizeof(ys_ie_log_type_t)-sizeof(ys_ie_head_t);

	pst_searchlogs_c2d->time_range.stIEHdr.u16IEID = YS_IE_ID_TIME_RANGE;
	pst_searchlogs_c2d->time_range.stIEHdr.u16BodyLen = sizeof(ys_ie_time_range_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_startpbs_c2d(ys_msg_startpbs_c2d_t* pst_startpbs_c2d)
{
	pst_startpbs_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_startpbs_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_startpbs_c2d->resource_code.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	pst_startpbs_c2d->resource_code.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	pst_startpbs_c2d->media_ip.stIEHdr.u16IEID = YS_IE_ID_MEDIA_IP;
	pst_startpbs_c2d->media_ip.stIEHdr.u16BodyLen = sizeof(ys_ie_media_ip_t)-sizeof(ys_ie_head_t);

	pst_startpbs_c2d->vod_file.stIEHdr.u16IEID = YS_IE_ID_VOD_FILE;
	pst_startpbs_c2d->vod_file.stIEHdr.u16BodyLen = sizeof(ys_ie_vod_file_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_setpbs_c2d(ys_msg_setpbs_c2d_t* pst_setpbs_c2d)
{
	pst_setpbs_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_setpbs_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_setpbs_c2d->task_no.stIEHdr.u16IEID = YS_IE_ID_TASK_NO;
	pst_setpbs_c2d->task_no.stIEHdr.u16BodyLen = sizeof(ys_ie_task_no_t)-sizeof(ys_ie_head_t);

	pst_setpbs_c2d->play_status.stIEHdr.u16IEID = YS_IE_ID_PLAY_STATUS;
	pst_setpbs_c2d->play_status.stIEHdr.u16BodyLen = sizeof(ys_ie_play_status_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_stoppbs_c2d(ys_msg_stoppbs_c2d_t* pst_stoppbs_c2d)
{
	pst_stoppbs_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_stoppbs_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_stoppbs_c2d->task_no.stIEHdr.u16IEID = YS_IE_ID_TASK_NO;
	pst_stoppbs_c2d->task_no.stIEHdr.u16BodyLen = sizeof(ys_ie_task_no_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_getpbs_c2d(ys_msg_getpbs_c2d_t* pst_getpbs_c2d)
{
	pst_getpbs_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_getpbs_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_getpbs_c2d->task_no.stIEHdr.u16IEID = YS_IE_ID_TASK_NO;
	pst_getpbs_c2d->task_no.stIEHdr.u16BodyLen = sizeof(ys_ie_task_no_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_starttalk_c2d(ys_msg_starttalk_c2d_t* pst_starttalk_c2d)
{
	pst_starttalk_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_starttalk_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_starttalk_c2d->resource_code_in.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	pst_starttalk_c2d->resource_code_in.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	pst_starttalk_c2d->resource_code_out.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	pst_starttalk_c2d->resource_code_out.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	pst_starttalk_c2d->media_ip_send_remote.stIEHdr.u16IEID = YS_IE_ID_MEDIA_IP;
	pst_starttalk_c2d->media_ip_send_remote.stIEHdr.u16BodyLen = sizeof(ys_ie_media_ip_t)-sizeof(ys_ie_head_t);

	pst_starttalk_c2d->media_ip_recv_remote.stIEHdr.u16IEID = YS_IE_ID_MEDIA_IP;
	pst_starttalk_c2d->media_ip_recv_remote.stIEHdr.u16BodyLen = sizeof(ys_ie_media_ip_t)-sizeof(ys_ie_head_t);

	pst_starttalk_c2d->media_transport.stIEHdr.u16IEID = YS_IE_ID_MEDIA_TRANSPORT;
	pst_starttalk_c2d->media_transport.stIEHdr.u16BodyLen = sizeof(ys_ie_media_transport_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_stoptalk_c2d(ys_msg_stoptalk_c2d_t* pst_stoptalk_c2d)
{
	pst_stoptalk_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_stoptalk_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_stoptalk_c2d->task_no.stIEHdr.u16IEID = YS_IE_ID_TASK_NO;
	pst_stoptalk_c2d->task_no.stIEHdr.u16BodyLen = sizeof(ys_ie_task_no_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_gettime_c2d(ys_msg_gettime_c2d_t* pst_gettime_c2d)
{
	pst_gettime_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_gettime_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_settime_c2d(ys_msg_settime_c2d_t* pst_settime_c2d)
{
	pst_settime_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_settime_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_settime_c2d->time_zone_info.stIEHdr.u16IEID = YS_IE_ID_DEV_TIME_ZONE_INFO;
	pst_settime_c2d->time_zone_info.stIEHdr.u16BodyLen = sizeof(ys_ie_time_zone_info_t)-sizeof(ys_ie_head_t);

	pst_settime_c2d->modify_time.stIEHdr.u16IEID = YS_IE_ID_MODIFY_TIME_TYPE;
	pst_settime_c2d->modify_time.stIEHdr.u16BodyLen = sizeof(ys_ie_modify_time_t)-sizeof(ys_ie_head_t);

	pst_settime_c2d->ntp.stIEHdr.u16IEID = YS_IE_ID_DEV_NTP;
	pst_settime_c2d->ntp.stIEHdr.u16BodyLen = sizeof(ys_ie_ntp_t)-sizeof(ys_ie_head_t);

	pst_settime_c2d->time.stIEHdr.u16IEID = YS_IE_ID_DEV_TIME;
	pst_settime_c2d->time.stIEHdr.u16BodyLen = sizeof(ys_ie_time_t)-sizeof(ys_ie_head_t);

	pst_settime_c2d->dst.stIEHdr.u16IEID = YS_IE_ID_DST;
	pst_settime_c2d->dst.stIEHdr.u16BodyLen = sizeof(ys_ie_dst_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_getAO_c2d(ys_msg_getAO_c2d_t* pst_getAO_c2d)
{
	pst_getAO_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_getAO_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_getAO_c2d->resource_code.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	pst_getAO_c2d->resource_code.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);
}

TOOL_VOID tool_ptc_ys_fillIeHead_getAO2_c2d(ys_msg_getAO2_c2d_t* pst_getAO2_c2d)
{
	pst_getAO2_c2d->resource_code.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	pst_getAO2_c2d->resource_code.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	pst_getAO2_c2d->pstIEAlarmOutInfo.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_getAO2_c2d->pstIEAlarmOutInfo.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

}


TOOL_VOID tool_ptc_ys_fillIeHead_setAO_c2d(ys_msg_setAO_c2d_t* pst_setAO_c2d)
{
	pst_setAO_c2d->user_log_info.stIEHdr.u16IEID = YS_IE_ID_USER_LOG_INFO;
	pst_setAO_c2d->user_log_info.stIEHdr.u16BodyLen = sizeof(ys_ie_user_log_info_t)-sizeof(ys_ie_head_t);

	pst_setAO_c2d->resource_code.stIEHdr.u16IEID = YS_IE_ID_RESOURCE_CODE;
	pst_setAO_c2d->resource_code.stIEHdr.u16BodyLen = sizeof(ys_ie_resource_code_t)-sizeof(ys_ie_head_t);

	pst_setAO_c2d->pstIEAlarmOutInfo.stIEHdr.u16IEID = YS_IE_ID_ALARM_OUTPUT_INFO;
	pst_setAO_c2d->pstIEAlarmOutInfo.stIEHdr.u16BodyLen = sizeof(ys_ie_alarmout_t)-sizeof(ys_ie_head_t);
}

TOOL_INT32 tool_ptc_ys_sendMsg(TOOL_VOID* sock_item, ys_msg_t* msg)
{
	msg->head.u16MessageFlag = YS_MSG_FLAG;
	msg->head.u8Version = 5;
	msg->head.u8SubVersion = 0;
	msg->head.u8Type = 0;
	msg->head.u16Status = 0;
	msg->head.u32SrcModId = 0;
	static TOOL_UINT32 seq = 0;
	seq ++;
	msg->head.u32SeqNum = ((msg->head.u32SrcModId<<28) || (seq&0x0FFFFFFF));	
	msg->head.u32FromHandle = 0;
	msg->head.u32ToHandle = 0;
	msg->head.u8EndianType = 0;
	msg->head.u8Res1 = 0;
	msg->head.u16Res2 = 0;
	TOOL_INT32 len = sizeof(ys_msg_head_t)+msg->head.u32ContentLength;
	if (tool_sock_item_send((tool_sock_item_t *)sock_item, msg, len) != len)
		return -1;
//	log_debug("send u16Command(0x%08x) u32ContentLength(%d,%d)", msg->head.u16Command, msg->head.u32ContentLength, len);
	return 0;	
}

TOOL_INT32 tool_ptc_ys_sendSearch(TOOL_VOID* sock_item, ys_search_t* search)
{
	search->head.u16MessageFlag = YS_MSG_FLAG;
	search->head.u8Version = 5;
	search->head.u8SubVersion = 0;
	search->head.u8Type = 0;
	search->head.u16Status = 0;
	search->head.u32SrcModId = 0;
	static TOOL_UINT32 seq = 0;
	seq ++;
	search->head.u32SeqNum = ((search->head.u32SrcModId<<28) || (seq&0x0FFFFFFF));	
	search->head.u32FromHandle = 0;
	search->head.u32ToHandle = 0;
	search->head.u8EndianType = 0;
	search->head.u8Res1 = 0;
	search->head.u16Res2 = 0;
	TOOL_INT32 len = sizeof(ys_msg_head_t)+search->head.u32ContentLength;
	if (tool_sock_item_send((tool_sock_item_t *)sock_item, search, len) != len)
		return -1;
//	log_debug("send u16Command(0x%08x) u32ContentLength(%d,%d)", msg->head.u16Command, msg->head.u32ContentLength, len);
	return 0;	
}

TOOL_INT32 tool_ptc_ys_recvMsg(TOOL_VOID* sock_item, ys_msg_t* msg)
{
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, &msg->head, sizeof(ys_msg_head_t)) != sizeof(ys_msg_head_t))
		return -1;
	if (msg->head.u16MessageFlag != YS_MSG_FLAG)
	{
		log_error("msg->head.flag(0x%08x)", msg->head.u16MessageFlag);
		return -1;
	}
	if (msg->head.u32ContentLength > YS_MSG_SIZE)
	{
		log_error("msg->head.len(%d) cmd(0x%08x)", msg->head.u32ContentLength, msg->head.u16Command);
		return -1;
	}
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, msg->data, msg->head.u32ContentLength) != msg->head.u32ContentLength)
		return -1;
	if (msg->head.u8Type == 3)
	{
		log_error("type(3) code(%d)", msg->head.u16Status);
		if (msg->head.u16Status == YS_ERR_DECODE_IE_FAILED)
			log_error("protocol error");
		else if (msg->head.u16Status == YS_ERR_LIVE_INPUT_NOT_READY)
			log_error("no channel");
		else if (msg->head.u16Status == YS_ERR_USER_WRONG_PASSWD)
			log_error("wrong password");
		else if (msg->head.u16Status == YS_ERR_TALK_EXISTED)
			log_error("audio_in or audio_out busy");
		return -1;
	}
//	log_debug("recv message(0x%08x) len(%d)", msg->head.cmd, msg->head.len);
	return 0;
}

TOOL_INT32 tool_ptc_ys_recvSearch(TOOL_VOID* sock_item, ys_search_t* search)
{
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, &search->head, sizeof(ys_msg_head_t)) != sizeof(ys_msg_head_t))
		return -1;
	if (search->head.u16MessageFlag != YS_MSG_FLAG)
	{
		log_error("msg->head.flag(0x%08x)", search->head.u16MessageFlag);
		return -1;
	}
	if (search->head.u32ContentLength > YS_SEARCH_SIZE)
	{
		log_error("msg->head.len(%d) cmd(0x%08x)", search->head.u32ContentLength, search->head.u16Command);
		return -1;
	}
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, search->data, search->head.u32ContentLength) != search->head.u32ContentLength)
		return -1;
	if (search->head.u8Type == 3)
	{
		log_error("type(3) code(%d)", search->head.u16Status);
		if (search->head.u16Status == YS_ERR_DECODE_IE_FAILED)
			log_error("protocol error");
		else if (search->head.u16Status == YS_ERR_LIVE_INPUT_NOT_READY)
			log_error("no channel");
		else if (search->head.u16Status == YS_ERR_USER_WRONG_PASSWD)
			log_error("wrong password");
		else if (search->head.u16Status == YS_ERR_TALK_EXISTED)
			log_error("audio_in or audio_out busy");
		return -1;
	}
//	log_debug("recv message(0x%08x) len(%d)", msg->head.cmd, msg->head.len);
	return 0;
}

TOOL_INT32 tool_ptc_xm_sendMsg(TOOL_VOID* sock_item, xm_msg_t* msg)
{
	msg->head.head_flag = XM_HEAD_FLAG;
	msg->head.version = 0;
	msg->head.reserve1 = 0;
	msg->head.reserve2 = 0;
	msg->head.sequence_number = 0;
	msg->head.total_packet = 0;
	msg->head.cur_packet = 0;

	TOOL_INT32 len = sizeof(xm_msg_head_t)+msg->head.data_len;
	if (tool_sock_item_send((tool_sock_item_t *)sock_item, msg, len) != len)
		return -1;
	return 0;	
}

TOOL_INT32 tool_ptc_xm_recvMsg(TOOL_VOID* sock_item, xm_msg_t* msg)
{
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, &msg->head, sizeof(xm_msg_head_t)) != sizeof(xm_msg_head_t))
		return -1;
	if (msg->head.head_flag != XM_HEAD_FLAG)
	{
		log_error("msg->head.flag(0x%08x)", msg->head.head_flag);
		return -1;
	}
	if (msg->head.data_len >= XM_MSG_SIZE)
	{
		log_error("msg->head.len(%d) cmd(0x%08x)", msg->head.data_len, msg->head.command_id);
		return -1;
	}
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, msg->data, msg->head.data_len) != msg->head.data_len)
		return -1;
	msg->data[msg->head.data_len] = 0;
	return 0;
}

TOOL_INT32 tool_ptc_xm_recvSearch(TOOL_VOID* sock_item, xm_search_t* search)
{
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, &search->head, sizeof(xm_msg_head_t)) != sizeof(xm_msg_head_t))
		return -1;
	if (search->head.head_flag != XM_HEAD_FLAG)
	{
		log_error("search->head.flag(0x%08x)", search->head.head_flag);
		return -1;
	}
	if (search->head.data_len >= XM_SEARCH_SIZE)
	{
		log_error("search->head.len(%d) cmd(0x%08x)", search->head.data_len, search->head.command_id);
		return -1;
	}
	if (tool_sock_item_recv((tool_sock_item_t *)sock_item, search->data, search->head.data_len) != search->head.data_len)
		return -1;
	search->data[search->head.data_len] = 0;
	return 0;
}

TOOL_INT32 tool_ptc_xm_xm2qh(tool_sock_item_t* tmp_sock_item, TOOL_UINT8* data, ptc_frame_head_t** frame_head, TOOL_UINT32* frame_len)
{
	xm_iframe_head_t* iframe = NULL;
	xm_pframe_head_t* pframe = NULL;
	xm_aframe_head_t* aframe = NULL;
	ptc_frame_head_t ptc_frame_head;
	tool_mem_memset(&ptc_frame_head, sizeof(ptc_frame_head_t));
	TOOL_UINT32* type = (TOOL_UINT32*)data;
	if (*type == XM_FRAME_TYPE_VI)
	{
		iframe = (xm_iframe_head_t*)data;
		*frame_head = (ptc_frame_head_t*)(data + sizeof(xm_iframe_head_t) - sizeof(ptc_frame_head_t));
		ptc_frame_head.len = iframe->length;
		ptc_frame_head.no = tmp_sock_item->seq++;
		tool_time_Date2Time(iframe->year+2000-1900, iframe->month-1, iframe->day, iframe->hour, iframe->minute, iframe->second, &ptc_frame_head.sec);
//		log_debug("%04d-%02d-%02d %02d:%02d:%02d", iframe->year+2000, iframe->month, iframe->day, iframe->hour, iframe->minute, iframe->second);
		ptc_frame_head.usec = tmp_sock_item->usec;
		ptc_frame_head.width = (iframe->width_h*256+iframe->width_l)*8;
		ptc_frame_head.height = (iframe->height_h*256+iframe->height_l)*8;
		ptc_frame_head.frame_type = PTC_FRAME_I;
		if (iframe->sub_type == 1)
			ptc_frame_head.frame_sub_type = PTC_VIDEO_ENCODE_MPEG4;
		else
			ptc_frame_head.frame_sub_type = PTC_VIDEO_ENCODE_H264;
		ptc_frame_head.frame_rate = iframe->frame_rate;
		
	//	log_debug("iframe->frame_rate(%d)", iframe->frame_rate);

		tmp_sock_item->width = ptc_frame_head.width;
		tmp_sock_item->height = ptc_frame_head.height;
		tmp_sock_item->fps_video = ptc_frame_head.frame_rate;	
		if (tmp_sock_item->fps_video < 1 || tmp_sock_item->fps_video > 30)
			tmp_sock_item->fps_video = 30;
		tmp_sock_item->sec= ptc_frame_head.sec;
		tmp_sock_item->usec += (900*1000/tmp_sock_item->fps_video);
		if (tmp_sock_item->usec >= 1000*1000)
		{
			tmp_sock_item->sec ++;
			tmp_sock_item->usec %= (1000*1000);
		}
		tmp_sock_item->frame_sub_type = ptc_frame_head.frame_sub_type;
	}
	else if (*type == XM_FRAME_TYPE_VP)
	{
		pframe = (xm_pframe_head_t*)data;
		*frame_head = (ptc_frame_head_t*)(data + sizeof(xm_pframe_head_t) - sizeof(ptc_frame_head_t));
		ptc_frame_head.len = pframe->length;
		ptc_frame_head.no = tmp_sock_item->seq++;
		ptc_frame_head.sec = tmp_sock_item->sec;
		ptc_frame_head.usec = tmp_sock_item->usec;
		ptc_frame_head.width = tmp_sock_item->width;	
		ptc_frame_head.height = tmp_sock_item->height;
		ptc_frame_head.frame_type = PTC_FRAME_P;
		ptc_frame_head.frame_sub_type = tmp_sock_item->frame_sub_type;
		ptc_frame_head.frame_rate = tmp_sock_item->fps_video;	

		if (tmp_sock_item->fps_video < 1 || tmp_sock_item->fps_video > 30)
			tmp_sock_item->fps_video = 30;
		tmp_sock_item->sec= ptc_frame_head.sec;
		tmp_sock_item->usec += (900*1000/tmp_sock_item->fps_video);
		if (tmp_sock_item->usec >= 1000*1000)
		{
			tmp_sock_item->sec ++;
			tmp_sock_item->usec %= (1000*1000);
		}
	}
	else if (*type == XM_FRAME_TYPE_A)
	{
		aframe = (xm_aframe_head_t*)data;
		*frame_head = (ptc_frame_head_t*)(data + sizeof(xm_aframe_head_t) - sizeof(ptc_frame_head_t));
		
		ptc_frame_head.len = aframe->length;
		ptc_frame_head.no = tmp_sock_item->seq++;
		ptc_frame_head.sec = tmp_sock_item->sec;
		ptc_frame_head.usec = tmp_sock_item->usec;
		ptc_frame_head.width = 0;
		ptc_frame_head.height = 0;
		ptc_frame_head.frame_type = PTC_FRAME_A;
		if (aframe->sub_type == 0xa)
			ptc_frame_head.frame_sub_type = PTC_AUDIO_ENCODE_G711U;
		else
			ptc_frame_head.frame_sub_type = PTC_AUDIO_ENCODE_G711A;
		ptc_frame_head.frame_rate = 50;
	}
	else
	{
		return 0;
	}

	ptc_frame_head.flag = PTC_FRAME_FLAG;
	tool_mem_memcpy(*frame_head, &ptc_frame_head, sizeof(ptc_frame_head_t));
	*frame_len = ptc_frame_head.len + sizeof(ptc_frame_head_t);
//	TOOL_UINT8* frame_data = (TOOL_UINT8*)(*frame_head+2);
//	log_debug("0x%02x 0x%02x 0x%02x 0x%02x (%d,%d)", frame_data[0], frame_data[1], frame_data[2], frame_data[3], frame_data, *frame_head);
	return 1;
}

TOOL_INT32 tool_ptc_xm_recvBuf(TOOL_VOID* sock_item, TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_INT32 size)
{
	xm_msg_head_t msg_head;
	tool_sock_item_t* sock = (tool_sock_item_t*)sock_item;
	if (tool_sock_item_recv(sock, &msg_head, sizeof(xm_msg_head_t)) != sizeof(xm_msg_head_t))
	{		
		log_debug("msg_head");
		return -1;
	}
    if (msg_head.data_len <= 0)
    {
        log_debug("msg_head.data_len(%d)", msg_head.data_len);
        return 0;
    }
	if (msg_head.data_len + *len > size)
	{
		log_error("msg_head.data_len(%d) *len(%d) size(%d)", msg_head.data_len, *len, size);
		return -1;
	}

	TOOL_INT32 type = 0;
	if (tool_sock_item_recvPeek(sock, &type, sizeof(TOOL_INT32)) < 0)
	{
		log_error("tool_sock_item_recvPeek");
		return -1;
	}
	if (type == XM_FRAME_TYPE_VI ||
		type == XM_FRAME_TYPE_VP ||
		type == XM_FRAME_TYPE_A)
	{
//		log_debug("tool_ptc_xm_recvBuf");
		*len = 0;
	}

	if (tool_sock_item_recv(sock, buf+*len, msg_head.data_len) != msg_head.data_len)
	{		
		log_debug("data");
		return -1;
	}
	*len += msg_head.data_len;	
	return 0;	
}


TOOL_INT32 tool_ptc_xm_checkBuf(TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_UINT8** left_buf, TOOL_UINT32* left_len)
{
	if (*len < 4)
		return 0;
	
	TOOL_UINT32* type = (TOOL_UINT32*)buf;	
	if (*type == XM_FRAME_TYPE_VI)
	{
		if (*len < sizeof(xm_iframe_head_t))
			return 0;
		xm_iframe_head_t* pst_iframe = (xm_iframe_head_t*)buf;
		if (*len < pst_iframe->length + sizeof(xm_iframe_head_t))
			return 0;
		*left_buf = buf  + (pst_iframe->length + sizeof(xm_iframe_head_t));
		*left_len = *len - (pst_iframe->length + sizeof(xm_iframe_head_t));
		*len = 0;
		return 1;
	}
	else if (*type == XM_FRAME_TYPE_VP)
	{
		if (*len < sizeof(xm_pframe_head_t))
			return 0;
		xm_pframe_head_t* pst_pframe = (xm_pframe_head_t*)buf;
		if (*len < pst_pframe->length + sizeof(xm_pframe_head_t))
			return 0;
		*left_buf = buf  + (pst_pframe->length + sizeof(xm_pframe_head_t));
		*left_len = *len - (pst_pframe->length + sizeof(xm_pframe_head_t));
		*len = 0;
		return 1;		
	}
	else if (*type == XM_FRAME_TYPE_A)
	{
		if (*len < sizeof(xm_aframe_head_t))
			return 0;
		xm_aframe_head_t* pst_aframe = (xm_aframe_head_t*)buf;
		if (*len < pst_aframe->length + sizeof(xm_aframe_head_t))
			return 0;
		*left_buf = buf  + (pst_aframe->length + sizeof(xm_aframe_head_t));
		*left_len = *len - (pst_aframe->length + sizeof(xm_aframe_head_t));
		*len = 0;
		return 1;			
	}
	else if (*type == XM_FRAME_TYPE_PIC)
	{
		if (*len < sizeof(xm_picframe_head_t))
			return 0;
		xm_picframe_head_t* pst_picframe = (xm_picframe_head_t*)buf;
		if (*len < pst_picframe->length + sizeof(xm_picframe_head_t))
			return 0;
		*left_buf = buf  + (pst_picframe->length + sizeof(xm_picframe_head_t));
		*left_len = *len - (pst_picframe->length + sizeof(xm_picframe_head_t));
		*len = 0;
		return 0;
	}
	else if (*type == XM_FRAME_TYPE_INFO)
	{
		if (*len < sizeof(xm_infoframe_head_t))
			return 0;
		xm_infoframe_head_t* pst_infoframe = (xm_infoframe_head_t*)buf;
		if (*len < pst_infoframe->length + sizeof(xm_infoframe_head_t))
			return 0;
		*left_buf = buf  + (pst_infoframe->length + sizeof(xm_infoframe_head_t));
		*left_len = *len - (pst_infoframe->length + sizeof(xm_infoframe_head_t));
		*len = 0;
		return 0;
	}
	else
	{
		log_error("*len(%d) type(0x%08x)", *len, *type);
		*left_buf = NULL;
		*left_len = 0;
		*len = 0;
		return 0;
	}

//	log_error("*len(%d) type(0x%08x)", *len, *type);
//	return -1;
}

TOOL_INT32 tool_ptc_xm_recvRts(TOOL_VOID* sock_item, TOOL_UINT8* buf, TOOL_UINT32* len, TOOL_UINT32 size, 
	ptc_frame_head_t** frame_head, TOOL_UINT32* frame_len,
	TOOL_UINT8** left_buf, TOOL_UINT32* left_len)
{
	buf  += sizeof(ptc_frame_head_t);
	size -= sizeof(ptc_frame_head_t);

//	log_debug("buf     : 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", buf[0], buf[1], buf[2], buf[3], *len);
//	if (*left_buf)
//		log_debug("left_buf: 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", (*left_buf)[0], (*left_buf)[1], (*left_buf)[2], (*left_buf)[3], *left_len);

	if (*left_len > 0)
	{
		tool_mem_memmove(buf, *left_buf, *left_len);
		*len = *left_len;
		*left_len = 0;
	}

//	log_debug("buf     : 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", buf[0], buf[1], buf[2], buf[3], *len);
//	if (*left_buf)
//		log_debug("left_buf: 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", (*left_buf)[0], (*left_buf)[1], (*left_buf)[2], (*left_buf)[3], *left_len);

	TOOL_INT32 ret = tool_ptc_xm_checkBuf(buf, len, left_buf, left_len);

//	log_debug("buf     : 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", buf[0], buf[1], buf[2], buf[3], *len);
//	if (*left_buf)
//		log_debug("left_buf: 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", (*left_buf)[0], (*left_buf)[1], (*left_buf)[2], (*left_buf)[3], *left_len);
	
	if (ret < 0)
		return ret;
	else if (ret > 0)
		return tool_ptc_xm_xm2qh((tool_sock_item_t *)sock_item, buf, frame_head, frame_len);	

//	log_debug("buf     : 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", buf[0], buf[1], buf[2], buf[3], *len);
//	if (*left_buf)
//		log_debug("left_buf: 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", (*left_buf)[0], (*left_buf)[1], (*left_buf)[2], (*left_buf)[3], *left_len);

	ret = tool_ptc_xm_recvBuf(sock_item, buf, len, size);
	if (ret < 0)
		return ret;

//	log_debug("buf     : 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", buf[0], buf[1], buf[2], buf[3], *len);
//	if (*left_buf)
//		log_debug("left_buf: 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", (*left_buf)[0], (*left_buf)[1], (*left_buf)[2], (*left_buf)[3], *left_len);
	
	ret = tool_ptc_xm_checkBuf(buf, len, left_buf, left_len);

//	log_debug("buf     : 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", buf[0], buf[1], buf[2], buf[3], *len);
//	if (*left_buf)
//		log_debug("left_buf: 0x%02x 0x%02x 0x%02x 0x%02x len(%d)", (*left_buf)[0], (*left_buf)[1], (*left_buf)[2], (*left_buf)[3], *left_len);
	
	if (ret <= 0)
		return ret;
	return tool_ptc_xm_xm2qh((tool_sock_item_t *)sock_item, buf, frame_head, frame_len);
}

TOOL_INT32 tool_ptc_xm_recvTalk(TOOL_VOID* sock_item, ptc_frame_audio_t* pst_ptc_frame)
{
	tool_sock_item_t* sock = (tool_sock_item_t*)sock_item;
	xm_msg_head_t msg_head;
	if (tool_sock_item_recv(sock, &msg_head, sizeof(xm_msg_head_t)) != sizeof(xm_msg_head_t))
	{		
		log_debug("msg_head");
		return -1;
	}
	if (msg_head.head_flag != 0XFF || msg_head.command_id != XM_CMD_FRAME_TALK_D2C || msg_head.data_len > PTC_AUDIO_FRAME_SIZE)
	{
		log_error("msg_head.head_flag(%02x) msg_head.command_id(0x%08x) msg_head.data_len(%d) PTC_AUDIO_FRAME_SIZE(%d)", 
			msg_head.head_flag, msg_head.command_id, msg_head.data_len, PTC_AUDIO_FRAME_SIZE);
		return -1;
	}

	xm_aframe_head_t aframe_head;
	if (tool_sock_item_recv(sock, &aframe_head, sizeof(xm_aframe_head_t)) != sizeof(xm_aframe_head_t))
	{		
		log_debug("msg_head");
		return -1;
	}
	if (aframe_head.length > PTC_AUDIO_FRAME_SIZE || aframe_head.type != XM_FRAME_TYPE_A)
	{
		log_error("msg_head.data_len(%d) PTC_AUDIO_FRAME_SIZE(%d) aframe_head.type(0x%08x", 
			aframe_head.length, PTC_AUDIO_FRAME_SIZE, aframe_head.type);
		return -1;
	}
	tool_mem_memset(&pst_ptc_frame->frame_head, sizeof(ptc_frame_head_t));
	pst_ptc_frame->frame_head.len = aframe_head.length;
	pst_ptc_frame->frame_head.no = sock->seq++;
	pst_ptc_frame->frame_head.frame_type = PTC_FRAME_A;
	pst_ptc_frame->frame_head.frame_sub_type = PTC_AUDIO_ENCODE_G711A;
	pst_ptc_frame->frame_head.frame_rate = 50;
	pst_ptc_frame->frame_head.flag = PTC_FRAME_FLAG;
	tool_time_t tt;
	tool_sysf_time(&tt);
	pst_ptc_frame->frame_head.sec = tt.sec;
	pst_ptc_frame->frame_head.usec = tt.usec;

	if (tool_sock_item_recv(sock, pst_ptc_frame->frame_body, aframe_head.length) != aframe_head.length)
	{		
		log_debug("data");
		return -1;
	}
	return 0;	
}

TOOL_INT32 tool_ptc_xm_sendTalk(TOOL_VOID* sock_item, ptc_frame_audio_t* pst_ptc_frame, TOOL_UINT32 session_id)
{
	xm_msg_head_t msg_head;
	tool_mem_memset(&msg_head, sizeof(xm_msg_head_t));
	msg_head.head_flag = XM_HEAD_FLAG;
	msg_head.session_id = session_id;
	msg_head.command_id = XM_CMD_FRAME_TALK_C2D;
	msg_head.data_len = sizeof(xm_aframe_head_t)+pst_ptc_frame->frame_head.len;

	xm_aframe_head_t aframe_head;
	tool_mem_memset(&aframe_head, sizeof(xm_aframe_head_t));
	aframe_head.type = XM_FRAME_TYPE_A;
	aframe_head.sub_type = 0x0e;
	aframe_head.sample = 0x02;
	aframe_head.length = pst_ptc_frame->frame_head.len;

	if (tool_sock_item_send((tool_sock_item_t*)sock_item, &msg_head, sizeof(xm_msg_head_t)) != sizeof(xm_msg_head_t) ||
		tool_sock_item_send((tool_sock_item_t*)sock_item, &aframe_head, sizeof(xm_aframe_head_t)) != sizeof(xm_aframe_head_t) ||
		tool_sock_item_send((tool_sock_item_t*)sock_item, pst_ptc_frame->frame_body, pst_ptc_frame->frame_head.len) != pst_ptc_frame->frame_head.len)
		return -1;
//	log_debug("pst_ptc_frame->frame_head.len(%d)", pst_ptc_frame->frame_head.len);
	
	return 0;		
}


#if defined __cplusplus
}
#endif


