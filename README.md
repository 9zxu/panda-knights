# panda knights

A top-down 2D shooter written in C with Allegro 5. Play a panda knight, shoot slimes, collect coins, buy weapons in the shop, and kill every enemy to win.

## Getting Started

Tested on macOS (Apple Silicon) with Allegro 5.2.11 from Homebrew.

```sh
brew install allegro pkg-config
make run
```

`make run` builds into `build/`, copies the assets next to the binary, and starts the game from there. The game loads assets and writes `leaderboard.txt` and `log.txt` relative to the working directory, so run it from `build/`.

Other targets: `make clean` (objects and binary), `make distclean` (whole `build/`).

## How to play

- Menu: username must be shorter than 9 characters.
- Move: WASD by default. Custom keys are set in the setting scene by clicking a box.
- Weapon: press `1` to change the weapon.
- Settings: press `ESC` in game. Drag the sliders to change volume, click BACK to apply.
- Shop: click to buy. The price tag is yellow if affordable, red if not.
- Win: kill all the enemies.
- Leaderboard: ranked by coins + health - time.

## Help

Open an issue at https://github.com/9zxu/panda-knights/issues.

## License

All rights reserved, no use permitted. See [LICENSE](LICENSE).
