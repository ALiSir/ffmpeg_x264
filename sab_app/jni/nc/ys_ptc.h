
#if defined __cplusplus
extern "C"
{
#endif

#ifndef __PTC_YS_H__
#define __PTC_YS_H__

#define YS_MSG_SIZE				(10*1024)
#define YS_SEARCH_SIZE			(500*1024)
#define YS_MSG_FLAG				0xBCBC
#define YS_MSG_PORT				6060
#define YS_STREAM_PORT			7070
#define YS_TALK_FRAME_SIZE		320

/**
    �豸��������
**/
typedef enum 
{
    YS_DEV_TYPE_ANALOG_ENCODER = 0x10,  /**< ģ������豸 */
    YS_DEV_TYPE_DIGITAL_ENCODER,        /**< ���ֱ����豸 */
    YS_DEV_TYPE_SOFTWARE_DECODER,       /**< ��������� */
    YS_DEV_TYPE_HARDWARE_DECODER,       /**< Ӳ�������� */
    YS_DEV_TYPE_STORAGE_RECORD,         /**< �洢��¼�豸 */
    YS_DEV_TYPE_VIRTUAL_DEVICE = 0x20,  /**< �����豸 */
    YS_DEV_TYPE_BUTT
}YS_DEV_TYPE_E;

/**
    �豸������Դ����
**/
typedef enum tagResType
{
    YS_RES_TYPE_VIDEO_IN_CHL = 0x01,            /**< ��Ƶ�������ͨ�� */
    YS_RES_TYPE_VIDEO_DISPLAY_OUTPUT_PORT,      /**< ��Ƶ��ʾ����˿� */
    YS_RES_TYPE_INDEPENDENT_AUDIO_INPUT_CHL,    /**< ������Ƶ����ͨ�� */
    YS_RES_TYPE_INDEPENDENT_AUDIO_OUTPUT_CHL,   /**< ������Ƶ���ͨ�� */
    YS_RES_TYPE_ATTACHED_AUDIO_INPUT_CHL,       /**< ��·��Ƶ���ͨ�� */
    YS_RES_TYPE_ATTACHED_AUDIO_OUTPUT_CHL,      /**< ��·��Ƶ���ͨ�� */
    YS_RES_TYPE_ALARM_IN_CHL,                   /**< �澯����ͨ�� */
    YS_RES_TYPE_ALARM_OUT_CHL,                  /**< �澯���ͨ�� */
    YS_RES_TYPE_SERIAL_CHL,                     /**< ����ͨ�� */
    YS_RES_TYPE_DISK_SLOT,                      /**< ���̲�λ�� */
    YS_RES_TYPE_ALARM_BUZZER_CHL,               /**< ������ͨ�� */
    YS_RES_TYPE_NETWARK_CHL,                    /**< ����ͨ�� */
    YS_RES_TYPE_DECODE_CHL,                     /**< ����ͨ�� */
    YS_RES_TYPE_BUTT
}YS_RES_TYPE_E;


typedef enum 
{
    /* ��¼ҵ��0~99*/
    YS_MSG_ID_USER_LOGIN = 0,                     /**< �û���¼*/
    YS_MSG_ID_USER_LOGOUT,                        /**< �û�ע��*/
    YS_MSG_ID_USER_KEEPLIVE,                      /**< �û�����*/
    YS_MSG_ID_BIND_NOTIFY,                        /**< ����������Ϣ(�󶨷���ͨѶ�˿�) */
    YS_MSG_ID_UNBIND_NOTIFY,                      /**< ȡ������*/
    YS_MSG_ID_NOTIFY_DEV_STATUS,                  /**< �ϱ��豸״̬�������ߡ���Ƶ��ʧ��*/

    /* ʵ��������ҵ��100~199*/
    YS_MSG_ID_START_LIVE = 100,                   /**< ������ͨʵ�� */
    YS_MSG_ID_START_LIVE_MMI,                     /**< �����˻�ʵ�� */
    YS_MSG_ID_STOP_LIVE,                          /**< �ر�ʵ�� */
    YS_MSG_ID_START_TALK,                         /**< ������ͨ�����Խ� */
    YS_MSG_ID_START_TALK_MMI,                     /**< �����˻������Խ� */
    YS_MSG_ID_STOP_TALK,                          /**< ֹͣ�����Խ�*/
    YS_MSG_ID_NOTIFY_LIVE_EXCEPTION,              /**< ʵ�����쳣�ϱ���Ϣ */
    YS_MSG_ID_MAKE_KEY_FRAME,                     /**< ���ùؼ�֡ */

    /* ¼���ѯ���ط�ҵ��200~299*/
    YS_MSG_ID_VOD_QRY_MONTH = 200,                /**< �����ѯ¼��״̬ */
    YS_MSG_ID_VOD_QRY,                            /**< ¼���ѯ */
    YS_MSG_ID_VOD_OPEN,                           /**< �����ط�ҵ�� */
    YS_MSG_ID_VOD_OPEN_MMI,                       /**< �����˻��ط�ҵ�� */
    YS_MSG_ID_VOD_PLAY,                           /**< ����(����)�ط�ҵ��*/
    YS_MSG_ID_VOD_CLOSE,                          /**< ֹͣ�ط�ҵ�� */
    YS_MSG_ID_VOD_GET_PLAYSTATUS,                 /**< ��ȡ�ط���Ϣ */
    YS_MSG_ID_NOTIFY_VOD_EXCEPTION,               /**< �ط����쳣�ϱ���Ϣ */
    YS_MSG_ID_VOD_JOIN_GROUP,
    YS_MSG_ID_VOD_JOIN_GROUP_MMI,
    YS_MSG_ID_VOD_LEAVE_GROUP,
    YS_MSG_ID_VOD_QUERY_LOCK_FILE,
    YS_MSG_ID_VOD_SET_LOCK_FILE,
    YS_MSG_ID_VOD_QUERY_LOCK_FILE_INFO,
    YS_MSG_ID_VOD_QUERY_TIME_RANGE,

    /* ¼������300~399*/
    YS_MSG_ID_RECORD = 300,                       /**< ����/ֹͣ¼�� */
    YS_MSG_ID_QUERY_RECORD_STATUS,                /**< ��ѯ��ǰ¼��״̬ */

    /* ��̨���á����� 400~499*/
    YS_MSG_ID_SET_PRESET = 400,                   /**< Ԥ��λ���� */
    YS_MSG_ID_QUERY_PRESET,                       /**< Ԥ��λ��ѯ */
    YS_MSG_ID_QUERY_TRACK,                        /**< Ѳ���켣��ѯ */
    YS_MSG_ID_START_RECORD_TRACK,                 /**< Ѳ���켣¼�ƿ�ʼ */
    YS_MSG_ID_STOP_RECORD_TRACK,                  /**< Ѳ���켣¼�ƽ��� */
    YS_MSG_ID_QUERY_CRUISE,                       /**< Ѳ����·��ѯ */
    YS_MSG_ID_SET_CRUISE,                         /**< Ѳ����·���� */
    YS_MSG_ID_PTZ_COMMAND,                        /**< ��̨���� */

    /* ������ȡ500~599 */
    YS_MSG_ID_QUERY_VIDEO_CHL_LIST = 500,         /**< ��ѯ��Ƶͨ���б�(����Ϣ) */
    YS_MSG_ID_QUERY_VIDEO_CHL_DETAIL_LIST,        /**< ��ѯ��Ƶͨ���б�(��ϸ��Ϣ) */
    YS_MSG_ID_QUERY_ALARM_IN_LIST,                /**< ��ѯ���뿪�����б� */
    YS_MSG_ID_QUERY_ALARM_OUT_LIST,               /**< ��ѯ����������б� */
    YS_MSG_ID_QUERY_SERIAL_LIST,                  /**< ��ѯ�����б� */
    YS_MSG_ID_QUERY_AUDIO_LIST,                   /**< ��ѯ������Դ�б� */

    /* ��������600~699 */
    YS_MSG_ID_QUERY_DEV_INFO = 600,               /**< �豸��Ϣ��ѯ */
    YS_MSG_ID_SET_DEV_INFO,                       /**< �豸��Ϣ���� */
    YS_MSG_ID_QUERY_TIME,                         /**< �豸ʱ���ѯ */
    YS_MSG_ID_SET_TIME,                           /**< �豸ʱ������ */

    /* ͨ������700~899 */
    YS_MSG_ID_QUERY_CHANNEL_BASIC_INFO = 700,     /**< ͨ��������Ϣ��ѯ */
    YS_MSG_ID_SET_CHANNEL_BASIC_INFO,             /**< ͨ��������Ϣ���� */
    YS_MSG_ID_QUERY_VIDEO_PARAMETER,              /**< ͨ����ѧ������ѯ */
    YS_MSG_ID_SET_VIDEO_PARAMETER,                /**< ͨ����ѧ�������� */
    YS_MSG_ID_QUERY_CHANNEL_NAME,                 /**< ͨ������������ѯ */
    YS_MSG_ID_SET_CHANNEL_NAME,                   /**< ͨ�������������� */
    YS_MSG_ID_QUERY_STREAM_INFO,                  /**< ͨ����Ƶ������ѯ */
    YS_MSG_ID_SET_STREAM_INFO,                    /**< ͨ����Ƶ�������� */
    YS_MSG_ID_QUERY_PTZ,                          /**< ��̨���ò�ѯ */
    YS_MSG_ID_SET_PTZ,                            /**< ��̨���� */
    YS_MSG_ID_QUERY_RECORD_PLAN,                  /**< ¼��ƻ���ѯ */
    YS_MSG_ID_SET_RECORD_PLAN,                    /**< ¼��ƻ����� */
    YS_MSG_ID_QUERY_MOTION_DETECTION,             /**< �˶�����ѯ */
    YS_MSG_ID_SET_MOTION_DETECTION,               /**< �˶�������� */
    YS_MSG_ID_QUERY_VIDEO_LOSS,                   /**< ��Ƶ��ʧ��ѯ */
    YS_MSG_ID_SET_VIDEO_LOSS,                     /**< ��Ƶ��ʧ���� */
    YS_MSG_ID_QUERY_PRIVACY_MASK,                 /**< ��Ƶ�ڸǲ�ѯ */
    YS_MSG_ID_SET_PRIVACY_MASK,                   /**< ��Ƶ�ڸ����� */
    YS_MSG_ID_QUERY_TAMPER_PROOF,                 /**< ��Ƶ�ڵ���ѯ */
    YS_MSG_ID_SET_TAMPER_PROOF,                   /**< ��Ƶ�ڵ����� */
    YS_MSG_ID_QUERY_TEXT_OVERLAY,                 /**< OSD ���Ӳ�ѯ */
    YS_MSG_ID_SET_TEXT_OVERLAY,                   /**< OSD �������� */
    YS_MSG_ID_QUERY_HOLIDAY,                      /**< ���ղ�ѯ */
    YS_MSG_ID_SET_HOLIDAY,                        /**< �������� */
    YS_MSG_ID_SET_AUDIO_CHL_FOR_MMI,              /**< �˻�������Ƶͨ�� */

    /* �û�Ȩ������ 900~999*/
    YS_MSG_ID_QUERY_USER_LIST = 900,              /**< ��ȡ�û��б� */
    YS_MSG_ID_QUERY_USER,                         /**< ��ѯ�����û�������Ϣ */
    YS_MSG_ID_ADD_USER,                           /**< �����û� */
    YS_MSG_ID_MODIFY_USER,                        /**< �޸��û� */
    YS_MSG_ID_DELETE_USER,                        /**< ɾ���û� */

    /* ���������� 1000~1099 */
    YS_MSG_ID_QUERY_ALARM_INPUT = 1000,           /**< ���뿪������ѯ */
    YS_MSG_ID_SET_ALARM_INPUT,                    /**< ���뿪�������� */
    YS_MSG_ID_QUERY_ALARM_OUTPUT,                 /**< �����������ѯ */
    YS_MSG_ID_SET_ALARM_OUTPUT,                   /**< ������������� */
    YS_MSG_ID_MANUALLY_ACTION_OUTPUT,             /**< ������������� */
    YS_MSG_ID_GET_ALARM_OUTPUT_STATUS,            /**< ��ȡ������״̬ */

    /* �������� 1100~1199 */
    YS_MSG_ID_QUERY_SERIAL = 1100,                /**< �������ò�ѯ */
    YS_MSG_ID_SET_SERIAL,                         /**< �������� */

    /* �������� 1200~1299 */
    YS_MSG_ID_QUERY_NETWORK = 1200,               /**< ����������ò�ѯ */
    YS_MSG_ID_SET_NETWORK,                        /**< ��������������� */
    YS_MSG_ID_QUERY_PPPOE,                        /**< PPPoE��Ϣ��ѯ */
    YS_MSG_ID_SET_PPPOE,                          /**< PPPoE��Ϣ���� */
    YS_MSG_ID_QUERY_DDNS,                         /**< DDNS��Ϣ��ѯ */
    YS_MSG_ID_SET_DDNS,                           /**< DDNS��Ϣ���� */
    YS_MSG_ID_QUERY_UPNP,                         /**< UPNP��Ϣ��ѯ */
    YS_MSG_ID_SET_UPNP,                           /**< UPNP��Ϣ���� */
    YS_MSG_ID_QUERY_SMTP,                         /**< SMTP��Ϣ��ѯ */
    YS_MSG_ID_SET_SMTP,                           /**< SMTP��Ϣ���� */
    YS_MSG_ID_QUERY_PORT,                         /**< �˿ڻ�ȡ */
    YS_MSG_ID_SET_PORT,                           /**< �˿����� */

    /* �쳣���� 1300~1399 */
    YS_MSG_ID_QUERY_EXCEPTION = 1300,             /**< �쳣��ѯ */
    YS_MSG_ID_SET_EXCEPTION,                      /**< �쳣���� */

    /* �������� 1400~1499 */
    YS_MSG_ID_QUERY_DISK_LIST = 1400,             /**< ������Ϣ�б� */
    YS_MSG_ID_SET_DISK_PROPERTY,                  /**< ������������ */
    YS_MSG_ID_INIT_DISK,                          /**< ���̳�ʼ�� */

    /* ά��ҵ�� 1500~1599 */
    YS_MSG_ID_DEV_RESTART = 1500,                 /**< �豸���� */
    YS_MSG_ID_DEV_SHUTDOWN,                       /**< �豸�ػ� */
    YS_MSG_ID_SIMPLE_RECOVER_DEFAULT_CONFIG,      /**< �򵥻ָ�Ĭ������ */
    YS_MSG_ID_RECOVER_DEFAULT_CONFIG,             /**< ��ȫ�ָ�Ĭ������ */
    YS_MSG_ID_CONFIG_IMPORT,                      /**< ���õ��� */
    YS_MSG_ID_CONFIG_EXPORT,                      /**< ���õ��� */
    YS_MSG_ID_SYSTEM_UPGRADE,                     /**< �豸���� */
    YS_MSG_ID_SYSTEM_DYNAMIC_INFO,                /**< ϵͳ��̬��Ϣ */
    YS_MSG_ID_SET_AUTO_MAINTAIN,                  /**< �����Զ�ά����Ϣ */ 
    YS_MSG_ID_QUERY_AUTO_MAINTAIN,                /**< ��ѯ�Զ�ά����Ϣ */

    /* �澯����־ 1600~1699*/
    YS_MSG_ID_ALARM_ACTION_MONITOR = 1600,        /**< �澯������������֪ͨ��Ϣ */
    YS_MSG_ID_QUERY_LOG,                          /**< ��ѯ��־ */

    /* IPC 1700~1899 */
    YS_MSG_ID_ADD_IPC = 1700,                     /**< ���IPC */
    YS_MSG_ID_MOD_IPC,                            /**< �޸�IPC */
    YS_MSG_ID_DEL_IPC,                            /**< ɾ��IPC */
    YS_MSG_ID_QUERY_SINGLE_IPC,                   /**< ��ѯ����IPC��Ϣ */
    YS_MSG_ID_QUERY_ALL_IPC,                      /**< ��ѯIPC�б���Ϣ */
    YS_MSG_ID_PROBE_BY_FAMILY,                    /**< �鲥̽�� */
    YS_MSG_ID_PROBE_BY_IP,                        /**< ������̽�� */
    YS_MSG_ID_GET_PROBED_IPC,                     /**< ��ȡ����������Ϣ */

    /* �ڲ�VMP��Ϣ���� 10000~*/
    YS_MSG_ID_BUTT = 0xFFFF                    /**< ��Чֵ */
}YS_MSG_ID_E;

/* IEID */
typedef enum 
{
    /* ����IE 0~499*/
    YS_IE_ID_USER_SIMPLE_INFO = 0,                   /**< �û��˺ţ��û��������룩 */
    YS_IE_ID_IP_ADDRESS       = 1,                   /**< IP��ַ�����ͣ���ַ���˿ڣ� */
    YS_IE_ID_USER_LOG_INFO    = 2,                   /**< �û���¼��Ϣ���û�������¼ID����¼IP�� */
    YS_IE_ID_TASK_NO          = 3,                   /**< ҵ��� */
    YS_IE_ID_TIME_RANGE       = 4,                   /**< ��ʼ����ʱ��� */
    YS_IE_ID_RESOURCE_CODE    = 5,                   /**< ��Դ�����Ϣ */
    YS_IE_ID_RESOURCE_INFO    = 6,                   /**< ��Դ���룫���� */
    YS_IE_ID_COPY_TO          = 7,                   /**< ���Ƶ� */
    YS_IE_ID_ENABLED          = 8,                   /**< ʹ�ܱ�־ */
    YS_IE_ID_SCHEDULE         = 9,                   /**< �ƻ� */
    YS_IE_ID_DECODING_CHANNEL_CODE = 10,             /**< ͨ����� */
    YS_IE_ID_DEV_ABILITY_INFO      = 11,             /**< �豸������ */

    /* ����IE 500~ */
    YS_IE_ID_PORT_MAPPING = 500,                     /**< �ͻ�����Ҫ�Ķ˿���Ϣ */
    YS_IE_ID_MEDIA_IP     = 501,                     /**< ����ҵ��Э�̽������շ���IP��Ϣ */
    YS_IE_ID_MEDIA_TRANSPORT = 502,                  /**< ����Э�� */
    YS_IE_ID_MEDIA_VIDEO_STREAM_INDEX = 503,         /**< ��Ƶ������ */
    YS_IE_ID_MEDIA_AUDIO_STREAM_INFO  = 504,         /**< ��Ƶ���� */
    YS_IE_ID_EXCEPTION_REASON = 505,                 /**< �쳣ԭ�� */
    YS_IE_ID_VOD_MONTH_STATUS = 506,                 /**< ��λ��ʾĳ���¼��״̬�����֧��64�� */
    YS_IE_ID_STORAGE_TYPE     = 507,                 /**< ¼������ */
    YS_IE_ID_RECORD_CTRL      = 508,                 /**< ¼�����ͣ����� */
    YS_IE_ID_STORAGE_STATUS   = 509,                 /**< ¼��״̬������ͨ�� */
    YS_IE_ID_VOD_FILE         = 510,                 /**< ¼�����Ϣ */
    YS_IE_ID_PLAY_STATUS      = 511,                 /**< �طſ�����Ϣ */
    YS_IE_ID_DEV_BASIC_INFO   = 512,                 /**< �豸������Ϣ */
    YS_IE_ID_DEV_FIRMWARE_INFO = 513,                /**< �豸�̼���Ϣ */
    YS_IE_ID_DEV_TIME_ZONE_INFO = 514,               /**< �豸ʱ����Ϣ */
    YS_IE_ID_MODIFY_TIME_TYPE = 515,                 /**< �޸�ʱ������ */
    YS_IE_ID_DEV_NTP     = 516,                      /**< NTP������Ϣ */
    YS_IE_ID_DEV_TIME    = 517,                      /**< �豸ʱ����Ϣ */
    YS_IE_ID_OSD_TIME    = 518,                      /**< ʱ��OSD */
    YS_IE_ID_OSD_NAME    = 519,                      /**< ����OSD */
    YS_IE_ID_OSD_PROPERTY = 520,                     /**< OSD���� */
    YS_IE_ID_VIDEO_DISPLAY_INFO = 521,               /**< ��Ƶ��ʾ������Ϣ */
    YS_IE_ID_AUDIO_STREAM_INFO  = 522,               /**< ��Ƶ���� */
    YS_IE_ID_VIDEO_STREAM_INFO  = 523,               /**< ��Ƶ������ */
    YS_IE_ID_RECORD_ADVANCED    = 524,               /**< ¼��߼����� */
    YS_IE_ID_AREA_SENSITIVITY   = 525,               /**< ���������� */
    YS_IE_ID_AREA_INFO          = 526,               /**< �������� */
    YS_IE_ID_LINKAGE_METHOD     = 527,               /**< �澯�������� */
    YS_IE_ID_OSD_TEXT_OVERLAY   = 528,               /**< �ַ�����OSD */
    YS_IE_ID_HOLIDAY_INFO       = 529,               /**< ������Ϣ */
    YS_IE_ID_USER_NAME          = 530,               /**< �û��� */
    YS_IE_ID_USER_LIST_INFO     = 531,               /**< �û�������Ϣ���б��ã� */
    YS_IE_ID_USER_BASE_INFO     = 532,               /**< �û���Ϣ */
    YS_IE_ID_USER_PERMISSION    = 533,               /**< �û�Ȩ����Ϣ */
    YS_IE_ID_ALARM_INPUT_INFO   = 534,               /**< ���뿪����������Ϣ */
    YS_IE_ID_ALARM_OUTPUT_INFO  = 535,               /**< ���������������Ϣ */
    YS_IE_ID_ALARM_OUTPUT_STATUS = 536,              /**< ���������״̬ */
    YS_IE_ID_IPV4_INFO    = 537,                     /**< IPv4��Ϣ */
    YS_IE_ID_IPV6_INFO    = 538,                     /**< IPv6��Ϣ */
    YS_IE_ID_NIC_INFO     = 539,                     /**< ����������Ϣ */
    YS_IE_ID_DNS_INFO     = 540,                     /**< DNS��Ϣ */
    /**add by lzj*/
    YS_IE_ID_DDNS_SERIAL_NUM        = 541,
    YS_IE_ID_DDNS_CONNECT_STATUS    = 542,
    YS_IE_ID_DDNS_CONFIGURABLE      = 543,
    YS_IE_ID_DDNS_SHOW_INFO         = 544,
    YS_IE_ID_DYNAMIC_PORT_MAPPING   = 545,           /**< ��̬�˿�ӳ�� */
    YS_IE_ID_PORT_STATUS            = 546,           /**< �˿�״̬ */
    YS_IE_ID_SMTP_SERVER_INFO       = 547,           /**< �ʼ���������Ϣ */
    YS_IE_ID_ATTACHED_IMAGE         = 548,           /**< ͼƬ������Ϣ */
    YS_IE_ID_SENDER_INFO            = 549,           /**< ��������Ϣ */
    YS_IE_ID_RECIEVER_INFO          = 550,           /**< �ռ�����Ϣ */
    YS_IE_ID_STATIC_PORT_MAPPING    = 551,           /**< ��̬�˿�ӳ�� */
    YS_IE_ID_FILENAME_PATH          = 552,           /**< �ļ�·�����ļ����� */
    YS_IE_ID_LOG_TYPE               = 553,           /**< ��־���� */
    YS_IE_ID_OPT_LOG_INFO           = 554,           /**< ��־��Ϣ */
    YS_IE_ID_PRESET_INDEX           = 555,           /**< Ԥ��λ���� */
    YS_IE_ID_PRESET_LIST            = 556,           /**< Ԥ��λ�б� */
    YS_IE_ID_TRACK_INDEX            = 557,           /**< �켣���� */
    YS_IE_ID_TRACK_LIST             = 558,           /**< �켣�б� */
    YS_IE_ID_CRUISE_INFO            = 559,           /**< Ѳ����·��Ϣ */
    YS_IE_ID_PTZ_COMMAND            = 560,           /**< ��̨���� */
    YS_IE_ID_PTZ_PARAM              = 561,           /**< ��̨���ò��� */
    YS_IE_ID_SERIAL_INFO            = 562,           /**< ������Ϣ */
    YS_IE_ID_CONNECT_STATUS         = 563,           /**< ����״̬ */
    YS_IE_ID_MAPPING_MODE           = 564,           /**< ӳ��ģʽ */
    YS_IE_ID_DISK_INFO              = 565,           /**< ������Ϣ */
    YS_IE_ID_DISK_PROPERTY          = 566,           /**< �������� */
    YS_IE_ID_EXCEPTION_TYPE         = 567,           /**< �쳣���� */
    YS_IE_ID_IPC_DEV_INFO           = 568,           /**< IPC�豸��Ϣ */
    YS_IE_ID_IPC_LIST_DEV_INFO      = 569,           /**< IPC�豸�б���Ϣ */
    YS_IE_ID_PROBE_REQUEST_BY_FAMILY = 570,          /**< �鲥̽�� */
    YS_IE_ID_PROBE_REQUEST_BY_IP     = 571,          /**< ����������̽���豸 */
    YS_IE_ID_DISCOVERY_DEV           = 572,          /**< ̽�⵽���豸����Ϣ */
    YS_IE_ID_SESSION_ID              = 573,          /**< �����ỰID         */
    YS_IE_ID_VIDEO_CHL_SIMPLE_INFO_LIST  = 574,      /**< ��Ƶͨ������Ϣ�б�ֻ���ذ󶨵�ͨ����*/
    YS_IE_ID_VIDEO_CHL_DETAIL_INFO_LIST  = 575,      /**< ��Ƶͨ����ϸ��Ϣ�б�ֻ���ذ󶨵�ͨ����*/
    YS_IE_ID_SERIAL_LIST                 = 576,      /**< �����б� */
    YS_IE_ID_IP_INFO                     = 577,      /**< IP��ַ��Ϣ */
    YS_IE_ID_ALARM_INPUT_INNER           = 578,      /**< ���뿪�����ڲ���Ϣ*/
    YS_IE_ID_ALARM_OUTPUT_INNER          = 579,      /**< ����������ڲ���Ϣ */
    YS_IE_ID_SYS_DYNAMIC_INFO            = 580,      /**< ϵͳ��̬��Ϣ */
    YS_IE_ID_USER_LOGIN_TYPE             = 581,      /**< �û���¼���� */
    YS_IE_ID_AUTO_MAINTAIN_INFO          = 582,      /**< �Զ�ά����Ϣ */
    YS_IE_ID_AUDIO_CHL_INFO_LIST         = 583,      /**< �����Խ���Դ�б���Ϣ */
    YS_IE_ID_AUDIO_CHL_BIND              = 584,
    YS_IE_ID_REC_FILE_NAME               = 585,
    YS_IE_ID_RECORD_FILE                 = 586,
    YS_IE_ID_REC_FILE_LOCK_STATUS        = 587,
    YS_IE_ID_LOCK_FILE_PAGE_INFO         = 588,
    YS_IE_ID_LOCK_TOTAL_FILE_COUNT       = 589,
    YS_IE_ID_LOCK_RECORD_FILE_LIST       = 590,
    /* �ڲ�IE 5000*/

	YS_IE_ID_DST						 = 625,

    YS_IE_ID_BUTT = 0xFFFF                     /**< ��Чֵ */
}YS_IE_ID_E;

typedef enum 
{
    YS_AF_INET 		= 2,      /* for IPv4, AF_INET = 2 */
    YS_AF_INET6 	= 10,     /* for IPv6, AF_INET6 = 10 */
}YS_IP_DOMAIN_TYPE_E;

/* �û���¼���� */
typedef enum 
{
    YS_USER_LOGIN_TYPE_WEB,                    /**< web���¼ */
    YS_USER_LOGIN_TYPE_MMI,                    /**< �˻����¼ */
    YS_USER_LOGIN_TYPE_MMI_SPEC,               /**< �˻������¼(Ŀǰֻ���ڴ�����¼�û���¼������־) */
    YS_USER_LOGIN_TYPE_THIRD,                  /**< ��������¼ */
    YS_USER_LOGIN_TYPE_MOBILE,                 /**< �ֻ��ͻ��˵�¼ */

    YS_USER_LOGIN_TYPE_MAX,                    /**< �û���¼�������ֵ */
    YS_USER_LOGIN_TYPE_INVALID = 0XFFFF        /**< �û���¼������Чֵ */
}YS_USER_LOGIN_TYPE_E;

/* ��̨���� */
typedef enum 
{
    YS_PTZ_CMD_IRISCLOSESTOP        =0x0101,       /**< ��Ȧ��ֹͣ */
    YS_PTZ_CMD_IRISCLOSE            =0x0102,       /**< ��Ȧ�� */
    YS_PTZ_CMD_IRISOPENSTOP         =0x0103,       /**< ��Ȧ��ֹͣ */
    YS_PTZ_CMD_IRISOPEN             =0x0104,       /**< ��Ȧ�� */

    YS_PTZ_CMD_FOCUSNEARSTOP        =0x0201,       /**< ���ۼ�ֹͣ */
    YS_PTZ_CMD_FOCUSNEAR            =0x0202,       /**< ���ۼ� */
    YS_PTZ_CMD_FOCUSFARSTOP         =0x0203,       /**< Զ�ۼ� ֹͣ*/
    YS_PTZ_CMD_FOCUSFAR             =0x0204,       /**< Զ�ۼ� */

    YS_PTZ_CMD_ZOOMTELESTOP         =0x0301,       /**< �Ŵ�ֹͣ */
    YS_PTZ_CMD_ZOOMTELE             =0x0302,       /**< �Ŵ� */
    YS_PTZ_CMD_ZOOMWIDESTOP         =0x0303,       /**< ��Сֹͣ */
    YS_PTZ_CMD_ZOOMWIDE             =0x0304,       /**< ��С */

    YS_PTZ_CMD_TILTUPSTOP           =0x0401,       /**< ����ֹͣ */
    YS_PTZ_CMD_TILTUP               =0x0402,       /**< ���� */
    YS_PTZ_CMD_TILTDOWNSTOP         =0x0403,       /**< ����ֹͣ */
    YS_PTZ_CMD_TILTDOWN             =0x0404,       /**< ���� */
    YS_PTZ_CMD_UPTELESTOP           =0x0411,       /**< ���ϷŴ�ֹͣ */
    YS_PTZ_CMD_UPTELE               =0x0412,       /**< ���ϷŴ� */
    YS_PTZ_CMD_DOWNTELESTOP         =0x0413,       /**< ���·Ŵ�ֹͣ */
    YS_PTZ_CMD_DOWNTELE             =0x0414,       /**< ���·Ŵ� */
    YS_PTZ_CMD_UPWIDESTOP           =0x0421,       /**< ������Сֹͣ */
    YS_PTZ_CMD_UPWIDE               =0x0422,       /**< ������С */
    YS_PTZ_CMD_DOWNWIDESTOP         =0x0423,       /**< ������Сֹͣ */
    YS_PTZ_CMD_DOWNWIDE             =0x0424,       /**< ������С */

    YS_PTZ_CMD_PANRIGHTSTOP         =0x0501,       /**< ����ֹͣ */
    YS_PTZ_CMD_PANRIGHT             =0x0502,       /**< ���� */
    YS_PTZ_CMD_PANLEFTSTOP          =0x0503,       /**< ����ֹͣ */
    YS_PTZ_CMD_PANLEFT              =0x0504,       /**< ���� */
    YS_PTZ_CMD_RIGHTTELESTOP        =0x0511,       /**< ���ҷŴ�ֹͣ */
    YS_PTZ_CMD_RIGHTTELE            =0x0512,       /**< ���ҷŴ� */
    YS_PTZ_CMD_LEFTTELESTOP         =0x0513,       /**< ����Ŵ�ֹͣ */
    YS_PTZ_CMD_LEFTTELE             =0x0514,       /**< ����Ŵ� */

    YS_PTZ_CMD_RIGHTWIDESTOP        =0x0521,       /**< ������Сֹͣ */
    YS_PTZ_CMD_RIGHTWIDE            =0x0522,       /**< ������С */
    YS_PTZ_CMD_LEFTWIDESTOP         =0x0523,       /**< ������Сֹͣ */
    YS_PTZ_CMD_LEFTWIDE             =0x0524,       /**< ������С */

    YS_PTZ_CMD_PRESAVE              =0x0601,       /**< Ԥ��λ���� */
    YS_PTZ_CMD_PRECALL              =0x0602,       /**< Ԥ��λ���� */
    YS_PTZ_CMD_PREDEL               =0x0603,       /**< Ԥ��λɾ�� */

    YS_PTZ_CMD_LEFTUPSTOP           =0x0701,       /**< ����ֹͣ */
    YS_PTZ_CMD_LEFTUP               =0x0702,       /**< ���� */
    YS_PTZ_CMD_LEFTDOWNSTOP         =0x0703,       /**< ����ֹͣ */
    YS_PTZ_CMD_LEFTDOWN             =0x0704,       /**< ���� */
    YS_PTZ_CMD_LEFTUPTELESTOP       =0x0711,       /**< ���ϷŴ�ֹͣ */
    YS_PTZ_CMD_LEFTUPTELE           =0x0712,       /**< ���ϷŴ� */
    YS_PTZ_CMD_LEFTDOWNTELESTOP     =0x0713,       /**< ���·Ŵ�ֹͣ */
    YS_PTZ_CMD_LEFTDOWNTELE         =0x0714,       /**< ���·Ŵ� */
    YS_PTZ_CMD_LEFTUPWIDESTOP       =0x0721,       /**< ������Сֹͣ */
    YS_PTZ_CMD_LEFTUPWIDE           =0x0722,       /**< ������С */
    YS_PTZ_CMD_LEFTDOWNWIDESTOP     =0x0723,       /**< ������Сֹͣ */
    YS_PTZ_CMD_LEFTDOWNWIDE         =0x0724,       /**< ������С */

    YS_PTZ_CMD_RIGHTUPSTOP          =0x0801,       /**< ����ֹͣ */
    YS_PTZ_CMD_RIGHTUP              =0x0802,       /**< ���� */
    YS_PTZ_CMD_RIGHTDOWNSTOP        =0x0803,       /**< ����ֹͣ */
    YS_PTZ_CMD_RIGHTDOWN            =0x0804,       /**< ���� */
    YS_PTZ_CMD_RIGHTUPTELESTOP      =0x0811,       /**< ���ϷŴ�ֹͣ */
    YS_PTZ_CMD_RIGHTUPTELE          =0x0812,       /**< ���ϷŴ� */
    YS_PTZ_CMD_RIGHTDOWNTELESTOP    =0x0813,       /**< ���·Ŵ�ֹͣ */
    YS_PTZ_CMD_RIGHTDOWNTELE        =0x0814,       /**< ���·Ŵ� */
    YS_PTZ_CMD_RIGHTUPWIDESTOP      =0x0821,       /**< ������Сֹͣ */
    YS_PTZ_CMD_RIGHTUPWIDE          =0x0822,       /**< ������С */
    YS_PTZ_CMD_RIGHTDOWNWIDESTOP    =0x0823,       /**< ������Сֹͣ */
    YS_PTZ_CMD_RIGHTDOWNWIDE        =0x0824,       /**< ������С */

    YS_PTZ_CMD_ALLSTOP              = 0x0901,      /**< ȫͣ������ */
    YS_PTZ_CMD_ABSPOSITION          = 0x0902,      /**< ������������ԽǶ� */
    YS_PTZ_CMD_GUARD                = 0x0903,      /**< �ؿ���λ�����������̨ģ������ */
    YS_PTZ_CMD_SPEEDSET             = 0x0904,      /**< �����ٶ� */

    YS_PTZ_CMD_BRUSHON              = 0x0A01,      /**< ��ˢ�� */
    YS_PTZ_CMD_BRUSHOFF             = 0x0A02,      /**< ��ˢ�� */

    YS_PTZ_CMD_LIGHTON              = 0x0B01,      /**< �ƿ� */
    YS_PTZ_CMD_LIGHTOFF             = 0x0B02,      /**< �ƹ� */

    YS_PTZ_CMD_HEATON               = 0x0C01,      /**< ���ȿ� */
    YS_PTZ_CMD_HEATOFF              = 0x0C02,      /**< ���ȹ� */

    YS_PTZ_CMD_INFRAREDON           = 0x0D01,      /**< ���⿪ */
    YS_PTZ_CMD_INFRAREDOFF          = 0x0D02,      /**< ����� */

    YS_PTZ_CMD_SCANCRUISE           = 0x0E01,      /**< ��̨����ɨè */
    YS_PTZ_CMD_SCANCRUISESTOP       = 0x0E02,      /**< ��̨����ɨè */

    YS_PTZ_CMD_TRACKCRUISE          = 0x0F01,      /**< ��̨�켣Ѳ�� */
    YS_PTZ_CMD_TRACKCRUISESTOP      = 0x0F02,      /**< ֹͣ��̨�켣Ѳ�� */
    YS_PTZ_CMD_TRACKCRUISEREC       = 0x0F03,      /**< ��ʼ¼�ƹ켣 */
    YS_PTZ_CMD_TRACKCRUISERECSTOP   = 0x0F04,      /**< ֹͣ¼�ƹ켣 */

    YS_PTZ_CMD_PRESETCRUISE         = 0x1001,      /**< ��̨��Ԥ��λѲ�� ���������ֲ�����̨ģ������ */
    YS_PTZ_CMD_PRESETCRUISESTOP     = 0x1002,      /**< ��̨��Ԥ��λѲ��ֹͣ���������ֲ�����̨ģ������ */
    YS_PTZ_CMD_CRUISEDEL            = 0X1003,      /**< ɾ����· */
    YS_PTZ_CMD_CRUISEADDPRESET      = 0x1004,      /**< ����·�����Ԥ��λ */
    YS_PTZ_CMD_CRUISEADDSTART       = 0x1005,      /**< ��ʼ����·�����Ԥ��λ */
    YS_PTZ_CMD_CRUISEADDSTOP        = 0x1006,      /**< ��������·�����Ԥ��λ */

    YS_PTZ_CMD_AREAZOOMIN           = 0x1101,      /**< ����Ŵ� */
    YS_PTZ_CMD_AREAZOOMOUT          = 0x1102,      /**< ������С */

    YS_PTZ_CMD_BUTT,
} YS_PTZ_CMD_E;

/* ¼��洢���� */
typedef enum 
{
    YS_RECORD_TYPE_NORMAL = 0,      /**< ����洢 */
    YS_RECORD_TYPE_MOVEDETECT,      /**< �˶����洢 */
    YS_RECORD_TYPE_ALARM,           /**< �澯�洢 */
    YS_RECORD_TYPE_MA,              /**< �˶����͸澯�洢 */
    YS_RECORD_TYPE_M_A,             /**< �˶�����澯�洢�洢 */
    YS_RECORD_TYPE_UW_MANUAL,       /**< �ֶ��洢 */

    YS_RECORD_TYPE_MAX,             /**< �洢�������ֵ */
    YS_RECORD_TYPE_INVALID = 0xFF   /**< �洢������Чֵ */
}YS_RECORD_TYPE_E;

/* ����״̬ö�ٶ��� */
typedef enum 
{
    /** ����״̬ */
    YS_PBS_STATUS_16_BACKWARD = 0,     /**< 16���ٺ��˲��� */
    YS_PBS_STATUS_8_BACKWARD = 1,      /**< 8���ٺ��˲��� */
    YS_PBS_STATUS_4_BACKWARD = 2,      /**< 4���ٺ��˲��� */
    YS_PBS_STATUS_2_BACKWARD = 3,      /**< 2���ٺ��˲��� */
    YS_PBS_STATUS_1_BACKWARD = 4,      /**< �����ٶȺ��˲��� */
    YS_PBS_STATUS_HALF_BACKWARD = 5,   /**< 1/2���ٺ��˲��� */
    YS_PBS_STATUS_QUARTER_BACKWARD = 6,/**< 1/4���ٺ��˲��� */
    YS_PBS_STATUS_QUARTER_FORWARD = 7, /**< 1/4���ٲ��� */
    YS_PBS_STATUS_HALF_FORWARD = 8,    /**< 1/2���ٲ��� */
    YS_PBS_STATUS_1_FORWARD = 9,       /**< �����ٶ�ǰ������ */
    YS_PBS_STATUS_2_FORWARD = 10,      /**< 2����ǰ������ */
    YS_PBS_STATUS_4_FORWARD = 11,      /**< 4����ǰ������ */
    YS_PBS_STATUS_8_FORWARD = 12,      /**< 8����ǰ������ */
    YS_PBS_STATUS_16_FORWARD = 13,     /**< 16����ǰ������ */

    /** ����״̬ */
    YS_PBS_STATUS_1_DOWN = 51,         /**< �����ٶ����� */
    YS_PBS_STATUS_2_DOWN = 52,         /**< 2�������� */
    YS_PBS_STATUS_4_DOWN = 53,         /**< 4�������� */
    YS_PBS_STATUS_8_DOWN = 54,         /**< 8�������� */

    /** ��չ����״̬ */
    YS_PBS_STATUS_PAUSE = 100,         /**< ��ͣ���� */
    YS_PBS_STATUS_RESUME = 101,        /**< �ָ����� */
    YS_PBS_STATUS_STEP_FORWARD = 102,  /**< ����ǰ������ */
    YS_PBS_STATUS_STEP_BACKWARD = 103, /**< �������˲��� */
    YS_PBS_STATUS_JUMP = 104,          /**< ��ת���� */

    YS_PBS_STATUS_INVALID
}YS_PBS_STATUS_E;

/* ������ö�� */
typedef enum 
{
    YS_LOG_MAINTYPE_ALL = 0,              /* ȫ�����͵���־*/

    YS_LOG_MAINTYPE_ALARM = 1,            /* �澯�� */
    YS_LOG_MAINTYPE_EXCEPTION,            /* �쳣�� */
    YS_LOG_MAINTYPE_OPERATION,            /* ������ */
    YS_LOG_MAINTYPE_MESSAGE,              /* ��Ϣ�� */

    YS_LOG_MAINTYPE_BUTT                  /* ��Чֵ */
}YS_LOG_MAINTYPE_E;

/* ��־������ID �б� */
typedef enum 
{
    YS_LOG_ALL_SUB_TYPES = 0x0101,                 /*  ������Ϣ����־*/

    /* ��Ϣ����־*/
    YS_LOG_MSG_HDD_INFO       = 300,               /* Ӳ����Ϣ */
    YS_LOG_MSG_SMART_INFO     = 301,               /* S.M.A.R.T��Ϣ */
    YS_LOG_MSG_REC_OVERDUE    = 302,               /* ����¼��ɾ�� */
    YS_LOG_MSG_PIC_REC_OVERDUE = 303,              /* ����ͼƬ�ļ�ɾ�� */

    /* �澯����־����������־ID */
    YS_LOG_ALARM_MOTION_DETECT = 350,              /* �ƶ����澯 */
    YS_LOG_ALARM_MOTION_DETECT_RESUME = 351,       /* �ƶ����澯�ָ� */
    YS_LOG_ALARM_VIDEO_LOST = 352,                 /* ��Ƶ��ʧ�澯 */
    YS_LOG_ALARM_VIDEO_LOST_RESUME = 353,          /* ��Ƶ��ʧ�澯�ָ� */
    YS_LOG_ALARM_VIDEO_TAMPER_DETECT = 354,        /* �ڵ����澯 */
    YS_LOG_ALARM_VIDEO_TAMPER_RESUME = 355,        /* �ڵ����澯�ָ� */
    YS_LOG_ALARM_INPUT_SW = 356,                   /* ���뿪�����澯 */
    YS_LOG_ALARM_INPUT_SW_RESUME = 357,            /* ���뿪�����澯�ָ� */
    YS_LOG_ALARM_IPC_ONLINE = 358,                 /* IPC �豸���� */
    YS_LOG_ALARM_IPC_OFFLINE = 359,                /* IPC �豸���� */

    /* �쳣����־����������־ID */
    YS_LOG_EXCEP_DISK_ONLINE = 400,                /* �������� */
    YS_LOG_EXCEP_DISK_OFFLINE = 401,               /* �������� */
    YS_LOG_EXCEP_DISK_ERR = 402,                   /* �����쳣 */
    YS_LOG_EXCEP_STOR_ERR = 403,                   /* �洢���� */
    YS_LOG_EXCEP_STOR_ERR_RECOVER = 404,           /* �洢����ָ� */
    YS_LOG_EXCEP_STOR_DISOBEY_PLAN = 405,          /* δ���ƻ��洢 */
    YS_LOG_EXCEP_STOR_DISOBEY_PLAN_RECOVER = 406,  /* δ���ƻ��洢�ָ� */
    YS_LOG_EXCEP_ILLEGAL_ACCESS = 407,             /* �Ƿ����� */
    YS_LOG_EXCEP_IP_CONFLICT = 408,                /* IP��ַ��ͻ */
    YS_LOG_EXCEP_NET_BROKEN = 409,                 /* ����Ͽ� */
    YS_LOG_EXCEP_PIC_REC_ERR = 410,                /* ץͼ����,��ȡͼƬ�ļ�ʧ�� */
    YS_LOG_EXCEP_VIDEO_EXCEPTION = 411,            /* ��Ƶ�����쳣(ֻ���ģ��ͨ��) */
    YS_LOG_EXCEP_VIDEO_MISMATCH = 412,             /* ��Ƶ��ʽ��ƥ��  */
    YS_LOG_EXCEP_RESO_MISMATCH = 413,              /* ����ֱ��ʺ�ǰ�˷ֱ��ʲ�ƥ�� */
    YS_LOG_EXCEP_TEMP_EXCE = 414,                  /* �¶��쳣 */

    /* ��������־����������־ID */
    /* ҵ���� */
    YS_LOG_OPSET_LOGIN = 450,                      /* �û���¼ */
    YS_LOG_OPSET_LOGOUT = 451,                     /* ע����½ */
    YS_LOG_OPSET_USER_ADD = 452,                   /* �û���� */
    YS_LOG_OPSET_USER_DEL = 453,                   /* �û�ɾ�� */
    YS_LOG_OPSET_USER_MODIFY = 454,                /* �û��޸� */

    YS_LOG_OPSET_START_REC = 455,                  /* ��ʼ¼�� */
    YS_LOG_OPSET_STOP_REC = 456,                   /* ֹͣ¼�� */
    YS_LOG_OPSETR_PLAY = 457,                      /* �ط� */
    YS_LOG_OPSET_DOWNLOAD = 458,                   /* ���� */
    YS_LOG_OPSET_PTZCTRL = 459,                    /* ��̨���� */
    YS_LOG_OPSET_PREVIEW = 460,                    /* ʵ��Ԥ�� */
    YS_LOG_OPSET_REC_TRACK_START = 461,            /* �켣¼�ƿ�ʼ */
    YS_LOG_OPSET_REC_TRACK_STOP = 462,             /* �켣¼��ֹͣ */
    YS_LOG_OPSET_START_TALKBACK = 463,             /* ��ʼ�����Խ� */
    YS_LOG_OPSET_STOP_TALKBACK = 464,              /* ֹͣ�����Խ� */

    YS_LOG_OPSET_IPC_ADD = 465,                    /* ���IPC */
    YS_LOG_OPSET_IPC_DEL = 466,                    /* ɾ��IPC */
    YS_LOG_OPSET_IPC_SET = 467,                    /* ����IPC */

    /* ������ */
    YS_LOG_OPSET_DEV_BAS_CFG = 500,                /* �豸������Ϣ���� */
    YS_LOG_OPSET_TIME_CFG = 501,                   /* �豸ʱ������ */
    YS_LOG_OPSET_SERIAL_CFG = 502,                 /* �豸�������� */

    YS_LOG_OPSET_CHL_BAS_CFG = 503,                /* ͨ���������� */
    YS_LOG_OPSET_CHL_NAME_CFG = 504,               /* ͨ���������� */
    YS_LOG_OPSET_CHL_ENC_VIDEO = 505,              /* ��Ƶ����������� */
    YS_LOG_OPSET_CHL_DIS_VIDEO = 506,              /* ͨ����Ƶ��ʾ�������� */
    YS_LOG_OPSET_PTZ_CFG = 507,                    /* ��̨���� */
    YS_LOG_OPSET_CRUISE_CFG = 508,                 /* Ѳ����·���� */
    YS_LOG_OPSET_PRESET_CFG = 509,                 /* Ԥ�õ����� */
    YS_LOG_OPSET_VIDPLAN_CFG = 510,                /* ¼��ƻ����� */
    YS_LOG_OPSET_MOTION_CFG = 511,                 /* �˶�������� */
    YS_LOG_OPSET_VIDLOSS_CFG = 512,                /* ��Ƶ��ʧ���� */
    YS_LOG_OPSET_COVER_CFG = 513,                  /* ��Ƶ�ڵ����� */
    YS_LOG_OPSET_MASK_CFG = 514,                   /* ��Ƶ�ڸ����� */
    YS_LOG_OPSET_SCREEN_OSD_CFG = 515,             /* OSD�������� */

    YS_LOG_OPSET_ALARMIN_CFG = 516,                /* ������������ */
    YS_LOG_OPSET_ALARMOUT_CFG = 517,               /* ����������� */
    YS_LOG_OPSET_ALARMOUT_OPEN_MAN = 518,          /* �ֶ������������,�˻� */
    YS_LOG_OPSET_ALARMOUT_CLOSE_MAN = 519,         /* �ֶ��رձ������,�˻� */

    YS_LOG_OPSET_ABNORMAL_CFG = 520,               /* �쳣���� */
    YS_LOG_OPSET_HDD_CFG = 521,                    /* Ӳ������ */

    YS_LOG_OPSET_NET_IP_CFG = 522 ,                /* TCP/IP���� */
    YS_LOG_OPSET_NET_PPPOE_CFG = 523,              /* PPPOE���� */
    YS_LOG_OPSET_NET_PORT_CFG = 524,               /* �˿����� */

    YS_LOG_OPSET_NET_DDNS_CFG = 525,               /**DDNS����*/
    /* ά���� */
    YS_LOG_OPSET_START_DVR = 600,                  /* ���� */
    YS_LOG_OPSET_STOP_DVR = 601,                   /* �ػ� */
    YS_LOG_OPSET_REBOOT_DVR = 602,                 /* �����豸 */
    YS_LOG_OPSET_UPGRADE = 603,                    /* �汾���� */
    YS_LOG_OPSET_LOGFILE_EXPORT = 604,             /* ������־�ļ� */
    YS_LOG_OPSET_CFGFILE_EXPORT = 605,             /* ���������ļ� */
    YS_LOG_OPSET_CFGFILE_IMPORT = 606,             /* ���������ļ� */
    YS_LOG_OPSET_CONF_SIMPLE_INIT = 607,           /* �򵥻ָ����� */
    YS_LOG_OPSET_CONF_ALL_INIT = 608,              /* �ָ��������� */
    YS_LOG_OPSET_CONF_AUTO_MAINTAIN = 609          /* �Զ�ά�� */

} YS_LOG_SUBTYPE_E;

/* GMTʱ��ö�ٶ��� */
typedef enum 
{
    YS_TIME_ZONE_1200M,    /** ��GMTʱ���ӳ�12Сʱ */
    YS_TIME_ZONE_1100M,    /** ��GMTʱ���ӳ�11Сʱ */
    YS_TIME_ZONE_1000M,    /** ��GMTʱ���ӳ�10Сʱ */
    YS_TIME_ZONE_0900M,    /** ��GMTʱ���ӳ�9Сʱ */
    YS_TIME_ZONE_0800M,    /** ��GMTʱ���ӳ�8Сʱ*/
    YS_TIME_ZONE_0700M,    /** ��GMTʱ���ӳ�7Сʱ */
    YS_TIME_ZONE_0600M,    /** ��GMTʱ���ӳ�6Сʱ */
    YS_TIME_ZONE_0500M,    /** ��GMTʱ���ӳ�5Сʱ */
    YS_TIME_ZONE_0430M,    /** ��GMTʱ���ӳ�4����Сʱ */
    YS_TIME_ZONE_0400M,    /** ��GMTʱ���ӳ�4Сʱ */
    YS_TIME_ZONE_0330M,    /** ��GMTʱ���ӳ�3����Сʱ */
    YS_TIME_ZONE_0300M,    /** ��GMTʱ���ӳ�3Сʱ */
    YS_TIME_ZONE_0200M,    /** ��GMTʱ���ӳ�2Сʱ */
    YS_TIME_ZONE_0100M,    /** ��GMTʱ���ӳ�1Сʱ */
    YS_TIME_ZONE_0000,     /** ��׼GMTʱ�� */
    YS_TIME_ZONE_0100P,    /** ��GMTʱ������1Сʱ */
    YS_TIME_ZONE_0200P,    /** ��GMTʱ������2Сʱ */
    YS_TIME_ZONE_0300P,    /** ��GMTʱ������3Сʱ */
    YS_TIME_ZONE_0330P,    /** ��GMTʱ������3����Сʱ */
    YS_TIME_ZONE_0400P,    /** ��GMTʱ������4Сʱ */
    YS_TIME_ZONE_0430P,    /** ��GMTʱ������4����Сʱ */
    YS_TIME_ZONE_0500P,    /** ��GMTʱ������5Сʱ */
    YS_TIME_ZONE_0530P,    /** ��GMTʱ������5����Сʱ */
    YS_TIME_ZONE_0545P,    /** ��GMTʱ������5Сʱ45���� */
    YS_TIME_ZONE_0600P,    /** ��GMTʱ������6Сʱ */
    YS_TIME_ZONE_0630P,    /** ��GMTʱ������6����Сʱ */
    YS_TIME_ZONE_0700P,    /** ��GMTʱ������7Сʱ */
    YS_TIME_ZONE_0800P,    /** ��GMTʱ������8Сʱ */
    YS_TIME_ZONE_0900P,    /** ��GMTʱ������9Сʱ */
    YS_TIME_ZONE_0930P,    /** ��GMTʱ������9����Сʱ */
    YS_TIME_ZONE_1000P,    /** ��GMTʱ������10Сʱ */
    YS_TIME_ZONE_1100P,    /** ��GMTʱ������11Сʱ */
    YS_TIME_ZONE_1200P,    /** ��GMTʱ������12Сʱ */
    YS_TIME_ZONE_1300P,    /** ��GMTʱ������13Сʱ */

    YS_TIME_ZONE_MAX,              /** GMTʱ�����ö��ֵ */
    YS_TIME_ZONE_INVALID = 0xFFFF  /** ��ЧGMTʱ��ö��ֵ */
}YS_TIME_ZONE_E;

/* �豸Уʱ����ö�� */
typedef enum 
{
    YS_TIME_MODIFY_HANDLE = 0,                 /**< �ֶ��޸�ʱ�� */
    YS_TIME_MODIFY_NTP,                        /**< NTP�޸�ʱ�� */
    YS_TIME_MODIFY_SERVER,                     /**< �������޸�ʱ�� */

    YS_TIME_MODIFY_MAX,                        /**< �޸�ʱ��ö�����ֵ */
    YS_TIME_MODIFY_INVALID = 0xFFFF            /**< �޸�ʱ��ö����Чֵ */
}YS_TIME_MODIFY_E;

 /* �������������ģʽ */
typedef enum 
{
    YS_AO_MODE_OPEN   = 1,    /**< ���� */
    YS_AO_MODE_CLOSE  = 2,    /**< ���� */
    YS_AO_MODE_BUTT
}YS_AO_MODE_E;

#define YS_ERR_BASE                 50000      /*ҵ�����ģ����������, ����ֵ��BPͳһ���� */

typedef enum tagUwareErrCode
{
    /* �ڲ�ҵ������� */
    YS_ERR_SYSCALL_FALIED             = (YS_ERR_BASE + 1),       /* ϵͳ�������ô��� */
    YS_ERR_CREATE_VMP_MSG_FAILED      = (YS_ERR_BASE + 2),       /* VMP��Ϣ����ʧ�� */
    YS_ERR_CHANNEL_OVER_SPEC          = (YS_ERR_BASE + 3),       /* ͨ��������� */
    YS_ERR_NOT_SUPPORT_DIGITAL_DEVICE = (YS_ERR_BASE + 4),       /* ��֧�������豸 */

    /* ������Ϣ���ݴ��� */
    YS_ERR_RESCODE_NO_EXIST           = (YS_ERR_BASE + 100),     /* ��Դ���벻���� */
    YS_ERR_DECODE_IE_FAILED           = (YS_ERR_BASE + 101),     /* ��ȡ��ϢIEʧ�� */
    YS_ERR_ENCODE_IE_FAILED           = (YS_ERR_BASE + 102),     /* �����ϢIEʧ�� */
    YS_ERR_MSG_DATA_INVALID           = (YS_ERR_BASE + 103),     /* ��Ϣ���ݴ��� */

    /* ���ô���,�������������Լ��ڲ���Ϣ�·� */
    YS_ERR_CALL_DRV_COMMON            = (YS_ERR_BASE + 200),     /* ��������ͨ��ʧ�� */
    YS_ERR_GET_CFG_FAILED             = (YS_ERR_BASE + 201),     /* ��ȡ������Ϣ���� */
    YS_ERR_SET_CFG_FAILED             = (YS_ERR_BASE + 202),     /* ����������Ϣ���� */
    YS_ERR_AREA_PARAM_ILLEGAL         = (YS_ERR_BASE + 203),     /* ��������Ƿ� */
    YS_ERR_SYNC_CFG_FAILED            = (YS_ERR_BASE + 204),     /* ����ͬ����������ϵͳ����ʱ��ͬ���������� */
    YS_ERR_SIMPLE_RECOVER_CFG_FAILED  = (YS_ERR_BASE + 205),     /* �򵥻ָ����ó��� */
    YS_ERR_RECOVER_CFG_FAILED         = (YS_ERR_BASE + 206),     /* �ָ����ó��� */
    YS_ERR_FILE_CHECK_FAILED          = (YS_ERR_BASE + 207),     /* �ļ�У����� */
    YS_ERR_EXPORT_CFG_FAILED          = (YS_ERR_BASE + 208),     /* �������ó��� */
    YS_ERR_IMPORT_CFG_FAILED          = (YS_ERR_BASE + 209),     /* �������ó��� */
    YS_ERR_SET_DRV_FAILED             = (YS_ERR_BASE + 210),     /* ����������Ϣ���� */
    YS_ERR_GET_DRV_FAILED             = (YS_ERR_BASE + 211),     /* ��������ȡ��Ϣ���� */
    YS_ERR_QRY_LOG_NUM_FAILED         = (YS_ERR_BASE + 212),     /* ��ѯ��־�������� */
    YS_ERR_READ_LOG_FAILED            = (YS_ERR_BASE + 213),     /* ��ȡ������־ʱ���� */
    YS_ERR_SEND_CFG_MSG_FAILED        = (YS_ERR_BASE + 214),     /* �·�������Ϣʧ�� */
    YS_ERR_CONF_FILE_NOEXIST          = (YS_ERR_BASE + 215),     /* �����ļ������� */
    YS_ERR_CONF_FILE_INVALID          = (YS_ERR_BASE + 216),     /* �����ļ����Ϸ� */
    YS_ERR_CONF_FORBID_WRITE          = (YS_ERR_BASE + 217),     /* ��ֹд������ */
    YS_ERR_CONF_NOEXIST               = (YS_ERR_BASE + 218),     /* ��������� */
    YS_ERR_COND_USER_NOEXIST          = (YS_ERR_BASE + 219),     /* ָ�����û������� */
    YS_ERR_COND_USER_EXIST            = (YS_ERR_BASE + 220),     /* ָ�����û��Ѵ��� */
    YS_ERR_UPDATE_VERSION_FAIL        = (YS_ERR_BASE + 221),     /* �豸�������汾�Ų�ƥ�� */
    YS_ERR_UPDATE_BOARDTYPE_NOT_MATCH = (YS_ERR_BASE + 222),     /* �豸�������豸���Ͳ�ƥ�� */
    YS_ERR_UPDATE_OTHER_REASON        = (YS_ERR_BASE + 223),     /* �豸���������������ڴ��������ԭ��ʧ�� */
    uw_ERR_USER_FULL                  = (YS_ERR_BASE + 224),     /* �û����� */
    YS_ERR_CONFIGING                  = (YS_ERR_BASE + 225),     /* �����·������У����Ժ� */
    /* Added by chenjian 00524, 2014-08-15 of ά����־����*/
    YS_ERR_MAINTAIN_EXPORT_RUNNING    = (YS_ERR_BASE + 226),     /* ά����Ϣ���ڵ��� */

    /* ʵ��ҵ������� */
    YS_ERR_LIVE_EXISTED               = (YS_ERR_BASE + 300),     /* ʵ��ҵ���Ѿ����� */
    YS_ERR_LIVE_INPUT_NOT_READY       = (YS_ERR_BASE + 301),     /* ý����δ׼������ */
    YS_ERR_LIVE_OUTPUT_BUSY           = (YS_ERR_BASE + 399),     /* ʵ��ҵ����ʾ��Դæ */

    /* �ط�ҵ������� */
    YS_ERR_VOD_ABEND                  = (YS_ERR_BASE + 400),
    YS_ERR_VOD_END                    = (YS_ERR_BASE + 401),
    YS_ERR_VOD_CALL_DRV               = (YS_ERR_BASE + 402),
    YS_ERR_VOD_CB_NOT_FIND            = (YS_ERR_BASE + 403),
    YS_ERR_VOD_OVER_ABILITY           = (YS_ERR_BASE + 404),

    /* ��̨������� */
    YS_ERR_SET_PRESET_FAILED          = (YS_ERR_BASE + 500),     /* Ԥ��λ����ʧ�� */
    YS_ERR_QUERY_PRESET_FAILED        = (YS_ERR_BASE + 501),     /* Ԥ��λ��ѯʧ�� */
    YS_ERR_QUERY_TRACK_FAILED         = (YS_ERR_BASE + 502),     /* �켣��ѯʧ�� */
    YS_ERR_START_RECORD_TRACK_FAILED  = (YS_ERR_BASE + 503),     /* ��ʼ¼�ƹ켣ʧ�� */
    YS_ERR_STOP_RECORD_TRACK_FAILED   = (YS_ERR_BASE + 504),     /* ����¼�ƹ켣ʧ�� */
    YS_ERR_QUERY_CRUISE_FAILED        = (YS_ERR_BASE + 505),     /* Ѳ����·��ѯʧ�� */
    YS_ERR_SET_CRUISE_FAILED          = (YS_ERR_BASE + 506),     /* Ѳ����·����ʧ�� */
    YS_ERR_PTZ_COMMAND_FAILED         = (YS_ERR_BASE + 507),     /* ��̨����ʧ�� */
    YS_ERR_PRESET_IN_CRUISE           = (YS_ERR_BASE + 508),     /* Ԥ��λ��Ѳ����·��ʹ��, �޷�ɾ�� */

    /* RMģ�� */
    YS_ERR_IPC_CHAN_EXISTED           = (YS_ERR_BASE + 600),     /* �󶨵�IPCͨ���Ѵ���*/
    YS_ERR_IPC_CB_NOT_EXISTED         = (YS_ERR_BASE + 601),     /* IPC���ƿ鲻���� */
    YS_ERR_IPC_NOT_ONLINE             = (YS_ERR_BASE + 602),
    /* Added by c00524 2014/7/23 for IVSD10113,���弴�ö��� */
    YS_ERR_IPADDR_USE_UP              = (YS_ERR_BASE + 603),     /* IP��ַ�ľ� */

    /* ����ģ����ش����� */
    YS_ERR_IPADDR_CONFLICT            = (YS_ERR_BASE + 700),     /* ip��ַ��ͻ */
    YS_ERR_UPNP_PORT_EMPLOY           = (YS_ERR_BASE + 701),     /* �˿���ռ�� */
    YS_ERR_DOWNLOAD                   = (YS_ERR_BASE + 702),     /* �����쳣 */
    /* Begin: Added by maqiang/01512, 2014-11-29, for Email Function IVSD14293 */
    YS_ERR_TEST_EMAIL                 = (YS_ERR_BASE + 703),     /* �����ʼ�ʧ�� */
    /* End: Added by maqiang/01512, 2014-11-29, for Email Function IVSD14293 */

    /* �û�ģ������� */
    YS_ERR_USER_NOT_ONLINE            = (YS_ERR_BASE + 800),     /* �û������� */
    YS_ERR_USER_WRONG_PASSWD          = (YS_ERR_BASE + 801),     /* �û�������� */
    YS_ERR_USER_NO_SUCH_USER          = (YS_ERR_BASE + 802),     /* û�и��û� */
    YS_ERR_USER_NO_AUTH               = (YS_ERR_BASE + 803),     /* �û���Ȩ�� */
    YS_ERR_USER_MAX_NUM               = (YS_ERR_BASE + 804),     /* �û�����-�����ٱ���� */
    YS_ERR_USER_EXIST                 = (YS_ERR_BASE + 805),     /* �û��Ѵ��� */
    YS_ERR_USER_LOGIN_MAX_NUM         = (YS_ERR_BASE + 806),     /* �û���¼���Ѵ����� */
    YS_ERR_TALK_EXISTED               = (YS_ERR_BASE + 900),    /* �����Խ��Ѵ��� */
    YS_ERR_TALK_NO_EXISTED            = (YS_ERR_BASE + 901),    /* ����ҵ�񲻴��� */
    YS_ERR_TALK_RESCODE_INVALID       = (YS_ERR_BASE + 902),    /* �����Խ���Դ������Ч */
    YS_ERR_TALK_FAILED                = (YS_ERR_BASE + 903),    /* �����Խ�ʧ�� */
    YS_ERR_AUDIO_RES_USED_BY_TALK,                       		/* ������Դ�ѱ��Խ�ʹ��*/

    /* OAGģ����ش����� */
    YS_ERR_OAG_STREAM_NOEXIST         = (YS_ERR_BASE + 1000),     /* �������� */

    YS_ERR_OAG_CFG_CONTENT_INVALID    = (YS_ERR_BASE + 1011),     /* xml��Ч���� */
    YS_ERR_OAG_CFG_ITEMNUM_ERR        = (YS_ERR_BASE + 1012),     /* ��ȡxml�ڵ�����쳣 */
    YS_ERR_OAG_CFG_ALARM_OVER_MAX     = (YS_ERR_BASE + 1013),     /* �澯�����Ѵ������ */

    YS_ERR_OAG_OPEN_DLL_FAILED        = (YS_ERR_BASE + 1020),     /* ����DLL�ļ�ʧ�� */
    YS_ERR_OAG_CLOSE_DLL_FAILED       = (YS_ERR_BASE + 1021),     /* ж��DLL�ļ�ʧ�� */
    YS_ERR_OAG_GET_FUNC_ADDR_FAILED   = (YS_ERR_BASE + 1022),     /* ��ȡ������ַʧ�� */
    YS_ERR_OAG_LOAD_SDK_FAILED        = (YS_ERR_BASE + 1023),     /* ����SDKʧ�� */
    YS_ERR_OAG_INIT_SDK_FAILED        = (YS_ERR_BASE + 1024),     /* ��ʼ��SDKʧ�� */

    YS_ERR_OAG_CREATE_OBJECT_FAILED   = (YS_ERR_BASE + 1030),     /* ��������ʧ�� */
    YS_ERR_OAG_CONN_DEV_TIMEOUT       = (YS_ERR_BASE + 1031),     /* �����豸��ʱ */
    YS_ERR_OAG_OPENDEVICE_FAILED      = (YS_ERR_BASE + 1032),     /* ���豸ʧ�� */
    YS_ERR_OAG_CLOSEDEVICE_FAILED     = (YS_ERR_BASE + 1033),     /* �ر��豸ʧ�� */
    YS_ERR_OAG_SETCONF_FAILED         = (YS_ERR_BASE + 1034),     /* ���ò���ʧ�� */
    YS_ERR_OAG_GETCONF_FAILED         = (YS_ERR_BASE + 1035),     /* ��ȡ���ò���ʧ�� */
    YS_ERR_OAG_UNKNOWN_ALRAM_TYPE     = (YS_ERR_BASE + 1036),     /* ����ʶ��澯���� */
    YS_ERR_OAG_CLOSECHL_FAILED        = (YS_ERR_BASE + 1037),     /* �ر�ͨ��ʧ�� */
    YS_ERR_OAG_MISDATA                = (YS_ERR_BASE + 1038),     /* �������ݴ��� */
    YS_ERR_OAG_CREATE_CHANOBJ_FAILED  = (YS_ERR_BASE + 1039),     /* ����ͨ�����ʹ�������ʧ�� */
    YS_ERR_OAG_GET_HANDLE_FAILED      = (YS_ERR_BASE + 1040),     /* ��ȡ���ʧ�� */
    YS_ERR_OAG_UNKNOWN_PTZ_CMD        = (YS_ERR_BASE + 1041),     /* ��ʶ���Ѳ������ */
    YS_ERR_OAG_OPEN_ALARM_FAILED      = (YS_ERR_BASE + 1042),     /* �򿪸澯ʧ�� */
	/* Added by c00524 2014/8/26 for �������豸���� */
    YS_ERR_OAG_SWITCH_DEV_UNCONN      = (YS_ERR_BASE + 1043),     /* �������豸δ���� */
	YS_ERR_OAG_SET_TIME_FAILED        = (YS_ERR_BASE + 1044),     /* ʱ��ͬ��ʧ�� */ /* Added by z00348 2014.10.9 for IVSD12560 */
    YS_ERR_OAG_GET_NOALARM            = (YS_ERR_BASE + 1045),     /* û�и澯��Ϣ */ /* Added by c00524 2014/12/11 for IVSD11305,�����·�ʱ������澯 */
    YS_ERR_MAX_BUTT

} UWARE_ERR_CODE_E;


#pragma pack(4)

typedef struct
{	
    unsigned short	u16MessageFlag;  /*�̶�ֵ0xBCBC VMP��Ϣ��ʶ����ʾ��һ��VMP��Ϣ*/
    unsigned char   u8Version:4;     /*Э��汾�ţ��̶���д5*/
    unsigned char   u8SubVersion:4;  /*Э���Ӱ汾�ţ��̶���д0*/
    unsigned char   u8Type;          /*��Ϣ���ͣ�0ʱ��ʾ������Ϣ��1ʱ��ʾ֪ͨ������Ϣ��2ʱ��ʾOK��Ӧ��3ʱ��ʾFAILED��Ӧ*/
    unsigned short  u16Command;      /*��ϢID ÿһ����Ϣ���Ͷ����Լ�����ϢID�����VMP�ĵ� һ���ֽڹ���???*/
    unsigned short  u16Status;       /*��Ӧԭ���� UWARE_ERR_CODE_E  */
    unsigned int  	u32SeqNum;       /*��4λ��ʾģ��������28λ��ʾ˳��ţ��������Ӧ��Ϣ���кű���һ��,ʹ��VMP_MSG_SEQ_NUM����*/
    unsigned int  	u32SrcModId;     /*Դģ��ID*/
    unsigned int  	u32DstModId;     /*Ŀ��ģ��ID*/
    unsigned int  	u32FromHandle;   /*������˽�У���Ӧ��Ϣ�д���*/
    unsigned int  	u32ToHandle;     /*������˽�У���Ӧ��Ϣ�д���*/
    unsigned int  	u32ContentLength;/*VMP��Ϣ�ܳ���*/
    unsigned char   u8EndianType;    /*��С������*/
    unsigned char   u8Res1;          /*�����ֶ�*/
    unsigned short  u16Res2;         /*�����ֶ�*/
}ys_msg_head_t;

typedef struct
{
	ys_msg_head_t	head;
	unsigned char	data[YS_MSG_SIZE];
}ys_msg_t;

typedef struct
{
	ys_msg_head_t	head;
	unsigned char	data[YS_SEARCH_SIZE];
}ys_search_t;

typedef struct 
{
    unsigned short	u16IEID;                /*IEID*/
    unsigned short	u16BodyLen;             /*IE����*/
}ys_ie_head_t;

typedef struct
{
    unsigned short  u16DomainType;       	/* ��֧������ֵ,  ��BP_IP_DOMAIN_TYPE_E  */
    unsigned short  be16Rsvd;           	/* Ԥ��,����4�ֽڶ��� */

    union
    {
        unsigned int be32Ipv4Addr;      	/* For IPv4,������Ƕ�������ʽ,Ϊ����Ĵ���� */
        unsigned short abe16Ipv6Addr[8];  	/* For IPv6 */
    };
}ys_ip_addr_t;

/* �豸������ */
typedef struct 
{
    unsigned char 	u8DevType;              /* �豸���� */
    unsigned char	u8AlarmInPortNum;       /* ����������� */
    unsigned char	u8AlarmOutPortNum;      /* ����������� */
    unsigned char 	u8DiskNum;              /* Ӳ�̸��� */
    unsigned char 	u8ChanNum;              /* ģ��ͨ������ */
    unsigned char 	u8IPChanNum;            /* �������ͨ������ */
    unsigned char 	u8ZeroChanNum;          /* ����ͨ������ */
    unsigned char 	u8AudioInwithRecordNum; /* ��¼������Ƶ���� */
    unsigned char 	u8AudioInNum;           /* ����������Ƶ���� */
    unsigned char 	u8AudioOutNum;          /* ���������Ƶ���� */
    unsigned char 	u8Com232Num;            /* ����232���� */
    unsigned char 	u8Com422Num;            /* ����422���� */
    unsigned char 	u8Com485Num;            /* ����485���� */
    char szReserved[31];       /* Ԥ�� */
}ys_dev_ability_t;

/** ҵ����Դ���� */
typedef struct 
{
    unsigned char	u8DevType;              /**< �豸���,�ο�YS_DEV_TYPE_E */
    unsigned char	u8DevIndex;             /**< �豸��ţ���ͬ�����豸�ֱ����� */
    unsigned char	u8MappingChlIndex;      /**< ӳ������Ƶͨ�����,���漰ͨ�����豸������00,��1��ʼ��� */
    unsigned char	u8Reserved;             /**< ���� */
    unsigned char	u8ResType;              /**< ��Դ���,�ο�YS_RES_TYPE_E */
    unsigned char	u8SlotID;               /**< �ڲ�ʹ�ã����� */
    unsigned char	u8PortID;               /**< �ڲ�ʹ�ã����� */
    unsigned char	u8ChlID;                /**< ��Դ���,��1��ʼ��� */
}ys_resource_code_t;

typedef struct
{
	unsigned char	ucTcpFlag;
	unsigned char	ucTcpchnl;
	unsigned short	usBodySize;
}ys_tcp_head_t;

typedef struct  
{ 
	unsigned short	usMsgId; 
	unsigned short	usLen; 
	unsigned short	usCmd; 
	unsigned char	aucSession[16]; 
	unsigned short	usReserved; 
}ys_play_session_t;

//////////////////////////////////////////////////////////////////////

#define YS_USER_NAME_LEN	20
#define YS_PASSWD_LEN		64

/* YS_IE_ID_USER_SIMPLE_INFO
 * �û��˺ţ��û��������룩 */
typedef struct 
{
    ys_ie_head_t	stIEHdr;

    char            szUserName[YS_USER_NAME_LEN];
    char            szUserPasswd[YS_PASSWD_LEN];	/*��Ҫʹ��MD5����*/
}ys_ie_user_simple_t;

/* YS_IE_ID_IP_ADDRESS
 * IP��ַ�����ͣ���ַ���˿ڣ� */
typedef struct
{
	ys_ie_head_t	stIEHdr;
	ys_ip_addr_t	stIPAddress;
	unsigned short	u16Port;
	char			szReserved[2];
}ys_ie_ip_address_t;

/* YS_IE_ID_USER_LOGIN_TYPE
 * �û���¼���� */
typedef struct 
{
    ys_ie_head_t	stIEHdr;

    unsigned char	u8LoginType;    /* ���UW_USER_LOGIN_TYPE_E */
    char			szReserved[3];
}ys_ie_user_log_type_t;

/* YS_IE_ID_USER_LOG_INFO
 * �û���¼��Ϣ���û�������¼ID����¼IP�� */
typedef struct
{
	ys_ie_head_t	stIEHdr;
	char			szUserName[YS_USER_NAME_LEN];
	unsigned int 	u32UserLoginHandle;
	ys_ip_addr_t	stUserLoginIP;
}ys_ie_user_log_info_t;

/* YS_IE_ID_PORT_MAPPING
 * �ͻ�����Ҫ�Ķ˿���Ϣ */
typedef struct 
{
    ys_ie_head_t	stIEHdr;

    unsigned short	u16SDKPort;
    unsigned short	u16LivePort;
    unsigned short	u16VODPort;
    char			szReserved[2];
}ys_ie_port_mapping_t;

/* YS_IE_ID_DEV_ABILITY_INFO
 * �豸��������Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    ys_dev_ability_t    stDevInfo;
}ys_ie_dev_ability_t;

/* YS_IE_ID_RESOURCE_CODE
 * ��Դ�����Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    ys_resource_code_t	stResourceCode;
}ys_ie_resource_code_t;

/* YS_IE_ID_MEDIA_IP
 * ����ҵ��Э�̽������շ���IP��Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned char       u8RecvSendFlag;
    unsigned char		u8Reserved1;
    unsigned short		u16Port;
    ys_ip_addr_t		stIPAddress;
}ys_ie_media_ip_t;

/* YS_IE_ID_MEDIA_TRANSPORT
 * ����Э�� */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32TransportProtocal;
}ys_ie_media_transport_t;

/* YS_IE_ID_MEDIA_VIDEO_STREAM_INDEX
 * ��Ƶ������ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32StreamIndex;
}ys_ie_video_index_t;

/* YS_IE_ID_TASK_NO
 * ҵ��� */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32Task_No;
}ys_ie_task_no_t;

/* YS_IE_ID_SESSION_ID
 * ���������е�SessionID */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    char				szSessionID[16];
}ys_ie_session_id_t;

/* YS_IE_ID_PTZ_COMMAND
 * ��̨���ò��� */
typedef struct 
{
	ys_ie_head_t		stIEHdr;
	unsigned short		u16Command;
	unsigned char		u8Param1;
	unsigned char		u8Param2;
	char				szReserved[8];
}ys_ie_ptz_command_t;

/* YS_IE_ID_TIME_RANGE
 * ��ʼ����ʱ��� */
typedef struct
{
	ys_ie_head_t		stIEHdr;
	unsigned int		u32Begin;
	unsigned int		u32End;
}ys_ie_time_range_t;

/* YS_IE_ID_VOD_FILE
 * ¼�����Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    char	            szRecordFileName[64];
    unsigned int		u32RecordType;			//YS_RECORD_TYPE_E	
    unsigned int		u32Begin;
    unsigned int		u32End;
}ys_ie_vod_file_t;

/* YS_IE_ID_STORAGE_TYPE
 * ¼������ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
	unsigned int		u32RecordType;			//YS_RECORD_TYPE_E	
}ys_ie_storage_type_t;

/* YS_IE_ID_PLAY_STATUS
 * �طſ�����Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32PlayTime;
    unsigned int		u32PlayStatus;		//YS_PBS_STATUS_E
}ys_ie_play_status_t;

/* YS_IE_ID_LOG_TYPE
 * ��־���� */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned short      u16MainType;		//YS_LOG_MAINTYPE_E
    unsigned short      u16SubType;			//YS_LOG_SUBTYPE_E
}ys_ie_log_type_t;

#define YS_NAME_MAX_LEN		64

/* YS_IE_ID_OPT_LOG_INFO
 * ��־��Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32Time;
    unsigned short		u16MainType;		//YS_LOG_MAINTYPE_E
    unsigned short		u16SubType;			//YS_LOG_SUBTYPE_E
    unsigned char		acChannCode[8];
    unsigned char		szUserName[YS_NAME_MAX_LEN];
    ys_ip_addr_t 		ip_addr;
}ys_ie_log_info_t;

/* YS_IE_ID_MEDIA_AUDIO_STREAM_INFO
 * ��Ƶ���� */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32SampleRate;
    unsigned int		u32AudioFormat;
    unsigned char		u8RecvSend;
    unsigned char		u8Reserved1;
    unsigned short		u16Reserved2;
}ys_ie_audio_info_t;

/* YS_IE_ID_DEV_TIME_ZONE_INFO
 * �豸ʱ����Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		bSummerTimeEnable;
    unsigned char		u8TimeZone;				//YS_TIME_ZONE_E
    unsigned char		u8Reserved1;
    unsigned short		u16Reserved2;
}ys_ie_time_zone_info_t;

/* YS_IE_ID_MODIFY_TIME_TYPE
 * Уʱ���� */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned char		u8ModifyTimeType;		//YS_TIME_MODIFY_E
    char				szReserved[3];
}ys_ie_modify_time_t;

/* YS_IE_ID_DEV_NTP
 * NTP������Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    ys_ip_addr_t		stNTPServerIP;
    unsigned short		u16NTPPort;
    unsigned short		u16UpdateMin;
}ys_ie_ntp_t;

/* YS_IE_ID_DEV_TIME
 * �豸ʱ����Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    unsigned int		u32Time;
}ys_ie_time_t;

/* YS_IE_ID_DST
 * �豸����ʱ��Ϣ */
typedef struct
{
	ys_ie_head_t		stIEHdr;
    unsigned int		bIsDSTEnable;
	unsigned char		u8StartMonth;
	unsigned char		u8StartWeekofMonth;
	unsigned char		u8StartWeekDay;
	unsigned char		u8StartHour;
	unsigned char		u8EndMonth;
	unsigned char		u8EndWeekofMonth;
	unsigned char		u8EndWeekDay;
	unsigned char		u8EndHour;
	unsigned char		u8Period;
	unsigned char		reserve[3];
}ys_ie_dst_t;

/* YS_IE_ID_ALARM_OUTPUT_INFO
 * ���������������Ϣ */
typedef struct 
{
    ys_ie_head_t		stIEHdr;
    char				szAlarmOutputName[64];
	unsigned char		u8DefaultStatus;		//YS_AO_MODE_E
	char				szReserved[7];
	unsigned int		u32DurationSec;
}ys_ie_alarmout_t;

//////////////////////////////////////////////////////////////////////

//login 
//YS_MSG_ID_USER_LOGIN
typedef struct
{
	ys_ie_user_simple_t		user_simple;
	ys_ie_ip_address_t		ip_address;
	ys_ie_user_log_type_t	user_log_type;
}ys_msg_login_c2d_t;

typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_port_mapping_t	port_mapping;
	ys_ie_dev_ability_t		dev_ability;
}ys_msg_login_d2c_t;

//logout 
//YS_MSG_ID_USER_LOGOUT
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_user_log_type_t	user_log_type;
}ys_msg_logout_c2d_t;

typedef struct
{
	
}ys_msg_logout_d2c_t;

//heartbeat 
//YS_MSG_ID_USER_KEEPLIVE
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
}ys_msg_heartbeat_c2d_t;

typedef struct
{
	
}ys_msg_heartbeat_d2c_t;

//start rts 
//YS_MSG_ID_START_LIVE
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_media_ip_t		media_ip;
	ys_ie_media_transport_t	media_transport;
	ys_ie_video_index_t		video_index;
}ys_msg_startrts_c2d_t;

typedef struct
{
	ys_ie_media_ip_t		media_ip;
	ys_ie_task_no_t			task_no;
	ys_ie_session_id_t		session_id;
}ys_msg_startrts_d2c_t;

//stop rts 
//YS_MSG_ID_STOP_LIVE
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_task_no_t			task_no;
}ys_msg_stoprts_c2d_t;

typedef struct
{
	
}ys_msg_stoprts_d2c_t;

//rts data
typedef struct  
{ 
	ys_tcp_head_t			stHeader; 
	ys_play_session_t		stPlaySession; 
}ys_msg_play_session_tcp_t;

//ptz
//YS_MSG_ID_PTZ_COMMAND
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_ptz_command_t		ptz_command;
}ys_msg_ptz_c2d_t;

typedef struct
{
	
}ys_msg_ptz_d2c_t;


//searchrecords 
//YS_MSG_ID_VOD_QRY 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_time_range_t		time_range;
//	ys_ie_storage_type_t	storage_type;
}ys_msg_searchrecords_c2d_t;

#define YS_VOID_FILE_NUM		128

typedef struct
{
	ys_ie_vod_file_t		stAr_vod_file[YS_VOID_FILE_NUM];
}ys_msg_searchrecords_d2c_t;

//startpbs 
//YS_MSG_ID_VOD_OPEN 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_resource_code_t	resource_code;
	ys_ie_media_ip_t		media_ip;
	ys_ie_vod_file_t		vod_file;
}ys_msg_startpbs_c2d_t;

typedef struct
{
	ys_ie_task_no_t			task_no;
	ys_ie_media_ip_t		media_ip;
	ys_ie_session_id_t		session_id;
}ys_msg_startpbs_d2c_t;

//setpbs 
//YS_MSG_ID_VOD_PLAY 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_task_no_t			task_no;
	ys_ie_play_status_t		play_status;
}ys_msg_setpbs_c2d_t;

typedef struct
{
	
}ys_msg_setpbs_d2c_t;

//getpbs 
//YS_MSG_ID_VOD_GET_PLAYSTATUS 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;
	ys_ie_task_no_t 		task_no;
}ys_msg_getpbs_c2d_t;

typedef struct
{
	ys_ie_task_no_t 		task_no;
	ys_ie_play_status_t		play_status;
}ys_msg_getpbs_d2c_t;

//stoppbs 
//YS_MSG_ID_VOD_CLOSE 
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
	ys_ie_task_no_t 		task_no;				/* ��Ҫֹͣ�������Խ���Դҵ��� */
}ys_msg_stoppbs_c2d_t;

typedef struct
{
	
}ys_msg_stoppbs_d2c_t;


//searchlogs
//YS_MSG_ID_QUERY_LOG
typedef struct 
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
	ys_ie_log_type_t		log_type;
    ys_ie_time_range_t		time_range;
}ys_msg_searchlogs_c2d_t;

#define YS_LOG_MAX_NUM			2048

typedef struct
{
	ys_ie_log_info_t		log_info[YS_LOG_MAX_NUM];/* ��־��Ϣ,��ӦIE *//* ��־��������෵��2000�� */
}ys_msg_searchlogs_d2c_t;

//starttalk
//YS_MSG_ID_START_TALK
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
	ys_ie_resource_code_t	resource_code_in;		/* ������Ƶ��Դ���룬���� */
	ys_ie_resource_code_t	resource_code_out;		/* �����Ƶ��Դ���룬���� */
	ys_ie_media_ip_t		media_ip_send_remote;	/* Զ�˷�����IP��ַ�Ͷ˿ڣ����� */
	ys_ie_media_ip_t		media_ip_recv_remote;	/* Զ��������IP��ַ�Ͷ˿ڣ����� */
	ys_ie_media_transport_t	media_transport;		/* ����Э�飬���� */
}ys_msg_starttalk_c2d_t;

typedef struct
{
	ys_ie_task_no_t			task_no;				/* ҵ��ţ����� */
	ys_ie_media_ip_t		media_ip_send_local;	/* �豸�˷�����IP��ַ�Ͷ˿ڣ����� */
	ys_ie_media_ip_t		media_ip_recv_local;	/* �豸��������IP��ַ�Ͷ˿ڣ����� */
	ys_ie_audio_info_t		audio_info_in_local;	/* �豸��������Ƶ���������� */
	ys_ie_audio_info_t		audio_info_out_local;	/* �豸�˷�����Ƶ���������� */
	ys_ie_session_id_t		session_id;				/* �������� */
}ys_msg_starttalk_d2c_t;

//stoptalk
//YS_MSG_ID_STOP_TALK
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
	ys_ie_task_no_t 		task_no;				/* ��Ҫֹͣ�������Խ���Դҵ��� */
}ys_msg_stoptalk_c2d_t;

typedef struct
{
	
}ys_msg_stoptalk_d2c_t;

//gettime
//YS_MSG_ID_QUERY_TIME
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
}ys_msg_gettime_c2d_t;

typedef struct
{
	ys_ie_time_zone_info_t	time_zone_info;			/** ʱ����ϢIE---YS_TIME_ZONE_E */
	ys_ie_modify_time_t		modify_time;			/** �޸�����IE(�ֶ���NTP��������ͬ��)---YS_TIME_MODIFY_E */
	ys_ie_ntp_t				ntp;					/** NTP������IE */
	ys_ie_time_t			time;					/** ʱ����ϢIE */
	ys_ie_dst_t				dst;					/** ����ʱ */
}ys_msg_gettime_d2c_t;

//settime
//YS_MSG_ID_SET_TIME
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
	ys_ie_time_zone_info_t	time_zone_info;			/** ʱ����ϢIE---YS_TIME_ZONE_E */
	ys_ie_modify_time_t		modify_time;			/** �޸�����IE(�ֶ���NTP��������ͬ��)---YS_TIME_MODIFY_E */
	ys_ie_ntp_t				ntp;					/** NTP������IE */
	ys_ie_time_t			time;					/** ʱ����ϢIE */
	ys_ie_dst_t				dst;					/** ����ʱ */
}ys_msg_settime_c2d_t;

typedef struct
{

}ys_msg_settime_d2c_t;



//getAO2 YS_MSG_ID_GET_ALARM_OUTPUT_STATUS
typedef struct
{
	ys_ie_resource_code_t	resource_code;			/** ��Դ���� Req*/
	ys_ie_alarmout_t		pstIEAlarmOutInfo;	
}ys_msg_getAO2_c2d_t;

//getAO YS_MSG_ID_QUERY_ALARM_OUTPUT
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
	ys_ie_resource_code_t	resource_code;			/** ��Դ���� Req*/
}ys_msg_getAO_c2d_t;

typedef struct
{
	ys_ie_alarmout_t		pstIEAlarmOutInfo;
}ys_msg_getAO_d2c_t;

//setAO YS_MSG_ID_SET_ALARM_OUTPUT
typedef struct
{
	ys_ie_user_log_info_t	user_log_info;			/* �û���¼��Ϣ������ */
	ys_ie_resource_code_t	resource_code;			/** ��Դ���� Req*/
	ys_ie_alarmout_t		pstIEAlarmOutInfo;		/** ���������Ϣ Req*/
}ys_msg_setAO_c2d_t;

typedef struct
{

}ys_msg_setAO_d2c_t;

#pragma pack()


#endif

#if defined __cplusplus
}
#endif

