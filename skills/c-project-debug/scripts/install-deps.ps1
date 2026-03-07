#Requires -RunAsAdministrator
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Write-Host "Installing debugging tools for Windows..."
Write-Host ""
Write-Host "MSVC debugger is included with Visual Studio 2022."
Write-Host "Ensure Visual Studio 2022 with C++ workload is installed."
Write-Host ""

# WinDbg (optional, for advanced debugging)
if (Get-Command winget -ErrorAction SilentlyContinue) {
    Write-Host "Installing WinDbg Preview via winget..."
    winget install --id Microsoft.WinDbg --accept-package-agreements --accept-source-agreements
} elseif (Get-Command choco -ErrorAction SilentlyContinue) {
    Write-Host "Installing WinDbg via choco..."
    choco install -y windbg
} else {
    Write-Host "winget/choco not found. Install WinDbg manually from Microsoft Store."
}

Write-Host "Done."
