#define SUCCESS_JASON_CODE "0000"
#define HEARTBEAT_CMD_MAX 8
#define HBCMD_SENDTIME_STR "sendTime"
static void epoll_ADD(int epfd,int fd){
	struct epoll_event ev;
	ev.data.fd=fd;
	ev.events=EPOLLIN|EPOLLET;
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1){
		perror("epoll_ADD");
		exit(-1);
	}
}
static void epoll_DEL(int epfd,int fd){
	struct epoll_event ev;
	ev.data.fd=fd;
	if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev)==-1)
	perror("epoll_DEL");

}
static void time_creator(int*fd ,long interval){
	struct itimerspec time_new;
	struct timespec now;
	long long nosec;
	int sec,us;
	if((*fd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK))<0){
		perror("timer create");
		exit(-1);
	}
	clock_gettime(CLOCK_MONOTONIC,&now);
	sec=interval/1000000;
	us=interval%1000000;
	nosec=((long long)now.tv_sec+sec)*1000*1000*1000+now.tv_nsec+us*1000;
	time_new->it_value.tv_sec=nosec/(1000*1000*1000);
	time_new->it_value.tv_nsec=nosec%(1000*1000*1000);
	time_new->it_interval.tv_sec=sec;
	time_new->it_interval.tv_nsec=us*1000;
	timerfd_settime(*fd,TFD_TIMER_ABSTIME,&time_new,0);
}
static int add_timer(int epfd,int timerfd){
	int ret;
	struct epoll_event ev;
	ev.data.fd=timerfd;
	ev.events=EPOLLIN|EPOLLET|EPOLLOUT;
	if((ret=epoll_ctl(epfd,EPOLL_CTL_ADD,timerfd,&ev))<0)
		return -1;
	return 0;
}
static int del_timer(int epfd,int timerfd){
	int ret;
	struct epoll_event ev;
	ev.data.fd=timerfd;
	if((ret=epoll_ctl(epfd,EPOLL_CTL_DEL,timerfd,&ev))!=0)
		return -1;
	return 0;
}

static response_jparse(char*json){

	cJSON *code, *msg, *cmd, *key, *val, *reqId;
	size_t keylen;
	int num, i;
	int retval = -1;
	cJSON *root = cJSON_Parse(json);

	printf("Json Response parse...\n");
	if (root == NULL) 
	{
		printf("%s:Invalid json text\n",__func__);
		return retval;
	}

	code = cJSON_GetObjectItem(root, "code");
	if (code == NULL || strcmp(code->valuestring, SUCCESS_JASON_CODE) != 0) 
	{
		printf("%s:Invalid code in heartbeat json\n",__func__);
		return retval;
	}

	msg = cJSON_GetObjectItem(root, "msg");
	if (msg == NULL) 
	{
		printf("%s:Null msg\n",__func__);
		return 0;
	}

	num = cJSON_GetArraySize(msg);
	if (num > HEARTBEAT_CMD_MAX) 
		num = HEARTBEAT_CMD_MAX;

	for (i = 0; i < num; i++) 
	{
		printf("%s:msg {%d}", __func__,i);
		cmd = cJSON_GetArrayItem(msg, i);
		if (cmd == NULL){
			printf("is empty array\n");
			return 0;
		}

		key = cJSON_GetObjectItem(cmd, "command");
		if (key == NULL)
		{
			printf("no command msg\n");
			return 0;
		}else{
			if (strncmp(key->valuestring,"open",strlen("open")) == 0){
				JX102R_Open_Channel(channel_number, true, gSendToIP, gSendToPort);
				printf("is remote open\n");
			}else if (strncmp(key->valuestring, "close", strlen("close")) == 0){
				JX102R_Close_Channel(channel_number,  gSendToIP, gSendToPort);
				printf("is remote close\n");
			}else{
				printf("error command %s\n",key->valuestring);
			}
		}

		reqId = cJSON_GetObjectItem(cmd, "reqId");
		if (reqId == NULL){
			printf("%s:reqId  not found \n",__func__);
			return 0;
		}

		if (key->valuestring != NULL){
			if(strncmp(key->valuestring, HBCMD_SENDTIME_STR, keylen) == 0){
				char *time_msg = serialize_json_sendtime(STORE_ID, mac_addr, reqId->valuestring);
				printf("time_msg: {%s}\n", time_msg);
				send_heartbeat_post_to_server(heartbeat_send_URL, time_msg, strlen(time_msg));
			}
		}else {
			printf("NULL HBCMD_SENDTIME_STR \n");
			return 0;
		}
	return retval;
}
static long get_systime(){
	struct timeval tv;
	long value=0;
	memset(&tv,0,sizeof(tv));
	gettimeofday(&tv,NULL);
	value=tv.tv_sec*1000+tv.tv_usec;
	return value;
}
char * serialize_json_QR(const char * token,const char*gateid){
	cJSON*root=NULL;
	char*val=NULL;
	long long time_stamp=0;
	static char tv_str[64];
	memset(tv_str,0,64);
	if((root=cJSON_CreateObject())==NULL){
		printf("%s:obj create\n",__func__);
		return NULL;
	}
	cJSON_AddStringToObject(root,"token",token);
	cJSON_AddStringToObject(root,"gateid",gateid);
	time_stamp=get_systime();
	snprintf(tv_str,63,"%llu",time_stamp);
	cJSON_AddStringToObject(root,"timestamp",tv_str);
	json=cJSON_Print(root);
	if(root)
		cJSON_Delete(root);
	return json;
}
void chk_and_open_door(std::string str_QR_URL, std::string str_QRmsg, int msgLen)
{
	clock_t start = 0;
	clock_t ends = 0;
	start = clock();

	const char *QR_URL = str_QR_URL.c_str();
	const char *QRmsg = str_QRmsg.c_str();

	if (NULL == QRmsg)
	{
		FileLog::GetFileLog().rotate_logger()->info("QRmsg should not be null, send qr_door msg fail !");
		return;
	}

	CURLcode res;
	bool ret = false;

	CURL *curl_qr;
	curl_qr = curl_easy_init();
	if (curl_qr) 
	{
	
		/* send all data to this function  */
		curl_easy_setopt(curl_qr, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	
		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl_qr, CURLOPT_WRITEDATA, (void *)&chunk_qr);
	
		/* some servers don't like requests that are made without a user-agent
		field, so we provide one */
		curl_easy_setopt(curl_qr, CURLOPT_USERAGENT, "libcurl-agent/1.0");	
	}
	else
	{
		FileLog::GetFileLog().rotate_logger()->info("curl_qr init fail! send qr_door msg fail !");
		return;
	}

	curl_easy_setopt(curl_qr, CURLOPT_POSTFIELDS, QRmsg);
	curl_easy_setopt(curl_qr, CURLOPT_URL, QR_URL);
	curl_easy_setopt(curl_qr, CURLOPT_PROXY, PROXY);
	/* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
	itself */
	curl_easy_setopt(curl_qr, CURLOPT_POSTFIELDSIZE, msgLen);
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "content-type: application/json");
	curl_easy_setopt(curl_qr, CURLOPT_HTTPHEADER, headers);

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl_qr);

	if (curl_qr)
	{
		curl_easy_cleanup(curl_qr);
		curl_qr = NULL;
	}
	if (res != CURLE_OK)
	{
		FileLog::GetFileLog().rotate_logger()->info("send qr_door msg fail !");
	}
	else
	{
		FileLog::GetFileLog().rotate_logger()->info("send qr_door msg success !");
		if (strncmp(chunk_qr.memory, "A1A2A3A41", strlen("A1A2A3A41")) == 0)
			ret = true;
		else
			ret = false;
	}

	ends = clock();
	if (ends - start > 1500)
	{
		FileLog::GetFileLog().rotate_logger()->info("send post QR_code to server take {} ms ", ends - start);
		FileLog::GetFileLog().rotate_logger()->info("send qr_door msg fail !");
		return;
		
	}

	if (ret)
	{
		FileLog::GetFileLog().rotate_logger()->info("send post QR_code to server take {} ms ", ends - start);
#ifdef DOOR_ALPHA 
		JX102R_Open_Channel(channel_number, false, gSendToIP, gSendToPort);
#else
		open_door();
#endif
		printf("%s:door opened !\n",__func__);
		PlaySound(_T("C:\\QR_CODE\\欢迎来到京东无人超市.wav"),NULL, SND_FILENAME | SND_SYNC);//单次播放
	}
	else
	{
		FileLog::GetFileLog().rotate_logger()->info("qr_code inconsistent!");
	}
}