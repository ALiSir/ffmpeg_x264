
#if defined __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "tool_log.h"
#include "tool_json.h"
#include "tool_sysf.h"

#define TOOL_JSON_NULL					"TOOL_JSON_NULL"
#define TOOL_JSON_ROOT_ARRAY_NAME		"TOOL_JSON_ROOT_ARRAY_NAME"

TOOL_INT32 tool_json_parseDoubleQuote(tool_json_t* pst_json)
{
	while (*(pst_json->pch_index) != '"')
	{
		if (*(pst_json->pch_index) == 0)
		{
			log_error("can not find '\"'");
			return -1;
		}
		pst_json->pch_index ++;
	}
	return 0;
}

TOOL_INT32 tool_json_parseDoubleQuoteCaseMidBracket(tool_json_t* pst_json)
{
	while (*(pst_json->pch_index) != '"' && *(pst_json->pch_index) != '[')
	{
		if (*(pst_json->pch_index) == 0)
		{
			log_error("can not find '\"'");
			return -1;
		}
		pst_json->pch_index ++;
	}
	return 0;
}

TOOL_INT32 tool_json_parseSemicolon(tool_json_t* pst_json)
{
	while (*(pst_json->pch_index) != ':')
	{
		if (*(pst_json->pch_index) == 0)
		{
			log_error("can not find '\"'");
			return -1;
		}
		pst_json->pch_index ++;
	}
	return 0;
}

TOOL_INT32 tool_json_parseCommaCaseBigBracket(tool_json_t* pst_json)
{
	while (*(pst_json->pch_index) != ',')
	{
		if (*(pst_json->pch_index) == '}')
			return 0;
		if (*(pst_json->pch_index) == 0)
		{
			log_error("can not find ','");
			return -1;
		}		
		pst_json->pch_index ++;
	}
	return 0;
}

TOOL_INT32 tool_json_parseCommaCaseMidBracket(tool_json_t* pst_json)
{
	while (*(pst_json->pch_index) != ',')
	{
		if (*(pst_json->pch_index) == ']')
			return 0;
		if (*(pst_json->pch_index) == 0)
		{
			log_error("can not find ','");
			return -1;
		}		
		pst_json->pch_index ++;
	}
	return 0;
}


TOOL_INT32 tool_json_parseNull(tool_json_t* pst_json)
{
	TOOL_INT8* pch_index = pst_json->pch_index;
	while (1)
	{
		if (*pch_index == 0)
		{
			log_error("can not find null");
			return -1;
		}
		if (*pch_index == ',' || *pch_index == '}' || *pch_index == '"')
			return 0;
		if (tool_str_strncmp(pch_index, "null", 4) == 0)
			break;
		pch_index++;
	}
	pst_json->st_item[pst_json->n32_num].pch_value = TOOL_JSON_NULL;
	pst_json->st_item[pst_json->n32_num].e_type = TOOL_JSON_ITEM_TYPE_NULL;
	pst_json->st_item[pst_json->n32_num].pch_array_name = pst_json->pch_cur_array_name;
	pst_json->st_item[pst_json->n32_num].n32_array_index = pst_json->n32_cur_array_index;
	pst_json->n32_num ++;	
	pst_json->pch_index = pch_index+4;
	return 1;
}

TOOL_INT32 tool_json_parseNumber(tool_json_t* pst_json)
{
	TOOL_INT8* pch_index = pst_json->pch_index;
	TOOL_INT32 flag = 0;
	while (1)
	{
		if (*pch_index == 0)
		{
			log_error("can not find null");
			return -1;
		}

		if (flag == 0)
		{
			if (*pch_index == '"' || *pch_index == '[')
				return 0;

			if (*pch_index == '-' || (*pch_index >= '0' && *pch_index <= '9'))
			{
				flag = 1;
			}
			else
			{
				log_error("*pch_index(%d)(%c)", *pch_index, *pch_index);
				return -1;
			}			
		}
		else
		{
			if (*pch_index == ',' || *pch_index == '}')
				break;
			if (*pch_index < '0' || *pch_index > '9')
			{
				log_error("*pch_index(%d)(%c)", *pch_index, *pch_index);
				return -1;
			}
		}
		pch_index++;
	}

	pst_json->st_item[pst_json->n32_num].pch_value = pst_json->pch_index;
	pst_json->st_item[pst_json->n32_num].n32_number = tool_sysf_atoi(pst_json->pch_index);
	pst_json->st_item[pst_json->n32_num].e_type = TOOL_JSON_ITEM_TYPE_NUMBER;
	pst_json->st_item[pst_json->n32_num].pch_array_name = pst_json->pch_cur_array_name;
	pst_json->st_item[pst_json->n32_num].n32_array_index = pst_json->n32_cur_array_index;
	pst_json->n32_num ++;	
	pst_json->pch_index = pch_index;

	return 1;
}



TOOL_INT32 tool_json_parseName(tool_json_t* pst_json)
{
	//name-begin
	if (tool_json_parseDoubleQuote(pst_json) < 0)
		return -1;
	pst_json->pch_index ++;

	//name-end
	pst_json->st_item[pst_json->n32_num].pch_name = pst_json->pch_index;
	if (tool_json_parseDoubleQuote(pst_json) < 0)
		return -1;
	*(pst_json->pch_index) = 0;
	pst_json->pch_index ++;

	//Semicolon
	if (tool_json_parseSemicolon(pst_json) < 0)
		return -1;
	pst_json->pch_index ++;

	return 0;
}

TOOL_INT32 tool_json_parseValue(tool_json_t* pst_json)
{
	TOOL_INT32 ret = tool_json_parseNull(pst_json);
	if (ret < 0)
		return -1;
	else if (ret > 0)
		return 0;

	ret = tool_json_parseNumber(pst_json);
	if (ret < 0)
		return -1;
	else if (ret > 0)
		return 0;

	//value-begin
	if (tool_json_parseDoubleQuoteCaseMidBracket(pst_json) < 0)
		return -1;
	if (*(pst_json->pch_index) == '[')
	{
		if (tool_json_parseMidBracket(pst_json, pst_json->st_item[pst_json->n32_num].pch_name) < 0)
			return -1;
		return 0;
	}
	pst_json->pch_index ++;

	//value-end
	pst_json->st_item[pst_json->n32_num].pch_value = pst_json->pch_index;
	pst_json->st_item[pst_json->n32_num].e_type = TOOL_JSON_ITEM_TYPE_STRING;
	if (tool_json_parseDoubleQuote(pst_json) < 0)
		return -1;
	*(pst_json->pch_index) = 0;
	pst_json->st_item[pst_json->n32_num].pch_array_name = pst_json->pch_cur_array_name;
	pst_json->st_item[pst_json->n32_num].n32_array_index = pst_json->n32_cur_array_index;
	pst_json->n32_num ++;
	pst_json->pch_index ++;

	return 0;
}

TOOL_INT32 tool_json_parseMidBracket(tool_json_t* pst_json, TOOL_INT8* pch_array_name)
{
	// [
	pst_json->pch_index ++;

	TOOL_INT32 i = 0;
	while (1)
	{
		pst_json->pch_cur_array_name = pch_array_name;
		pst_json->n32_cur_array_index = i;

		if (tool_json_parseBigBracket(pst_json) < 0)
			return -1;
		pst_json->pch_index ++;

		if (tool_json_parseCommaCaseMidBracket(pst_json) < 0)
			return -1;
		if (*(pst_json->pch_index) == ']')
		{
			pst_json->pch_cur_array_name = TOOL_JSON_ROOT_ARRAY_NAME;
			pst_json->n32_cur_array_index = 0;
			break;
		}
		pst_json->pch_index ++;		

		i++;
	}	

	return 0;
}

TOOL_INT32 tool_json_parseBigBracket(tool_json_t* pst_json)
{
	while (*(pst_json->pch_index) != '{')
	{
		if (*(pst_json->pch_index) == 0)
		{
			log_error("can not find '{'");
			return -1;
		}
		pst_json->pch_index ++;
	}
	pst_json->pch_index ++;

	while (1)
	{	
		if (tool_json_parseName(pst_json) < 0)
			return -1;

		if (tool_json_parseValue(pst_json) < 0)
			return -1;

		if (tool_json_parseCommaCaseBigBracket(pst_json) < 0)
			return -1;
		if (*(pst_json->pch_index) == '}')	
			break;
		pst_json->pch_index ++;		
	}

	return 0;
}

TOOL_VOID tool_json_print(tool_json_t* pst_json, TOOL_INT8* pch_str)
{
	log_state("str(%s)", pch_str);
	TOOL_INT32 i = 0;
	for (i = 0; i < pst_json->n32_num; i++)
	{
//		log_state("i(%d) (%s)[%d] name(%d) value(%d)", i, pst_json->st_item[i].pch_array_name, pst_json->st_item[i].n32_array_index,
//			pst_json->st_item[i].pch_name, pst_json->st_item[i].pch_value);	

		if (pst_json->st_item[i].e_type == TOOL_JSON_ITEM_TYPE_NUMBER)
		{
			log_state("i(%d) (%s)[%d] name(%s) value(%d)", i, pst_json->st_item[i].pch_array_name, 
				pst_json->st_item[i].n32_array_index, pst_json->st_item[i].pch_name, pst_json->st_item[i].n32_number);
		}
		else
		{
			log_state("i(%d) (%s)[%d] name(%s) value(%s)", i, pst_json->st_item[i].pch_array_name, 
				pst_json->st_item[i].n32_array_index, pst_json->st_item[i].pch_name, pst_json->st_item[i].pch_value);
		}
	}
}

TOOL_INT32 tool_json_parse(tool_json_t* pst_json, TOOL_INT8* pch_str)
{
	if (pst_json == NULL || pch_str == NULL)
		log_fatal("pst_json(%x) str(%x)", pst_json, pch_str);

	tool_mem_memset(pst_json, sizeof(tool_json_t));
	TOOL_INT32 i = 0;
	for (i = 0; i < TOOL_JSON_ITEM_NUM; i++)
	{
		pst_json->st_item[i].n32_no = i;
	}
	tool_str_strncpy(pst_json->sz_str, pch_str, tool_str_strlen(pch_str));
	pst_json->pch_index = pst_json->sz_str;
	pst_json->pch_cur_array_name = TOOL_JSON_ROOT_ARRAY_NAME;
	pst_json->n32_cur_array_index = 0;

	if (tool_json_parseBigBracket(pst_json) < 0)
	{
		log_error("pch_str(%s)", pch_str);
		return -1;
	}

//	tool_json_print(pst_json, pch_str);
	
	return 0;
}

TOOL_INT8* tool_json_getValue(tool_json_t* pst_json, TOOL_INT8* pch_name)
{
	if (pst_json == NULL || pch_name == NULL)
		log_fatal("pst_json(%x) pch_name(%x)", pst_json, pch_name);
	TOOL_INT32 i = 0;
	for (i = 0; i < pst_json->n32_num; i++)
	{
		if (pst_json->st_item[i].pch_name == NULL)
			log_fatal("i(%d)", i);

		if (tool_mem_memcmp(pst_json->st_item[i].pch_name, pch_name, tool_str_strlen(pch_name)+1) == 0)
			return pst_json->st_item[i].pch_value;
	}
	return NULL;
}

TOOL_INT8* tool_json_getValue_V2(tool_json_t* pst_json, TOOL_INT8* pch_array_name, TOOL_INT32 n32_array_index, TOOL_INT8* pch_name)
{
	if (pst_json == NULL || pch_array_name == NULL || pch_name == NULL)
		log_fatal("pst_json(%x) pch_array_name(%x) pch_name(%x)", pst_json, pch_array_name, pch_name);
	TOOL_INT32 i = 0;
	for (i = 0; i < pst_json->n32_num; i++)
	{
		if (pst_json->st_item[i].pch_array_name == NULL || pst_json->st_item[i].pch_name == NULL)
			log_fatal("i(%d)", i);

		if (tool_mem_memcmp(pst_json->st_item[i].pch_array_name, pch_array_name, tool_str_strlen(pch_array_name)+1) == 0 &&
			pst_json->st_item[i].n32_array_index == n32_array_index &&
			tool_mem_memcmp(pst_json->st_item[i].pch_name, pch_name, tool_str_strlen(pch_name)+1) == 0)
			return pst_json->st_item[i].pch_value;
	}
	return NULL;
}


typedef enum
{
	TOOL_JSON_CH_BLANK				= 0,	//\r\t\n
	TOOL_JSON_CH_QUOTATION			= 1,	//"
	TOOL_JSON_CH_COLON				= 2,	//:
	TOOL_JSON_CH_COMMA				= 3,	//,
	TOOL_JSON_CH_BEGIN_BRACKET		= 4,	//[
	TOOL_JSON_CH_END_BRACKET		= 5,	//]
	TOOL_JSON_CH_BEGIN_BRACE		= 6,	//{
	TOOL_JSON_CH_END_BRACE			= 7,	//}
	TOOL_JSON_CH_FALSE				= 8,	//false
	TOOL_JSON_CH_TRUE				= 9,	//true
	TOOL_JSON_CH_NULL				= 10,	//null
	TOOL_JSON_CH_NUMBER				= 11,	//number
	TOOL_JONS_CH_FINISH				= 99,	//0
}TOOL_JSON_CH_E;


TOOL_INT32 tool_json2_char(TOOL_INT8* ch)
{
//	log_debug("*ch(%d)(%c)", *ch, *ch);
	
	if (*ch == ' ' || *ch == '\r' || *ch == '\n' || *ch == '\t' || *ch == '\v' || *ch == '\f')
		return TOOL_JSON_CH_BLANK;
	if (*ch == 0)
		return TOOL_JONS_CH_FINISH;	
	if (*ch == '"')
		return TOOL_JSON_CH_QUOTATION;
	if (*ch == ':')
		return TOOL_JSON_CH_COLON;
	if (*ch == ',')
		return TOOL_JSON_CH_COMMA;
	if (*ch == '[')
		return TOOL_JSON_CH_BEGIN_BRACKET;
	if (*ch == ']')
		return TOOL_JSON_CH_END_BRACKET;
	if (*ch == '{')
		return TOOL_JSON_CH_BEGIN_BRACE;
	if (*ch == '}')
		return TOOL_JSON_CH_END_BRACE;
	if (tool_str_strncmp(ch, "false", 5) == 0)
		return TOOL_JSON_CH_FALSE;
	if (tool_str_strncmp(ch, "true", 4) == 0)
		return TOOL_JSON_CH_TRUE;
	if (tool_str_strncmp(ch, "null", 4) == 0)
		return TOOL_JSON_CH_NULL;
	if (*ch == '+' || *ch == '-' || (*ch >= '0' && *ch <= '9'))
		return TOOL_JSON_CH_NUMBER;
	
	return -1;
}


TOOL_INT32 tool_json2_getValue(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_INT8* pch_value, TOOL_INT32 size)
{
	if (pch_str == NULL || pch_name == NULL || pch_value == NULL)
		log_fatal("pch_str(%x) pch_name(%x) pch_value(%x)", pch_str, pch_name, pch_value);

	TOOL_INT8 name[128] = {0};
	snprintf(name, sizeof(name), "\"%s\"", pch_name);
	TOOL_INT8* pch_index = tool_str_strstr((TOOL_INT8 *)pch_str, name);
	if (pch_index == NULL)
	{
		log_error("pch_index name(%s) pch_str(%s)", name, pch_str);
		return -1;
	}

	TOOL_INT32 offset = tool_str_strlen(name);
	pch_index += offset;

	TOOL_INT32 ret = 0;
	while (1)
	{
		ret = tool_json2_char(pch_index);		
		if (ret == TOOL_JSON_CH_COLON)
		{
			pch_index ++;
			break;
		}
		else if (ret == TOOL_JSON_CH_BLANK)
		{
			pch_index ++;
			continue;
		}
		else
		{
			log_error("Semicolon");
			return -1;
		}
	}

	TOOL_INT32 i = 0; 
	while (1)
	{
		ret = tool_json2_char(pch_index);		
		if (ret == TOOL_JSON_CH_QUOTATION)
		{
			pch_index ++;
			for (i = 0; i < size; i++)
			{
				if (i == size-1 || *(pch_index) == 0 || *(pch_index) == '"')
				{
					pch_value[i] = 0;
					break;
				}
				pch_value[i] = *(pch_index);
				pch_index ++;
			}
			break;
		}
		else if (ret == TOOL_JSON_CH_FALSE)
		{
			tool_str_strncpy(pch_value, "false", 5);
			break;
		}
		else if (ret == TOOL_JSON_CH_TRUE)
		{
			tool_str_strncpy(pch_value, "true", 4);
			break;
		}
		else if (ret == TOOL_JSON_CH_NULL)
		{
			tool_str_strncpy(pch_value, "null", 4);
			break;
		}
		else if (ret == TOOL_JSON_CH_NUMBER)
		{
			for (i = 0; i < size; i++)
			{
				if (i == size-1 || *(pch_index) == 0 || *(pch_index) == ',' || *(pch_index) == '}')
				{
					pch_value[i] = 0;
					break;
				}
				pch_value[i] = *(pch_index);
				pch_index ++;
			}
			break;
		}
		else if (ret == TOOL_JSON_CH_BLANK)
		{
			pch_index ++;
			continue;
		}
		else
		{
			log_error("value begin brace ret(%d)", ret);
			return -1;
		}
	}

	return 0;
}

TOOL_INT32 tool_json2_getNumber(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_INT32* pch_number)
{
	if (pch_str == NULL || pch_name == NULL || pch_number == NULL)
		log_fatal("pch_str(%x) pch_name(%x) pch_number(%x)", pch_str, pch_name, pch_number);
	
	TOOL_INT8 value[64] = {0};
	if (tool_json2_getValue(pch_str, pch_name, value, sizeof(value)) < 0)
		return -1;
	if (tool_str_strncmp(value, "false", 5) == 0)
		*pch_number = 0;
	else if (tool_str_strncmp(value, "true", 4) == 0)
		*pch_number = 1;
	else if (tool_str_strncmp(value, "0x", 2) == 0 || tool_str_strncmp(value, "0X", 2) == 0)
		*pch_number = tool_str_strtoul(value, 16);
	else
		*pch_number = tool_sysf_atoi(value);
	return 0;		
}

TOOL_INT32 tool_json2_getInt64(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_INT64* pch_number)
{
	if (pch_str == NULL || pch_name == NULL || pch_number == NULL)
		log_fatal("pch_str(%x) pch_name(%x) pch_number(%x)", pch_str, pch_name, pch_number);
	
	TOOL_INT8 value[64] = {0};
	if (tool_json2_getValue(pch_str, pch_name, value, sizeof(value)) < 0)
		return -1;
	if (tool_str_strncmp(value, "false", 5) == 0)
		*pch_number = 0;
	else if (tool_str_strncmp(value, "true", 4) == 0)
		*pch_number = 1;
	else if (tool_str_strncmp(value, "0x", 2) == 0 || tool_str_strncmp(value, "0X", 2) == 0)
		*pch_number = tool_str_strtoul(value, 16);
	else
		*pch_number = tool_sysf_atoll(value);
		log_debug("value(%s) *pch_number(%lld)", value, *pch_number);
	return 0;			
}


TOOL_INT32 tool_json2_getIpv4(TOOL_VOID* pch_str, TOOL_INT8* pch_name, TOOL_UINT8* pch_ip)
{
	if (pch_str == NULL || pch_name == NULL || pch_ip == NULL)
		log_fatal("pch_str(%x) pch_name(%x) pch_ip(%x)", pch_str, pch_name, pch_ip);
	
	TOOL_INT8 value[64] = {0};
	if (tool_json2_getValue(pch_str, pch_name, value, sizeof(value)) < 0)
		return -1;
	TOOL_INT32 ip0 = 0;
	TOOL_INT32 ip1 = 0;
	TOOL_INT32 ip2 = 0;
	TOOL_INT32 ip3 = 0;
	if (sscanf(value, "%d.%d.%d.%d", &ip0, &ip1, &ip2, &ip3) != 4)
	{
		log_error("value(%s)", value);
		return -1;
	}
	pch_ip[0] = ip0;
	pch_ip[1] = ip1;
	pch_ip[2] = ip2;
	pch_ip[3] = ip3;
	return 0;			
}


TOOL_INT32 tool_json2_getArray(TOOL_VOID* pch_str, TOOL_INT8* pch_array_name, TOOL_INT8* pch_array_value, TOOL_INT32 x_size, TOOL_INT32 y_size)
{	
	if (pch_str == NULL || pch_array_name == NULL || pch_array_value == NULL || x_size <= 0 || y_size <= 0)
	{
		log_fatal("pch_str(%x) pch_array_name(%x) pch_array_value(%x) x_size(%d) y_size(%d)", 
			pch_str, pch_array_name, pch_array_value, x_size, y_size);
	}
	
	TOOL_INT8* pch_index = NULL;

	while (1)
	{
		pch_index = tool_str_strstr((TOOL_INT8 *)pch_str, pch_array_name);
		if (pch_index == NULL)
		{
			log_error("pch_index(%s)", pch_str);
			return -1;
		}
		pch_index += tool_str_strlen(pch_array_name);		
		if (*pch_index == 0)
		{
			log_error("end");
			return -1;
		}
		if (*pch_index != '"')
		{
			pch_str = pch_index;
			continue;
		}
		pch_index ++;
		
		while (tool_json2_char(pch_index) == TOOL_JSON_CH_BLANK) {pch_index++;}
		if (*pch_index == 0)
		{
			log_error("end");
			return -1;
		}
		if (*pch_index != ':')
		{
			pch_str = pch_index;
			continue;
		}
		pch_index ++;
		
		while (tool_json2_char(pch_index) == TOOL_JSON_CH_BLANK) {pch_index++;}
		if (*pch_index == 0)
		{
			log_error("end");
			return -1;
		}
		if (*pch_index == '[')
			break;
	}
	pch_index ++;
	while (tool_json2_char(pch_index) == TOOL_JSON_CH_BLANK) {pch_index++;}

	if (*pch_index == ']')
		return 0;

	TOOL_INT32 i = 0;
	TOOL_INT32 j = 0;
	while (1)
	{
		if (i >= x_size)
		{
			log_error("x_size(%d)", x_size);
			return -1;
		}

		if (*pch_index == 0)
		{
			log_error("end");
			return -1;
		}
		if (*pch_index != '{')
		{
			log_error("{");
			return -1;
		}		

		j = 0;
		while (1)
		{
			if (j >= y_size)
			{
				log_error("y_size(%d)", y_size);
				return -1;
			}

			if (*pch_index == 0)
			{
				log_error("end");
				return -1;
			}
			if (*(pch_index-1) == '}')
			{			
				pch_array_value[i*y_size+j] = 0;
				break;
			}

			pch_array_value[i*y_size+j] = *pch_index;
			j++;
			pch_index ++;
		}
		i++;
		while (tool_json2_char(pch_index) == TOOL_JSON_CH_BLANK) {pch_index++;}

		if (*pch_index == 0)
		{
			log_error("end");
			return -1;
		}
		if (*pch_index == ']')
			break;
		if (*pch_index != ',')
		{
			log_error(",");
			return -1;
		}
		pch_index ++;
		while (tool_json2_char(pch_index) == TOOL_JSON_CH_BLANK) {pch_index++;}
	}

//	TOOL_INT32 k = 0;
//	for (k = 0; k < i; k ++)
//	{
//		log_debug("k(%d)(%s)", k, &pch_array_value[k*y_size]);
//	}

	
	return i;
}


#if defined __cplusplus
}
#endif

