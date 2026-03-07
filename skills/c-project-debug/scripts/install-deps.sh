#!/bin/bash
set -e

OS="$(uname -s)"

case "$OS" in
    Linux*)
        echo "Installing debugging tools for Linux..."
        sudo apt install -y gdb
        ;;
    Darwin*)
        echo "Installing debugging tools for macOS..."
        echo "LLDB is included with Xcode Command Line Tools."
        xcode-select --install 2>/dev/null || echo "Xcode CLT already installed."
        ;;
    *)
        echo "Unsupported OS: $OS"
        echo "For Windows, use install-deps.ps1 instead."
        exit 1
        ;;
esac

echo "Done."
