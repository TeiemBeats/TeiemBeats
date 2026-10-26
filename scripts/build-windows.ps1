$ErrorActionPreference = 'Stop'
$project = Split-Path $PSScriptRoot -Parent
Set-Location $project
if (!(Get-Command cmake -ErrorAction SilentlyContinue)) { throw 'Install Visual Studio 2022 with Desktop development with C++ and CMake tools, then run this script in its developer terminal.' }
$juce = Join-Path $project '..\dependencies\JUCE'
if (!(Test-Path "$juce\CMakeLists.txt")) {
  git clone --depth 1 --branch 8.0.12 https://github.com/juce-framework/JUCE.git $juce
  if ($LASTEXITCODE -ne 0) { throw 'JUCE download failed.' }
}
cmake -S . -B build -A x64 "-DJUCE_PATH=$juce"
if ($LASTEXITCODE -ne 0) { throw 'Configure failed.' }
cmake --build build --config Release --parallel 4
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
ctest --test-dir build -C Release --output-on-failure
if ($LASTEXITCODE -ne 0) { throw 'Tests failed; do not install this build.' }
Write-Host 'Build and tests passed. Run scripts/install-windows.ps1 to install.'
