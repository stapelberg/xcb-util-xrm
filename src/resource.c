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
#include "externals.h"

#include "resource.h"
#include "database.h"
#include "match.h"
#include "util.h"

/* Forward declarations */
static int __resource_get(xcb_xrm_database_t *database, const char *res_name, const char *res_class,
                         xcb_xrm_resource_t **_resource);
static void __resource_free(xcb_xrm_resource_t *resource);

/*
 * Returns the string value of a resource.
 * If the resource cannot be found, NULL is returned.
 *
 * Note that the string is owned by the caller and must be free'd.
 *
 * @param database The database to query.
 * @param res_name The fully qualified resource name string.
 * @param res_class The fully qualified resource class string. This argument
 * may be left empty / NULL, but if given, it must contain the same number of
 * components as res_name.
 * @returns The string value of the resource or NULL otherwise.
 */
char *xcb_xrm_resource_get_string(xcb_xrm_database_t *database,
        const char *res_name, const char *res_class) {
    char *value = NULL;

    xcb_xrm_resource_t *resource;
    if (__resource_get(database, res_name, res_class, &resource) < 0) {
        __resource_free(resource);
        return NULL;
    }

    assert(resource->value != NULL);
    value = strdup(resource->value);
    __resource_free(resource);

    return value;
}

/*
 * Returns the long value of a resource.
 * If the resource cannot be found or its value cannot be converted to a long,
 * LONG_MIN is returned.
 *
 * @param database The database to query.
 * @param res_name The fully qualified resource name string.
 * @param res_class The fully qualified resource class string. This argument
 * may be left empty / NULL, but if given, it must contain the same number of
 * components as res_name.
 * @returns The long value of the resource or NULL otherwise.
 */
long xcb_xrm_resource_get_long(xcb_xrm_database_t *database,
        const char *res_name, const char *res_class) {
    long value;
    char *str = xcb_xrm_resource_get_string(database, res_name, res_class);

    value = xcb_xrm_convert_to_long(str);
    FREE(str);

    return value;
}

/*
 * Returns the bool value of a resource.
 *
 * The return value of this function is determined by the following steps which
 * are executed in this order:
 *  - If the resource cannot be found, false is returned.
 *  - If the value can be converted to a long, the result will be the
 *    truthiness of the converted number.
 *  - If the value is one of "true", "on" or "yes" (case-insensitive), true is
 *    returned.
 *  - If the value is one of "false", "off" or "no" (case-insensitive), false
 *    is returned.
 *  - Otherwise, false is returned.
 *
 * @param database The database to query.
 * @param res_name The fully qualified resource name string.
 * @param res_class The fully qualified resource class string. This argument
 * may be left empty / NULL, but if given, it must contain the same number of
 * components as res_name.
 * @returns The bool value of the resource or NULL otherwise.
 */
bool xcb_xrm_resource_get_bool(xcb_xrm_database_t *database,
        const char *res_name, const char *res_class) {
    bool value;
    char *str = xcb_xrm_resource_get_string(database, res_name, res_class);

    value = xcb_xrm_convert_to_bool(str);
    FREE(str);

    return value;
}

static int __resource_get(xcb_xrm_database_t *database, const char *res_name, const char *res_class,
                         xcb_xrm_resource_t **_resource) {
    xcb_xrm_resource_t *resource;
    xcb_xrm_entry_t *query_name = NULL;
    xcb_xrm_entry_t *query_class = NULL;
    int result = SUCCESS;

    if (database == NULL || TAILQ_EMPTY(database)) {
        *_resource = NULL;
        return -FAILURE;
    }

    *_resource = calloc(1, sizeof(struct xcb_xrm_resource_t));
    if (_resource == NULL) {
        result = -FAILURE;
        goto done;
    }
    resource = *_resource;

    if (res_name == NULL || xcb_xrm_entry_parse(res_name, &query_name, true) < 0) {
        result = -FAILURE;
        goto done;
    }

    /* For the resource class input, we allow NULL and empty string as
     * placeholders for not specifying this string. Technically this is
     * violating the spec, but it seems to be widely used. */
    if (res_class != NULL && strlen(res_class) > 0 &&
            xcb_xrm_entry_parse(res_class, &query_class, true) < 0) {
        result = -1;
        goto done;
    }

    /* We rely on name and class query strings to have the same number of
     * components, so let's check that this is the case. The specification
     * backs us up here. */
    if (query_class != NULL &&
            xcb_xrm_entry_num_components(query_name) != xcb_xrm_entry_num_components(query_class)) {
        result = -1;
        goto done;
    }

    result = xcb_xrm_match(database, query_name, query_class, resource);
done:
    xcb_xrm_entry_free(query_name);
    xcb_xrm_entry_free(query_class);
    return result;
}

static void __resource_free(xcb_xrm_resource_t *resource) {
    if (resource == NULL)
        return;

    FREE(resource->value);
    FREE(resource);
}
