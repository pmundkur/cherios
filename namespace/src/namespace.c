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

#include "lib.h"
#include "sys/mman.h"

typedef struct {
	void * act_reference;
	void * act_default_id;
} bind_t;

const int bind_len = 0x80;
bind_t bind[bind_len];
int count;

void ns_init(void) {
	/* We need to bootstrap the namespace refs ourselves using our
	   ctrl cap, since the generic libuser was provided NULL
	   caps. */
	void * ctrl;
	extern void * namespace_ref, * namespace_id;

	ctrl = act_get_ctrl();
	namespace_ref = act_ctrl_get_ref(ctrl);
	namespace_id  = act_ctrl_get_id(ctrl);

	bzero(bind, sizeof(bind));
	count = 0;
}

static int validate_idx(int nb) {
	if(nb <  0       ) { return 0; }
	if(nb >= bind_len) { return 0; }
	return 1;
}

static int validate_act_caps(void * act_reference, void * act_default_id) {
	if(cheri_gettag(act_reference) == 0) { return 0; }
	if(cheri_gettag(act_default_id) == 0) { return 0; }
	if(cheri_getsealed(act_reference) == 0) { return 0; }
	if(cheri_getsealed(act_default_id) == 0) { return 0; }
	if(cheri_gettype(act_reference) !=
	   cheri_gettype(act_default_id)) { return 0; }
	/* todo: check otype range and permissions */
	return 1;
}

/* Get reference for service 'n' */
void * ns_get_reference(int nb) {
	if(!validate_idx(nb)) return NULL;

	/* If service not in use, will already return NULL */
	printf("%s: ref request for port %d\n", __func__, nb);
	return bind[nb].act_reference;
}

/* Get default identifier for service 'n' */
void * ns_get_identifier(int nb) {
	if(!validate_idx(nb)) return NULL;

	/* If service not in use, will already return NULL */
	printf("%s: id request for port %d\n", __func__, nb);
	return bind[nb].act_default_id;
}


/* Register a module a service 'nb' */
static int ns_register_core(int nb, void * act_reference, void * act_default_id) {
	if(bind[nb].act_reference != NULL) {
		printf("%s: port already in use\n", __func__);
		return -1;
	}

	bind[nb].act_reference  = act_reference;
	bind[nb].act_default_id = act_default_id;

	/* By convention, the first service registration is from the
	 * mem-mgr.
	 */
	if (count == 0) {
		mmap_set_act(act_reference, act_default_id);
		printf("%s: #%d (mem-mgr) registered at port %d\n", __func__, count, nb);
	} else {
		printf("%s: #%d registered at port %d\n", __func__, count, nb);
	}

	count++;
	return 0;
}

int ns_register(int nb, void * act_reference, void * act_default_id) {
	if(!validate_idx(nb) || !validate_act_caps(act_reference, act_default_id))
		return -1;

	return ns_register_core(nb, act_reference, act_default_id);
}

int ns_get_num_services(void) {
	return count;
}
