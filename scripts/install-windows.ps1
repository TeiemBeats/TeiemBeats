param([string]$Bundle = (Join-Path $PSScriptRoot '..\build\TeiemTG_artefacts\Release\VST3\Teiem TG Mastering.vst3'))
$ErrorActionPreference = 'Stop'
$binary = Join-Path $Bundle 'Contents\x86_64-win\Teiem TG Mastering.vst3'
if (!(Test-Path $binary)) { throw 'Windows x64 VST3 bundle not found. Build the Windows release first.' }
$destination = Join-Path $env:LOCALAPPDATA 'Programs\Common\VST3'
New-Item -ItemType Directory -Force -Path $destination | Out-Null
$target = Join-Path $destination 'Teiem TG Mastering.vst3'
if (Test-Path $target) { throw "A plugin already exists at $target. Preserve it before installing this build." }
Copy-Item -Recurse -Path $Bundle -Destination $destination
Write-Host "Installed to $target. Add this directory to your DAW's VST3 paths if required, then rescan."
