# Pure Facade (Classic GoF)

This variant shows the **classic GoF facade**: the facade adds a convenient,
intent-level shortcut over a set of subsystems, but **does not seal them off**.
The client keeps **two paths** — through the facade, or straight to a subsystem.

This README is also a guided **build walkthrough**: implement it yourself while
learning **CMake + Make**.

---

## 1. Design Decisions

| Decision | Value | Why |
| -------- | ----- | --- |
| Who creates the subsystems | The **client (`main`)** | Classic GoF: the facade only holds references and orchestrates. |
| Does the facade allocate memory | **No** | The client passes in pointers → that is what makes the "two paths" demo natural. |
| Subsystem headers | **Public** (`include/`) | The client must be able to call them directly. |
| Facade struct | **Transparent is fine** | This variant does *not* enforce encapsulation. |
| Demo focus | Use the facade, then bypass it | Mirrors the "two paths" idea in `../README.md` §1. |

---

## 2. Directory Layout

```
pure-facade/
├── include/
│   ├── projector.h
│   ├── amplifier.h
│   ├── dvd_player.h
│   └── home_theater_facade.h
├── src/
│   ├── projector.c
│   ├── amplifier.c
│   ├── dvd_player.c
│   └── home_theater_facade.c
├── main.c
├── CMakeLists.txt
├── .gitignore          # build/
└── README.md           # this file
```

---

## 3. Interface Contracts

Write the implementations yourself; these are the contracts they must satisfy.

### Subsystems

Each subsystem is a `struct` plus init/operation functions carrying some state
(`is_on`, etc.):

- `projector.h` — `Projector { int is_on; int input_source; }`,
  `projector_init / on / off / set_input / wide_screen_mode`
- `amplifier.h` — `Amplifier { int is_on; int volume; int surround; }`,
  `amplifier_init / on / off / set_volume / set_surround_sound`
- `dvd_player.h` — `DvdPlayer { int is_on; int is_playing; char title[64]; }`,
  `dvd_player_init / on / off / play / stop / eject`

### Facade

Only holds pointers, does **not** own the subsystems:

```c
/* home_theater_facade.h */
typedef struct {
    Projector *projector;
    Amplifier *amplifier;
    DvdPlayer *dvd_player;
} HomeTheaterFacade;

void home_theater_facade_init(HomeTheaterFacade *f,
                              Projector *p, Amplifier *a, DvdPlayer *d);
void home_theater_facade_watch_movie(HomeTheaterFacade *f, const char *title);
void home_theater_facade_end_movie(HomeTheaterFacade *f);
void home_theater_facade_listen_to_music(HomeTheaterFacade *f, const char *album);
void home_theater_facade_stop_music(HomeTheaterFacade *f);
```

> Note: the classic variant uses `init`, not `create` — **no malloc**, the
> objects can live on the stack.

---

## 4. Implementation Steps (in order)

1. **Three subsystem headers** — write the contracts first: types + function
   declarations only.
2. **Three subsystem implementations** — each operation `printf`s a log line and
   has a simple state guard (e.g. `amplifier_set_volume` only takes effect when
   `is_on`).
3. **Facade header** — as above; it `#include`s the three subsystem headers.
4. **Facade implementation** — each method calls the subsystems in the right
   order. Suggested order for `watch_movie`:
   `projector_on → amplifier_on → set_volume(5) → surround(1) →
   projector_set_input(DVD) → wide_screen → dvd_on → dvd_play(title)`.
5. **`main.c`** — two sections:
   - (a) through the facade: `init`, then `watch_movie` / `end_movie`;
   - (b) **bypassing the facade**: call `projector_off(&p)`,
     `amplifier_set_volume(&a, 2)`, etc. directly, proving the second path
     exists.
6. **`CMakeLists.txt`** — see §5.
7. **Build, run, and check the output.**
8. **Record the result** — once it runs, paste the actual output into a
   `## Sample Output` section of this README.

---

## 5. Build with CMake + Make

`CMakeLists.txt` skeleton:

```cmake
cmake_minimum_required(VERSION 3.10)
project(pure_facade LANGUAGES C)
add_executable(pure_facade
    main.c
    src/projector.c
    src/amplifier.c
    src/dvd_player.c
    src/home_theater_facade.c
)
target_include_directories(pure_facade PRIVATE ${CMAKE_SOURCE_DIR}/include)
```

**Mental model (hold on to this and CMake becomes easy):**

```
CMake = generator (configure): reads CMakeLists.txt → generates a Makefile
make  = executor  (build):     reads the Makefile   → runs gcc to compile/link
```

**Commands (in the MSYS2 UCRT64 terminal, from `pure-facade/`):**

```bash
cmake -S . -B build -G "MinGW Makefiles"   # configure: generates build/Makefile
cmake --build build                        # build: this is really just running make
./build/pure_facade.exe                    # run
```

**To actually "see" make:**

```bash
mingw32-make -C build            # call make directly; same as `cmake --build build`
mingw32-make -C build clean      # clean target
cmake --build build --target clean
```

To go deeper: `build/Makefile` is *generated* by CMake (don't edit it by hand),
and `build/CMakeCache.txt` is a configure-time artifact. That division of labor
is the whole point of CMake vs. Makefile.

---

## 6. Acceptance Criteria

After running, the program should print **two sections**:

- one where the facade does the whole chain (projector → amplifier → DVD,
  setting volume/surround, playing the movie);
- one where `main` talks to the subsystems **directly**, proving bypass works.

Each subsystem function should only produce output when its device `is_on` —
use this to check that your state guards are correct.

---

## 7. Sample Output

The two paths produce the **same effect** — same subsystems, same order, same
result — which is the whole point: the facade adds convenience, not behavior.
Path B also shows the client can drive the subsystems itself.

```
--- Path A: through the facade ---
Projector: on
Projector: input = DVD
Projector: widescreen = on
Amplifier: on
Amplifier: volume = 5
Amplifier: surround sound = on
DVD Player: on
DVD Player: playing "Inception"
DVD Player: stopped "Inception"
DVD Player: ejected
DVD Player: off
Amplifier: off
Projector: off

--- Path B: direct calls to subsystems ---
Projector: on
Projector: input = DVD
Projector: widescreen = on
Amplifier: on
Amplifier: volume = 5
Amplifier: surround sound = on
DVD Player: on
DVD Player: playing "dvd title"
DVD Player: stopped "dvd title"
DVD Player: ejected
DVD Player: off
Amplifier: off
Projector: off
```

---

## 8. Notes

- `.gitignore` should contain `build/`.
- Keep the facade free of `malloc` in this variant; ownership stays with the
  client.
