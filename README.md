# bbb.agent

Max/MSP external package that provides AI agent infrastructure for live performance.

## Overview

Six MVP externals communicate with a local helper service (Node.js/TypeScript) over WebSocket. The helper handles LLM API calls, streaming, schema validation, and conversation history. Max externals handle inlet/outlet, message conversion, and scheduler safety.

```
bbb.agent.hub ←→ helper service (WebSocket on localhost)
    ↕
bbb.agent.chat / bbb.agent.toolrouter / bbb.agent.state / bbb.agent.guard / bbb.agent.ping
```

## Externals

| Object | Description |
|---|---|
| `bbb.agent.hub` | WebSocket connection manager — connect, disconnect, auto-reconnect, status |
| `bbb.agent.chat` | LLM chat with streaming delta output, cancel support, conversation history |
| `bbb.agent.toolrouter` | Route validated tool calls from LLM — allow/deny by name, reject unknown |
| `bbb.agent.guard` | Safety guard — numeric range clip/reject, rate limiting, selector denylist |
| `bbb.agent.state` | Collect patch state as key-value pairs + event ring buffer, JSON output |
| `bbb.agent.ping` | Connection diagnostic — round-trip latency measurement via hub |

## Quick Start

### 1. Build externals

```bash
cmake -B build
cmake --build build
```

Output: `externals/*.mxo` (universal binary: x86_64 + arm64).

Requirements: macOS, CMake 3.19+, Xcode CLI tools.

### 2. Start helper service

```bash
cd helper
cp .env.example .env
# Edit .env: set OPENAI_API_KEY=sk-...
npm install
npm run dev
```

Helper listens on `ws://localhost:8765`.

### 3. Open Max

Add `externals/` to Max's search path (or use the package in `~/Documents/Max 8/Packages/`).

Basic usage:

```
[bbb.agent.hub @host localhost @port 8765]
  |
[connect]

[bbb.agent.chat @model gpt-4.1]
  |
[prompt Hello, what can you do?]
  |
[send]
```

## Signal Flow

```
[bbb.agent.chat]  --chat.request JSON-->  [bbb.agent.hub]  --WebSocket-->  helper (OpenAI API)
                                                                |
                    <--route data--        <--data JSON--      <--streaming delta--
                         |
                [bbb.agent.toolrouter]
                    |           |
               [route ...]   [reject]
                    |
            [bbb.agent.guard]
                |       |
            [pass]   [block]

[bbb.agent.state] --set/event--> [json] --prepend context--> [bbb.agent.chat]
```

## External Details

### bbb.agent.hub

Manages the single WebSocket connection to the helper service. All other externals route through this.

**Messages:** `connect`, `disconnect`, `restart`, `status`, `apikey <provider> <key>`, `send <json>`

**Attributes:** `@host`, `@port`, `@reconnect`, `@reconnect_interval`

**Outlets:** status events (left), response data (right)

### bbb.agent.chat

Sends prompts to the LLM and receives streaming responses. Uses `object_findregistered` to discover the hub.

**Messages:** `prompt <text>`, `system <text>`, `send`, `clear`, `cancel`, `delta`, `done`, `tool`, `error`

**Attributes:** `@model`, `@temperature`, `@stream`, `@history`, `@max_history`, `@timeout`

**Outlets:** response stream (left), tool calls (middle), status/error (right)

### bbb.agent.toolrouter

Validates tool calls against an allowlist and routes them as Max messages.

**Messages:** `tool <name> <args>`, `allow <name>`, `deny <name>`, `enable <name> <0|1>`, `list`, `clear`

**Attributes:** `@strict`

**Outlets:** routed messages (left), rejected (middle), status (right)

### bbb.agent.guard

Clips or rejects messages based on numeric ranges, rate limits, and deny rules.

**Messages:** (any message), `rule <selector> <min> <max> @clip 1 @rate 50 @deny 0`, `deny <name>`, `approve <name>`, `bypass <0|1>`, `clear`

**Attributes:** `@bypass`, `@mode` (0=reject, 1=clip, 2=dryrun), `@rate_limit`

**Outlets:** allowed (left), blocked (middle), status (right)

### bbb.agent.state

Collects key-value state and timestamped events into a JSON blob for LLM context.

**Messages:** `set <key> <value>`, `unset <key>`, `event <type> <payload>`, `dump`, `json`, `clear`

**Attributes:** `@max_events`

**Outlets:** state pairs (left), JSON string (right)

## Project Structure

```
bbb.agent/
  CMakeLists.txt
  cmake/bbb_external.cmake          # bbb_add_external() macro
  source/projects/
    bbb.agent.hub/                   # WebSocket client external
    bbb.agent.chat/                  # LLM chat external
    bbb.agent.toolrouter/            # Tool call routing external
    bbb.agent.guard/                 # Safety guard external
    bbb.agent.state/                 # State collection external
    bbb.agent.ping/                  # Connection diagnostic
  helper/                            # Node.js helper service
    src/
      index.ts                       # WebSocket server + message router
      handlers.ts                    # OpenAI streaming + tool call handling
      history.ts                     # Conversation history management
    .env.example
  deps/
    min-api/                         # Cycling '74 min-api (git submodule)
    IXWebSocket/                     # WebSocket client library (git submodule)
  externals/                         # Built .mxo files (gitignored)
```

## Helper Service

The helper is a Node.js/TypeScript WebSocket server that:

- Handles LLM API calls via OpenAI SDK (streaming)
- Accumulates tool call deltas from streaming responses
- Manages conversation history per client connection
- Supports runtime API key injection via `api.keys` message
- Responds to `ping` with latency timestamps

## Dependencies & Licenses

| Library | License | Used by |
|---|---|---|
| [min-api](https://github.com/Cycling74/min-api) | MIT — Copyright 2018, The Min-API Authors | All externals |
| [IXWebSocket](https://github.com/machinezone/IXWebSocket) | BSD-3-Clause — Copyright 2018 Machine Zone, Inc. | `bbb.agent.hub` |
| [openai (npm)](https://github.com/openai/openai-node) | Apache-2.0 — Copyright 2025 OpenAI | helper |
| [ws (npm)](https://github.com/websockets/ws) | MIT — Copyright Einar Otto Stangvik | helper |
| [dotenv (npm)](https://github.com/motdotla/dotenv) | BSD-2-Clause — Copyright Scott Motte | helper |

## Requirements

- macOS 10.11+ or Windows 10+
- CMake 3.19+
- Xcode CLI tools (macOS) or Visual Studio 2022 (Windows)
- Max 8 or Max 9
- Node.js 18+ (for helper service)

## Build (CI)

GitHub Actions builds on every push to `main`:

- **macOS**: Universal binary (x86_64 + arm64)
- **Windows**: x64 (Visual Studio 2022)

Artifacts are available for download from each workflow run.
