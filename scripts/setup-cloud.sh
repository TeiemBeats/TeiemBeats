#!/usr/bin/env bash
set -euo pipefail
# Use the existing isolated task checkout. Do not create a worktree.
mkdir -p /workspace/tools /workspace/dependencies /workspace/sysroot /workspace/apt/lists/partial /workspace/apt/cache/archives/partial /workspace/apt/empty
if [[ ! -x /workspace/tools/cmake/data/bin/cmake ]]; then
 python -m pip install --target /workspace/tools cmake==4.4.4
fi
if [[ ! -d /workspace/dependencies/JUCE/.git ]]; then
 git clone --depth 1 --branch 8.0.12 https://github.com/juce-framework/JUCE.git /workspace/dependencies/JUCE
fi
[[ $(git -C /workspace/dependencies/JUCE rev-parse HEAD) == 29396c22c93392d6738e021b83196283d6e4d850 ]] || { echo 'JUCE revision differs; preserve and inspect it before proceeding.' >&2; exit 1; }
printf '%s\n' 'deb [signed-by=/usr/share/keyrings/debian-archive-keyring.gpg] https://deb.debian.org/debian trixie main' > /workspace/apt/sources.list
apt_options=(-o Dir::Etc::Parts=/workspace/apt/empty -o Dir::Etc::sourcelist=/workspace/apt/sources.list -o Dir::Etc::sourceparts=/workspace/apt/empty -o Dir::State::lists=/workspace/apt/lists -o Dir::Cache::archives=/workspace/apt/cache/archives -o Debug::NoLocking=1)
APT_CONFIG=/dev/null apt-get "${apt_options[@]}" update
APT_CONFIG=/dev/null apt-get "${apt_options[@]}" --download-only --no-install-recommends -y install libx11-dev libxext-dev libxinerama-dev libxrandr-dev libxcursor-dev libfreetype6-dev libfontconfig1-dev libasound2-dev xvfb
for package in /workspace/apt/cache/archives/*.deb; do
 [[ $package == *mingw* ]] && continue
 dpkg-deb -x "$package" /workspace/sysroot
done
python - <<'PY'
from pathlib import Path
root=Path('/workspace/sysroot/usr/lib/x86_64-linux-gnu')
for file in list(root.iterdir()):
 if file.is_symlink() and not file.exists():
  target=root/file.readlink().name
  runtime=Path('/usr/lib/x86_64-linux-gnu')/file.readlink().name
  if runtime.exists() and not target.exists():target.symlink_to(runtime)
PY
bash /workspace/TeiemBeats/scripts/cloud-build.sh
