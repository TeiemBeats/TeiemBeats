# VocaMAX validation

Current-instance checks on Linux x86_64:

- Release VST3 built with JUCE 8.0.12 / GCC 14.
- 54 DSP checks pass across 44.1, 48 and 96 kHz: compensated bypass latency, gain, EQ, mono fold, compression, noise gate, output ceiling, pitch detection, tempo delay, every effect and all six character controls, and a 230 Hz → approximately 220 Hz monophonic pitch-correction check.
- 65 host parameters registered; input and choice state serialize and restore; factory preset and reset values checked; processed output is nonzero.
- VST3 module loads and enumerates its factory classes.
- Native editor renders at 1152×768 and resizes to 1536×1024. The input knob updates the host parameter. Preview is an actual editor capture driven by a test signal.
- Existing TG tests remain passing (six CTest suites pass across both products).

The dedicated Windows workflow builds using MSVC, tests DSP/state/editor, and packages the Windows VST3 with an install script. Its result must be checked on GitHub; this cloud environment cannot query the GitHub Actions API. Mixcraft loading and audio playback have not been tested here.

Visual limitation: the inline reference has not been supplied as a filesystem image asset. The current interface is a drawn recreation. Exact reference artwork can be loaded from PNG/JPG in Settings, or embedded by adding Assets/faceplate.png and rebuilding. The original image is needed to verify that appearance precisely. No claim of pixel-exact rendering or proprietary audio equivalence is made.
