# Code Style

## License Header

Every `.c` and `.h` file must start with the project license block:

```c
/** Copyright (c) {YEAR}, {AUTHOR} <{EMAIL}>
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
```

## Naming Convention

**Prefix Rule:** Use the `{project}` value (lowercase with hyphens, as entered during setup) as the namespace prefix. In C identifiers (functions, types, macros), replace hyphens with underscores. In file names, keep hyphens as-is.

| Context | Project `mylib` | Project `hello-lib` |
|---------|----------------|---------------------|
| C identifier prefix | `mylib_` | `hello_lib_` |
| Source file prefix | `mylib-` | `hello-lib-` |

| Category | Pattern | Example (project: `mylib`) |
|----------|---------|---------------------------|
| Public functions | `{project}_<module>_<action>` | `mylib_list_insert` |
| Static functions | `_<module>_<action>` | `_tcp_flush_writes` |
| Static callbacks | `_<module>_<subject>_<event>_cb` | `_tcp_conn_io_cb` |
| Types | `{project}_<module>_t` | `mylib_list_t` |
| Node types | `{project}_<module>_node_t` | `mylib_heap_node_t` |
| Function pointer typedefs | `{project}_<module>_<purpose>_fn_t` | `mylib_rbtree_cmp_fn_t` |
| Internal types (file-scope) | `_<name>_t` | `_node_t` |
| Static variables (file-scope) | `_<name>` | `_echo_loop` |