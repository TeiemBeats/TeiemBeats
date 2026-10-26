# Teiem TG Mastering

Native stereo VST3 mastering plugin built with JUCE 8.0.12. The supplied reference image is embedded as the faceplate, with interactive host-automatable controls, output and gain-reduction meters, and session recall.

This is an independent prototype, **not an accurate sonic duplicate of the Waves/Abbey Road plugin**. It does not contain their source code, circuit models, measured responses, or presets. The artwork comes from the user-supplied reference; confirm appropriate rights before distributing it. JUCE's AGPL/commercial licensing terms also apply (see the JUCE checkout's LICENSE.md).

## Features

- Stereo input gain, balance, channel swap, left polarity inversion, stereo rotation and approximate tape tilt.
- High/low-pass filters and variable-frequency presence EQ.
- Stereo-linked dynamics with 3 modes, recovery, ratio, makeup, dry/wet blend and detector high-pass.
- Four RBJ EQ bands, each with variable frequency, gain and low-shelf/bell/high-shelf shape.
- Mid/side stereo width, independent output trims and stereo/left/right/mono/side monitoring.
- 37 host parameters, per-module enable, global bypass, editable track label (local editor only), resizing and reset.

The photographic ST/DUO/MS buttons, module-link switches and unused monitor buttons are artwork only. Input tape EQ uses a continuous tilt approximation rather than original NAB/IEC curves. Dynamics use a fixed -12 dBFS threshold and are not a brickwall or true-peak limiter. Controls add a moving indicator over the reference's fixed knob artwork. Parameter changes are block-rate and may click under abrupt automation; do not treat this prototype as a finished mastering replacement.

## Build

Requires CMake 3.22+, C++17 compiler and JUCE pinned to commit `29396c22c93392d6738e021b83196283d6e4d850` (tag 8.0.12).

```sh
git clone --depth 1 --branch 8.0.12 https://github.com/juce-framework/JUCE.git ../dependencies/JUCE
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH=../dependencies/JUCE
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
```

On Linux install the X11, Xext, Xinerama, Xrandr, Xcursor, ALSA, FreeType and Fontconfig development packages. In this cloud machine run `bash scripts/cloud-build.sh`; it uses libraries extracted into `/workspace/sysroot` without requiring system writes.

On Windows build with Visual Studio 2022's C++ tools. On macOS use Xcode command line tools. Those builds have not been validated here. AU is not currently a configured target.

## Install the Linux build

Extract the release archive, then copy the entire `Teiem TG Mastering.vst3` directory to `~/.vst3/`. Rescan VST3 plugins in a Linux DAW and load **Teiem TG Mastering** onto a stereo track or master bus. This Linux binary cannot load in Windows or macOS DAWs. Use a safety limiter downstream when auditioning.

Build artifact: `build/TeiemTG_artefacts/Release/VST3/Teiem TG Mastering.vst3`.

Tests exercise DSP at 44.1, 48 and 96 kHz, processor bypass, parameter registration, state serialization, and Linux VST3 dynamic loading/factory discovery. An interactive commercial DAW session is not validated in this cloud machine.

## Windows x64 (requested target)

The Windows workflow at `.github/workflows/windows-vst3.yml` builds with Visual Studio 2022, runs DSP and session-state tests, and uploads a VST3 bundle. It has not been run on GitHub from this session. After pushing these files, run **Windows VST3** in GitHub Actions, download the artifact and retain its complete bundle directory structure.

For a local Windows build, use the build commands above in a Visual Studio developer terminal with `cmake -S . -B build -A x64 -DJUCE_PATH=../dependencies/JUCE`. Install with `powershell -ExecutionPolicy Bypass -File scripts/install-windows.ps1`, or copy the entire bundle into `C:\Program Files\Common Files\VST3\` with administrator permission. Rescan the VST3 folder in your DAW. Start with low monitoring volume; this processor can boost gain.

### Mixcraft 10 Pro Studio

Use the **Windows x64 VST3** build, not the Linux `.so` bundle. With Visual Studio 2022 C++ tools installed, run `powershell -ExecutionPolicy Bypass -File scripts/build-windows.ps1` from its developer terminal, then run the installer above. In Mixcraft, open **File → Preferences → Plug-Ins**, add the installation folder to its VST search folders if necessary, and rescan/restart. Locate **Teiem TG Mastering** in the effects list and insert it on a stereo track or the master bus. The per-user installer prints the exact folder to add. Menu labels may vary slightly by Mixcraft build.

The cloud cross-build was attempted with MinGW; JUCE 8.0.12 explicitly rejects that compiler. Windows output requires the supplied MSVC workflow or a local Visual Studio build. No Windows binary or Mixcraft compatibility result has been produced in this session.
