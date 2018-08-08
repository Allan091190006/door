#include<stdio.h>
#include<sys/epoll.h>
#include<stdio.h>
#include<errno.h>
#include"door.h"
#define EPOLL_MAXFD 5
#define CONFIG_FILE ALPHA_QR_CONFIG.INI
struct server_config{
	int CBR_NUMBER;
	int channel_number;
	int comm_number;
	int gSendToPort;
	char QR_IP_HEAD[256];
	char QR_SCAN[256];
	char PROXY[32];
	char gSendToIP[32];
	char gFromIP[32];
	char STORE_ID[20];
	char MAC_ADDR[20];
	char heartbeat_URL[256];
	char heartbeat_send_URL[256];
};
struct server_config local_config;
int static connect_to_server()
{
	chunk_heartbeat.memory = (char *)malloc(MAX_MSG_LEN);  /* will be grown as needed by realloc above */
	chunk_heartbeat.size = 0;    /* no data at this point */

	chunk_qr.memory = (char *)malloc(MAX_MSG_LEN);  /* will be grown as needed by realloc above */
	chunk_qr.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);

	return true;
}
void task_process(){
	clock_t start,end;
	int m_read,m_write;
	int epoll_fd=0;
	int nfds,idx;
	struct epoll_event events[EPOLL_MAXFD];
	if(-1==(epoll_fd=epoll_create(EPOLL_MAXFD))){
		perror("epoll_create");
		exit(-1);
	}
	epoll_ADD(epoll_fd,targetfd);
	strncpy(local_config.heartbeat_URL+strlen(local_config.heartbeat_URL),local_config.MAC_ADDR,strlen(local_config.MAC_ADDR));
	while(1){
		if((nfds=epoll_wait(epoll_fd,events,MAXFD,-1))<0){
			if(errno!=EINTR)
			perror("epoll_wait");
		}else if(nfds>0){
			for(idx=0;idx<nfds;idx++)
				if(events[idx].data.fd==targetfd){
					start=clock();
					usleep(70000);
					end=clock();
					if(end-start>250)
						m_read=m_write=0;
					else{
						int qr_scan_len=m_write-m_read;
						if(qr_scan_len>0){
						
						
						
						
						}
					
					
					}

				}
				if(events[idx].data.fd==targetfd){
					char*response=NULL;
					response=send_heartbeat_get_to_server();	
					if(response!=NULL)
						response_jparse(response);
		
		
		
		
		}
		
	
	}


}
static int init_server_configure(void){



}
int main(int argc, const char *argv[])
{
	init_server_configure();
	connect_to_server();
	dev_uart_open();
	task_process();
	return 0;
}
