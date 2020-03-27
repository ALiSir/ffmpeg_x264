
#if defined __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "tool_log.h"
#include "tool_http.h"
#include "tool_type.h"
#include "tool_sysf.h"

typedef struct
{
	TOOL_INT32 	code;
	TOOL_INT8	desc[TOOL_HTTP_STR_SIZE];
}tool_http_code_t;

TOOL_INT32 tool_http_GetDesc(TOOL_INT32 code, TOOL_INT8 *desc, TOOL_INT32 size)
{
	if (desc == NULL || size <= 0)
		return -1;

	static tool_http_code_t pst_http_code[] = 
	{
		{100, "Continue"},
		{101, "Switching Protocols"},
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-Authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partial Content"},
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{305, "Use Proxy"},
		{307, "Temporary Redirect"},
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Time-out"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Request Entity Too Large"},
		{414, "Request-URI Too Large"},
		{415, "Unsupported Media Type"},
		{416, "Requested range not satisfiable"},
		{417, "Expectation Failed"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Time-out"},
		{505, "HTTP Version not supported"},
		{000, ""}
	};

	TOOL_INT32 i = 0;
	while (1)
	{
		if (pst_http_code[i].code == 0)
			return -1;

		if (pst_http_code[i].code == code)
		{
			tool_str_strncpy(desc, pst_http_code[i].desc, size-1);
			break;
		}
		i++;
	}

	return 0;
}

TOOL_INT32 tool_http_ParseLine(tool_http_t* http, TOOL_INT8* line)
{
	if (http == NULL || line == NULL)
		log_fatal("arg");

	TOOL_INT8* begin = NULL, *end = NULL;

	if (tool_str_strstr(line, " HTTP/"))
	{
		begin = line;
		end = tool_str_strstr(begin, " ");
		if (end == NULL || end-begin >= TOOL_HTTP_PARAM_SIZE)
		{
			log_error("can't find first blank(%s)", line);
			return -1;
		}
		tool_str_strncpy(http->command, begin, end-begin);
		
		begin = end+1;
		end = tool_str_strstr(begin, " HTTP/");
		if (end == NULL || end-begin >= TOOL_HTTP_URL_SIZE)
		{
			log_error("can't find second blank(%s)", line);
			return -1;
		}
		tool_str_strncpy(http->url, begin, end-begin);

		begin = end+1;
		end = tool_str_strstr(begin, "\r\n");
		if (end == NULL || end-begin >= TOOL_HTTP_PARAM_SIZE)
		{
			log_error("can't find CLRF(%s)", line);
			return -1;
		}
		tool_str_strncpy(http->version, begin, end-begin);
		return 0;
	}

	if (tool_str_strncmp(line, "HTTP/", tool_str_strlen("HTTP/")) == 0)
	{
		begin = line;
		end = tool_str_strstr(begin, " ");
		if (end == NULL || end-begin >= TOOL_HTTP_PARAM_SIZE)
		{
			log_error("can't find first blank(%s)", line);
			return -1;
		}
		tool_str_strncpy(http->version, begin, end-begin);
	
		begin = end+1;
		end = tool_str_strstr(begin, " ");
		if (end == NULL || end-begin >= TOOL_HTTP_PARAM_SIZE)
		{
			log_error("can't second second blank(%s)", line);
			return -1;
		}
		tool_str_strncpy(http->command, begin, end-begin);
		
		begin = end+1;
		end = tool_str_strstr(begin, "\r\n");
		if (end == NULL || end-begin >= TOOL_HTTP_URL_SIZE)
		{
			log_error("can't find CLRF(%s)", line);
			return -1;
		}
		tool_str_strncpy(http->url, begin, end-begin);
		return 0;		
	}

	begin = tool_str_strstr(line, ":");
	if (begin == NULL)
	{
		log_error("can't find :(%s)", line);
		return -1;
	}
	
	begin++;
	while (*begin == ' ')
		begin++;
	
	end = tool_str_strstr(line, "\r\n");
	if (end == NULL)
	{
		log_error("can't find CLRF(%s)", line);
		return -1;
	}
	
	if (end-begin >= TOOL_HTTP_PARAM_SIZE)
	{
		log_error("end-begin(%d)>=(%d)", end-begin, TOOL_HTTP_PARAM_SIZE);
		return -1;
	}

	if (tool_str_strncmp(line, "Seq", tool_str_strlen("Seq")) == 0)
	{
		http->Seq = tool_sysf_atoi(begin);
	}
	else if (tool_str_strncmp(line, "Session", tool_str_strlen("Session")) == 0)
	{
		tool_str_strncpy(http->Session, begin, end-begin);
	}
	else if (tool_str_strncmp(line, "x-sessioncookie", tool_str_strlen("x-sessioncookie")) == 0)
	{
		tool_str_strncpy(http->x_sessioncookie, begin, end-begin);
	}
	else if (tool_str_strncmp(line, "Content-Type", tool_str_strlen("Content-Type")) == 0)
	{
		tool_str_strncpy(http->ContentType, begin, end-begin);
	}
	else if (tool_str_strncmp(line, "Authorization", tool_str_strlen("Authorization")) == 0)
	{
		tool_str_strncpy(http->Authorization, begin, end-begin);
	}
	else if (tool_str_strncmp(line, "Host", tool_str_strlen("Host")) == 0)
	{
		tool_str_strncpy(http->Host, begin, end-begin);
	}
	else if (tool_str_strncmp(line, "Content-Length", tool_str_strlen("Content-Length")) == 0)
	{
		http->ContentLength = tool_sysf_atoi(begin);
		if (http->ContentLength >= 32767)
			http->ContentLength = 0;
	}

	return 0;
}

TOOL_VOID tool_http_Init(tool_http_t *http)
{
	if (http == NULL)
		log_fatal("arg");

	tool_mem_memset(http, sizeof(tool_http_t)-TOOL_HTTP_BODY_SIZE);
}

TOOL_INT32 tool_http_Recv(tool_http_t *http, tool_sock_item_t* sock, TOOL_INT8* buf, TOOL_INT32* size)
{
	if (http == NULL)
		log_fatal("arg error");
	
	TOOL_INT32 len = 0, ret = 0;
	TOOL_INT8 tmpBuf[TOOL_HTTP_MSG_SIZE] = {0};
	TOOL_INT8 line[TOOL_HTTP_LINE_SIZE] = {0};
	tool_mem_memset(http, sizeof(tool_http_t));
	
	while (1)
	{
		tool_mem_memset(line, TOOL_HTTP_LINE_SIZE);
		ret = tool_sock_item_recvLine(sock, line, TOOL_HTTP_LINE_SIZE);
		if (ret < 0)
		{
			log_error("recv http line");
			return -1;
		}

		if (len+ret > TOOL_HTTP_MSG_SIZE)
		{
			log_error("len(%d)+ret(%d) > TOOL_HTTP_MSG_SIZE(%d)", len, ret, TOOL_HTTP_MSG_SIZE);
			return -1;
		}
		tool_mem_memcpy(tmpBuf+len, line, ret);
		len += ret;
		
		if (tool_str_strncmp(line, "\r\n", tool_str_strlen("\r\n")) == 0)
			break;

		if (tool_http_ParseLine(http, line) < 0)
		{
			log_error("parse http line");
			return -1;
		}
	}

	if (http->ContentLength > 0)
	{
		if (http->ContentLength > TOOL_HTTP_BODY_SIZE)
		{
			log_error("ContentLength(%d)", http->ContentLength);
			return -1;
		}
	
		if (tool_sock_item_recv(sock, http->body, http->ContentLength) != http->ContentLength)
		{
			log_error("recv body error");
			return -1;
		}

		if (len+http->ContentLength > TOOL_HTTP_MSG_SIZE)
		{
			log_error("len(%d)+ContentLength(%d) > TOOL_HTTP_MSG_SIZE(%d)", len, http->ContentLength, TOOL_HTTP_MSG_SIZE);
			return -1;
		}
		tool_mem_memcpy(tmpBuf+len, http->body, http->ContentLength);
		len += http->ContentLength;
	}
//	log_debug("http recv(%s)", tmpBuf);

	if (buf && size)
	{
		if (*size < len)
		{
			log_error("size(%d) < len(%d)", *size, len);
			return -1;
		}
		tool_mem_memcpy(buf, tmpBuf, len);
		*size = len;
	}

	return 0;	
}

TOOL_INT32 tool_http_Send(tool_http_t *http, tool_sock_item_t* sock, TOOL_INT8* buf, TOOL_INT32* size)
{
	if (http == NULL)
		log_fatal("arg error");

	TOOL_INT8 send_buf[TOOL_HTTP_MSG_SIZE] = {0};
	TOOL_INT32 code = tool_sysf_atoi(http->command);
	if (code)
	{
		TOOL_INT8 desc[TOOL_HTTP_STR_SIZE] = {0};
		if (tool_http_GetDesc(code, desc, TOOL_HTTP_STR_SIZE))
		{
			log_error("can't find desc by(%d)", code);
			return -1;
		}
	
		snprintf(send_buf, TOOL_HTTP_MSG_SIZE-1, "%s %s %s\r\n", http->version, http->command, desc);
	}
	else
	{
		snprintf(send_buf, TOOL_HTTP_MSG_SIZE-1, "%s %s %s\r\n", http->command, http->url, http->version);	
	}

	TOOL_INT8 line[TOOL_HTTP_LINE_SIZE] = {0};
	
	if (http->Seq)
	{
		snprintf(line, TOOL_HTTP_LINE_SIZE-1, "Seq:%d\r\n", http->Seq);
		tool_str_strcat(send_buf, line);
	}

	if (tool_str_strlen(http->Session))
	{
		snprintf(line, TOOL_HTTP_LINE_SIZE-1, "Session:%s\r\n", http->Session);
		tool_str_strcat(send_buf, line);
	}

	if (tool_str_strlen(http->x_sessioncookie))
	{
		snprintf(line, TOOL_HTTP_LINE_SIZE-1, "x-sessioncookie:%s\r\n", http->x_sessioncookie);
		tool_str_strcat(send_buf, line);
	}

	if (tool_str_strlen(http->ContentType))
	{
		snprintf(line, TOOL_HTTP_LINE_SIZE-1, "Content-Type:%s\r\n", http->ContentType);
		tool_str_strcat(send_buf, line);
	}

	if (tool_str_strlen(http->Authorization))
	{
		snprintf(line, TOOL_HTTP_LINE_SIZE-1, "Authorization:%s\r\n", http->Authorization);
		tool_str_strcat(send_buf, line);
	}

	if (tool_str_strlen(http->Host))
	{
		snprintf(line, TOOL_HTTP_LINE_SIZE-1, "Host:%s\r\n", http->Host);
		tool_str_strcat(send_buf, line);
	}

	if (http->ContentLength)
	{
		snprintf(line, TOOL_HTTP_LINE_SIZE-1, "Content-Length:%d\r\n", http->ContentLength);
		tool_str_strcat(send_buf, line);
	}

	tool_str_strcat(send_buf, "\r\n");

	TOOL_INT32 len = tool_str_strlen(send_buf);
	if (http->ContentLength)
	{
		if (len + http->ContentLength > TOOL_HTTP_BODY_SIZE)
		{
			log_error("len(%d)+ContentLength(%d)>TOOL_HTTP_BODY_SIZE(%d)", 
				len, http->ContentLength, TOOL_HTTP_BODY_SIZE);
			return -1;
		}
	
		tool_mem_memcpy(send_buf+len, http->body, http->ContentLength);
		len += http->ContentLength;
	}

	if (sock >= 0)
	{
		if (tool_sock_item_send(sock, send_buf, len) != len)
		{
			log_error("resp HTTP (%s)", http->command);
			return -1;
		}
//		log_debug("http send_buf(%s)", send_buf);
	}

	if (buf && size)
	{
		if (*size > len)
		{
			tool_mem_memcpy(buf, send_buf, len);
		}
		else
		{
			tool_mem_memcpy(buf, send_buf, *size);
		}
		*size = len;
	}

	return 0;
}

#if defined __cplusplus
}
#endif

