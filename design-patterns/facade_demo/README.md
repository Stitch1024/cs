# Facade Pattern - C Demo

## File Structure
```
facade_demo/
├── include/                 # Public API (client only includes this)
│   └── home_theater_facade.h
├── internal/                # Internal headers (not exposed to client)
│   ├── projector.h
│   ├── amplifier.h
│   └── dvd_player.h
├── src/                     # Implementation files
│   ├── projector.c
│   ├── amplifier.c
│   ├── dvd_player.c
│   └── home_theater_facade.c
├── main.c                   # Demo program
└── CMakeLists.txt           # CMake build configuration
```

## Build

### Using CMake (Recommended)
```bash
# Configure (generates build files)
cmake -S . -B build

# Build
cmake --build build

# Run
./build/facade_demo.exe
```

### Using GCC (Manual)
```bash
gcc -Iinclude -Iinternal -o facade_demo.exe main.c src/*.c
```

### CMakeLists.txt Explanation
```cmake
cmake_minimum_required(VERSION 3.10)
project(facade_demo LANGUAGES C)

add_executable(facade_demo
    main.c
    src/home_theater_facade.c
    src/projector.c
    src/amplifier.c
    src/dvd_player.c
)

target_include_directories(facade_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include    # Public API
    ${CMAKE_SOURCE_DIR}/internal   # Internal headers
)
```

| Command | Purpose |
|---------|---------|
| `cmake_minimum_required` | Required CMake version |
| `project` | Project name and language |
| `add_executable` | Source files to compile |
| `target_include_directories` | Header search paths |

## Run
```bash
./build/facade_demo.exe
```

## Sample Output
```
===== Facade Pattern Demo =====

1. Watch movie using facade:
=== Get ready to watch a movie... ===
Projector: warming up...
Amplifier: powering on
Amplifier: volume set to 5
Amplifier: surround sound enabled
Projector: input set to DVD
Projector: wide screen mode enabled
DVD Player: powering on
DVD Player: playing 'Inception'
=== Enjoy the movie! ===

2. End movie using facade:
=== Shutting movie theater down... ===
DVD Player: stopped
DVD Player: ejecting disc
Amplifier: powering off
Projector: cooling down...
=== Goodbye! ===
```

## Code Explanation

### Directory Purpose
| Directory | Purpose | Client Access |
|-----------|---------|---------------|
| `include/` | Public API | Yes |
| `internal/` | Internal headers | No (hidden from client) |
| `src/` | Implementation | No |

### Subsystems (Internal - Not Exposed)
- Projector: `projector_on()`, `projector_off()`, `projector_set_input()`, `projector_wide_screen_mode()`
- Amplifier: `amplifier_on()`, `amplifier_off()`, `amplifier_set_volume()`, `amplifier_set_surround_sound()`
- DVD Player: `dvd_player_on()`, `dvd_player_off()`, `dvd_player_play()`, `dvd_player_stop()`, `dvd_player_eject()`

### Facade (Public API)
- `home_theater_facade_watch_movie()`: One-click movie watching (calls all subsystems)
- `home_theater_facade_end_movie()`: One-click end movie
- `home_theater_facade_listen_to_music()`: One-click music listening
- `home_theater_facade_stop_music()`: One-click stop music

### How It Works
1. `main.c` only includes `home_theater_facade.h`
2. Subsystem headers are in `internal/`, hidden from client
3. Client code cannot call `projector_on()` etc. - compiler error
4. All subsystem operations go through facade only

## Facade Pattern Benefits
1. **Simplified Interface**: Client only calls one function instead of multiple subsystem functions
2. **Reduced Coupling**: Client does not depend on concrete subsystems
3. **Improved Maintainability**: Subsystem changes do not affect client code
4. **Enforced Encapsulation**: Directory structure enforces access control at compile time
