/*************************************************************************
 *
 * PURPOSE :  HTTP �ͻ��˳���, ��ȡ��ҳ.
 *  
 * AUTHOR  :  ������
 * 
 **************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include "net_client.h"
 
#pragma comment(lib, "ws2_32.lib")  /* WinSockʹ�õĿ⺯�� */
 
/* ���峣�� */
#define HTTP_DEF_PORT     80  /* ���ӵ�ȱʡ�˿� */
#define HTTP_BUF_SIZE   1024  /* �������Ĵ�С   */
#define HTTP_HOST_LEN    256  /* ���������� */
 
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
 * ��������: ���������в���, �ֱ�õ�������, �˿ںź��ļ���. �����и�ʽ:
 *           [http://www.baidu.com:8080/index.html]
 *
 * ����˵��: [IN]  buf, �ַ���ָ������;
 *           [OUT] host, ��������;
 *           [OUT] port, �˿�;
 *           [OUT] file_name, �ļ���;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/

 void http_parse_request_url( char *buf, char *host, 
                            unsigned short *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;
 
    /* ���������Ŀ�ʼλ�� */
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
    {   /* �õ��ļ��� */
		//file = strrchr(host_end, '/');
        //if (file && (file + 1) != buf_end)
        //    strcpy(file_name, file + 1);
		file = host_end;
		strcpy(file_name, file );
	
    }
    if (colon) /* �õ��˿ں� */
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
 
    /* �õ�������Ϣ */
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

	while((fgets(init_host,1024,file_init_url)) != NULL)//���ж�ȡ
	{
		init_host[strlen(init_host)-1]='\0';
		
		p1=(struct init_url_table *)malloc(sizeof(struct init_url_table));
		//memcpy(p1->url,init_host,strlen(init_host));
		strcpy(p1->url,init_host);
		p1->next = NULL;
		//p1->next = head;
		//��ͷ�ڵ��β�巨
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
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
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
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */
 
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
 
    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(*port);
    serv_addr.sin_addr.s_addr = addr;
 
    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
//        return -1; 
    }
 
    /* ���� HTTP ���� */
	
    //send_len = sprintf(data_buf, http_req_hdr_tmpl, file_name, host, *port);
	send_len = sprintf(data_buf, http_req_hdr_tmpl, file_name, host);
	printf("request_buf��\n%s\n",data_buf);
    result = send(http_sock, data_buf, send_len, 0);
	//result = write(http_sock, data_buf, send_len);
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        printf("[Web] fail to send, error = %d\n", WSAGetLastError());
        //return -1; 
    }
 
    file_web = fopen(file_name_for_save, "a+");
     
    do /* ������Ӧ�����浽�ļ��� */
    {
        result = recv(http_sock, data_buf, HTTP_BUF_SIZE, 0);
        if (result > 0)
        {
            fwrite(data_buf, 1, result, file_web);
 
            /* ����Ļ����� */
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