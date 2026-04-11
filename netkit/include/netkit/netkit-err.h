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

/**
 * @brief Error codes returned by netkit functions.
 *
 * All values are negative. Zero means success.
 */
typedef enum netkit_err_e {
    NETKIT_ERR_NOMEM       = -1,  /**< Memory allocation failed. */
    NETKIT_ERR_INVALID     = -2,  /**< Invalid argument or state. */
    NETKIT_ERR_EOF         = -3,  /**< End of stream / connection closed. */
    NETKIT_ERR_TIMEOUT     = -4,  /**< Operation timed out. */
    NETKIT_ERR_CONNRESET   = -5,  /**< Connection reset by peer. */
    NETKIT_ERR_CONNREFUSED = -6,  /**< Connection refused. */
    NETKIT_ERR_ADDRINUSE   = -7,  /**< Address already in use. */
    NETKIT_ERR_CANCELLED   = -8,  /**< Operation cancelled. */
    NETKIT_ERR_UNKNOWN     = -99  /**< Unmapped system error. */
} netkit_err_t;

/**
 * @brief Return a human-readable string for an error code.
 *
 * @param err  One of the NETKIT_ERR_* values.
 *
 * @return Static string describing the error. Never NULL.
 */
extern const char* netkit_err_strerror(int err);
