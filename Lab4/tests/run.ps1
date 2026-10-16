# Host test for the scheduler: real scheduler.c, fake CMSIS intrinsics.
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$out = Join-Path $PSScriptRoot 'build'
New-Item -ItemType Directory -Force $out | Out-Null
gcc -std=c11 -Wall -Wextra -Werror -I"$PSScriptRoot\stub" -I"$root\LAB4\user_code\Inc" `
    "$PSScriptRoot\test_scheduler.c" "$root\LAB4\user_code\Src\scheduler.c" -o "$out\test_scheduler.exe"
if ($LASTEXITCODE -ne 0) { throw 'Compile failed.' }
& "$out\test_scheduler.exe"
if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
