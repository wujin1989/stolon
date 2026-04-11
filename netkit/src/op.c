/** Copyright (c) 2026, abc <xxx@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include "op.h"

#include <stdlib.h>

_op_t* _op_create(_op_type_t type, void* handle, void* cb, void* data) {
    _op_t* op = (_op_t*)calloc(1, sizeof(_op_t));
    if (!op) {
        return NULL;
    }
    op->type        = type;
    op->handle      = handle;
    op->cb          = cb;
    op->user_data   = data;
    op->accept_sock = INVALID_SOCKET;
    return op;
}

void _op_destroy(_op_t* op) {
    if (!op) {
        return;
    }
    if (op->accept_sock != INVALID_SOCKET) {
        closesocket(op->accept_sock);
        op->accept_sock = INVALID_SOCKET;
    }
    free(op);
}
