/***********************************************
 * 原始手动添加URL地址池
 *
 ***********************************************/
struct init_url_table
{
		char url[1024];
		struct init_url_table *next;
};
 
struct init_url_table *head, *tail;

#define HTTP_DEF_PORT     80  /* 连接的缺省端口 */
#define HTTP_BUF_SIZE   1024  /* 缓冲区的大小   */
#define HTTP_HOST_LEN    256  /* 主机名长度 */
