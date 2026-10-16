$ErrorActionPreference = 'Stop'
$sourceHex = Join-Path $PSScriptRoot 'Debug\LAB4.hex'
# Proteus/lab4.pdsprj loads lab_4.hex from its own folder.
$targetHex = Join-Path $PSScriptRoot '..\Proteus\lab_4.hex'
if (-not (Test-Path -LiteralPath $sourceHex)) { throw 'Build LAB4/Debug first.' }
Copy-Item -LiteralPath $sourceHex -Destination $targetHex -Force
Get-FileHash -LiteralPath $sourceHex,$targetHex -Algorithm SHA256
