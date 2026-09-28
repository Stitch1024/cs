# Notes & Pitfalls

Knowledge collected while building the facade variants step by step. These are
**transferable** and mostly recur across variants, so they live here rather than
inside a single demo.

Legend: things marked *(hit)* are mistakes we actually made and fixed.

---

## 1. C Language

### Headers

- **A header is an interface**: types + function *declarations* only — no `{ }`
  bodies, no global variables. Implementation goes in the `.c`.
- **Include guard** — every header:
  ```c
  #ifndef X_H
  #define X_H
  /* ... */
  #endif /* X_H */
  ```
  Prevents double inclusion (which would redefine the struct). `#pragma once`
  is a shorter non-standard alternative.
- **Declaration vs definition**: the `.h` *promises* (`void foo(T *x);`), the
  `.c` *fulfills* it with the exact same signature. Signature mismatch →
  *conflicting types* / *undefined reference*.
- **`#include "my.h"`** (quotes) for your own project headers, **`<stdio.h>`**
  (angle brackets) for system headers.
- **Include your own header first** in the `.c`. The compiler then checks your
  definitions against the declarations — free error-catching.
- **`static`** on a function = file-local. Never mark public API functions
  `static`, or the linker can't find them.

### Pointers

- **`->` vs `.`**: `->` on a **pointer** to a struct, `.` on a **value**.
  *(hit)* `facade->projector` on a value `HomeTheaterFacade facade;` is a
  compile error — use `facade.projector`.
- **Pass a pointer to mutate**: `void projector_on(Projector *p)` can change the
  caller's object; `void projector_on(Projector p)` only changes a copy.
- **`const char *`** for read-only string parameters (`dvd_player_play(..., const char *title)`).

### Strings & buffers

- **Never `strcpy` into a fixed buffer.** *(hit)* `char title[64]` +
  `strcpy(d->title, src)` overflows if `src` is longer. Use:
  ```c
  snprintf(d->title, sizeof(d->title), "%s", title);
  ```
  `snprintf` truncates and always null-terminates.
- **Clear a buffer with `memset`** when you want a known-clean state:
  `memset(d->title, 0, sizeof(d->title));`.
- **`sizeof(array)` doesn't work once it decays to a pointer** — know whether
  you hold an array or a pointer.

### printf

- **Format specifier must match the argument type.** *(hit)*
  `printf("input = %d\n", source == 1 ? "DVD" : "HDMI")` — `%d` expects an
  `int`, got a `char *`. This is **undefined behavior**: may print garbage or
  crash. Use `%s`. `-Wall` warns about exactly this.

---

## 2. Build (CMake + Make)

- **The division of labor**:
  - **CMake** = generator (configure): reads `CMakeLists.txt` → writes a
    `Makefile`.
  - **make** = executor (build): reads the `Makefile` → runs `gcc`.
  - `build/Makefile` is *generated* — never edit it by hand.
- **Out-of-source build** keeps artifacts out of the source tree:
  ```bash
  cmake -S . -B build -G "MinGW Makefiles"   # configure
  cmake --build build                        # = runs make
  ```
- **`cmake --build build` is just make.** To *see* make:
  `mingw32-make -C build` (same thing), `mingw32-make -C build clean`.
- **Add `-Wall -Wextra` early**:
  ```cmake
  target_compile_options(<target> PRIVATE -Wall -Wextra)
  ```
  This is the first net that catches format-specifier bugs, unused variables,
  etc.
- **`.gitignore`**: at minimum `build/`.

---

## 3. Design & Methodology

- **Facade = simplify + orchestrate, not convert.** It adds a convenient path;
  it does not change subsystem behavior.
- **Two client paths** (classic GoF): via the facade, **or** directly to a
  subsystem. The client still owns the subsystems.
- **Ownership decides malloc**: classic facade → client owns → no `malloc` in
  the facade. The encapsulated variant flips this (facade owns → `create` /
  `destroy`).
- **State guards create implicit ordering dependencies.** Because
  `amplifier_set_volume` is guarded by `if (a->is_on)`, you **must** call
  `amplifier_on` first or it silently does nothing. Concentrating this order in
  one place is exactly what a facade is for.
- **Ordering pitfalls are silent.** *(hit)* In `main.c`, `dvd_player_off`
  before `dvd_player_eject` made eject a no-op, because eject is guarded by
  `if (d->is_on)`. Wrong order = missing output, no crash — so always verify
  against expected output.
- **Keep state self-consistent.** *(hit)* `dvd_player_off` set `is_on = 0` but
  left `is_playing = 1`, so "off while playing" left a stale flag. Reset
  related fields together.
- **Same scenario, two variants** makes the trade-off visible: compare
  `pure-facade/` (bypass allowed) against `encapsulated-facade/` (facade-only).
