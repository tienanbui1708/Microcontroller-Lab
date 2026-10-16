param([string]$CubeIde = 'C:\ST\STM32CubeIDE_1.19.0\STM32CubeIDE\stm32cubeidec.exe')
$ErrorActionPreference = 'Stop'
# CubeMX generates project LAB4 directly in this folder, next to LAB4.ioc.
$projectPath = $PSScriptRoot
$ideWorkspace = Join-Path $env:TEMP 'MCU_LAB4_cubeide'
$logPath = Join-Path $PSScriptRoot 'build.log'
if (-not (Test-Path -LiteralPath $CubeIde -PathType Leaf)) { throw 'STM32CubeIDE not found. Use -CubeIde with its location.' }
if (-not (Test-Path -LiteralPath (Join-Path $projectPath '.project'))) { throw 'Generate code from LAB4.ioc with CubeMX first (see README).' }
& $CubeIde --launcher.suppressErrors -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data $ideWorkspace -import $projectPath -build 'LAB4/Debug' 2>&1 | Tee-Object -FilePath $logPath
if ($LASTEXITCODE -ne 0) { throw 'Build failed. See LAB4/build.log.' }
$hexPath = Join-Path $projectPath 'Debug\LAB4.hex'
if (-not (Test-Path -LiteralPath $hexPath)) { throw 'No HEX produced. Enable Convert to Intel Hex in CubeIDE.' }
# Proteus/lab4.pdsprj loads lab_4.hex from its own folder.
Copy-Item -LiteralPath $hexPath -Destination (Join-Path $PSScriptRoot '..\Proteus\lab_4.hex') -Force
Get-FileHash -LiteralPath $hexPath -Algorithm SHA256
