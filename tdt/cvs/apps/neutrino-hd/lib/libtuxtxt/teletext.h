#ifndef __teletext_h__
#define __teletext_h__

int tuxtxt_init();
void tuxtxt_close();
void tuxtxt_start(int tpid, int source = 0);  // Start caching
int  tuxtxt_stop(); // Stop caching
#ifdef EVOLUX
int tuxtx_main(int _rc, int pid, int page = 0, int source = 0, int repeat_blocker = 250, int repeat_genericblocker = 125);
#else
int tuxtx_main(int _rc, int pid, int page = 0, int source = 0);
#endif
void tuxtx_stop_subtitle();
int tuxtx_subtitle_running(int *pid, int *page, int *running);
void tuxtx_pause_subtitle(bool pause = 1);
void tuxtx_set_pid(int pid, int page, const char * cc);

#endif
