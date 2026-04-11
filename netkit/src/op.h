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

_Pragma("once")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>

#define OP_READ_BUF_SIZE 65536

typedef enum _op_type_e {
    _OP_ACCEPT  = 1,
    _OP_CONNECT = 2,
    _OP_READ    = 3,
    _OP_WRITE   = 4,
    _OP_RECV    = 5,
    _OP_SEND    = 6
} _op_type_t;

typedef struct _op_s {
    OVERLAPPED  overlapped;  /**< Must be first field for IOCP casting. */
    _op_type_t  type;        /**< Operation type discriminator. */
    void*       handle;      /**< Back-pointer to owning handle. */
    void*       cb;          /**< User callback (cast to specific fn type). */
    void*       user_data;   /**< User data for callback. */
    WSABUF      wsa_buf;     /**< Buffer descriptor for WSA operations. */
    uint8_t     read_buf[OP_READ_BUF_SIZE]; /**< Inline read buffer. */
    SOCKET      accept_sock; /**< Pre-created socket for AcceptEx. */
    uint8_t     addr_buf[2 * (sizeof(struct sockaddr_in6) + 16)]; /**< AcceptEx address buffer. */
} _op_t;

/**
 * @brief Allocate and zero-initialize an op.
 *
 * @param type    Operation type.
 * @param handle  Back-pointer to the owning handle.
 * @param cb      User callback function pointer.
 * @param data    User data for callback.
 *
 * @return Pointer to the new op, or NULL on allocation failure.
 */
extern _op_t* _op_create(_op_type_t type, void* handle, void* cb, void* data);

/**
 * @brief Free an op.
 *
 * @param op  The op to free. NULL is safe.
 */
extern void _op_destroy(_op_t* op);
