/***********************************************
 * ԭʼ�ֶ����URL��ַ��
 *
 ***********************************************/
struct init_url_table
{
		char url[1024];
		struct init_url_table *next;
};
 
struct init_url_table *head, *tail;

#define HTTP_DEF_PORT     80  /* ���ӵ�ȱʡ�˿� */
#define HTTP_BUF_SIZE   1024  /* �������Ĵ�С   */
#define HTTP_HOST_LEN    256  /* ���������� */
