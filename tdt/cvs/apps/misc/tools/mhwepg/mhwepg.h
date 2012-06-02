#ifndef EPG_H
#define EPH_H

#include "si_tables.h"

/* Structures for EPG tables parsing */
typedef struct {
   u_char	network_id_hi;
   u_char	network_id_lo;
   u_char	transponder_id_hi;
   u_char	transponder_id_lo;
   u_char	channel_id_hi;
   u_char	channel_id_lo;
   u_char	name[16];
} channel_name_t;


#define EPG_TITLE_LEN 46
typedef struct epg_title {
   u_char table_id                               :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char section_syntax_indicator               :1;
   u_char dummy                                  :1;
   u_char                                        :2;
   u_char section_length_hi                      :4;
#else
   u_char section_length_hi                      :4;
   u_char                                        :2;
   u_char dummy                                  :1;
   u_char section_syntax_indicator               :1;
#endif
   u_char section_length_lo                      :8;
   u_char channel_id                             :8;
   u_char theme_id                               :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char day                                    :3;
   u_char hours		                         :5;
#else
   u_char hours		                         :5;
   u_char day                                    :3;
#endif
#if BYTE_ORDER == BIG_ENDIAN
   u_char minutes                                :6;
   u_char		                         :1;
   u_char summary_available                      :1;
#else
   u_char summary_available                      :1;
   u_char		                         :1;
   u_char minutes                                :6;
#endif
   u_char                                        :8;
   u_char                                        :8;
   u_char duration_hi                            :8;
   u_char duration_lo                            :8;
   u_char title                                [23];
   u_char ppv_id_hi                              :8;
   u_char ppv_id_mh                              :8;
   u_char ppv_id_ml                              :8;
   u_char ppv_id_lo                              :8;
   u_char program_id_hi                          :8;
   u_char program_id_mh                          :8;
   u_char program_id_ml                          :8;
   u_char program_id_lo                          :8;
   u_char                                        :8;
   u_char                                        :8;
   u_char                                        :8;
   u_char                                        :8;
} epg_title_t;

#define EPG_SUMMARY_LEN 11
typedef struct epg_summary {
   u_char table_id                               :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char section_syntax_indicator               :1;
   u_char dummy                                  :1;
   u_char                                        :2;
   u_char section_length_hi                      :4;
#else
   u_char section_length_hi                      :4;
   u_char                                        :2;
   u_char dummy                                  :1;
   u_char section_syntax_indicator               :1;
#endif
   u_char section_length_lo                      :8;
   u_char program_id_hi                          :8;
   u_char program_id_mh                          :8;
   u_char program_id_ml                          :8;
   u_char program_id_lo                          :8;
   u_char                                        :8;
   u_char                                        :8;
   u_char                                        :8;
   u_char nb_replays                             :8;
} epg_summary_t;

#define EPG_REPLAY_LEN 7
typedef struct epg_replay {
   u_char channel_id                             :8;
   u_char replay_mjd_hi                          :8;
   u_char replay_mjd_lo                          :8;
   u_char replay_time_h                          :8;
   u_char replay_time_m                          :8;
   u_char replay_time_s                          :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char last                                   :1;
   u_char                                        :1;
   u_char vo                                     :1;
   u_char vm                                     :1;
   u_char                                        :3;
   u_char subtitles                              :1;
#else
   u_char subtitles                              :1;
   u_char                                        :3;
   u_char vm                                     :1;
   u_char vo                                     :1;
   u_char                                        :1;
   u_char last                                   :1;
#endif
} epg_replay_t;


struct date {
   int	day;
   int	month;
   int	year;
};

struct program {
      u_char channel_id;
      u_char theme_id;
      time_t time;
      u_char summary_available;
      u_short duration;
#ifdef NEUTRINO
      char title[0x40];
#else
      char title[24];
#endif
      u_int ppv;
      u_int id;
};

struct summary {
      u_int id;
      char * text;
      u_char nb_replays;
      time_t time [10];
      u_char channel_id [10];
      u_char subtitles [10];
      u_char vm [10];
      u_char vo [10];
      u_char last_replay [10];
};

/* Functions prototypes */

void SetFiltSection (int fd, unsigned short pid, unsigned char tid);

#endif

