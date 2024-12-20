#ifndef _EXTERN_H_
#define _EXTERN_H_

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <netinet/in.h>

#include "logmessage.h"
#include "xmalloc.h"

/*
 * Define a macro to get the size of statically allocated arrays.
 */
#if !defined( ARRAY_SIZE )
#define ARRAY_SIZE(_x) (sizeof(_x) / sizeof(_x[0]))
#endif /*!ARRAY_SIZE*/

/* === Variable types === */
struct pingmode {
	struct sockaddr to;
	int sock_fp;
	unsigned char *packet;
};

struct filemode {
	int mtime;
	time_t stat_mtime;
	time_t stat_changed;
};

struct ifmode {
	unsigned long bytes;
};

struct tempmode {
	int	in_use;
	unsigned char have1, have2, have3;
};

union wdog_options {
	struct pingmode net;
	struct filemode file;
	struct ifmode iface;
	struct tempmode temp;
};

struct list {
	char *name;
	int version;
	time_t last_time;
	int repair_count;
	union wdog_options parameter;
	struct list *next;
};

/* === Constants === */

#define DATALEN         (64 - 8)
#define MAXIPLEN        60
#define MAXICMPLEN      76
#define MAXPACKET       (65536 - 60 - 8)	/* max packet size */

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define ENUM_NO   0
#define ENUM_YES  1
#define ENUM_AUTO 2

#define TS_SIZE	12

/* === External variables === */
/* From configfile.c */
extern int tint;
extern int logtick;
extern int ticker;
extern int schedprio;
extern int maxload1;
extern int maxload5;
extern int maxload15;
extern int minpages;
extern int minalloc;
extern int maxswap;
extern int maxtemp;
extern int pingcount;
extern int temp_poweroff;
extern int sigterm_delay;
extern int repair_max;

extern char *devname;
extern char *admin;

extern int	test_timeout;		/* test-binary time out value. */
extern int	repair_timeout;		/* repair-binary time out value. */
extern int	dev_timeout;		/* Watchdog hardware time-out. */
extern int	retry_timeout;		/* Retry on non-critical errors. */

extern char *logdir;
extern char *write_file;
extern char *heartbeat;
extern int hbstamps;

extern int refresh_use_settimeout;
extern int refresh_ignore_errors;
extern int realtime;

extern struct list *tr_bin_list;
extern struct list *file_list;
extern struct list *target_list;
extern struct list *pidfile_list;
extern struct list *iface_list;
extern struct list *temp_list;

extern struct list *memtimer;
extern struct list *alloctimer;
extern struct list *loadtimer;

extern char *repair_bin;

/* = Not (yet) from config file. = */

extern int softboot;
extern int verbose;
extern int log_killed_PIDs;

/* From daemon-pid.c */
extern pid_t daemon_pid;

/* === Function prototypes === */

#ifndef GCC_NORETURN
#ifdef __GNUC__
#define GCC_NORETURN __attribute__((noreturn))
#else
#define GCC_NORETURN
#endif				/*!__GNUC__ */
#endif				/*!GCC_NORETURN */

/** file_stat.c **/
int check_file_stat(struct list *);
int check_file_stat_safe(struct list *file);

/** file_table.c **/
int check_file_table(void);

/** keep_alive.c **/
int open_watchdog(char *name, int timeout);
int set_watchdog_timeout(int timeout);
int keep_alive(void);
int get_watchdog_fd(void);
int close_watchdog(void);
void safe_sleep(int sec);

/** load.c **/
int open_loadcheck(void);
int check_load(void);
int close_loadcheck(void);

/** net.c **/
int check_net(char *target, int sock_fp, struct sockaddr to, unsigned char *packet, int time, int count);
int open_netcheck(struct list *tlist);
int close_netcheck(struct list *tlist);

/** temp.c **/
int open_tempcheck(struct list *tlist);
int check_temp(struct list *act);
int close_tempcheck(void);

/** test_binary.c **/
int check_bin(char *, int, int);
void free_process(void);

/** pidfile.c **/
int check_pidfile(struct list *);

/** iface.c **/
int check_iface(struct list *);

/** memory.c **/
int open_memcheck(void);
int check_memory(void);
int close_memcheck(void);
int check_allocatable(void);

/** shutdown.c **/
void do_shutdown(int errorcode);
void sigterm_handler(int arg);
void terminate(int ecode) GCC_NORETURN;

/** heartbeat.c **/
int open_heartbeat(void);
int write_heartbeat(void);
int close_heartbeat(void);

/** lock_mem.c **/
void lock_our_memory(int do_lock, int priority, pid_t pid);
void unlock_our_memory(void);

/** daemon-pid.c **/
int write_pid_file(const char *fname);
int remove_pid_file(void);
int wd_daemon(int nochdir, int noclose);

/** configfile.c **/
void read_config(char *configfile);
void free_all_lists(void);

/** killall5.c **/
void killall5(int sig, int aflag);
int kill_process_tree(pid_t pid, int sig);

/** errorcodes.c **/
const char *wd_strerror(int err);

/** sigterm.c **/
extern volatile sig_atomic_t _running;
void sigterm_handler(int arg);

/** run-as-child.c **/
#define FLAG_CHILD_SYNC			0x01
int exec_as_func(int flags, void *ptr);
int run_func_as_child(int timeout, int (*funcptr)(int, void *), int code, void *ptr);

/** reopenstd.c **/
#define FLAG_REOPEN_STD_TEST	0x02
#define FLAG_REOPEN_STD_REPAIR	0x04
void set_reopen_dir(const char *dname);
int reopen_std_files(int flags);

/** send-email.c **/
int send_email(int errorcode, void *ptr);

#endif /*_EXTERN_H_*/
