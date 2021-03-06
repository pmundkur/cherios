/*-
 * Copyright (c) 2016 Hadrien Barral
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "mips.h"
#include "object.h"
#include "namespace.h"
#include "assert.h"

extern void * vblk_ref;
extern void * vblk_id;

static inline void virtio_check_refs(void) {
	if(vblk_ref == NULL) {
		vblk_ref = namespace_get_ref(4);
		vblk_id  = namespace_get_id(4);
	}
	assert(vblk_ref != NULL);
	assert(vblk_id  != NULL);
}

static inline void virtio_blk_session(void * mmio_cap) {
	virtio_check_refs();
	vblk_id = ccall_c_c(vblk_ref, vblk_id, -1, mmio_cap);
}


static inline int virtio_blk_init(void) {
	virtio_check_refs();
	return ccall_n_r(vblk_ref, vblk_id, 0);
}

static inline int virtio_read(void * buf, size_t sector) {
	virtio_check_refs();
	return ccall_rc_r(vblk_ref, vblk_id, 1, sector, buf);
}

static inline int virtio_write(const void * buf, size_t sector) {
	virtio_check_refs();
	return ccall_rc_r(vblk_ref, vblk_id, 2, sector, buf);
}

static inline int virtio_blk_status(void) {
	virtio_check_refs();
	return ccall_n_r(vblk_ref, vblk_id, 3);
}

static inline size_t virtio_blk_size(void) {
	virtio_check_refs();
	return ccall_n_r(vblk_ref, vblk_id, 4);
}
