# RoundFrame

A GTK-based application that creates rounded screen frames for multiple monitors using GTK Layer Shell.
<img width="1920" height="1080" alt="frame_light" src="https://github.com/user-attachments/assets/a3dae921-c29b-4b8c-99dd-271856f7b2e4" />
<img width="1920" height="1080" alt="frame_dark" src="https://github.com/user-attachments/assets/aeecf071-fbbd-4b04-aced-cfe0bf837982" />

## Prerequisites

- GTK+ 3.0
- GTK Layer Shell
- Cairo
- GIO 2.0

## Build Instructions

```
git clone https://github.com/SsubezZ/roundframe.git
make
./roundframe
```

## Usage

```
./roundframe [options]
```

Options:

- `-r, --radius <N>`: Corner radius (default: 12)
- `-g, --gap <GAP>`: Gap from screen edges (1 or 4 values: top right bottom left)
- `-b, --border <N>`: Border width (default: 0)
- `-a, --alpha <ALPHA>`: Alpha values (1 or 2 values: [light] [dark])
- `-l, --layer <TYPE>`: Layer type: top, overlay, bottom, background (default: bottom)
- `--log`: Enable debug logging
