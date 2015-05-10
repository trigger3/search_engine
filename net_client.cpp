/*************************************************************************
 *
 * PURPOSE :  HTTP 客户端程序, 获取网页.
 *  
 * AUTHOR  :  LIHUI
 * 
 **************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include "net_client.h"
 
#pragma comment(lib, "ws2_32.lib")  /* WinSock使用的库函数 */
 
/* 定义常量 */
#define HTTP_DEF_PORT     80  /* 连接的缺省端口 */
#define HTTP_BUF_SIZE   1024  /* 缓冲区的大小   */
#define HTTP_HOST_LEN    256  /* 主机名长度 */
 
char *http_req_hdr_tmpl = "GET %s HTTP/1.1\r\n"
	"Host: %s\r\n"
	"Connection: keep-alive\r\n"
	"Cache-Control: max-age=0\r\n"
	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36\r\n"
	"DNT: 1\r\n"
	"Accept-Charset: utf-8,Unicode\r\n"
	//"Accept-Encoding: gzip,deflate,sdch\r\n"
	//"Accept-Language: zh-CN,zh;q=0.8\r\n"
	"Accept-Language: zh-CN\r\n"
	"\r\n";


/**************************************************************************
 *
 * 函数功能: 解析命令行参数, 分别得到主机名, 端口号和文件名. 命令行格式:
 *           [http://www.baidu.com:8080/index.html]
 *
 * 参数说明: [IN]  buf, 字符串指针数组;
 *           [OUT] host, 保存主机;
 *           [OUT] port, 端口;
 *           [OUT] file_name, 文件名;
 *
 * 返 回 值: void.
 *
 **************************************************************************/

 void http_parse_request_url( char *buf, char *host, 
                            unsigned short *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;
 
    /* 查找主机的开始位置 */
	printf("hello http_parse_request_url\n");
     
    begin =strstr(buf, "//");
    begin = (begin ? begin + 2 : buf);
     
    colon = strchr(begin, ':');
    host_end = strchr(begin, '/');
 
    if (host_end == NULL)
    {
        host_end = buf_end;
    }
    else
    {   /* 得到文件名 */
		//file = strrchr(host_end, '/');
        //if (file && (file + 1) != buf_end)
        //    strcpy(file_name, file + 1);
		file = host_end;
		strcpy(file_name, file );
	
    }
    if (colon) /* 得到端口号 */
   {
        colon++;
 
        length = host_end - colon;
        memcpy(port_buf, colon, length);
        port_buf[length] = 0;
        *port = atoi(port_buf);
 
        host_end = colon - 1;
    }
	else
		*port = 80;
 
    /* 得到主机信息 */
    length = host_end - begin;
    memcpy(host, begin, length);
    host[length] = 0;
}

 
 struct init_url_table * get_init_url_table(void)
{
	printf("hello init_url_table\n");
	char init_host[1024];

	struct init_url_table *p1;
	head = NULL;
	tail = NULL;

	FILE *file_init_url;

	if((file_init_url = fopen("init_url_table.txt", "r+")) == NULL)
	{
		printf("Open init_url_table error!");
		exit(0);
	}

	while((fgets(init_host,1024,file_init_url)) != NULL)//按行读取
	{
		init_host[strlen(init_host)-1]='\0';
		
		p1=(struct init_url_table *)malloc(sizeof(struct init_url_table));
		//memcpy(p1->url,init_host,strlen(init_host));
		strcpy(p1->url,init_host);
		p1->next = NULL;
		//p1->next = head;
		//带头节点的尾插法
		if(head==NULL)
            head=p1;
        else
            tail->next=p1;
        tail=p1;
		printf("host:%s,length:%d\n",p1->url,strlen(p1->url));
	}
	fclose(file_init_url);
	
	return head;
}
 
void http_request (char *buff, char *host,
					unsigned short *port, char *file_name, char *file_name_for_save)
{
    WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
    struct hostent *host_ent;
     
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE];
    //char host[HTTP_HOST_LEN] = "127.0.0.1";
    //unsigned short port = HTTP_DEF_PORT;
    unsigned long addr;
    //char file_name[HTTP_HOST_LEN] = "index.html";
   // char file_nameforsave[HTTP_HOST_LEN] = "index1.txt";
    FILE *file_web;
 
    //http_parse_request_url(buff, host, port, file_name);
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */
 
    addr = inet_addr(host);
	//printf("addr:%f",addr);
    if (addr == INADDR_NONE)
    {
        host_ent = gethostbyname(host);
        if (!host_ent)
        {
            printf("[Web] invalid host\n");
//            return -1;
        }
         
        memcpy(&addr, host_ent->h_addr_list[0], host_ent->h_length);
    }
 
    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(*port);
    serv_addr.sin_addr.s_addr = addr;
 
    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
//        return -1; 
    }
 
    /* 发送 HTTP 请求 */
	
    //send_len = sprintf(data_buf, http_req_hdr_tmpl, file_name, host, *port);
	send_len = sprintf(data_buf, http_req_hdr_tmpl, file_name, host);
	printf("request_buf：\n%s\n",data_buf);
    result = send(http_sock, data_buf, send_len, 0);
	//result = write(http_sock, data_buf, send_len);
    if (result == SOCKET_ERROR) /* 发送失败 */
    {
        printf("[Web] fail to send, error = %d\n", WSAGetLastError());
        //return -1; 
    }
 
    file_web = fopen(file_name_for_save, "a+");
     
    do /* 接收响应并保存到文件中 */
    {
        result = recv(http_sock, data_buf, HTTP_BUF_SIZE, 0);
        if (result > 0)
        {
            fwrite(data_buf, 1, result, file_web);
 
            /* 在屏幕上输出 */
            data_buf[result] = 0;
            printf("%s", data_buf);
        }
    } while(result > 0);
 
    fclose(file_web);
    closesocket(http_sock);
    WSACleanup();
}

int main()
{
	int i=1;
	char host[HTTP_HOST_LEN] = "127.0.0.1";
    unsigned short port = HTTP_DEF_PORT;
	char file_name[HTTP_HOST_LEN] = "index.html";
	char file_name_for_save[HTTP_HOST_LEN];
	
	
	struct init_url_table *p1=(struct init_url_table *)malloc(sizeof(struct init_url_table));

	head=get_init_url_table();
	//p1 = head->next;
	p1 = head;
	
	for(p1; p1!=NULL; p1=p1->next)
	{	
		http_parse_request_url(p1->url,  host, &port, file_name);
		sprintf(file_name_for_save,"file/%d.txt",i);
		//sprintf(file_name_for_save,"%d.txt",i);
		http_request(p1->url,  host, &port, file_name,file_name_for_save);
		printf("web:%s,port:%d,file_name:%s,host:%s\n",p1 -> url, port, file_name, host);
		i++;
	}
	
	return 0;
}
