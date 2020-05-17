/*
 * mount.h - libmount API
 *
 * Copyright (C) 2008-2009 Karel Zak <kzak@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifndef _LIBMOUNT_MOUNT_H
#define _LIBMOUNT_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USE_UNSTABLE_LIBMOUNT_API
# warning libmount API is not stable yet!
#endif

#include <stdio.h>


#define LIBMOUNT_VERSION   "2.18.0"

/**
 * mnt_cache:
 *
 * Stores canonicalized paths and evaluated tags
 */
typedef struct _mnt_cache mnt_cache;

/**
 * mnt_lock:
 *
 * Stores information about locked file (e.g. /etc/mtab)
 */
typedef struct _mnt_lock mnt_lock;

/**
 * mnt_iter:
 *
 * Generic iterator (stores state about lists)
 */
typedef struct _mnt_iter mnt_iter;

/**
 * mnt_optls:
 *
 * Mount options list (stores parsed mount options)
 */
typedef struct _mnt_optls mnt_optls;

/**
 * mnt_optent:
 *
 * Parsed mount option - "mnt_optls" entry
 */
typedef struct _mnt_optent mnt_optent;

/**
 * mnt_optmap:
 *
 * Mount options description (map)
 */
struct mnt_optmap
{
	const char	*name;	 /* option name[=%<type>] (e.g. "loop[=%s]") */
	int		id;	 /* option ID or MS_* flags (e.g MS_RDONLY) */
	int		mask;	 /* MNT_{MFLAG,MDATA,INVMASK,...} mask */
};

/*
 * mount options map masks
 */
#define MNT_MFLAG	(1 << 1) /* use the mask as mount(2) flag */
#define MNT_MDATA	(1 << 2) /* use the option as mount(2) data */
#define MNT_INVERT	(1 << 3) /* invert the mountflag */
#define MNT_NOMTAB	(1 << 4) /* skip in the mtab option string */

/**
 * mnt_fs:
 *
 * Parsed fstab/mtab/mountinfo entry
 */
typedef struct _mnt_fs mnt_fs;

/**
 * mnt_tab:
 *
 * List of mnt_fs entries (parsed fstab/mtab/mountinfo)
 */
typedef struct _mnt_tab mnt_tab;

/* init.c */
extern void mnt_init_debug(int mask);

/* version.c */
extern int mnt_parse_version_string(const char *ver_string);
extern int mnt_get_library_version(const char **ver_string);

/* utils.c */
extern int mnt_fstype_is_netfs(const char *type);
extern int mnt_fstype_is_pseudofs(const char *type);
extern int mnt_match_fstype(const char *type, const char *pattern);
extern int mnt_match_options(const char *optstr, const char *pattern);

/* cache.c */
extern mnt_cache *mnt_new_cache(void);
extern void mnt_free_cache(mnt_cache *cache);
extern const char *mnt_cache_find_path(mnt_cache *cache, const char *path);
extern const char *mnt_cache_find_tag(mnt_cache *cache,
                        const char *token, const char *value);
extern int mnt_cache_read_tags(mnt_cache *cache, const char *devname);
extern int mnt_cache_device_has_tag(mnt_cache *cache, const char *devname,
                                const char *token, const char *value);

extern char *mnt_cache_find_tag_value(mnt_cache *cache,
				const char *devname, const char *token);

extern char *mnt_resolve_path(const char *path, mnt_cache *cache);
extern char *mnt_resolve_tag(const char *token, const char *value, mnt_cache *cache);
extern char *mnt_resolve_spec(const char *spec, mnt_cache *cache);

/* optstr.c */
extern int mnt_optstr_next_option(char **optstr, char **name, size_t *namesz,
				char **value, size_t *valuesz);
extern int mnt_optstr_append_option(char **optstr, const char *name,
				const char *value);
extern int mnt_optstr_get_option(char *optstr, const char *name,
				char **value, size_t *valsz);
extern int mnt_optstr_set_option(char **optstr, const char *name,
				const char *value);
extern int mnt_optstr_remove_option(char **optstr, const char *name);

/* iter.c */
enum {

	MNT_ITER_FORWARD = 0,
	MNT_ITER_BACKWARD
};
extern mnt_iter *mnt_new_iter(int direction);
extern void mnt_free_iter(mnt_iter *mi);
extern void mnt_reset_iter(mnt_iter *mi, int direction);
extern int mnt_iter_get_direction(mnt_iter *itr);

/* optmap.c */
enum {
	MNT_LINUX_MAP = 1,
	MNT_USERSPACE_MAP
};
extern const struct mnt_optmap *mnt_get_builtin_optmap(int id);

/* optent.c */
extern const struct mnt_optmap *mnt_optent_get_map(mnt_optent *op);
extern const struct mnt_optmap *mnt_optent_get_mapent(mnt_optent *op);
extern const char *mnt_optent_get_type(mnt_optent *op);
extern int mnt_optent_set_value(mnt_optent *op, const char *data);
extern int mnt_optent_has_value(mnt_optent *op);
extern int mnt_optent_require_value(mnt_optent *op);
extern int mnt_optent_is_inverted(mnt_optent *op);
extern int mnt_optent_strtoul_value(mnt_optent *op, unsigned long int *number);
extern int mnt_optent_strtol_value(mnt_optent *op, long int *number);
extern int mnt_optent_strtoull_value(mnt_optent *op, unsigned long long int *number);
extern const char *mnt_optent_get_value(mnt_optent *op);
extern int mnt_optent_strlen_value(mnt_optent *op);
extern int mnt_optent_snprintf_value(mnt_optent *op, char *str, size_t size);
extern char *mnt_optent_dup_value(mnt_optent *op);
extern const char *mnt_optent_get_name(mnt_optent *op);
extern int mnt_optent_get_mask(mnt_optent *op);
extern int mnt_optent_get_id(mnt_optent *op);
extern int mnt_optent_get_flag(mnt_optent *op, int *flags);
extern int mnt_optent_is_unknown(mnt_optent *op);
extern int mnt_optent_print_debug(mnt_optent *op, FILE *file);

/* optls.c */
extern mnt_optls *mnt_new_optls(void);
extern void mnt_free_optls(mnt_optls *ls);
extern int mnt_optls_add_map(mnt_optls *ls, const struct mnt_optmap *map);
extern int mnt_optls_add_builtin_map(mnt_optls *ls, int id);
extern mnt_optent *mnt_optls_add_option(mnt_optls *ls,
                        const char *name, const char *value);
extern int mnt_optls_parse_optstr(mnt_optls *ls, const char *optstr);
extern int mnt_optls_remove_option(mnt_optls *ls, const char *name);
extern int mnt_optls_remove_option_by_flags(mnt_optls *ls,
                const struct mnt_optmap *map, const int flags);
extern int mnt_optls_remove_option_by_iflags(mnt_optls *ls,
                const struct mnt_optmap *map, const int flags);
extern int mnt_optls_next_option(mnt_optls *ls, mnt_iter *itr,
                const struct mnt_optmap *map, mnt_optent **option);
extern mnt_optent *mnt_optls_get_option(mnt_optls *ls, const char *name);
extern int mnt_optls_get_ids(mnt_optls *ls, const struct mnt_optmap *map);
extern int mnt_optls_create_mountflags(mnt_optls *ls);
extern char *mnt_optls_create_mountdata(mnt_optls *ls);
extern char *mnt_optls_create_mtab_optstr(mnt_optls *ls);
extern char *mnt_optls_create_userspace_optstr(mnt_optls *ls);
extern int mnt_optls_print_debug(mnt_optls *ls, FILE *file);

/* lock.c */
extern mnt_lock *mnt_new_lock(const char *lockfile, pid_t id);
extern void mnt_free_lock(mnt_lock *ml);
extern const char *mnt_lock_get_lockfile(mnt_lock *ml);
extern const char *mnt_lock_get_linkfile(mnt_lock *ml);
extern void mnt_unlock_file(mnt_lock *ml);
extern int mnt_lock_file(mnt_lock *ml);

/* fs.c */
extern mnt_fs *mnt_new_fs(void);
extern void mnt_free_fs(mnt_fs *ent);
extern void *mnt_fs_get_userdata(mnt_fs *fs);
extern int mnt_fs_set_userdata(mnt_fs *fs, void *data);
extern const char *mnt_fs_get_source(mnt_fs *ent);
extern int mnt_fs_set_source(mnt_fs *ent, const char *source);
extern const char *mnt_fs_get_srcpath(mnt_fs *ent);
extern int mnt_fs_get_tag(mnt_fs *ent, const char **name, const char **value);
extern const char *mnt_fs_get_target(mnt_fs *ent);
extern int mnt_fs_set_target(mnt_fs *ent, const char *target);
extern const char *mnt_fs_get_fstype(mnt_fs *ent);
extern int mnt_fs_set_fstype(mnt_fs *ent, const char *fstype);
extern const char *mnt_fs_get_optstr(mnt_fs *ent);
extern int mnt_fs_set_optstr(mnt_fs *ent, const char *optstr);
extern const char *mnt_fs_get_vfs_optstr(mnt_fs *ent);
extern const char *mnt_fs_get_fs_optstr(mnt_fs *ent);
extern int mnt_fs_get_freq(mnt_fs *ent);
extern int mnt_fs_set_freq(mnt_fs *ent, int freq);
extern int mnt_fs_get_passno(mnt_fs *ent);
extern int mnt_fs_set_passno(mnt_fs *ent, int passno);
extern int mnt_fs_get_id(mnt_fs *fs);
extern int mnt_fs_get_parent_id(mnt_fs *fs);
extern dev_t mnt_fs_get_devno(mnt_fs *fs);
extern int mnt_fs_get_option(mnt_fs *ent, const char *name,
				char **value, size_t *valsz);

extern int mnt_fs_match_target(mnt_fs *fs, const char *target, mnt_cache *cache);
extern int mnt_fs_match_source(mnt_fs *fs, const char *source, mnt_cache *cache);
extern int mnt_fs_match_fstype(mnt_fs *fs, const char *types);
extern int mnt_fs_match_options(mnt_fs *fs, const char *options);

/* mtab/fstab line */
#define MNT_TAB_PRINTFMT	"%s %s %s %s %d %d\n"

extern int mnt_fprintf_line(
                        FILE *f,
                        const char *fmt,
                        const char *source,
                        const char *target,
                        const char *fstype,
                        const char *options,
                        int freq,
                        int passno);

extern int mnt_fs_fprintf(mnt_fs *ent, FILE *f, const char *fmt);
extern int mnt_fs_print_debug(mnt_fs *ent, FILE *file);

/* tab-parse.c */
extern mnt_tab *mnt_new_tab_from_file(const char *filename);
extern int mnt_tab_parse_file(mnt_tab *tb);
extern char *mnt_tab_strerror(mnt_tab *tb, char *buf, size_t buflen);
extern int mnt_tab_get_nerrs(mnt_tab *tb);

/* tab.c */
extern mnt_tab *mnt_new_tab(const char *filename);
extern void mnt_free_tab(mnt_tab *tb);
extern int mnt_tab_get_nents(mnt_tab *tb);
extern int mnt_tab_set_cache(mnt_tab *tb, mnt_cache *mpc);
extern mnt_cache *mnt_tab_get_cache(mnt_tab *tb);
extern const char *mnt_tab_get_name(mnt_tab *tb);
extern int mnt_tab_add_fs(mnt_tab *tb, mnt_fs *fs);
extern int mnt_tab_remove_fs(mnt_tab *tb, mnt_fs *fs);
extern int mnt_tab_next_fs(mnt_tab *tb, mnt_iter *itr, mnt_fs **fs);
extern int mnt_tab_next_child_fs(mnt_tab *tb, mnt_iter *itr,
	                        mnt_fs *parent, mnt_fs **chld);
extern int mnt_tab_get_root_fs(mnt_tab *tb, mnt_fs **root);
extern int mnt_tab_set_iter(mnt_tab *tb, mnt_iter *itr, mnt_fs *fs);

extern mnt_fs *mnt_tab_find_target(mnt_tab *tb, const char *path, int direction);
extern mnt_fs *mnt_tab_find_srcpath(mnt_tab *tb, const char *path, int direction);
extern mnt_fs *mnt_tab_find_tag(mnt_tab *tb, const char *tag,
				const char *val, int direction);
extern mnt_fs *mnt_tab_find_source(mnt_tab *tb, const char *source, int direction);

extern int mnt_tab_find_next_fs(mnt_tab *tb, mnt_iter *itr,
			int (*match_func)(mnt_fs *, void *), void *userdata,
		        mnt_fs **fs);

extern int mnt_tab_fprintf(mnt_tab *tb, FILE *f, const char *fmt);
extern int mnt_tab_update_file(mnt_tab *tb);


/*
 * mount(8) userspace options masks (MNT_MAP_USERSPACE map)
 */
#define MNT_MS_DFLTS	(1 << 1)
#define MNT_MS_NOAUTO	(1 << 2)
#define MNT_MS_USER	(1 << 3)
#define MNT_MS_USERS	(1 << 4)
#define MNT_MS_OWNER	(1 << 5)
#define MNT_MS_GROUP	(1 << 6)
#define MNT_MS_NETDEV	(1 << 7)
#define MNT_MS_COMMENT  (1 << 8)
#define MNT_MS_LOOP     (1 << 9)
#define MNT_MS_NOFAIL   (1 << 10)

/*
 * mount(2) MS_* masks (MNT_MAP_LINUX map)
 */
#ifndef MS_RDONLY
#define MS_RDONLY	 1	/* Mount read-only */
#endif
#ifndef MS_NOSUID
#define MS_NOSUID	 2	/* Ignore suid and sgid bits */
#endif
#ifndef MS_NODEV
#define MS_NODEV	 4	/* Disallow access to device special files */
#endif
#ifndef MS_NOEXEC
#define MS_NOEXEC	 8	/* Disallow program execution */
#endif
#ifndef MS_SYNCHRONOUS
#define MS_SYNCHRONOUS	16	/* Writes are synced at once */
#endif
#ifndef MS_REMOUNT
#define MS_REMOUNT	32	/* Alter flags of a mounted FS */
#endif
#ifndef MS_MANDLOCK
#define MS_MANDLOCK	64	/* Allow mandatory locks on an FS */
#endif
#ifndef MS_DIRSYNC
#define MS_DIRSYNC	128	/* Directory modifications are synchronous */
#endif
#ifndef MS_NOATIME
#define MS_NOATIME	0x400	/* 1024: Do not update access times. */
#endif
#ifndef MS_NODIRATIME
#define MS_NODIRATIME   0x800	/* 2048: Don't update directory access times */
#endif
#ifndef MS_BIND
#define	MS_BIND		0x1000	/* 4096: Mount existing tree also elsewhere */
#endif
#ifndef MS_MOVE
#define MS_MOVE		0x2000	/* 8192: Atomically move tree */
#endif
#ifndef MS_REC
#define MS_REC		0x4000	/* 16384: Recursive loopback */
#endif
#ifndef MS_VERBOSE
#define MS_VERBOSE	0x8000	/* 32768 */
#endif
#ifndef MS_RELATIME
#define MS_RELATIME	0x200000 /* 200000: Update access times relative
                                  to mtime/ctime */
#endif
#ifndef MS_UNBINDABLE
#define MS_UNBINDABLE	(1<<17)	/* 131072 unbindable*/
#endif
#ifndef MS_PRIVATE
#define MS_PRIVATE	(1<<18)	/* 262144 Private*/
#endif
#ifndef MS_SLAVE
#define MS_SLAVE	(1<<19)	/* 524288 Slave*/
#endif
#ifndef MS_SHARED
#define MS_SHARED	(1<<20)	/* 1048576 Shared*/
#endif
#ifndef MS_I_VERSION
#define MS_I_VERSION	(1<<23)	/* update inode I_version field */
#endif
#ifndef MS_STRICTATIME
#define MS_STRICTATIME	(1<<24) /* strict atime semantics */
#endif

/*
 * Magic mount flag number. Had to be or-ed to the flag values.
 */
#ifndef MS_MGC_VAL
#define MS_MGC_VAL 0xC0ED0000	/* magic flag number to indicate "new" flags */
#endif
#ifndef MS_MGC_MSK
#define MS_MGC_MSK 0xffff0000	/* magic flag number mask */
#endif


/* Shared-subtree options */
#define MS_PROPAGATION  (MS_SHARED|MS_SLAVE|MS_UNBINDABLE|MS_PRIVATE)

/* Options that we make ordinary users have by default.  */
#define MS_SECURE	(MS_NOEXEC|MS_NOSUID|MS_NODEV)

/* Options that we make owner-mounted devices have by default */
#define MS_OWNERSECURE	(MS_NOSUID|MS_NODEV)

#ifdef __cplusplus
}
#endif

#endif /* _LIBMOUNT_MOUNT_H */
