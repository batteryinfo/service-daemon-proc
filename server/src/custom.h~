//IPC Structure
#define SAMPLE_SOCKET_PATH "/tmp/sample"
#define MAX_APP_LIST 100
#define FILE_PATH "/home/developer/battery_info"

struct message_data{
	char name[50];
	int pid;
	int state;
};

struct app_data{
	char name[50];
	int pid;
	int state;
	unsigned int total_time;
	unsigned int prev_time;
	unsigned int lcd_time;
	
	struct timeval app_fore_start;
	struct timeval app_fore_end;	
};

struct app_list{
	struct app_data data[MAX_APP_LIST];
	int num_of_list;
	unsigned int cpu_total_time;
	unsigned int cpu_work_time;
	unsigned int cpu_idle_time;
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


int sample_custom_init(void);

