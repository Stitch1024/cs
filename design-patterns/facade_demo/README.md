# Facade Pattern

## 1. What Is the Facade Pattern?

The **Facade** pattern provides a **single, simplified interface** to a set of
interfaces in a subsystem. It wraps a group of related operations behind one
convenient entry point so that clients do not have to deal with the subsystem's
complexity directly.

> **Intent:** Provide a unified, higher-level interface to a set of interfaces
> in a subsystem. Facade makes the subsystem easier to use.

Key points:

- A facade **does not add new behavior**. It only **orchestrates** and
  **simplifies** calls to existing subsystems.
- A facade **does not forbid** direct access to the subsystem. It is an
  additional convenient path, not a wall. (You *may* choose to hide the
  subsystem to enforce encapsulation, but that is a design decision, not a
  requirement of the pattern.)
- A facade is usually **stateless** with respect to the flow it wraps — it holds
  references to the subsystems and calls them in the right order.

### Participants

| Role           | Responsibility                                                        |
| -------------- | --------------------------------------------------------------------- |
| **Facade**     | Knows which subsystem objects handle a request and calls them in order. |
| **Subsystems** | Do the real work. They have no knowledge of the facade.                |
| **Client**     | Calls the facade for the common case; may still call a subsystem directly. |

```
                              +-----------------------+
   Client ------------------>|        Facade         |
      |                       |  watch_movie()        |
      |                       |  end_movie()          |
      |                       +-----------+-----------+
      |                                   |  delegates / orchestrates
      |            +----------------------+-------------------+
      |            v                      v                   v
      |      +-----------+          +-----------+       +-----------+
      |      | Projector |          | Amplifier |       | DVDPlayer |
      |      +-----------+          +-----------+       +-----------+
      |            ^                      ^                   ^
      |            |                      |                   |
      |            +---------- optional direct calls --------+
      |                          (bypass the facade)
      |
      +---- The client keeps TWO paths: via the facade, or direct to a subsystem

       Subsystems (unaware of the facade)
```

### Two Client Paths: Bypassing the Facade Is Allowed

The optional path above is the part most people forget. The classic GoF facade
**does not seal off the subsystem** — it adds a convenient shortcut, it does not
remove the original road. So a client always has *two* possible paths:

1. **Via the facade** — the simple, intent-level path, fine for the common case.
2. **Directly to a subsystem** — the detailed path, for the remaining cases the
   facade does not cover.

Whether path 2 is actually *reachable* depends on a design choice, not on the
pattern itself:

| Choice | Client-visible paths | Result |
| ------ | -------------------- | ------ |
| **Subsystems public** (classic GoF) | facade **+** direct calls | Facade gives a convenient default for ~80% of cases; power users can still drop down to subsystem details for the other 20%. |
| **Subsystems hidden** (public header exposes only the facade; subsystem headers kept internal/private) | **facade only** | The facade becomes the single entry point — stronger encapsulation, but no bypass. |

Both are valid facades; they differ only in **whether bypass is allowed**. In the
classic form, allowing bypass is a *feature*: the facade provides a sensible
default while experts retain access to the details.

> Note: hiding the subsystems is really "Facade **+ enforced encapsulation**",
> a stronger variant — not the *minimal* facade. The minimal facade adds a
> shortcut and leaves the subsystem as it was.

---

## 2. Why Use the Facade?

### The problem without a facade

A client that needs to perform a multi-step task must know:

- **which** subsystems exist,
- **in what order** to call them,
- and **how** each one is configured.

That is a lot of knowledge leaking into the client. Every subsystem change can
break the client, and the same sequence gets duplicated everywhere it is needed.

```c
/* Without a facade: the client must know every detail and correct order */
projector_on(&projector);
amplifier_on(&amp);
amplifier_set_volume(&amp, 5);
amplifier_set_surround_sound(&amp, 1);
projector_set_input(&projector, SOURCE_DVD);
dvd_player_on(&dvd);
dvd_player_play(&dvd, "Inception");
```

### The same task with a facade

```c
/* With a facade: the client expresses intent, not mechanics */
home_theater_watch_movie(&theater, "Inception");
```

### Benefits

| Benefit                     | Explanation                                                        |
| --------------------------- | ------------------------------------------------------------------ |
| **Simplified interface**    | One call replaces many. Clients express *what*, not *how*.         |
| **Reduced coupling**        | Clients depend on the facade, not on concrete subsystems.          |
| **Centralized knowledge**   | The correct order/configuration lives in one place, not everywhere.|
| **Easier maintenance**      | Subsystem changes are absorbed by the facade.                      |
| **Easier testing**          | Clients can be tested against a simple seam.                       |
| **Layering**                | Facades define clean boundaries between architectural layers.      |

---

## 3. How to Use the Facade

### Step 1 — Identify the subsystem

List the classes/modules that clients must talk to in order to accomplish a
common task. (Here: `Projector`, `Amplifier`, `DVDPlayer`.)

### Step 2 — Identify the common tasks

Find the workflows clients repeat. Each workflow becomes a facade method.
(Here: `watch_movie`, `end_movie`, `listen_to_music`.)

### Step 3 — Define the facade interface

Expose **intent-level** methods, not subsystem-level ones. The facade method
should be named after the goal (`watch_movie`), not the steps
(`set_volume`).

### Step 4 — Delegate inside the facade

Each facade method calls the subsystem objects in the correct order, handling
configuration and sequencing.

### Step 5 — Keep the subsystem unaware

Subsystems must not reference the facade. Dependencies point **from** the
facade **to** the subsystems, never back.

### In C

Because C has no classes, a facade is typically modeled as:

- a **struct** holding references (or instances) of the subsystems, and
- a set of **functions** operating on that struct.

A good C facade keeps its internals private by making the facade struct
**opaque** (declared as an incomplete type in the public header, defined only in
the `.c` file). That way clients can use the facade but cannot reach into its
subsystem fields.

```c
/* facade.h — public */
typedef struct HomeTheater HomeTheater;               /* opaque */

HomeTheater *home_theater_create(void);
void         home_theater_destroy(HomeTheater *ht);
void         home_theater_watch_movie(HomeTheater *ht, const char *title);
```

```c
/* facade.c — private: the real struct is hidden here */
struct HomeTheater {
    Projector projector;
    Amplifier amplifier;
    DvdPlayer dvd;
};

void home_theater_watch_movie(HomeTheater *ht, const char *title) {
    projector_on(&ht->projector);
    amplifier_on(&ht->amplifier);
    amplifier_set_volume(&ht->amplifier, 5);
    projector_set_input(&ht->projector, SOURCE_DVD);
    dvd_player_on(&ht->dvd);
    dvd_player_play(&ht->dvd, title);
}
```

---

## 4. When to Use It (Scenarios)

Use the Facade pattern when:

- **A subsystem is complex** and clients only need a few high-level operations
  out of it (compilers, media frameworks, network stacks).
- **A task requires multiple steps in a fixed order** that clients should not
  have to remember.
- **You want to decouple layers** — e.g. a UI layer talking to a service layer
  through a single entry point.
- **You are wrapping a legacy or third-party API** to give your codebase a
  cleaner, project-specific interface.
- **You want to provide a coarse-grained API** on top of fine-grained
  components.
- **You want a stable seam for testing**, so clients can be swapped onto a
  simplified or mocked facade.

### Concrete everyday examples

- A **home theater** facade: `watch_movie()` instead of operating every device.
- A **compiler** facade: `compile()` instead of invoking parser, analyzer,
  optimizer, and code generator separately.
- A **network client** facade: `fetch(url)` instead of socket, TLS, HTTP, and
  retry handling.
- A **build tool** facade: `build()` instead of resolving dependencies,
  configuring, compiling, and linking by hand.

---

## 5. When NOT to Use It

- The subsystem is **already simple** — a facade adds a layer for no benefit.
- You actually need to **convert one interface into another** → that is
  **Adapter**, not Facade.
- You need to **add responsibilities** to an object without changing its
  interface → that is **Decorator**.
- You need to **control access** to an object → that is **Proxy**.
- You need to **centralize many-to-many communication** between peer objects →
  that is **Mediator**.
- The facade keeps growing until it knows about everything → it has become a
  **God object**; split it by use case.

---

## 6. Facade vs. Related Patterns

| Pattern          | Difference from Facade                                                        |
| ---------------- | ----------------------------------------------------------------------------- |
| **Adapter**      | Converts an interface into a *different* one. Facade *simplifies* an existing one. |
| **Decorator**    | Adds behavior while keeping the same interface. Facade adds no behavior.       |
| **Proxy**        | Same interface, controls access. Facade offers a *new, simpler* interface.     |
| **Mediator**     | Adds new behavior to coordinate peers. Facade only simplifies subsystem use.   |
| **Abstract Factory** | Creates families of objects. Facade merely uses existing ones.             |
| **Singleton**    | Often a facade is a suitable singleton, but that is independent of the pattern. |

---

## 7. Two Implementations in This Repository

This repository ships the **same home-theater scenario twice**, to contrast the
two facets of the pattern described in [Section 1](#1-what-is-the-facade-pattern):
the classic facade, and the facade-with-enforced-encapsulation.

| | [`pure-facade/`](./pure-facade/) | [`encapsulated-facade/`](./encapsulated-facade/) |
| --- | --- | --- |
| Style | Classic GoF facade | Facade **+ enforced encapsulation** |
| Client-visible paths | **Two** — via facade *and* direct to subsystems | **One** — facade only |
| Who owns the subsystems | The **client** creates them and passes them in | The **facade** creates and owns them |
| Subsystem headers | Public | Hidden in `internal/` |
| Facade struct | May be transparent | Fully **opaque** (defined only in the `.c`) |
| `main.c` shows | (a) the facade path, then (b) direct subsystem calls | the facade path only; any bypass fails to compile |
| Status | **Done** | TODO |

Both are facades; they differ only in **whether bypass is allowed**. The two
variants make each side of that trade-off concrete:

- **`pure-facade/`** answers *"how does a facade simplify things while leaving
  the subsystem open?"*
- **`encapsulated-facade/`** answers *"what does it look like when the facade
  is also the only door?"*

```
facade_demo/
├── README.md                  ← this file (pattern theory)
├── pure-facade/               ← classic: two paths (facade + direct)
└── encapsulated-facade/       ← facade only; bypass impossible
```

> Build instructions, sample output, and the exact call flow for each variant
> live in that variant's own `README.md`.

---

## 8. Summary

- **What:** A single simplified interface over a complex subsystem.
- **Why:** Reduce coupling, centralize knowledge, simplify client code.
- **How:** Wrap subsystem objects, expose intent-level methods, delegate inside.
- **When:** Complex subsystems, multi-step workflows, layer boundaries, legacy
  wrappers, coarse-grained APIs.

---

## References

- [Facade — Refactoring Guru](https://refactoring.guru/design-patterns/facade)
- *Design Patterns: Elements of Reusable Object-Oriented Software* (GoF), 1994
- *Head First Design Patterns* — the Home Theater example
