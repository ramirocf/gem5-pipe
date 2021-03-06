/*
 * Copyright (c) 2003-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gabe Black
 *          Nathan Binkert
 *          Ali Saidi
 */

#include "m5ops.h"

#define TWO_BYTE_OP(name, number)         \
        .globl name;                      \
        .func name;                       \
name:                                     \
        .byte 0x0F, 0x04;                 \
        .word number;                     \
        ret;                              \
        .endfunc;

TWO_BYTE_OP(arm, arm_func)
TWO_BYTE_OP(quiesce, quiesce_func)
TWO_BYTE_OP(quiesceNs, quiescens_func)
TWO_BYTE_OP(quiesceCycle, quiescecycle_func)
TWO_BYTE_OP(quiesceTime, quiescetime_func)
TWO_BYTE_OP(rpns, rpns_func)
TWO_BYTE_OP(m5_exit, exit_func)
TWO_BYTE_OP(m5_fail, fail_func)
TWO_BYTE_OP(m5_initparam, initparam_func)
TWO_BYTE_OP(m5_loadsymbol, loadsymbol_func)
TWO_BYTE_OP(m5_reset_stats, resetstats_func)
TWO_BYTE_OP(m5_dump_stats, dumpstats_func)
TWO_BYTE_OP(m5_dumpreset_stats, dumprststats_func)
TWO_BYTE_OP(m5_checkpoint, ckpt_func)
TWO_BYTE_OP(m5_readfile, readfile_func)
TWO_BYTE_OP(m5_writefile, writefile_func)
TWO_BYTE_OP(m5_debugbreak, debugbreak_func)
TWO_BYTE_OP(m5_switchcpu, switchcpu_func)
TWO_BYTE_OP(m5_addsymbol, addsymbol_func)
TWO_BYTE_OP(m5_panic, panic_func)
TWO_BYTE_OP(m5_work_begin, work_begin_func)
TWO_BYTE_OP(m5_work_end, work_end_func)

TWO_BYTE_OP(fi_activate_inst,fi_activate_func)
TWO_BYTE_OP(fi_read_init_all,init_fi_system)
TWO_BYTE_OP(m5_whofailed,whofailed)
