#ifndef _DVB_SUB_H
#define _DVB_SUB_H
//int dvbsub_initialise();
int dvbsub_init(int source);
int dvbsub_stop();
int dvbsub_close();
int dvbsub_start(int pid);
int dvbsub_pause();
int dvbsub_getpid();
void dvbsub_setpid(int pid);
//int dvbsub_terminate();
#endif
