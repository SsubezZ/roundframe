# RoundFrame

A GTK-based application that creates rounded screen frames for multiple monitors using GTK Layer Shell, with configuration via a config.ini file and CSS styling.

<img width="1920" height="1080" alt="frame_light" src="https://github.com/user-attachments/assets/f39d2bc2-9180-4f95-b2d4-c9c98d5e0f9c" />
<img width="1920" height="1080" alt="frame_dark" src="https://github.com/user-attachments/assets/c1de54dc-978c-48e5-a895-780f4631a242" />

## Prerequisites

- GTK+ 3.0
- GTK Layer Shell
- Cairo
- GIO 2.0

## Build Instructions

1. Install dependencies (`gtk3 gtk-layer-shell cairo glib`).
2. Clone the repo `git clone https://github.com/SsubezZ/roundframe.git`.
3. Run `make` to build the project.
4. Run `./roundframe` to execute the application.

## Usage

```
roundframe [OPTIONS]
Options:
  -l, --log    Enable logging
  -h, --help   Show this help message

```

Configuration is read from `~/.config/roundframe/config.ini`. Example:

```ini
[Layer]
type=BOTTOM
name=roundframe
keyboard_mode=none

[Layout]
gap_left=5
gap_right=5
gap_top=5
gap_bottom=5
inner_radius=12.0
exclusive=false
```

CSS styling is read from `~/.config/roundframe/style.css`. Example:

```css
.roundframe {
  background-color: @theme_bg_color;
  border: 2px solid @theme_fg_color;
}
```
