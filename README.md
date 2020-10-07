# Duel 6 Reloaded

## Description
Duel 6 Reloaded is a **cross-platform open-source last-man-standing 2D multi-player game**. The game currently supports up to **15 players** that can compete in **26 arenas**. The arenas consist of solid blocks, moving platforms and water sections. Players are able to move through the environment using walking, jumping and crouching and use weapons to eliminate their opponents. There is a diverse set of **12 available weapons** that differ in shot and reload speed, shot power and splash damage. In addition players can pick up various bonuses to boost their stamina and other characteristics. The number of rounds can be specified and the game collects player **statistics** during the gameplay such as number of wins, kills or accuracy. There are negative points for comitting suicide and drowning that discourage players from performing kamikaze attacks. The game features **customizable player profiles** and **level editor**.

## History

The very first version of the game has been developed in **2002** by [Ondrej Danek](http://www.ondrej-danek.net/en/blog) as a sequel to **Duel 5** which was popular 2D deathmatch game published in a Czech computer magazine **Level**. Since then the game code has been continually improved and later open-sourced.

## Technical overview

The game is written using the following technologies:
- C++ with C++11, C++14, C++17 features
- [SDL 2](www.libsdl.org)
- OpenGL (with a stable OpenGL 1 render backend and experimental OpenGL ES 2 and OpenGL 4 backends)
- [CMake](www.cmake.org)

There is a **builtin quake-style console** that can be used to modify game aspects at runtime.

## Supported platforms

The game has been tested on the following platforms:
- **MS Windows** with Visual Studio 2017 and MinGW 64 compilers
- **Linux** with GCC 7.1 compiler
- **Mac OS X** with LLVM/Clang compiler

Recommended IDE:
- **CLion** - Duel 6 Reloaded is supported by [JetBrains](https://www.jetbrains.com/?from=Duel6Reloaded)

## Customization

The game offers several customization options

### Player profiles

Each player has its own set of **skin colors** and **sounds** for various game situations. These can be configured through **JSON files** located in player directory.

### Level editor

Levels are saved in JSON format and there is an [HTML5 level editor](https://github.com/odanek/duel6r-editor) available in my GitHub repository that can be used to create new levels and modify the existing ones.

### Scripting

The game has built-in [Lua](https://www.lua.org/home.html) scripting. More information about the API can be found in **lua-scripting.txt**.

## Command line arguments

`-window  WxH`

sets windowed mode with defined width (W) and height (H) (e.g -window 1920x1080)

`-dedicated`

starts the server in dedicated mode (useful in combination with headless renderer)

`-publish -url http://localhost/api/duel6r-gsi -port 3000`

turns on game state integration - game will periodically POST data to the given URL in JSON format

format of the data:

when the client is still in menu:
```
{
	"state": "MENU"
}
```

when the game is running:

```
{
"state":"GAME",
"rounds":0,
"maxRounds":0,
"roundLimit":false,
"waterRising":false,
"players":[{
	"name":"Player1",
	"team":0,
	"ping":0,
	"reloadTime":0,
	"reloadInterval":0.66,
	"alive":true,
	"timeSinceHit":81.4201,
	"health":100,
	"air":200,
	"points":0,
	"kills":0,
	"deaths":0,
	"bonus":"invisibility",
	"bonusRemainingTime":9.45583,
	"ammo":14,
	"weapon":"machine gun"
	},{
	"name":"Player2",
	"team":0,
	"ping":0,
	"reloadTime":0,
	"reloadInterval":1.31,
	"alive":true,
	"timeSinceHit":2.42222,
	"health":75,
	"air":200,
	"points":0,
	"kills":0,
	"deaths":0,
	"bonus":"none",
	"bonusRemainingTime":0,
	"ammo":12,
	"weapon":"slime"},{ .... etc ...}]
}
```

## Future plans and milestones

- Computer opponents/bots - AI
- Network play
- Rewriting the remaining parts of original C code with global functions and variables using modern object-oriented code
- Providing extension API for weapons, players and other game attributes
- Collision detection and physics

## Contact

- Ondrej Danek - [ondrej.danek@gmail.com](mailto:ondrej.danek@gmail.com)

## License

Duel 6 Reloaded is [BSD licensed](https://github.com/odanek/duel6r/blob/master/LICENSE).
