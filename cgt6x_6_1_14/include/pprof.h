#ifndef _PPROF_H
#define _PPROF_H

#define PPROF_HANDLER_NAME "PPHNDL" /* DO NOT CHANGE THIS */

#define PPROF_FILE_MAGIC        "TI_PDD"
#define PPROF_DSET_MAGIC        "TIPPRD"
#define PPROF_NUM_HDR_FIELDS    3
#define PPROF_VERSION           1

#define BIG_ENDIAN_CODE         2
#define LITTLE_ENDIAN_CODE      1

extern far int __TI_prof_data_start;
extern far int __TI_prof_data_size;
extern far int __TI_pprof_out_hndl;

/* Used by pprof user API */
extern far int __TI_enable_exit_profile_output;

_CODE_ACCESS extern void __TI_init_pphandler();

_CODE_ACCESS extern void _output_path_profile_info();
#endif
