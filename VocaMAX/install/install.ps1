$ErrorActionPreference = 'Stop'
$bundle = Join-Path $PSScriptRoot 'VocaMAX.vst3'
$binary = Join-Path $bundle 'Contents\x86_64-win\VocaMAX.vst3'
if (!(Test-Path -LiteralPath $binary)) { throw 'Extract the complete VocaMAX Windows installer ZIP first. Keep VocaMAX.vst3 beside this script.' }
if (Get-Process -Name 'Mixcraft*' -ErrorAction SilentlyContinue) { throw 'Close Mixcraft before installing or updating VocaMAX, then run this installer again.' }
$folder = Join-Path $env:LOCALAPPDATA 'Programs\Common\VST3'
$target = Join-Path $folder 'VocaMAX.vst3'
$staging = Join-Path $env:TEMP ('VocaMAX-install-' + [guid]::NewGuid().ToString('N'))
$backup = $null
try {
    New-Item -ItemType Directory -Force -Path $folder, $staging | Out-Null
    Copy-Item -LiteralPath $bundle -Destination $staging -Recurse
    if (Test-Path -LiteralPath $target) {
        $backups = Join-Path $env:LOCALAPPDATA 'Mark33s Beats Labs\Backups'
        New-Item -ItemType Directory -Force -Path $backups | Out-Null
        $backup = Join-Path $backups ('VocaMAX-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + [guid]::NewGuid().ToString('N') + '.vst3')
        Move-Item -LiteralPath $target -Destination $backup
    }
    Move-Item -LiteralPath (Join-Path $staging 'VocaMAX.vst3') -Destination $target
} catch {
    if ($backup -and !(Test-Path -LiteralPath $target)) { Move-Item -LiteralPath $backup -Destination $target }
    throw
} finally {
    if (Test-Path -LiteralPath $staging) { Remove-Item -LiteralPath $staging -Recurse -Force }
}
Write-Host ''
Write-Host 'VocaMAX installed successfully.' -ForegroundColor Green
Write-Host "VST3 folder: $folder"
Write-Host 'In Mixcraft 10: File > Preferences > Plug-Ins > Edit VST/VSTi Folders.'
Write-Host 'Add the folder shown above, rescan/restart Mixcraft, then search the FX list for VocaMAX.'
if ($backup) { Write-Host "Previous version preserved at: $backup" }
