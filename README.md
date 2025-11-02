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

### Start

```
roundframe

```

### Reload

```
killall -USR1 roundframe
```

## Config

Configuration is read from `~/.config/roundframe/config.ini`. Example:

```ini
[frame]
name=roundframe
unique_names=true
layer=top
monitors=

[top]
thickness=0
exclusive=true

[bottom]
thickness=4
exclusive=true

[left]
thickness=4
exclusive=true

[right]
thickness=4
exclusive=true

[top-left]
radius=12
exclusive=false

[top-right]
radius=12
exclusive=false

[bottom-left]
radius=12
exclusive=false

[bottom-right]
radius=12
exclusive=false
```

GtkCSS styling is read from `~/.config/roundframe/style.css`. Example:

```css
* {
  transition: all 0.5s ease;
}

.top {
  background: alpha(@theme_bg_color, 0.75);
}

.bottom {
  background: alpha(@theme_bg_color, 0.75);
}

.left {
  background: alpha(@theme_bg_color, 0.75);
}

.right {
  background: alpha(@theme_bg_color, 0.75);
}

.top-left {
  background: alpha(@theme_bg_color, 0.75);
}

.top-right {
  background: alpha(@theme_bg_color, 0.75);
}

.bottom-left {
  background: alpha(@theme_bg_color, 0.75);
}

.bottom-right {
  background: alpha(@theme_bg_color, 0.75);
}
```
