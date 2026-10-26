# Validation — 2026-10-26

- Linux x86_64 Release VST3 compiled with GCC 14 and JUCE 8.0.12.
- CTest: 3/3 suites passed (21 DSP checks across 44.1/48/96 kHz, processor bypass/37 parameters/session recall, dynamic VST3 factory loading).
- Native editor rendered under Xvfb at 1260×804; changing its input slider updated the processor parameter. An initialization-time resize crash was found and fixed.
- Cloud setup script rerun successfully; local libraries and build tools work without system installation.
- Windows cross-compilation attempted and blocked by JUCE's explicit rejection of MinGW. No Windows VST3 was produced.
- Windows Visual Studio 2022 workflow, local build script, and installer supplied; not executed on Windows here.
- Mixcraft 10 Pro Studio installation and operation remain unverified.
- DSP is an independent approximation. Original ST/DUO/MS routing and exact proprietary circuit behavior are not implemented. See README for other limitations.
