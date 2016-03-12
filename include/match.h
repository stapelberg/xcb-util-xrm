/*
 * vim:ts=4:sw=4:expandtab
 *
 * Copyright © 2016 Ingo Bürk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or their
 * institutions shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the authors.
 *
 */
#ifndef __MATCH_H__
#define __MATCH_H__

#include "xrm.h"
#include "entry.h"

typedef enum xcb_xrm_match_type_t {
    MT_NONE = 1 << 0,

    MT_CLASS = 1 << 1,
    MT_NAME = 1 << 2,

    MT_EXACT = 1 << 3,
    MT_SINGLE = 1 << 4,
    MT_MULTI = 1 << 5
} xcb_xrm_match_type_t;

typedef struct xcb_xrm_match_t {
    /* Reference to the database entry this match refers to. */
	xcb_xrm_entry_t *entry;
    /* An array where the n-th element describes how the n-th element of the
     * query strings was matched. */
	int *matches;
} xcb_xrm_match_t;

/**
 * Finds the matching entry in the database given a full name / class query string.
 *
 */
int xcb_xrm_match(xcb_xrm_context_t *ctx, xcb_xrm_entry_t *query_name, xcb_xrm_entry_t *query_class,
        xcb_xrm_resource_t *resource);

#endif /* __MATCH_H__ */
