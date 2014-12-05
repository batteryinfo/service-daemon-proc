#include <Ecore.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <dlog.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <systemd/sd-daemon.h>
#include <sysman.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include "ss_log.h"

#define LOG_TAG		"SAMPLE" //DLOG Name
#define DLOG_ERR		DLOG_ERROR
#define PRT_ERR(fmt, arg...) \
	do { SLOG(LOG_ERR, LOG_TAG, fmt, ##arg); } while (0)

#define SAMPLE_SOCKET_PATH "/tmp/sample"
#define MAX_APP_LIST 100

static Ecore_Fd_Handler *custom_efd = NULL;
static int __custom_start(void);
//void quick_sort(struct app_data *data, int start, int end);


//IPC Structure
struct message_data{
	char name[50];
	int pid;
	int state;
};

struct app_data{
	char name[50];
	int pid;
	int state;
	unsigned int user_cpu_time;
	unsigned int system_cpu_time;
	unsigned int total_time;
};

struct app_list{
	struct app_data data[MAX_APP_LIST];
	int num_of_list;
};

struct statStuff { 
    int pid;			// %d 
    char comm[256];		// %s
    char state;			// %c
    int ppid;			// %d
    int pgrp;			// %d
    int session;		// %d
    int tty_nr;			// %d
    int tpgid;			// %d
    unsigned long flags;	// %lu
    unsigned long minflt;	// %lu
    unsigned long cminflt;	// %lu
    unsigned long majflt;	// %lu
    unsigned long cmajflt;	// %lu
    unsigned long utime;	// %lu
    unsigned long stime; 	// %lu
    long cutime;		// %ld
    long cstime;		// %ld
    long priority;		// %ld
    long nice;			// %ld
    long num_threads;		// %ld
    long itrealvalue;		// %ld
    unsigned long starttime;	// %lu
    unsigned long vsize;	// %lu
    long rss;			// %ld
    unsigned long rlim;		// %lu
    unsigned long startcode;	// %lu
    unsigned long endcode;	// %lu
    unsigned long startstack;	// %lu
    unsigned long kstkesp;	// %lu
    unsigned long kstkeip;	// %lu
    unsigned long signal;	// %lu
    unsigned long blocked;	// %lu
    unsigned long sigignore;	// %lu
    unsigned long sigcatch;	// %lu
    unsigned long wchan;	// %lu
    unsigned long nswap;	// %lu
    unsigned long cnswap;	// %lu
    int exit_signal;		// %d
    int processor;		// %d
    unsigned long rt_priority;	// %lu 
    unsigned long policy;	// %lu 
    unsigned long long delayacct_blkio_ticks;	// %llu 
} ; 


#define CREATE 1
#define RESET 5
#define RESUME 3
#define PAUSE 3
#define TERMINATE 2
static const char *_ae_name[8] = {
	"UNKNOWN","CREATE", "TERMINATE","PAUSE","RESUME","RESET","LOWMEM_POST","MEM_FLUSH"
};


struct app_list * al;

//static int cmp_pid(const void* p1, const void* p2){
//	if( (struct app


static int read_message(int fd, struct message_data *msg)
{
	//int i;

	read(fd, msg, sizeof(struct message_data));
	
	return 0;
}

unsigned int get_total_cpu_time(void){
	FILE *fp;
	char cpu_num[5];
	unsigned int usr;
	unsigned int usr_nice;
	unsigned int system;
	unsigned int idle;

	fp = fopen("/proc/stat", "r");
	if (fp == NULL)
	{
        	PRT_ERR("Fail to read stat file \n");
        	return 0;
    	}

	fscanf(fp, "%s %d %d %d %d", cpu_num, &usr, &usr_nice, &system, &idle);
	fclose(fp);


	return usr + usr_nice + system + idle;
}



unsigned int get_app_cpu_time(int pid){
	FILE *fp;
	char buf[256]; 
	sprintf(buf,"/proc/%d/stat",pid); 
	const char *format = "%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %lu %lu %llu"; 
	unsigned int time;
	struct statStuff *s = malloc(sizeof(struct statStuff));
		
	fp = fopen(buf, "r");
	if (fp == NULL)
	{
        	PRT_ERR("Fail to read pid:%d stat file \n", pid);
        	return 0;
    	}

	
        fscanf(fp, format, 
	    &s->pid,
	    s->comm,
	    &s->state,
	    &s->ppid,
	    &s->pgrp,
	    &s->session,
	    &s->tty_nr,
	    &s->tpgid,
	    &s->flags,
	    &s->minflt,
	    &s->cminflt,
	    &s->majflt,
	    &s->cmajflt,
	    &s->utime,
	    &s->stime,
	    &s->cutime,
	    &s->cstime,
	    &s->priority,
	    &s->nice,
	    &s->num_threads,
	    &s->itrealvalue,
	    &s->starttime,
	    &s->vsize,
	    &s->rss,
	    &s->rlim,
	    &s->startcode,
	    &s->endcode,
	    &s->startstack,
	    &s->kstkesp,
	    &s->kstkeip,
	    &s->signal,
	    &s->blocked,
	    &s->sigignore,
	    &s->sigcatch,
	    &s->wchan,
	    &s->nswap,
	    &s->cnswap,
	    &s->exit_signal,
	    &s->processor,
	    &s->rt_priority,
	    &s->policy,
	    &s->delayacct_blkio_ticks
	);

	PRT_ERR("Test output : path : %s, PID:%d, %s, user : %u, system : %u\n", buf, s->pid, s->comm, s->utime, s->stime);
	time = (s->utime) + (s->stime) ;
	close(fp);
	free(s);

	return time;

}

int update_app_list(struct message_data* msg){
	int i;
	bool found = false;
	
	if(msg->state == PAUSE || msg->state == RESET)
		return 0;
	//PAUSE, RESET skip
	//RESET come with RESUME for UI APP
	//If want check not ui app, need to check RESET state
	
	//First, update list
	for(i=0; i<al->num_of_list; i++){
		if(!strcmp(msg->name, al->data[i].name) ){
			//found
			al->data[i].pid = msg->pid;
			al->data[i].state = msg->state;
			
			found = true;
		}
		else{
			if(al->data[i].state != TERMINATE)
				al->data[i].state = PAUSE;
		}
	}

	if(!found){
		//add new app
		if(al->num_of_list >= MAX_APP_LIST){
			PRT_ERR("[gandan] Exceed App List Limit, Don't add app data\n");
		}
		else{
			al->data[al->num_of_list].pid = msg->pid;
			al->data[al->num_of_list].state = msg->state;
			strcpy(al->data[al->num_of_list].name, msg->name);
			al->data[al->num_of_list].user_cpu_time = 0;
			al->data[al->num_of_list].system_cpu_time = 0;
			al->data[al->num_of_list].total_time = 0;
			al->num_of_list++;
		}
	}
	

	//Second, Sort
	//quick_sort(al, 0, al->num_of_list);

	//Third, Update CPU Time


	//print app list
	PRT_ERR("\n\n---------------APP List-----------------\n");
	for(i=0; i<al->num_of_list; i++){
		PRT_ERR("[APP %d][Application:%s], state : %s, CPU TIME- : %u",al->data[i].pid, al->data[i].name, _ae_name[al->data[i].state], get_app_cpu_time(al->data[i].pid) );
	}
	PRT_ERR("Total Time : %u \n\n", get_total_cpu_time());

}

static int custom_cb(void *data, Ecore_Fd_Handler * fd_handler)
{
	int fd;
	struct message_data *msg; //-------

	//PRT_ERR("[gandan] %s: socket data received\n", __FUNCTION__);

	struct sockaddr_un client_address;
	int client_sockfd;
	int client_len;

	//-------------------------------------------------------------------//
	// Get data
	//
	if (!ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ)) {
		PRT_TRACE_ERR
		    ("ecore_main_fd_handler_active_get error , return\n");
		return 1;
	}
	fd = ecore_main_fd_handler_fd_get(fd_handler);
	msg = malloc(sizeof(struct message_data)); //-------
	if (msg == NULL) {
		PRT_TRACE_ERR("%s : Not enough memory", __FUNCTION__);
		return 1;
	}

	client_len = sizeof(client_address);
	client_sockfd = accept(fd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);

	if (client_sockfd == -1) {
		PRT_TRACE_ERR("socket accept error");
		free(msg);
		return -1;
	}

	read_message(client_sockfd, msg);
	PRT_ERR("[APP %d][Application:%s], state : %s",msg->pid, msg->name, _ae_name[msg->state]);
	//
	//-------------------------------------------------------------------//

	update_app_list(msg);


	free(msg);

	return 1;
}

static int custom_server_init(void)
{
	int fd;
	struct sockaddr_un serveraddr;

	al = malloc(sizeof(struct app_list));
	al->num_of_list = 0;
	//app-list

	PRT_ERR("[gandan] custom server init!!\n");
	PRT_ERR("[gandan] %s: custom_server_init\n", __FUNCTION__);

	if (access(SAMPLE_SOCKET_PATH, F_OK) == 0)
		unlink(SAMPLE_SOCKET_PATH);

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		PRT_ERR("%s: socket create failed\n", __FUNCTION__);
		return -1;
	}

	bzero(&serveraddr, sizeof(struct sockaddr_un));
	serveraddr.sun_family = AF_UNIX;
	strncpy(serveraddr.sun_path, SAMPLE_SOCKET_PATH, sizeof(serveraddr.sun_path));

	if (bind(fd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) < 0) {
		PRT_ERR("%s: socket bind failed\n", __FUNCTION__);
		close(fd);
		return -1;
	}

	if (chmod((SAMPLE_SOCKET_PATH), (S_IRWXU | S_IRWXG | S_IRWXO)) < 0)	
		/* 0777 */
		PRT_ERR("failed to change the socket permission");

	if (listen(fd, 5) < 0) {
		PRT_ERR("failed to listen");
		close(fd);
		return -1;
	}
	PRT_ERR("[gandan]  socket create & listen ok\n");

	return fd;
}


int sample_custom_init(void)
{
	return __custom_start();
}


static int __custom_start(void){
	int fd;

	fd = custom_server_init();
	if ( fd < 0 )
		return -1;

	//ECORE_FD_READ , ECORE_FD_WRITE
	custom_efd = ecore_main_fd_handler_add(fd, ECORE_FD_READ, custom_cb, NULL, NULL, NULL);

	if (!custom_efd ) {
		PRT_TRACE_ERR("error ecore_main_fd_handler_add");
		return -1;
	}
	return fd;

}


//-----------------------------------------------------------------------------------------------------
/*
void swap(struct app_data* a, struct app_data* b){
    struct app_data tmp;
	
	memcpy(&tmp, a, sizeof(struct app_data) );
	memcpy(a, b,  	sizeof(struct app_data) );
	memcpy(b, &tmp, 	sizeof(struct app_data) );
}
 
void quick_sort(struct app_data *data, int start, int end){
 
    if(start>=end) return;
 
    int mid=(start+end)/2;
    int pivot=data[mid].pid;
 
    swap(&data[start],&data[mid]);
 
    int p=start+1,q=end;
 
    while(1){
        while(data[p].pid<=pivot){ p++; }
        while(data[q].pid>pivot){ q--; }
 
        if(p>q) break;
 
        swap(&data[p],&data[q]);
    }
 
    swap(&data[start],&data[q]);
 
    quick_sort(data,start,q-1);
    quick_sort(data,q+1,end);
 
}
*/
