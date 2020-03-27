

#if defined __cplusplus
extern "C"
{
#endif

#ifdef __IS_SSL__

#include "bio.h"
#include "ssl.h"
#include "err.h"
#include "tool_type.h"
#include "tool_log.h"
#include "tool_ssl.h"
#include "tool_sysf.h"
#include "tool_sock.h"

#define TOOL_SSL_TIMEOUT		5
#define TOOL_SSL_USLEEP			(100*1000)

TOOL_VOID tool_ssl_init()
{
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
}

TOOL_INT32 tool_ssl_connect(tool_ssl_t* tssl, TOOL_INT8* cer_file, TOOL_INT8* key_file, TOOL_INT8* server_cer, TOOL_INT8* addr)
{
	tool_mem_memset(tssl, sizeof(tool_ssl_t));
	TOOL_INT32 ret = 0;
	do
	{
		tssl->ctx = SSL_CTX_new(SSLv23_client_method());
		if (tssl->ctx == NULL)
		{
			log_error("SSL_CTX_load_verify_locations(%s)", ERR_reason_error_string(ERR_get_error()));
			ret = -1;
			return -1;
		}

		if (SSL_CTX_use_certificate_file(tssl->ctx, cer_file, SSL_FILETYPE_PEM) <= 0 ||
			SSL_CTX_use_PrivateKey_file(tssl->ctx, key_file, SSL_FILETYPE_PEM) <= 0 ||
			SSL_CTX_check_private_key(tssl->ctx) == 0)
		{
			log_error("SSL_CTX_load_verify_locations(%d)(%s)", ERR_get_error(), ERR_reason_error_string(ERR_get_error()));
			ret = -1;
			break;
		}
		
		if (!SSL_CTX_load_verify_locations(tssl->ctx, server_cer, NULL)) 
		{
			log_error("SSL_CTX_load_verify_locations(%d)(%s)", ERR_get_error(), ERR_reason_error_string(ERR_get_error()));
			ret = -1;
			break;
        }
		
		/*
		tool_sock_item_t sock_item;
		if (tool_sock_item_open_v2(&sock_item, TOOL_SOCK_TYPE1_TCP, TOOL_SOCK_TYPE2_CLIENT, "gateway.sandbox.push.apple.com", 2195, NULL, 0) < 0)
		{
			log_error("tool_sock_item_open");
			ret = -1;
			break;
		}
		*/
		/*
		tssl->ssl = SSL_new(tssl->ctx);
		tssl->bio = BIO_new_socket(sock_item.fd, BIO_NOCLOSE);
		SSL_set_bio(tssl->ssl, tssl->bio, tssl->bio);
		log_debug("SSL_connect");
		if (SSL_connect(tssl->ssl) <= 0)
		{
			log_error("SSL_CTX_load_verify_locations(%s)", ERR_reason_error_string(ERR_get_error()));
			ret = -1;
			break;
		}
		log_debug("SSL_connect success");
		*/
		/*
		if (!SSL_CTX_load_verify_locations(tssl->ctx, file, NULL))
		{
		    log_error("SSL_CTX_load_verify_locations(%s)", ERR_reason_error_string(ERR_get_error()));
			return -1;
		}
		*/
		
		tssl->bio = BIO_new_ssl_connect(tssl->ctx);
		BIO_get_ssl(tssl->bio, &tssl->ssl);
//		SSL_set_mode(tssl->ssl, SSL_MODE_AUTO_RETRY);
		BIO_set_nbio(tssl->bio, 1);
		BIO_set_conn_hostname(tssl->bio, addr);

		TOOL_INT32 tick = 0;
		tool_sock_setTick(&tick);
		while (1)
		{
			if (tool_sock_isTickTimeout(tick, TOOL_SSL_TIMEOUT))
			{
				log_error("time out(%d,%d)", tick, TOOL_SSL_TIMEOUT);
				ret = -1;
				break;
			}
			
			if (BIO_do_connect(tssl->bio) <= 0)
			{
				tool_sysf_usleep(TOOL_SSL_USLEEP);
				continue;
			}
			else
				break;
		}
		if (ret < 0)
			break;

		TOOL_INT32 result = SSL_get_verify_result(tssl->ssl);
		if(result != X509_V_OK)
		{
			log_error("SSL_get_verify_result(%d)(%d)(%s)", result, ERR_get_error(), ERR_reason_error_string(ERR_get_error()));
			ret = -1;
			break;
		}
		/*
	    TOOL_INT8 peer_CN[256] = {0};
	    X509* peer = SSL_get_peer_certificate(tssl->ssl);
	    X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peer_CN, 255);
	    log_state("peer_CN(%s)", peer_CN);
*/
	}while (0);
	if (ret < 0)
	{
		if (tssl->bio)
		{
			BIO_reset(tssl->bio);
			BIO_free_all(tssl->bio);
		}
		if (tssl->ctx)
		{
			SSL_CTX_free(tssl->ctx);
		}
	}
	return ret;	
}

TOOL_INT32 tool_ssl_pend(tool_ssl_t* tssl)
{
	return SSL_pending(tssl->ssl);
}

TOOL_INT32 tool_ssl_read(tool_ssl_t* tssl, TOOL_VOID* buf, TOOL_INT32 len)
{
	TOOL_INT32 ret = 0;
	TOOL_UINT8* pIndex = (TOOL_UINT8*)buf;
	TOOL_UINT8* pStart = (TOOL_UINT8*)buf;
	TOOL_INT32 tick = 0;
	tool_sock_setTick(&tick);
	while (len > 0)
	{
		if (tool_sock_isTickTimeout(tick, TOOL_SSL_TIMEOUT))
		{
			log_error("time out(%d,%d)", tick, TOOL_SSL_TIMEOUT);
			return -1;
		}

		ret = BIO_read(tssl->bio, pIndex, len);
		if (ret == 0)
		{
			log_error("reset");
			return -1;
		}
		else if (ret < 0)
		{
			if (!BIO_should_retry(tssl->bio))
			{
				tool_sysf_usleep(TOOL_SSL_USLEEP); 
				continue;
			}
		
			log_error("BIO_read(%s)", ERR_reason_error_string(ERR_get_error()));
            return -1;
		}

		len -= ret;
		pIndex += ret;		
	}
	return (TOOL_INT32)(pIndex - pStart);
}

TOOL_INT32 tool_ssl_write(tool_ssl_t* tssl, TOOL_VOID* buf, TOOL_INT32 len)
{
	TOOL_INT32 ret = 0;
	TOOL_UINT8* pIndex = (TOOL_UINT8*)buf;
	TOOL_UINT8* pStart = (TOOL_UINT8*)buf;
	TOOL_INT32 tick = 0;
	tool_sock_setTick(&tick);
	while (len > 0)
	{
		if (tool_sock_isTickTimeout(tick, TOOL_SSL_TIMEOUT))
		{
			log_error("time out(%d,%d)", tick, TOOL_SSL_TIMEOUT);
			return -1;
		}
		ret = BIO_write(tssl->bio, pIndex, len);
		if (ret <= 0)
		{
			if (!BIO_should_retry(tssl->bio))
			{
				tool_sysf_usleep(TOOL_SSL_USLEEP); 
				continue;
			}
			log_error("BIO_write(%s)", ERR_reason_error_string(ERR_get_error()));
            return -1;
		}
		len -= ret;
		pIndex += ret;		
	}
	return (TOOL_INT32)(pIndex - pStart);
}

TOOL_VOID tool_ssl_close(tool_ssl_t* tssl)
{
	if (tssl->bio)
	{
		BIO_reset(tssl->bio);
		BIO_free_all(tssl->bio);
		tssl->bio = NULL;
	}
	if (tssl->ctx)
	{
		SSL_CTX_free(tssl->ctx);
		tssl->ctx = NULL;
	}	

}

TOOL_VOID tool_ssl_done()
{

}

#endif

#if defined __cplusplus
}
#endif


