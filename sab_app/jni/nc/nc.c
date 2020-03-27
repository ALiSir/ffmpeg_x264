
#if defined __cplusplus
extern "C"
{
#endif

#include "nc.h"
#include "nc_qh.h"
#include "nc_hy.h"
#include "nc_ys.h"
#include "nc_xm.h"
#include "tool_type.h"
#include "tool_sock.h"
#include "tool_sysf.h"
#include "tool_log.h"
#include "tool_md5.h"
#include "tool_base64.h"
#include "hdcctv_client.h"

TOOL_MUTEX g_test_mutex;
TOOL_COND g_test_cond;

TOOL_INT32 nc_getDevById(nc_dev* pst_nc_dev)
{
	nc_dev st_nc_dev;

	tool_mem_memcpy(&st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	if (hdcctv_client2_getDevById(&st_nc_dev) > 0)
	{
		if (tool_sock_item_ping(st_nc_dev.ip, st_nc_dev.msg_port, 0) >= 0)
		{
			tool_mem_memcpy(pst_nc_dev, &st_nc_dev, sizeof(nc_dev));
			return 1;
		}
	}

	tool_mem_memcpy(&st_nc_dev, pst_nc_dev, sizeof(nc_dev));
	if (nc_qh_getSearchById(&st_nc_dev) > 0)
	{
		if (tool_sock_item_ping(st_nc_dev.ip, st_nc_dev.msg_port, 0) >= 0)
		{
			tool_mem_memcpy(pst_nc_dev, &st_nc_dev, sizeof(nc_dev));
			return 1;
		}
	}

	if (tool_sock_item_ping("192.168.1.10", 5000, 0) >= 0)
	{
		tool_mem_memset(pst_nc_dev, sizeof(nc_dev));
		pst_nc_dev->ptc = NC_PTC_Qihan;
		tool_str_strncpy(pst_nc_dev->ip, "192.168.1.10", PTC_IP_SIZE-1);
		pst_nc_dev->msg_port = 5000;
		tool_str_strncpy(pst_nc_dev->user, "qihan", PTC_ID_SIZE-1);
		tool_str_strncpy(pst_nc_dev->pswd, "nahiq", PTC_ID_SIZE-1);
		return 1;
	}
	
	return 0;
}


TOOL_VOID* nc_test_run(TOOL_VOID* param)
{
	tool_sysf_sleep(5);
	tool_thread_lockMutex(&g_test_mutex);
	log_state("tool_thread_broadcastCond");
	tool_thread_broadcastCond(&g_test_cond);
	tool_thread_unlockMutex(&g_test_mutex);
	return NULL;
}

TOOL_VOID nc_test_cond()
{
	tool_thread_initMutex(&g_test_mutex);
	tool_thread_initCond(&g_test_cond);
	TOOL_THREAD pid;
	tool_thread_create(&pid, NULL, nc_test_run, NULL);

	while (1)
	{
		tool_thread_lockMutex(&g_test_mutex);
		if (tool_thread_timedwaitCond(&g_test_cond, &g_test_mutex, 1) < 0)
		{
			tool_thread_unlockMutex(&g_test_mutex);
			log_error("tool_thread_timedwaitCond timeout");
			continue;
		}
		log_state("tool_thread_timedwaitCond success");
		tool_thread_unlockMutex(&g_test_mutex);
		break;
	}
	tool_sysf_sleep(100);
}

/*
#include "h264.h"

TOOL_VOID nc_test_sps()
{
	H264Context h;
	TOOL_UINT8 buf[16] = {0x67, 0x64, 0x0, 0x33, 0xac, 0xe8, 0x02, 0x0, 0xc, 0x19};
	TOOL_INT32 bit_length = 9*8;
	init_get_bits(&h.gb, &buf[1], bit_length);
	ff_h264_decode_seq_parameter_set(&h);
	tool_sysf_sleep(100);
}
*/

/*
TOOL_VOID nc_test_char()
{
	char unicode[32] = {"ÖÐÎÄ"};
	unsigned char *utf8 = NULL;
	printf("tool_str_strlen(unicode)=(%d)\n", tool_str_strlen(unicode));
	unicode_to_utf8(unicode, tool_str_strlen(unicode), &utf8);
	printf("unicode=(%s)\n", unicode);
	printf("utf8=( %s )---()\n", utf8);
	free(utf8);
	
	tool_sysf_sleep(100);
}
*/


#include "tool_json.h"

TOOL_VOID nc_test_json()
{
	tool_json_t st_json;
	TOOL_INT8 str[2048] = 
		"{\"sn\":\"151001\",\"n\":\"nvr1\",\"t\":\"NVR\",\"ip\":\"202.169.102.101\",\" dp \":\"37098514\",\"li\":\"192.168.1.2\",\"pl\":\" http;sdk;sip;rtsp;live;replay;service;ftp \",\"lp\":\"80;8800;5060;554;52064;52062;52060;21\",\"np\":\"80;8800;5060;554;52064;52062;52060;21\",\"si\":\"Independent Mapping\",\"sf\":\"false\",\"dt\":\"\",\"du\":\"admin\"},"
		"{\"sn\":\"151002\",\"n\":\"nvr2\",\"t\":\"NVR\",\"ip\":\"202.169.101.112\",\" dp \":\"38098539\",\"li\":\"192.168.1.2\",\"pl\":\" http;sdk;sip;rtsp;live;replay;service;ftp \",\"lp\":\"80;8800;5060;554;52064;52062;52060;21\",\"np\":\"80;8800;5060;554;52064;52062;52060;21\",\"si\":\"Independent Mapping\",\"sf\":\"false\",\"dt\":\"\",\"du\":\"admin\"},"
		"{\"sn\":\"151003\",\"n\":\"nvr3\",\"t\":\"NVR\",\"ip\":\"202.169.102.205\",\" dp \":\"97038518\",\"li\":\"192.168.1.2\",\"pl\":\" http;sdk;rtsp;live \",\"lp\":\"80;6060;554;7070\",\"np\":\"80;8800;6060;7070\",\"si\":\"Independent Mapping\",\"sf\":\"false\",\"dt\":\"\",\"du\":\"admin\"}";
	tool_json_parse(&st_json, str);	
	tool_sysf_sleep(100);
}


/*
TOOL_VOID nc_test_ys_getDevInfo()
{	
	tool_sock_init();
	nc_ys_init();
	nc_dev st_dev;
	tool_mem_memset(&st_dev, sizeof(nc_dev));
	st_dev.ptc = NC_PTC_YuShi_Cloud;
	tool_str_strncpy(st_dev.dev_id, "210235C1A00123456789ea630f075d", tool_str_strlen("210235C1A00123456789ea630f075d"));
	nc_ys_getDevInfo(&st_dev);
	tool_sysf_sleep(100);
}
*/

TOOL_VOID nc_test_atoi()
{
	TOOL_INT8 sz_channel[16] = "0x1C";
	log_debug("(%d)", tool_sysf_atoi(sz_channel));
	tool_sysf_sleep(100);
}

TOOL_VOID nc_test_wash()
{
	TOOL_INT32 array[1000] = {0};
	tool_ran_wash(1234567890, array, 1000);
	tool_ran_wash(1234567890, array, 1000);
	tool_sysf_sleep(100);
}

TOOL_VOID nc_test_json2()
{
	TOOL_INT8 str[2048] = "{ \"Name\" : \"SystemInfo\", \"Ret\" : tree, \"SessionID\" : \"0x11\", \"SystemInfo\" : { \"AlarmInChannel\" : 4, \"AlarmOutChannel\" : 1, \"AudioInChannel\" : 8, \"BuildTime\" : \"2015-01-12 10:20:06\", \"CombineSwitch\" : 0, \"DeviceRunTime\" : \"0x000000D6\", \"DigChannel\" : 0, \"EncryptVersion\" : \"Unknown\", \"ExtraChannel\" : 0, \"HardWare\" : \"MBD6016E-E\", \"HardWareVersion\" : \"Unknown\", \"SerialNo\" : \"e5449c23b1a5ea5c\", \"SoftWareVersion\" : \"V4.02.R11.24340116.12201.1309\", \"TalkInChannel\" : 1, \"TalkOutChannel\" : 1, \"UpdataTime\" : \"\", \"UpdataType\" : \"0x00000000\", \"VideoInChannel\" : 8, \"VideoOutChannel\" : 1 } }";

	TOOL_INT8 Ret[128] = {0};
	TOOL_INT32 ret1 = tool_json2_getValue(str, "Ret", Ret, 128);
	log_debug("Ret(%s) ret1(%d)", Ret, ret1);
	
	TOOL_INT32 AlarmOutChannel_num = 0;
	TOOL_INT32 ret2 = tool_json2_getNumber(str, "AlarmOutChannel", &AlarmOutChannel_num);
	log_debug("AlarmOutChannel_num(%d) ret2(%d)", AlarmOutChannel_num, ret2);

	
	tool_sysf_sleep(100);
}

TOOL_VOID nc_test_md5()
{
	TOOL_INT8 in_buf[1024] = "123456";
	TOOL_INT8 out_buf[16] = {0};
	
	tool_md5_calc_xm((TOOL_UINT8*)in_buf, tool_str_strlen(in_buf), out_buf);
	log_debug("out_buf(%s)", out_buf);
	
	tool_sysf_sleep(100);
}

TOOL_VOID nc_test_base64()
{
	TOOL_INT8 in_buf[100] = "123456";
	TOOL_INT8 out_buf[1024] = {0};

	base64_encode((TOOL_UINT8*)in_buf, out_buf, 100);

	log_debug("out_buf(%s)", out_buf);
	
	tool_sysf_sleep(100);
}


TOOL_VOID nc_init()
{	
//	nc_test_sps();
//	nc_test_char();
//	nc_test_json();
//	nc_test_ys_getDevInfo();
//	nc_test_atoi();
//	nc_test_wash();
//	nc_test_json2();
//	nc_test_md5();
//	nc_test_base64();

	tool_sock_init();
	nc_qh_init();
	nc_hy_init();
	nc_ys_init();
	nc_xm_init();
	hdcctv_client2_init();	
}

TOOL_INT32 nc_setBuffer(TOOL_INT32 buffer_usec)
{
	nc_hy_setBuffer(buffer_usec);
	nc_qh_setBuffer(buffer_usec);
	nc_ys_setBuffer(buffer_usec);
	nc_xm_setBuffer(buffer_usec);
	hdcctv_client2_setBuffer(buffer_usec);
	return 0;
}

TOOL_VOID nc_done()
{
	hdcctv_client2_done();
    nc_qh_done();
	nc_hy_done();
	nc_ys_done();
	nc_xm_done();
}

TOOL_INT32 nc_searchDev(ptc_mcast_searchs_t* searchs)
{
	searchs->num = 0;

//	TOOL_THREAD qh_thd;
	ptc_mcast_searchs_t* pst_qh_searchs = (ptc_mcast_searchs_t*)tool_mem_malloc(sizeof(ptc_mcast_searchs_t), 0);
//	pst_qh_searchs->num = 0;
//	if (tool_thread_create(&qh_thd, NULL, (TOOL_FUNC)nc_qh_searchDev, (TOOL_VOID*)pst_qh_searchs) < 0)
//		log_fatal("tool_thread_create");
	nc_qh_getSearch(pst_qh_searchs);

	TOOL_THREAD hy_thd;
	ptc_mcast_searchs_t* pst_hy_searchs = (ptc_mcast_searchs_t*)tool_mem_malloc(sizeof(ptc_mcast_searchs_t), 0);
	pst_hy_searchs->num = 0;
	if (tool_thread_create(&hy_thd, NULL, (TOOL_FUNC)nc_hy_searchDev, (TOOL_VOID*)pst_hy_searchs) < 0)
		log_fatal("tool_thread_create");

	TOOL_THREAD ys_thd;
	ptc_mcast_searchs_t* pst_ys_searchs = (ptc_mcast_searchs_t*)tool_mem_malloc(sizeof(ptc_mcast_searchs_t), 0);
	pst_ys_searchs->num = 0;
	if (tool_thread_create(&ys_thd, NULL, (TOOL_FUNC)nc_ys_searchDev, (TOOL_VOID*)pst_ys_searchs) < 0)
		log_fatal("tool_thread_create");

	TOOL_THREAD xm_thd;
	ptc_mcast_searchs_t* pst_xm_searchs = (ptc_mcast_searchs_t*)tool_mem_malloc(sizeof(ptc_mcast_searchs_t), 0);
	pst_xm_searchs->num = 0;
	if (tool_thread_create(&xm_thd, NULL, (TOOL_FUNC)nc_xm_searchDev, (TOOL_VOID*)pst_xm_searchs) < 0)
		log_fatal("tool_thread_create");

	tool_sysf_sleep(5);	
//	tool_thread_join(qh_thd);
	tool_thread_join(hy_thd);
	tool_thread_join(ys_thd);
	tool_thread_join(xm_thd);

	searchs->num = 0;
	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	for (i = 0; i < pst_qh_searchs->num; i++)
	{
		if (searchs->num >= PTC_MCAST_SEARCH_SIZE)
			break;
		
		for (j = 0; j < searchs->num; j++)
		{
			if (tool_mem_memcmp(&searchs->search[j], &pst_qh_searchs->search[i], sizeof(ptc_mcast_search_t)) == 0)
				break;
		}
		if (j == searchs->num)
		{
			tool_mem_memcpy(&searchs->search[searchs->num], &pst_qh_searchs->search[i], sizeof(ptc_mcast_search_t));
			searchs->num ++;
		}
	}

	for (i = 0; i < pst_hy_searchs->num; i++)
	{
		if (searchs->num >= PTC_MCAST_SEARCH_SIZE)
			break;
		
		for (j = 0; j < searchs->num; j++)
		{
			if (tool_mem_memcmp(&searchs->search[j], &pst_hy_searchs->search[i], sizeof(ptc_mcast_search_t)) == 0)
				break;
		}
		if (j == searchs->num)
		{
			tool_mem_memcpy(&searchs->search[searchs->num], &pst_hy_searchs->search[i], sizeof(ptc_mcast_search_t));
			searchs->num ++;
		}
	}

	for (i = 0; i < pst_ys_searchs->num; i++)
	{
		if (searchs->num >= PTC_MCAST_SEARCH_SIZE)
			break;
		
		for (j = 0; j < searchs->num; j++)
		{
			if (tool_mem_memcmp(&searchs->search[j], &pst_ys_searchs->search[i], sizeof(ptc_mcast_search_t)) == 0)
				break;
		}
		if (j == searchs->num)
		{
			tool_mem_memcpy(&searchs->search[searchs->num], &pst_ys_searchs->search[i], sizeof(ptc_mcast_search_t));
			searchs->num ++;
		}
	}

	for (i = 0; i < pst_xm_searchs->num; i++)
	{
		if (searchs->num >= PTC_MCAST_SEARCH_SIZE)
			break;
		
		for (j = 0; j < searchs->num; j++)
		{
			if (tool_mem_memcmp(&searchs->search[j], &pst_xm_searchs->search[i], sizeof(ptc_mcast_search_t)) == 0)
				break;
		}
		if (j == searchs->num)
		{
			tool_mem_memcpy(&searchs->search[searchs->num], &pst_xm_searchs->search[i], sizeof(ptc_mcast_search_t));
			searchs->num ++;
		}
	}

	tool_mem_free(pst_qh_searchs);
	tool_mem_free(pst_hy_searchs);
	tool_mem_free(pst_ys_searchs);
	tool_mem_free(pst_xm_searchs);

	return 0;
}

#ifndef __NO_FFMPEG__

#include "lan_h264.h"

TOOL_INT32 nc_ParseSPS(TOOL_UINT8* sps, TOOL_INT32 len, TOOL_INT32* width, TOOL_INT32* height)
{
	if (sps == NULL || len <= 0 || width == NULL || height == NULL)
	{
		log_error("sps(%d) len(%d) width(%d) height(%d)", sps, len, width, height);
		return NC_ERROR_INPUT;
	}
	
	lan_H264Context h;
	lan_init_lan_get_bits(&h.gb, sps, len*8);
	lan_ff_h264_decode_seq_parameter_set(&h, width, height);
	if (*width == 1920 && *height == 1088)
		*height = 1080;
	return 0;
}

#endif

int nc_auth_v2(nc_dev* dev)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_auth_v2(dev);
}

int nc_login_v2(nc_dev* dev, NC_CB cb, void* param)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_login_v2(dev, cb, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_login_v2(dev, cb, param);
}

TOOL_INT32 nc_logout(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_logout(user_id);
}

TOOL_INT32 nc_getOnlineStatus(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_getOnlineStatus(user_id);
}

TOOL_INT32 nc_getChannelNum_v2(nc_dev* dev)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_getChannelNum_v2(dev);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return nc_ys_getChannelNum_v2(dev);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_getChannelNum_v2(dev);	
		return hdcctv_client2_getChannelNum_v2(dev);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_getChannelNum_v2(dev);	
	}
	else
		return nc_qh_getChannelNum_v2(dev);		
}

TOOL_INT32 nc_startRts_v2(nc_dev* dev, TOOL_INT32 channel, TOOL_INT32 stream_type, NC_CB cb, void* param)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_startRts_v2(dev, channel, stream_type, cb, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return nc_ys_startRts_v2(dev, channel, stream_type, cb, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
//			stream_type = 0;
			return nc_qh_startRts_v2(dev, channel, stream_type, cb, param);
		}
		stream_type = 1;
		return hdcctv_client2_startRts(dev, channel, stream_type, cb, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_startRts_v2(dev, channel, stream_type, cb, param);
	}
	else
	{
//		stream_type = 0;
		return nc_qh_startRts_v2(dev, channel, stream_type, cb, param);
	}
}

int nc_forceRtsKeyFrame(int user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_forceRtsKeyFrame(user_id);
}

TOOL_INT32 nc_stopRts(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_stopRts(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return nc_ys_stopRts(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return hdcctv_client2_stopRts(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_stopRts(user_id);
	}
	else
		return nc_qh_stopRts(user_id);
}

TOOL_INT32 nc_getRtsStatus(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_getRtsStatus(user_id);
}

TOOL_INT32 nc_setRtsLevel(TOOL_INT32 user_id, TOOL_INT32 level)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
//		return nc_qh_setRtsLevel(user_id, level);
		return NC_ERROR_NOSUPPORTED;
}

TOOL_INT32 nc_controlPTZ_v2(nc_dev* dev, TOOL_INT32 channel, PTC_PTZ_CMD_E cmd, TOOL_INT32 param)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_controlPTZ_v2(dev, channel, cmd, param);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		if (cmd == PTC_PTZ_CMD_AUTO_SCAN_START || cmd == PTC_PTZ_CMD_AUTO_SCAN_STOP)
			return NC_ERROR_NOSUPPORTED;
		return nc_ys_controlPTZ_v2(dev, channel, cmd, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_controlPTZ_v2(dev, channel, cmd, param);
		return hdcctv_client2_ctrlPtz(dev, channel, cmd, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_controlPTZ_v2(dev, channel, cmd, param);
	}
	else
		return nc_qh_controlPTZ_v2(dev, channel, cmd, param);
}


TOOL_INT32 nc_startTalk_v2(nc_dev* dev, NC_CB cb, void* param)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_startTalk_v2(dev, cb, param);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return nc_ys_startTalk_v2(dev, cb, param);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_startTalk_v2(dev, cb, param);
		return hdcctv_client2_startTalk(dev, cb, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_startTalk_v2(dev, cb, param);
	}
	else
		return nc_qh_startTalk_v2(dev, cb, param);
}

TOOL_INT32 nc_stopTalk(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_stopTalk(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return nc_ys_stopTalk(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return hdcctv_client2_stopTalk(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_stopTalk(user_id);
	}
	else
		return nc_qh_stopTalk(user_id);
}

TOOL_INT32 nc_sendTalk(TOOL_INT32 user_id, TOOL_UINT8* buf, TOOL_INT32 len)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_sendTalk(user_id, buf, len);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return nc_ys_sendTalk(user_id, buf, len);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return hdcctv_client2_sendTalk(user_id, buf, len);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_sendTalk(user_id, buf, len);
	}
	else
		return nc_qh_sendTalk(user_id, buf, len);
}

TOOL_INT32 nc_getAbility_v2(nc_dev* dev, ptc_cfg_ability_t* ability)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_getAbility_v2(dev, ability);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return nc_ys_getAbility_v2(dev, ability);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_getAbility_v2(dev, ability);
		return hdcctv_client2_getAbility_v2(dev, ability);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_getAbility_v2(dev, ability);
	}
	else
	    return nc_qh_getAbility_v2(dev, ability);
}

TOOL_INT32 nc_getDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_getDeviceStatus_v2(dev, status);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return nc_ys_getDeviceStatus_v2(dev, status);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_getDeviceStatus_v2(dev, status);
		return hdcctv_client2_getDeviceStatus_v2(dev, status);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_getDeviceStatus_v2(dev, status);
	}
	else
		return nc_qh_getDeviceStatus_v2(dev, status);
}

TOOL_INT32 nc_setDeviceStatus_v2(nc_dev* dev, ptc_cfg_status_t* status)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_setDeviceStatus_v2(dev, status);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return nc_ys_setDeviceStatus_v2(dev, status);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_setDeviceStatus_v2(dev, status);
		return hdcctv_client2_setDeviceStatus_v2(dev, status);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_setDeviceStatus_v2(dev, status);
	}
	else
		return nc_qh_setDeviceStatus_v2(dev, status);
}

TOOL_INT32 nc_getCfg(nc_dev* dev, ptc_cfg_t* cfg)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_getCfg(dev, cfg);
}

TOOL_INT32 nc_getDefaultCfg(nc_dev* dev, ptc_cfg_t* cfg)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_getDefaultCfg(dev, cfg);
}

TOOL_INT32 nc_setCfg(nc_dev* dev, ptc_cfg_t* cfg)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_setCfg(dev, cfg);
}

TOOL_INT32 nc_syncTime(nc_dev* dev, TOOL_UINT32 utc_sec, TOOL_INT32 zone, TOOL_INT32 dst_hour)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_syncTime(dev, utc_sec, zone, dst_hour);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return nc_ys_syncTime(dev, utc_sec, zone, dst_hour);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_syncTime(dev, utc_sec, zone, dst_hour);
		return hdcctv_client2_syncTime(dev, utc_sec, zone, dst_hour);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_syncTime(dev, utc_sec, zone, dst_hour);
	}
	else
		return nc_qh_syncTime(dev, utc_sec, zone, dst_hour);
}

TOOL_INT32 nc_getCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_getCfg_alarm(dev, alarm_type, alarm);
		return hdcctv_client2_getAlarmCfg(dev, alarm_type, alarm);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_getCfg_alarm(dev, alarm_type, alarm);
}

TOOL_INT32 nc_setCfg_alarm(nc_dev* dev, PTC_ALARM_TYPE_E alarm_type, ptc_cfg_alarm_t* alarm)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_setCfg_alarm(dev, alarm_type, alarm);
		return hdcctv_client2_setAlarmCfg(dev, alarm_type, alarm);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_setCfg_alarm(dev, alarm_type, alarm);
}

TOOL_INT32 nc_getCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* workMode)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
			return nc_qh_getCfg_all(dev, NC_QH_CFG_WORKMODE, workMode);
		}
		return hdcctv_client2_getCfg(dev, NC_QH_CFG_WORKMODE, workMode);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
	{
		return nc_qh_getCfg_all(dev, NC_QH_CFG_WORKMODE, workMode);
	}
}

TOOL_INT32 nc_setCfg_workMode(nc_dev* dev, ptc_cfg_workMode_t* workMode)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
			return nc_qh_setCfg_all(dev, NC_QH_CFG_WORKMODE, workMode);
		}
		return hdcctv_client2_setCfg(dev, NC_QH_CFG_WORKMODE, workMode);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
	{
//		return nc_qh_setCfg_workMode(dev, workMode);
		return nc_qh_setCfg_all(dev, NC_QH_CFG_WORKMODE, workMode);
	}
}

TOOL_INT32 nc_getCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* mobile)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_getCfg_mobile(dev, mobile);
}

TOOL_INT32 nc_setCfg_mobile(nc_dev* dev, ptc_cfg_mobile_t* mobile)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_setCfg_mobile(dev, mobile);
}

TOOL_INT32 nc_getCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* wifi)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
//			return nc_qh_getCfg_wifi(dev, wifi);
			return nc_qh_getCfg_all(dev, NC_QH_CFG_WIFI, wifi);
		}
//		return hdcctv_client2_getWiFiCfg(dev, wifi);
		return hdcctv_client2_getCfg(dev, NC_QH_CFG_WIFI, wifi);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
	{
//		return nc_qh_getCfg_wifi(dev, wifi);
		return nc_qh_getCfg_all(dev, NC_QH_CFG_WIFI, wifi);
	}
}

TOOL_INT32 nc_setCfg_wifi(nc_dev* dev, ptc_cfg_wifi_t* wifi)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
//			return nc_qh_setCfg_wifi(dev, wifi);
			return nc_qh_setCfg_all(dev, NC_QH_CFG_WIFI, wifi);
		}
//		return hdcctv_client2_setWiFiCfg(dev, wifi);
		return hdcctv_client2_setCfg(dev, NC_QH_CFG_WIFI, wifi);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
	{
//		return nc_qh_setCfg_wifi(dev, wifi);
		return nc_qh_setCfg_all(dev, NC_QH_CFG_WIFI, wifi);
	}
}

TOOL_INT32 nc_getCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* devRecord)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
//			return nc_qh_getCfg_devRecord(dev, devRecord);
			return nc_qh_getCfg_all(dev, NC_QH_CFG_DEVRECORD, devRecord);
		}
//		return hdcctv_client2_getDevRecordCfg(dev, devRecord);
		return hdcctv_client2_getCfg(dev, NC_QH_CFG_DEVRECORD, devRecord);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
	{
//		return nc_qh_getCfg_devRecord(dev, devRecord);
		return nc_qh_getCfg_all(dev, NC_QH_CFG_DEVRECORD, devRecord);
	}
}

TOOL_INT32 nc_setCfg_devRecord(nc_dev* dev, ptc_cfg_devRecord_t* devRecord)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
//			return nc_qh_setCfg_devRecord(dev, devRecord);
			return nc_qh_setCfg_all(dev, NC_QH_CFG_DEVRECORD, devRecord);
		}
//		return hdcctv_client2_setDevRecordCfg(dev, devRecord);
		return hdcctv_client2_setCfg(dev, NC_QH_CFG_DEVRECORD, devRecord);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
	{
//		return nc_qh_setCfg_devRecord(dev, devRecord);
		return nc_qh_setCfg_all(dev, NC_QH_CFG_DEVRECORD, devRecord);
	}
}

TOOL_INT32 nc_getCfg_av(nc_dev* dev, ptc_cfg_av_t* av)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_getCfg_all(dev, NC_QH_CFG_AV, av);
		return hdcctv_client2_getCfg(dev, HDCCTV_CLIENT_CFG_AV, av);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_getCfg_all(dev, NC_QH_CFG_AV, av);

}

TOOL_INT32 nc_setCfg_av(nc_dev* dev, ptc_cfg_av_t* av)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_setCfg_all(dev, NC_QH_CFG_AV, av);
		return hdcctv_client2_setCfg(dev, HDCCTV_CLIENT_CFG_AV, av);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_setCfg_all(dev, NC_QH_CFG_AV, av);

}

TOOL_INT32 nc_searchDates(nc_dev* dev, ptc_cfg_dates_t* dates)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_searchDates(dev, dates);
		return hdcctv_client2_searchDevDates(dev, dates);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_searchDates(dev, dates);
}

TOOL_INT32 nc_searchRecords(nc_dev* dev, ptc_cfg_records_t* records)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_searchRecords(dev, records);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return nc_ys_searchRecords(dev, records);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_searchRecords(dev, records);
		return hdcctv_client2_searchDevRecords(dev, records);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_searchRecords(dev, records);
	}
	else
		return nc_qh_searchRecords(dev, records);
}

TOOL_INT32 nc_startPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_startPbs(dev, record, cb, param);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return nc_ys_startPbs(dev, record, cb, param);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
			return nc_qh_startPbs(dev, record, cb, param);
		return hdcctv_client2_startDevPbs(dev, record, cb, param);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_startPbs(dev, record, cb, param);
	}
	else
		return nc_qh_startPbs(dev, record, cb, param);
}

TOOL_INT32 nc_setPbsPos(TOOL_INT32 user_id, TOOL_UINT32 pos)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_setPbsPos(user_id, pos);
}

TOOL_INT32 nc_pausePbs(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_pausePbs(user_id);
}

TOOL_INT32 nc_restartPbs(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_restartPbs(user_id);
}

TOOL_INT32 nc_setPbsSpeed(TOOL_INT32 user_id, TOOL_INT32 speed)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_setPbsSpeed(user_id, speed);
}

TOOL_INT32 nc_stopPbs(TOOL_INT32 user_id)
{
	if ((user_id & 0xFF000000) == NC_PTC_HuaYi)
	{
		return nc_hy_stopPbs(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_YuShi || (user_id & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		return nc_ys_stopPbs(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return hdcctv_client2_stopDevPbs(user_id);
	}
	else if ((user_id & 0xFF000000) == NC_PTC_XiongMai)
	{
		return nc_xm_stopPbs(user_id);
	}
	else
		return nc_qh_stopPbs(user_id);
}

TOOL_INT32 nc_searchLogs(nc_dev* dev, ptc_logs_t* logs)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return nc_hy_searchLogs(dev, logs);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return nc_ys_searchLogs(dev, logs);
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_searchLogs(dev, logs);
}

TOOL_INT32 nc_searchLogs_v2(nc_dev* dev, ptc_logs_v2_t* pst_logs_v2)
{
	TOOL_INT32 i = 0;
	TOOL_INT32 ret = 0;
	ptc_logs_t st_logs;
	st_logs.start_time = pst_logs_v2->start_time;
	st_logs.end_time = pst_logs_v2->end_time;
	st_logs.num = 0;

	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
	{
		ret = nc_hy_searchLogs(dev, &st_logs);
		pst_logs_v2->num = st_logs.num;
		for (i = 0; i < st_logs.num; i++)
		{
			tool_mem_memset(&pst_logs_v2->log_v2[i], sizeof(ptc_log_v2_t));
			tool_mem_memcpy(&pst_logs_v2->log_v2[i], &st_logs.log[i], sizeof(ptc_log_t));
		}
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
	{
		ret = nc_ys_searchLogs(dev, &st_logs);
		pst_logs_v2->num = st_logs.num;
		for (i = 0; i < st_logs.num; i++)
		{
			tool_mem_memset(&pst_logs_v2->log_v2[i], sizeof(ptc_log_v2_t));
			tool_mem_memcpy(&pst_logs_v2->log_v2[i], &st_logs.log[i], sizeof(ptc_log_t));
		}
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		if (nc_getDevById(dev) > 0)
		{
			ret = nc_qh_searchLogs(dev, &st_logs);
			pst_logs_v2->num = st_logs.num;
			for (i = 0; i < st_logs.num; i++)
			{
				tool_mem_memset(&pst_logs_v2->log_v2[i], sizeof(ptc_log_v2_t));
				tool_mem_memcpy(&pst_logs_v2->log_v2[i], &st_logs.log[i], sizeof(ptc_log_t));
			}
		}
		else
			ret = hdcctv_client2_searchLogs(dev, pst_logs_v2);
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		ret = nc_xm_searchLogs(dev, pst_logs_v2);
	}
	else
	{
		ret = nc_qh_searchLogs(dev, &st_logs);
		pst_logs_v2->num = st_logs.num;
		for (i = 0; i < st_logs.num; i++)
		{
			tool_mem_memset(&pst_logs_v2->log_v2[i], sizeof(ptc_log_v2_t));
			tool_mem_memcpy(&pst_logs_v2->log_v2[i], &st_logs.log[i], sizeof(ptc_log_t));
		}
	}
	
	return ret;
}


TOOL_INT32 nc_sendData(nc_dev* dev, TOOL_VOID* in_data, TOOL_UINT32 in_len, TOOL_VOID* out_data, TOOL_UINT32* out_len)
{
	if ((dev->ptc & 0xFF000000) == NC_PTC_HuaYi)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_YuShi || (dev->ptc & 0xFF000000) == NC_PTC_YuShi_Cloud)
		return NC_ERROR_NOSUPPORTED;
	else if ((dev->ptc & 0xFF000000) == NC_PTC_QiHan_Cloud)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else if ((dev->ptc & 0xFF000000) == NC_PTC_XiongMai)
	{
		return NC_ERROR_NOSUPPORTED;
	}
	else
		return nc_qh_sendData(dev, in_data, in_len, out_data, out_len);
}

TOOL_INT32 nc_startQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, HDCCTV_LANUAGE_E language, NC_CB cb, TOOL_VOID* param)
{
	return hdcctv_client_startQpns(token_type, mobile_token, language, cb, param);
}

TOOL_VOID nc_stopQpns()
{
	hdcctv_client_stopQpns();
}

TOOL_INT32 nc_getAlarms(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, TOOL_INT32 no, hdcctv_qpns_alarms_t* alarms)
{
	return hdcctv_client2_getAlarms(token_type, mobile_token, no, alarms);
}

TOOL_INT32 nc_getQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	return hdcctv_client2_getQpnsDeviceMaps(token_type, mobile_token, device_maps);
}

TOOL_INT32 nc_setQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	return hdcctv_client2_setQpnsDeviceMaps(token_type, mobile_token, device_maps);
}

TOOL_INT32 nc_addQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	return hdcctv_client2_addQpnsDeviceMaps(token_type, mobile_token, device_maps);
}

TOOL_INT32 nc_delQpnsDeviceMaps(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, hdcctv_qpns_device_maps_t* device_maps)
{
	return hdcctv_client2_delQpnsDeviceMaps(token_type, mobile_token, device_maps);
}

TOOL_INT32 nc_testQpns(HDCCTV_QPNS_TOKEN_TYPE_E token_type, TOOL_INT8* mobile_token, TOOL_INT8* dev_id)
{
	return hdcctv_client2_testQpns(token_type, mobile_token, dev_id);
}

TOOL_INT32 nc_getSms_reg(hdcctv_cloud_user_t* pst_cloud_user)
{
	return hdcctv_client2_getSms(pst_cloud_user, "reg");
}

TOOL_INT32 nc_regUser(hdcctv_cloud_user_t* pst_cloud_user, TOOL_INT8* code)
{
	return hdcctv_client2_regUser(pst_cloud_user, code);
}

TOOL_INT32 nc_getSms_pswd(hdcctv_cloud_user_t* pst_cloud_user)
{
	return hdcctv_client2_getSms(pst_cloud_user, "pswd");
}

TOOL_INT32 nc_resetPswd(hdcctv_cloud_user_t* pst_cloud_user, char* code)
{
	return hdcctv_client2_resetPswd(pst_cloud_user, code);
}

TOOL_INT32 nc_loginUser(hdcctv_cloud_user_t* pst_cloud_user)
{
	return hdcctv_client2_loginUser(pst_cloud_user);
}

TOOL_INT32 nc_logoutUser()
{
	return hdcctv_client2_logout();
}

TOOL_INT32 nc_setPswd(hdcctv_cloud_user_t* pst_cloud_user, TOOL_INT8* new_pswd)
{
	return hdcctv_client2_setPswd(pst_cloud_user, new_pswd);
}

TOOL_INT32 nc_addDev(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_t* pst_device)
{
	return hdcctv_client2_bindDev(pst_cloud_user, pst_device, "add");
}

TOOL_INT32 nc_delDev(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_t* pst_device)
{
	return hdcctv_client2_bindDev(pst_cloud_user, pst_device, "del");
}

TOOL_INT32 nc_getDevList(hdcctv_cloud_user_t* pst_cloud_user, hdcctv_device_list_t* pst_device_list)
{
	return hdcctv_client2_getDevList(pst_cloud_user, pst_device_list);
}

TOOL_INT32 nc_getCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, TOOL_INT8* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg)
{
	return hdcctv_client2_getCloudCfg(pst_cloud_user, dev_id, pst_cloud_cfg);
}

TOOL_INT32 nc_setCloudCfg(hdcctv_cloud_user_t* pst_cloud_user, TOOL_INT8* dev_id, hdcctv_cloud_cfg_t* pst_cloud_cfg)
{
	return hdcctv_client2_setCloudCfg(pst_cloud_user, dev_id, pst_cloud_cfg);
}

TOOL_INT32 nc_searchCloudDates(nc_dev* dev, ptc_cfg_dates_t* dates)
{
	return hdcctv_client2_searchCloudDates(dev, dates);
}

TOOL_INT32 nc_searchCloudRecords(nc_dev* dev, ptc_cfg_records_t* records)
{
	return hdcctv_client2_searchCloudRecords(dev, records);
}

TOOL_INT32 nc_startCloudPbs(nc_dev* dev, ptc_cfg_record_t* record, NC_CB cb, void* param)
{
	return hdcctv_client2_startCloudPbs(dev, record, cb, param);
}

TOOL_INT32 nc_stopCloudPbs(TOOL_INT32 pbs_id)
{
	return hdcctv_client2_stopCloudPbs(pbs_id);
}

#if defined __cplusplus
}
#endif
