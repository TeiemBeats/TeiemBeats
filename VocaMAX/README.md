# VocaMAX — Mark33s Beats Labs

Windows x64 VST3 vocal mix and mastering processor for Mixcraft 10 Pro Studio. This is a new, original audio implementation of the controls shown in the supplied VocaMAX concept image.

## Download and install

1. Open https://github.com/TeiemBeats/TeiemBeats/actions/workflows/vocamax-windows.yml .
2. Open a successful **VocaMAX Windows VST3** run. Download **VocaMAX-Windows-x64-Installer** from Artifacts.
3. Right-click the downloaded ZIP and choose **Extract All**. Close Mixcraft.
4. Open the extracted folder and double-click **Install-VocaMAX.cmd**. It installs the complete VST3 bundle to `%LOCALAPPDATA%\Programs\Common\VST3`, without administrator access. An existing VocaMAX version is backed up outside your plugin scan folder before replacement.
5. In Mixcraft 10, open **File → Preferences → Plug-Ins → Edit VST/VSTi Folders** (wording can vary). Add the folder printed by the installer. Rescan or restart Mixcraft.
6. Search the effects list for **VocaMAX** and add it to a vocal track or master bus. Lower monitoring volume before enabling the full chain; the pictured default settings intentionally enable many effects.

For a system-wide manual installation, copy the **entire** `VocaMAX.vst3` directory into `C:\Program Files\Common Files\VST3\` and accept the Windows administrator prompt. Keep the `Contents` directory and all files inside it intact. The installer package contains a Windows-rendered preview when its editor test passes.

## Appearance and original reference

The default interface is a scalable, drawn recreation of the supplied 1536×1024 image: silver machined panels, green/purple light rings, rotating metal knobs, the VocaMAX title and Mark33s Beats Labs branding, all module sections, waveform, power switches and numeric displays. It is not a pixel-exact copy of the original photograph: the inline chat image was visible but its source bytes were not available in the workspace.

For the **exact original artwork**, choose **Settings → Load reference faceplate (PNG/JPG)** and select the original, uncropped 3:2 image. The original faceplate is displayed directly; metallic knob centers rotate over it, while values, meters and the waveform update live. The selected local file path is saved with the Mixcraft session; retain the image at that path. To embed it permanently in the plugin, put the original PNG in `VocaMAX/Assets/faceplate.png` and rebuild. CMake embeds it automatically. JPG may be loaded at runtime; the build-time asset must be PNG.

UI scale is available in Settings at 60%, 75%, 100% and 120%. Drag knobs vertically, scroll or use arrow keys, and double-click to reset a control. The five workflow tabs highlight the relevant sections. Mastering mode buttons load character/EQ/compression presets; **Custom** preserves your settings. The pictured sample-rate list is a read-only display of the host rate; change sample rate and audio buffer size in Mixcraft's audio preferences.

## Working audio controls

- Input/output gain, monophonic key/scale-aware pitch correction, five EQ bands at 60 Hz / 250 Hz / 1.2 kHz / 5 kHz / 12 kHz.
- Stereo-linked compression, frequency-selective de-essing and sample-peak output limiter.
- Stereo width, comb-tank reverb, host-tempo delay, modulated flanger.
- Heat, three distortion shapes, three warmth curves, transient dynamics and three saturation characters.
- Clipper, threshold/release noise gate, maximizer gain, parallel squashing and mid/side imaging.
- Low-end, punch, clarity, width, harmonics and air character controls, eight mastering presets plus Custom.
- Every audio knob, selector, power switch and global bypass is host-automatable and recalled with the session. UI scale and optional faceplate path are recalled too. Waveform and meters display actual processed audio.

Pitch correction is an original YIN detector plus dual-grain pitch shifter for monophonic vocals, not a commercial Auto-Tune algorithm. Its pitch and bypass paths have 2048 samples of reported latency (42.7 ms at 48 kHz), which Mixcraft can compensate. It can produce grain or modulation artifacts on polyphonic or transient-heavy sources. It does not preserve formants. Set correction to zero for mastering full mixes. Nonlinear colour stages do not oversample; high drive can alias. The limiter catches sample peaks, not intersample true peaks; it is not a loudness-certified mastering limiter. No processing can be inferred precisely from a photograph alone.

## Build and test

This target is built from the repository root, alongside the existing TG project. Requires JUCE 8.0.12, Visual Studio 2022 C++ tools and CMake 3.22+ on Windows:

```powershell
git clone --depth 1 --branch 8.0.12 https://github.com/juce-framework/JUCE.git ../dependencies/JUCE
cmake -S . -B build -A x64 -DJUCE_PATH=../dependencies/JUCE
cmake --build build --config Release --target VocaMAX_VST3 vocamax_dsp_tests vocamax_processor_tests --parallel 4
ctest --test-dir build -C Release -R VocaMAX_ --output-on-failure
```

Windows VST3 output: `build/VocaMAX/VocaMAX_artefacts/Release/VST3/VocaMAX.vst3`.

For the prepared Linux cloud environment run `bash scripts/cloud-build.sh`. Linux checks native DSP, state recall, VST3 factory loading and editor rendering. It cannot establish Mixcraft compatibility. The dedicated GitHub Windows workflow builds with MSVC, runs DSP/state/editor checks and packages a per-user installer. JUCE does not support MinGW for this project.

JUCE is used under its applicable AGPL/commercial license; see the pinned JUCE checkout's LICENSE.md before distribution. No JUCE license is purchased or supplied by this project.
