
#ifndef __TOOL_JSON_H__
#define __TOOL_JSON_H__

#if defined __cplusplus
extern "C"
{
#endif


#include "tool_type.h"

typedef enum
{
	TOOL_JSON_ITEM_TYPE_STRING		= 0,
	TOOL_JSON_ITEM_TYPE_NULL		= 1,
	TOOL_JSON_ITEM_TYPE_NUMBER		= 2,
}TOOL_JSON_ITEM_TYPE_E;


typedef struct
{
	TOOL_INT32 	n32_no;
	TOOL_INT8*	pch_name;
	TOOL_INT8*	pch_value;
	TOOL_INT32	n32_number;
	
	TOOL_JSON_ITEM_TYPE_E e_type;
	TOOL_INT8*	pch_array_name;
	TOOL_INT32	n32_array_index;
}tool_json_item_t;

#define TOOL_JSON_STR_SIZE		(4*1024)
#define TOOL_JSON_ITEM_NUM		1024

typedef struct
{
	TOOL_INT8 sz_str[TOOL_JSON_STR_SIZE];
	tool_json_item_t st_item[TOOL_JSON_ITEM_NUM];
	TOOL_INT32 n32_num;
	TOOL_INT8* pch_index;
	
	TOOL_INT8*	pch_cur_array_name;
	TOOL_INT32	n32_cur_array_index;
}tool_json_t;

TOOL_INT32 tool_json_parseMidBracket(tool_json_t* pst_json, TOOL_INT8* pch_array_name);
TOOL_INT32 tool_json_parseBigBracket(tool_json_t* pst_json);

TOOL_INT32 tool_json_parse(tool_json_t* pst_json, TOOL_INT8* pch_str);
TOOL_INT8* tool_json_getValue(tool_json_t* pst_json, TOOL_INT8* pch_name);
TOOL_INT8* tool_json_getValue_V2(tool_json_t* pst_json, TOOL_INT8* pch_array_name, TOOL_INT32 n32_array_index, TOOL_INT8* pch_name);


TOOL_INT32 tool_json2_getValue(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_INT8* pch_value, TOOL_INT32 size);
TOOL_INT32 tool_json2_getNumber(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_INT32* pch_number);
TOOL_INT32 tool_json2_getInt64(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_INT64* pch_number);
TOOL_INT32 tool_json2_getIpv4(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_UINT8* pch_ip);


TOOL_INT32 tool_json2_getArray(TOOL_VOID* pch_str, TOOL_INT8* pch_array_name, TOOL_INT8* pch_array_value, TOOL_INT32 x_size, TOOL_INT32 y_size);


#if defined __cplusplus
}
#endif

#endif

