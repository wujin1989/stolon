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

#include "netkit/netkit-err.h"

#include "err.h"

#include <winsock2.h>

const char* netkit_err_strerror(int err) {
    switch (err) {
    case 0:
        return "success";
    case NETKIT_ERR_NOMEM:
        return "out of memory";
    case NETKIT_ERR_INVALID:
        return "invalid argument or state";
    case NETKIT_ERR_EOF:
        return "end of stream";
    case NETKIT_ERR_TIMEOUT:
        return "operation timed out";
    case NETKIT_ERR_CONNRESET:
        return "connection reset by peer";
    case NETKIT_ERR_CONNREFUSED:
        return "connection refused";
    case NETKIT_ERR_ADDRINUSE:
        return "address already in use";
    case NETKIT_ERR_CANCELLED:
        return "operation cancelled";
    case NETKIT_ERR_UNKNOWN:
        return "unknown error";
    default:
        return "unknown error";
    }
}

int netkit_err_from_wsa_code(int wsa_err) {
    switch (wsa_err) {
    case WSAECONNRESET:
        return NETKIT_ERR_CONNRESET;
    case WSAECONNREFUSED:
        return NETKIT_ERR_CONNREFUSED;
    case WSAEADDRINUSE:
        return NETKIT_ERR_ADDRINUSE;
    case WSAETIMEDOUT:
        return NETKIT_ERR_TIMEOUT;
    case WSA_OPERATION_ABORTED:
        return NETKIT_ERR_CANCELLED;
    case WSAESHUTDOWN:
        return NETKIT_ERR_EOF;
    default:
        return NETKIT_ERR_UNKNOWN;
    }
}

int netkit_err_from_wsa(void) {
    return netkit_err_from_wsa_code(WSAGetLastError());
}
