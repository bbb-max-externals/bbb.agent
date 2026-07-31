# bbb.agent — Max/MSP AI Agent Externals

## Project Status

Milestone 0 in progress. All six externals build on macOS and the helper service is implemented. `bbb.agent.hub` uses the reusable, thread-safe WebSocket transport in `source/bbb/`. `PLAN.md` is the authoritative product design document, but verify implementation claims against the current source.

## What This Is

Max/MSP external package that provides AI agent infrastructure for live performance. Six MVP externals: `bbb.agent.hub`, `bbb.agent.chat`, `bbb.agent.toolrouter`, `bbb.agent.state`, `bbb.agent.guard`, `bbb.agent.ping`. Communicates with a local helper service (Node.js/TypeScript) over WebSocket.

## Architecture

```
bbb.agent.hub ←→ helper service (WebSocket on localhost)
    ↕
bbb.agent.chat / bbb.agent.toolrouter / bbb.agent.state / bbb.agent.guard / bbb.agent.ping
```

All externals share a single WebSocket connection through `bbb.agent.hub`. Helper service (Node.js/TypeScript) handles LLM API calls, auth, streaming, schema validation, conversation history. API keys stored in helper's `.env` file. Max externals handle inlet/outlet, message conversion, scheduler safety. External never does network I/O on the main thread.

## Project Structure

```
source/bbb/
  └── bbb_agent_websocket_client.hpp  # reusable Max-independent transport
source/projects/<bbb.agent.xxx>/
  ├── <bbb.agent.xxx>.cpp
  ├── <bbb.agent.xxx>.maxhelp
  └── CMakeLists.txt        # calls bbb_add_external()
helper/                       # Node.js/TypeScript helper service
  ├── src/index.ts            # WebSocket echo server (Milestone 0)
  ├── package.json
  ├── tsconfig.json
  └── .env.example
```

Root CMakeLists.txt auto-scans `source/projects/` via `SUBDIRLIST` macro — adding a directory is enough, no root config edits needed.

## Skills

- **`/max-external`** — Create new C++ externals using min-api + CMake. Reads `templates/external.cpp` and applies placeholder substitution. See `.agents/skills/max-external/SKILL.md`.
- **`/max-patgen`** — Generate `.maxpat`/`.maxhelp` files as JSON. Has reference docs for Max/MSP objects and templates. See `.agents/skills/max-patgen/SKILL.md`.
- **`/max-external-githubactions`** — CI workflow template for building externals on macOS + Windows. See `.agents/skills/max-external-githubactions/SKILL.md`.

All are git submodules under `.agents/skills/`.

## Naming Convention

| Layer | Format | Example |
|---|---|---|
| Directory / filename | `bbb.xxx.yyy` (dots) | `bbb.osc.send` |
| C++ class / MIN_EXTERNAL | `bbb_xxx_yyy` (underscores) | `bbb_osc_send` |
| Build artifact | Same as directory + `.mxo` | `bbb.osc.send.mxo` |

`bbb_add_external()` handles the dot→underscore conversion automatically.

## External Build

```bash
cmake -B build
cmake --build build
```

Output: `externals/<name>.mxo`. Requires macOS, CMake 3.19+, Xcode CLI tools, min-api submodule at `deps/min-api/`.

## Helper Service

```bash
cd helper
npm install
npm run dev          # tsx for development (hot reload)
npm run build        # tsc → dist/
npm start            # node dist/index.js
```

Echo server on `ws://localhost:8765`. Responds to `ping` → `pong` with timestamp, echoes unknown messages. API keys in `.env` file.

## Key min-api Pitfalls

These bite every new external. Full list in `.agents/skills/max-external/docs/pitfalls.md`:

- **Attributes aren't set during constructor** — use `m_init_timer.delay(0)` to defer initialization
- **Outlet output is main-thread only** — worker threads must use `c74::min::queue<>`, not `output.send()`
- **`timer::delay()` from worker thread doesn't fire** — use `queue` instead
- **Use `enum_map` for int attribute dropdowns** — `range{"a","b"}` + `style::enum_index` causes "bad number" error
- **`cout`/`cerr` are member variables** — not `std::cout`
- **`std::filesystem` unavailable** — min-api sets deployment target to 10.11; use `c74::min::path` or string ops
- **`NIL` macro collision** — Max SDK defines `NIL` as `(void*)0`; conflicts with enum values in other libs
- **`m_maxobj` is private** — use `maxobj()` method instead
- **attribute setter must return `args`** — returning empty resets to default
- **`message` operator() is ambiguous with `{}` args** — use `message_obj(c74::min::atoms{}, -1)` instead of `message_obj({}, {})`
- **`std::map` etc. need explicit `#include <map>`** — `c74_min.h` doesn't transitively include STL containers

## Constraints

- API keys can be passed via `apikey` message to `bbb.agent.hub` (runtime only, never stored as attribute) or via helper's `.env` file as fallback
- Tool schemas use JSON Schema standard format; helper converts to provider-specific formats
- Patcher scripting is deny-by-default in guard
- No arbitrary shell execution from externals
- All tool actions must pass through a schema whitelist

## Git Submodules

```
.agents/skills/max-external                → git@github.com:2bbb/max-external
.agents/skills/max-patgen                  → git@github.com:2bbb/max-patgen.git
.agents/skills/max-external-githubactions  → git@github.com:2bbb/max-external-githubactions.git
deps/min-api                               → https://github.com/Cycling74/min-api.git
```
