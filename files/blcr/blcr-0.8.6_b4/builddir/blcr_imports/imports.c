/*
 * Berkeley Lab Checkpoint/Restart (BLCR) for Linux is Copyright (c)
 * 2003, The Regents of the University of California, through Lawrence
 * Berkeley National Laboratory (subject to receipt of any required
 * approvals from the U.S. Dept. of Energy).  All rights reserved.
 *
 * Portions may be copyrighted by others, as may be noted in specific
 * copyright notices within specific files.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: imports.c.in,v 1.3 2008/02/26 05:27:59 phargrov Exp $
 */

#ifndef EXPORT_SYMTAB
  #define EXPORT_SYMTAB
#endif

#include "blcr_config.h"
#include "blcr_imports.h"
#include "blcr_ksyms.h"

/* BEGIN AUTO-GENERATED FRAGMENT */
/* This file is autogenerated - do not edit or remove */
_CR_IMPORT_KCODE(signal_wake_up_state, 0xffffffff81082900)
_CR_IMPORT_KCODE(arch_setup_additional_pages, 0xffffffff81062490)
_CR_IMPORT_KCODE(syscall32_setup_pages, 0xffffffff81062640)
_CR_IMPORT_KCODE(arch_pick_mmap_layout, 0xffffffff8105d110)
_CR_IMPORT_KCODE(arch_unmap_area, 0xffffffff81186330)
_CR_IMPORT_KCODE(do_munmap, 0xffffffff81188800)
_CR_IMPORT_KCODE(do_mmap_pgoff, 0xffffffff811897a0)
_CR_IMPORT_KDATA(hugetlbfs_file_operations, 0xffffffff81657100)
_CR_IMPORT_KCODE(is_file_shm_hugepages, 0xffffffff81257f60)
_CR_IMPORT_KCODE(pmd_huge, 0xffffffff810606f0)
_CR_IMPORT_KCODE(follow_huge_pmd, 0xffffffff81060730)
_CR_IMPORT_KCODE(follow_huge_addr, 0xffffffff810606d0)
_CR_IMPORT_KCODE(copy_siginfo_to_user, 0xffffffff81085c20)
_CR_IMPORT_KCODE(group_send_sig_info, 0xffffffff810843b0)
_CR_IMPORT_KCODE(do_sigaltstack, 0xffffffff81081950)
_CR_IMPORT_KCODE(get_dumpable, 0xffffffff811cea80)
_CR_IMPORT_KDATA(suid_dumpable, 0xffffffff81e9bd60)
_CR_IMPORT_KCODE(set_dumpable, 0xffffffff811ce850)
_CR_IMPORT_KCODE(groups_search, 0xffffffff8109f520)
_CR_IMPORT_KCODE(detach_pid, 0xffffffff81093830)
_CR_IMPORT_KCODE(attach_pid, 0xffffffff810937f0)
_CR_IMPORT_KCODE(change_pid, 0xffffffff81093850)
_CR_IMPORT_KCODE(find_task_by_pid_ns, 0xffffffff81093900)
_CR_IMPORT_KCODE(free_pid, 0xffffffff810930e0)
_CR_IMPORT_KDATA(pid_hash, 0xffffffff81d61cf8)
_CR_IMPORT_KDATA(pidhash_shift, 0xffffffff81941a90)
_CR_IMPORT_KDATA(pidmap_lock, 0xffffffff81903140)
_CR_IMPORT_KDATA(anon_pipe_buf_ops, 0xffffffff8164c340)
_CR_IMPORT_KCODE(pipe_fcntl, 0xffffffff811d06b0)
_CR_IMPORT_KDATA(tasklist_lock, 0xffffffff81903040)
_CR_IMPORT_KDATA(shmem_file_operations, 0xffffffff81647ec0)
_CR_IMPORT_KDATA(ramfs_file_operations, 0xffffffff81656d00)
_CR_IMPORT_KCODE(do_pipe_flags, 0xffffffff811d04e0)
_CR_IMPORT_KCODE(sys_munmap, 0xffffffff81189b60)
_CR_IMPORT_KCODE(sys_dup2, 0xffffffff811e50d0)
_CR_IMPORT_KCODE(sys_lseek, 0xffffffff811c6540)
_CR_IMPORT_KCODE(sys_ftruncate, 0xffffffff811c4f30)
_CR_IMPORT_KCODE(sys_mprotect, 0xffffffff8118afe0)
_CR_IMPORT_KCODE(sys_setitimer, 0xffffffff81075b00)
_CR_IMPORT_KCODE(sys_prctl, 0xffffffff8108abc0)
_CR_IMPORT_KCODE(copy_fs_struct, 0xffffffff811f7ed0)
_CR_IMPORT_KCODE(free_fs_struct, 0xffffffff811f7e00)
_CR_IMPORT_KCODE(__audit_syscall_exit, 0xffffffff810fabe0)
_CR_IMPORT_KCODE(set_fs_pwd, 0xffffffff811f7b70)
_CR_IMPORT_KCODE(set_mm_exe_file, 0xffffffff8106b3e0)
_CR_IMPORT_KCODE(sys_mremap, 0xffffffff8118bd50)
_CR_IMPORT_KCODE(do_sigaction, 0xffffffff81086340)
_CR_IMPORT_KDATA(old_rsp, 0x000000000000af80)
_CR_IMPORT_KCODE(expand_fdtable, 0xffffffff811e43d0)
_CR_IMPORT_KCODE(sys_fchmod, 0xffffffff811c56b0)
_CR_IMPORT_KCODE(sys_link, 0xffffffff811d8940)
_CR_IMPORT_KCODE(sys_unlink, 0xffffffff811d8530)
_CR_IMPORT_KCODE(sys_mknod, 0xffffffff811d83b0)
/* END AUTO-GENERATED FRAGMENT */

const char *blcr_config_timestamp = BLCR_CONFIG_TIMESTAMP;
EXPORT_SYMBOL_GPL(blcr_config_timestamp);
