# Max/MSP AI Agents External 開発計画

## 1. 目的

Max/MSP 上で AI agent を安全かつ実用的に動かすための external 群を開発する。

単なる「LLM API 呼び出し object」ではなく、Max の message、scheduler、DSP、patcher scripting、ライブパフォーマンスの制約に合う agent 実行基盤を作る。

中心コンセプトは次の 4 つ。

1. **AI と通信する** — LLM / tool calling / streaming を Max object 化する
2. **Max の状態を理解させる** — patch、parameter、transport、MIDI/audio 状態を context 化する
3. **AI の行動を Max message に変換する** — tool call を route 可能な message にする
4. **安全に制御する** — 範囲制限、rate limit、承認 gate、暴走防止を入れる

---

## 2. 開発方針

### 2.1 最初から作り込みすぎない

初期版では「完全自律 agent」ではなく、以下を満たす実用的な最小構成を目指す。

- Max から LLM に prompt を送れる
- LLM からの streaming response を Max message として受け取れる
- tool call / function call を Max message に変換できる
- 許可された tool のみ実行できる
- 危険な出力や過剰な parameter 変更を止められる

### 2.2 Node for Max との差別化

Node for Max でも HTTP API 連携はできるため、external として作る価値は次に置く。

- 低レイテンシな streaming 出力
- Max scheduler をブロックしない非同期処理
- message / list / dictionary への自然な変換
- patcher scripting との安全な統合
- tool schema と route の標準化
- ライブ用途向けの guard / limiter

### 2.3 C/C++ external と helper process の分離

LLM API 通信、JSON parsing、embedding DB、長期 memory などは C external 内に全部詰め込むと保守が重い。

推奨構成:

```text
Max external layer
  - inlet / outlet
  - Max message conversion
  - scheduler-safe dispatch
  - dictionary/list handling
  - UI-facing errors

Local helper service
  - API client
  - auth / key management
  - streaming transport
  - vector DB / memory
  - tool schema validation
```

初期版では local helper service を **Node.js (TypeScript)** で実装し、Max external とは localhost WebSocket で通信する。

#### 技術選定理由

- LLM SDK（OpenAI / Anthropic）の公式 TypeScript サポートが最も成熟している
- WebSocket server 実装（ws / fastify）が安定している
- Max の Node for Max と将来的にリソース共有できる可能性
- `tsconfig.json` による型安全な開発

#### API key 管理

優先順位:

1. **Max からの明示的渡し** — `bbb.agent.hub` の `apikey` message 経由で helper に渡す
2. **Helper の `.env` ファイル** — 明示的渡しがない場合のフォールバック

```text
; Max 側で明示的に渡す場合
[loadbang]
   |
[apikey sk-xxxx]    ; または coll / dict から読む
   |
[bbb.agent.hub]
```

Helper service は接続時の handshake で API key を受け取る。未設定の場合は `.env` から読む。API key を Max patch ファイルに保存しない（`@apikey` attribute は設けない。patch 保存時に書き込まれるため）。

#### 接続モデル

全 external は **単一 WebSocket 接続を共有**する。`bbb.agent.hub` が helper service との接続を管理し、他の external は hub 経由で通信する。

```text
bbb.agent.hub ←→ helper service (WebSocket on localhost)
    ↕
bbb.agent.chat / bbb.agent.toolrouter / bbb.agent.state / bbb.agent.guard
```

---

## 3. MVP external 群

MVP では以下の 6 object を作る。

1. `bbb.agent.hub` — helper service との WebSocket 接続管理・再接続・ステータス
2. `bbb.agent.chat` — LLM との通信・streaming
3. `bbb.agent.toolrouter` — tool call の schema 検証・route
4. `bbb.agent.state` — Max patch 状態の集約・出力
5. `bbb.agent.guard` — 出力の安全制限
6. `bbb.agent.ping` — 接続診断（パッケージに残す）

---

## 4. `bbb.agent.hub`

### 4.1 役割

Helper service との単一 WebSocket 接続を管理する。他のすべての `bbb.agent.*` external は hub 経由で helper と通信する。

### 4.2 基本仕様

#### Inlets

1. command inlet
   - `connect`
   - `disconnect`
   - `restart`
   - `status`
   - `apikey <provider> <key>` — LLM provider の API key を helper に渡す（例: `apikey openai sk-xxx`）
   - `config <dict>` — helper の設定を渡す（model defaults 等）

#### Outlets

1. status outlet
   - `status connected`
   - `status disconnected`
   - `status connecting`
   - `status error`
2. data outlet（他の external からの request を helper に送信し、response を分配）

### 4.3 Attributes

```text
@host localhost
@port 8765
@reconnect 1
@reconnect_interval 3000
```

### 4.4 実装メモ

- 1 patch に 1 hub を配置する想定
- 他の external は hub の Max object を名前参照（`@hub` attribute またはデフォルト検索）で見つけて通信する
- helper service のライフサイクル管理（起動・停止）は hub の責務にするか、ユーザーが外部で管理するかは Milestone 0 で検証する
- reconnect 時は pending 中の request を queue に保持し、接続回復後に再送する
- `apikey` message で渡された key はメモリ上にのみ保持し、attribute にしない（patch 保存時の漏洩防止）
- `apikey` は connect 前後いつでも送信可能。helper は最新の key を使用する

---

## 5. `bbb.agent.chat`

### 5.1 役割

LLM との通信を担当する基本 object。

Max から prompt、system prompt、context、model parameters を送り、応答を streaming で outlet に出す。

### 5.2 基本仕様

#### Inlets

1. command inlet
   - `prompt <text>`
   - `system <text>`
   - `context <dict>`
   - `send`
   - `clear`
   - `cancel`
2. parameter inlet
   - `model <symbol>`
   - `temperature <float>`
   - `max_tokens <int>`
   - `stream <0|1>`

#### Outlets

1. response stream
   - `delta <text>`
   - `done <text>`
2. tool call stream
   - `tool <name> <dict>`
3. status / error
   - `status connecting`
   - `status streaming`
   - `status done`
   - `error <code> <message>`

### 5.3 Messages

```text
system You are a helpful Max/MSP assistant.
prompt Make the current patch darker over 30 seconds.
send
cancel
clear
model gpt-4.1
stream 1
temperature 0.3
```

### 5.4 Attributes

```text
@model gpt-4.1
@temperature 0.3
@stream 1
@history 1
@max_history 20
@timeout 30
@endpoint localhost:8765
```

注意: `@endpoint` は `bbb.agent.hub` 側で管理される。`bbb.agent.chat` は hub の名前で参照する。

### 5.5 実装メモ

- API key は Max patch に直接保存しない（helper service の `.env` で管理）
- Max scheduler を止めないため、通信は worker thread / helper service 側に逃がす
- streaming delta は queue 経由で main thread に戻す
- `cancel` で実行中 request を中断できるようにする
- **会話履歴は helper service 側で管理**する。`@history 1` の場合、helper が conversation history を保持し、次回リクエストに context として付与する。Max 側にはキャッシュしない
- `bbb.agent.hub` 経由で helper service と通信する。直接の WebSocket 接続は持たない

---

## 6. `bbb.agent.toolrouter`

### 6.1 役割

LLM の tool call / function call を Max message に変換する。

AI が直接 arbitrary message を出すのではなく、事前定義された schema に一致した action のみ route する。

### 6.2 基本仕様

#### Inlets

1. tool call inlet
   - `tool <name> <dict>`
   - `schema <dict>`
   - `loadschema <path>`
2. control inlet
   - `enable <tool_name> <0|1>`
   - `allow <tool_name>`
   - `deny <tool_name>`

#### Outlets

1. routed tool message
   - `<tool_name> <args...>`
2. rejected tool call
   - `reject <reason> <dict>`
3. debug / status
   - `status schema_loaded`
   - `error schema_invalid`

### 6.3 Tool schema 例

Tool schema は **JSON Schema 準拠**で定義する。Helper service が各 LLM provider（OpenAI / Anthropic 等）の function calling 形式に変換する。Max external 側は provider 非依存。

```json
{
  "tools": [
    {
      "name": "set_filter",
      "description": "Set filter cutoff and resonance",
      "parameters": {
        "type": "object",
        "properties": {
          "cutoff": { "type": "number", "minimum": 20, "maximum": 18000 },
          "resonance": { "type": "number", "minimum": 0.1, "maximum": 10.0 }
        },
        "required": ["cutoff", "resonance"]
      },
      "output": "set_filter cutoff resonance"
    },
    {
      "name": "play_sample",
      "description": "Trigger a sample by name and velocity",
      "parameters": {
        "type": "object",
        "properties": {
          "name": { "type": "string" },
          "velocity": { "type": "integer", "minimum": 0, "maximum": 127 }
        },
        "required": ["name", "velocity"]
      },
      "output": "play_sample name velocity"
    }
  ]
}
```

`output` フィールドは Max 側の出力形式の定義（JSON Schema 拡張）。Helper service はこの形式に基づいて tool call の引数を Max message に変換する。

### 6.4 Max usage 例

```text
[bbb.agent.chat]
     |
[bbb.agent.toolrouter @schema tools.json]
     |
[route set_filter play_sample scene_change]
```

AI から次の tool call が来た場合:

```json
{
  "name": "set_filter",
  "arguments": {
    "cutoff": 1200,
    "resonance": 0.8
  }
}
```

Max 側では次を出力する。

```text
set_filter 1200 0.8
```

### 6.5 実装メモ

- schema validation は helper service 側で行う
- Max external 側でも最低限の型チェックを行う
- 未定義 tool は必ず reject
- 引数不足、型不一致、範囲外は reject
- `@strict 1` の場合、schema にない追加引数も reject

---

## 7. `bbb.agent.state`

### 7.1 役割

Max patch の状態を AI に渡せる形にまとめる。

AI agent は context がないと適切に判断できないため、現在の BPM、scene、parameter、recent events などを dictionary / JSON に変換する。

### 7.2 基本仕様

#### Inlets

1. state update inlet
   - `set <key> <value>`
   - `merge <dict>`
   - `event <type> <payload>`
   - `clear`
2. query inlet
   - `dump`
   - `json`
   - `summary`

#### Outlets

1. state dictionary
2. JSON string
3. natural language summary

### 7.3 State 例

```json
{
  "transport": {
    "bpm": 128,
    "playing": true,
    "bar": 32,
    "beat": 2
  },
  "audio": {
    "level_db": -12.4,
    "density": 0.72,
    "brightness": 0.61
  },
  "scene": {
    "current": "breakdown",
    "next": "drop"
  },
  "parameters": {
    "filter_cutoff": 2400,
    "reverb_mix": 0.35,
    "delay_feedback": 0.42
  },
  "recent_events": [
    { "time": 12.4, "type": "midi_note", "note": 60, "velocity": 95 },
    { "time": 13.1, "type": "scene_change", "scene": "breakdown" }
  ]
}
```

### 7.4 Max usage 例

```text
[transport]
   |
[bbb.agent.state]
   |
[prepend context]
   |
[bbb.agent.chat]
```

### 7.5 実装メモ

- state は ring buffer で recent events を保持
- `@max_events 128` などで上限設定
- `summary` は helper service で自然言語化してもよい
- 初期版では手動 `set` / `event` 中心で十分
- 将来的には patcher introspection と連携する

---

## 8. `bbb.agent.guard`

### 8.1 役割

AI agent の出力を安全に制限する。

ライブ、展示、インスタレーションでは AI の誤出力が音量、照明、映像、patcher scripting に直接影響するため、guard object は必須。

### 8.2 基本仕様

#### Inlets

1. message inlet
   - 任意の Max message
2. rules inlet
   - `rule <dict>`
   - `loadrules <path>`
   - `bypass <0|1>`
   - `approve`
   - `deny`

#### Outlets

1. allowed message
2. blocked message
3. status / warning

### 8.3 Rule 例

```json
{
  "rules": [
    {
      "selector": "set_filter",
      "args": [
        { "type": "float", "min": 20, "max": 18000, "clip": true },
        { "type": "float", "min": 0.1, "max": 10.0, "clip": true }
      ],
      "rate_limit_ms": 50
    },
    {
      "selector": "master_gain",
      "args": [
        { "type": "float", "min": -60, "max": -3, "clip": true }
      ],
      "approval": true
    },
    {
      "selector": "script",
      "deny": true
    }
  ]
}
```

### 8.4 Max usage 例

```text
[bbb.agent.toolrouter]
     |
[bbb.agent.guard @rules safety.json]
     |
[route set_filter master_gain play_sample]
```

### 8.5 Guard modes

```text
@mode reject
@mode clip
@mode approve
@mode dryrun
```

- `reject`: 範囲外や不正 message を止める
- `clip`: 数値を安全範囲に丸める
- `approve`: 危険操作はユーザー承認後に通す
- `dryrun`: 実行せず、通るかどうかだけ確認する

### 8.6 実装メモ

- 初期版では selector / arg range / rate limit / deny のみで十分
- patcher scripting 系 message はデフォルト deny
- audio gain 系は conservative な上限を持つ
- `bypass 1` は明示的に UI 表示する

---

## 9. 補助 external 候補

MVP 後に追加する候補。

### 9.1 `bbb.agent.memory`

短期・長期 memory object。

機能:

- `remember <key> <value>`
- `recall <query>`
- `forget <key>`
- embedding search
- patch ごとの namespace

用途:

- performer の好みを覚える
- 過去の scene change を記録する
- sample / preset の説明を検索する

### 9.2 `bbb.agent.embed`

text / MIDI / audio feature を embedding 化する object。

用途:

- 類似 sample 検索
- preset recommendation
- gesture matching
- text-to-sound retrieval

### 9.3 `bbb.agent.listen~`

audio signal を監視し、AI agent 用の状態を出す MSP object。

出力例:

```text
density 0.73
brightness 0.61
onset 1
mood tense
confidence 0.82
```

初期版は ML ではなく通常の audio feature extraction でもよい。

### 9.4 `bbb.agent.plan`

目標から時間展開する action plan を出す object。

例:

```text
goal make this patch darker over 30 seconds
```

出力:

```text
step 0 set_filter 4000 0.4
step 10 set_filter 2400 0.5
step 20 set_filter 1200 0.7
step 30 reverb_mix 0.5
```

### 9.5 `bbb.agent.patchdoc`

patcher の構造を JSON / natural language に変換する object。

用途:

- patch の自動ドキュメント化
- AI が patch 構造を理解するための context 生成
- object graph の抽出

### 9.6 `bbb.agent.autopatch`

AI の提案から patcher scripting command を生成する object。

注意:

- 初期版では direct execution しない
- `dryrun` と `approval` を必須にする
- `bbb.agent.guard` と組み合わせる

### 9.7 `bbb.agent.ping`

Helper service との接続診断 object。MVP に含まれ、パッケージに残す。

#### Inlets

1. command inlet
   - `ping`
   - `latency`

#### Outlets

1. response outlet
   - `pong <timestamp>`
   - `latency <ms>`

#### Attributes

```text
@interval 0
```

- `@interval 0`: 手動 ping のみ（`ping` message で応答）
- `@interval N`: N ミリ秒ごとに自動 ping（接続監視用）

#### 実装メモ

- `bbb.agent.hub` 経由で helper service に ping を送る
- round trip time を測定して `latency` で出力
- 接続断時は `status disconnected` を出力

---

## 10. 全体アーキテクチャ

```text
+-------------------+        +-----------------------+
| Max patch          |        | Local helper service  |
|                   |        |                       |
| [bbb.agent.state]      | -----> | context builder       |
|       |            |        |                       |
| [bbb.agent.chat]       | <----> | LLM API client        |
|       |            |        | streaming manager     |
| [bbb.agent.toolrouter] | <----> | schema validator      |
|       |            |        |                       |
| [bbb.agent.guard]      |        | memory / embeddings   |
|       |            |        |                       |
| Max instruments    |        | auth / config         |
+-------------------+        +-----------------------+
```

### 10.1 Communication options

候補:

1. localhost WebSocket
2. localhost HTTP streaming
3. stdio subprocess
4. ZeroMQ

推奨初期案:

- `bbb.agent.chat` と helper service は WebSocket
- request / response は JSON
- streaming delta は event message として送る

### 10.2 Event protocol 例

#### Max to helper

```json
{
  "type": "chat.request",
  "id": "req_001",
  "model": "gpt-4.1",
  "system": "You are a Max/MSP performance assistant.",
  "prompt": "Make the sound darker over 30 seconds.",
  "context": {
    "bpm": 128,
    "scene": "breakdown"
  },
  "tools": ["set_filter", "reverb_mix", "play_sample"]
}
```

#### Helper to Max

```json
{ "type": "chat.delta", "id": "req_001", "text": "I will gradually" }
```

```json
{
  "type": "tool.call",
  "id": "req_001",
  "tool": "set_filter",
  "arguments": {
    "cutoff": 1800,
    "resonance": 0.7
  }
}
```

```json
{ "type": "chat.done", "id": "req_001" }
```

---

## 11. Repository 構成案

```text
bbb.ltc/
  README.md
  PLAN.md
  package-info.json
  source/projects/
    bbb.agent.hub/
      bbb.agent.hub.cpp
      bbb.agent.hub.maxhelp
      CMakeLists.txt
    bbb.agent.chat/
      bbb.agent.chat.cpp
      bbb.agent.chat.maxhelp
      CMakeLists.txt
    bbb.agent.toolrouter/
      bbb.agent.toolrouter.cpp
      bbb.agent.toolrouter.maxhelp
      CMakeLists.txt
    bbb.agent.state/
      bbb.agent.state.cpp
      bbb.agent.state.maxhelp
      CMakeLists.txt
    bbb.agent.guard/
      bbb.agent.guard.cpp
      bbb.agent.guard.maxhelp
      CMakeLists.txt
    bbb.agent.ping/
      bbb.agent.ping.cpp
      bbb.agent.ping.maxhelp
      CMakeLists.txt
  helper/
    package.json
    tsconfig.json
    src/
      index.ts
      apiClient.ts
      websocketServer.ts
      schemaValidator.ts
      conversationHistory.ts
      config.ts
    .env.example
  schemas/
    tools.example.json
    safety.example.json
  examples/
    00_ping_test.maxpat
    01_basic_chat.maxpat
    02_toolrouter_filter.maxpat
    03_state_context.maxpat
    04_guarded_agent.maxpat
```

---

## 12. Milestones

### Milestone 0: 技術検証

目的: `bbb.agent.hub` 経由で helper service と通信できることを確認する。

Deliverables:

- `bbb.agent.hub` external（WebSocket client、reconnect、status）
- `bbb.agent.ping` external（接続診断）
- helper service echo server（Node.js/TypeScript）
- Max から JSON message を送り、返信を受ける demo patch

完了条件:

- `bbb.agent.hub` が helper service に接続・切断・再接続できる
- `bbb.agent.ping` が round trip time を返す
- Max scheduler を止めずに round trip できる
- helper service disconnect 時に Max が落ちない
- reconnect できる

---

### Milestone 1: `bbb.agent.chat` MVP

Deliverables:

- `bbb.agent.chat` external
- helper service LLM API client（OpenAI SDK streaming）
- streaming response support（hub 経由）
- conversation history 管理（helper 側）
- basic error handling
- example patch

完了条件:

- `prompt` / `send` で応答が返る
- streaming delta が逐次 outlet から出る
- `cancel` が動く
- API error が Max に readable に返る

---

### Milestone 2: `bbb.agent.toolrouter` MVP

Deliverables:

- JSON tool schema loader
- tool call parser
- route output
- reject output
- schema example

完了条件:

- LLM の tool call を Max message に変換できる
- 未定義 tool を reject できる
- 型不一致を reject できる
- 範囲外値を reject できる

---

### Milestone 3: `bbb.agent.guard` MVP

Deliverables:

- safety rule loader
- selector denylist
- numeric range validation
- clip / reject mode
- rate limit

完了条件:

- 危険 message を止められる
- 数値を安全範囲に収められる
- 高頻度 message を rate limit できる
- blocked message を別 outlet に出せる

---

### Milestone 4: `bbb.agent.state` MVP

Deliverables:

- key-value state store
- recent event ring buffer
- dictionary output
- JSON output
- context injection example

完了条件:

- patch 状態を dictionary / JSON として出せる
- `bbb.agent.chat` に context として渡せる
- recent events の上限管理ができる

---

### Milestone 5: Integrated demo

Deliverables:

- guarded AI filter controller
- AI scene assistant
- AI sample trigger demo
- README tutorial

完了条件:

- AI が Max patch の状態を読み、tool call を出し、guard を通って instrument を操作できる
- demo が 10 分以上安定動作する
- helper service 再起動後も復帰できる

---

## 13. Testing plan

### 13.1 Unit tests

- JSON parser tests
- schema validation tests
- guard rule tests
- rate limit tests
- type conversion tests

### 13.2 Max-level tests

- malformed message を送っても crash しない
- helper service を落としても crash しない
- streaming 中に `cancel` しても crash しない
- rapid fire prompt に耐える
- patch 保存 / 再ロード後に attributes が復元される

### 13.3 Performance tests

- streaming delta 1000 messages/min
- tool call 100 messages/sec
- state update 1000 events/sec
- long-running patch 1 hour

### 13.4 Safety tests

- 未定義 tool call
- 範囲外 parameter
- prohibited selector
- repeated dangerous command
- patcher scripting attempt
- master gain overshoot

---

## 14. Design constraints

### 14.1 Scheduler safety

- Network I/O を main thread で行わない
- Long-running JSON parsing を scheduler thread で行わない
- outlet 出力は Max の thread model に合わせる
- streaming burst を queue / defer で調整する

### 14.2 Failure handling

- helper service disconnect は error outlet に出す
- request timeout を持つ
- API rate limit を readable に出す
- partial response を扱えるようにする
- crash より reject / error を優先する

### 14.3 Security

- API key を patch file に保存しない
- arbitrary shell execution を持たない
- patcher scripting は初期状態で deny
- file write は user-approved directory のみ
- tool schema にない action は実行しない

---

## 15. Example patches

### 15.1 Basic chat

目的: Max から LLM に質問して返答を受ける。

Objects:

```text
[bbb.agent.hub @host localhost @port 8765]
[bbb.agent.ping]

[textedit]
   |
[prepend prompt]
   |
[bbb.agent.chat @stream 1]
   |
[print ai]
```

### 15.2 Guarded filter control

目的: AI が filter cutoff を提案し、guard を通して安全に操作する。

```text
[bbb.agent.hub @host localhost @port 8765]

[bbb.agent.state]
   |
[bbb.agent.chat @tools tools.json]
   |
[bbb.agent.toolrouter @schema tools.json]
   |
[bbb.agent.guard @rules safety.json @mode clip]
   |
[route set_filter]
   |
[filtergraph~]
```

### 15.3 AI scene assistant

目的: scene 状態に応じて次の展開を提案する。

```text
[bbb.agent.hub @host localhost @port 8765]

[bbb.agent.state]
   |
[prepend context]
   |
[bbb.agent.chat]
   |
[bbb.agent.toolrouter]
   |
[route scene_change reverb_mix delay_feedback]
```

### 15.4 Dry-run autopatch

目的: AI が patch 変更案を出すが、実行はしない。

```text
[bbb.agent.hub @host localhost @port 8765]

[bbb.agent.chat]
   |
[bbb.agent.toolrouter @schema patch_tools.json]
   |
[bbb.agent.guard @rules autopatch_safety.json @mode dryrun]
   |
[print proposed_patch_change]
```

---

## 16. 開発順序

推奨順序:

1. helper service の WebSocket echo server（Node.js/TypeScript）
2. `bbb.agent.hub` の WebSocket 接続・再接続
3. `bbb.agent.ping` の接続診断
4. `bbb.agent.chat` の streaming 実装
5. tool call event protocol の確定
6. `bbb.agent.toolrouter` の schema validation（JSON Schema 準拠）
7. `bbb.agent.guard` の range / deny / rate limit
8. `bbb.agent.state` の dictionary output
9. integrated demo patch
10. docs / examples / package 化

---

## 17. 初期 API のたたき台

### 17.1 `bbb.agent.hub`

```text
connect
disconnect
restart
status
apikey <provider> <key>
config <dict>
```

Attributes: `@host`, `@port`, `@reconnect`, `@reconnect_interval`

### 17.2 `bbb.agent.chat`

```text
prompt <text>
system <text>
context <dict>
send
cancel
clear
model <symbol>
temperature <float>
stream <0|1>
```

### 17.3 `bbb.agent.toolrouter`

```text
tool <name> <dict>
schema <dict>
loadschema <path>
allow <name>
deny <name>
strict <0|1>
```

### 17.4 `bbb.agent.state`

```text
set <key> <value>
merge <dict>
event <type> <dict>
dump
json
summary
clear
```

### 17.5 `bbb.agent.guard`

```text
loadrules <path>
rule <dict>
mode reject|clip|approve|dryrun
bypass <0|1>
approve
deny
```

### 17.6 `bbb.agent.ping`

```text
ping
latency
```

Attributes: `@interval`

---

## 18. Risks

### 18.1 LLM API の仕様変更

対策:

- API client は helper service に隔離
- Max external は provider-agnostic な protocol にする
- model/provider 切替を config 化する

### 18.2 Streaming の burst による Max 側負荷

対策:

- delta batching
- output interval の attribute 化
- `@max_delta_rate` を設ける

### 18.3 AI による危険操作

対策:

- tool schema whitelist
- `bbb.agent.guard` を必須推奨
- patcher scripting は default deny
- dry-run mode

### 18.4 Debug が難しい

対策:

- request id を全 message に付与
- verbose logging mode
- helper service log file
- Max 側 status outlet

---

## 19. 成功条件

初期リリースの成功条件:

- Max patch 内で AI agent loop を組める
- LLM の tool call を Max message に安全に変換できる
- AI が patch 状態を context として参照できる
- guard によって危険な操作を止められる
- ライブ用途で落ちにくい
- example patch を見れば使い方が分かる

---

## 20. 最小デモのゴール

最初の公開 demo は次の内容にする。

**Demo: AI Performance Assistant**

ユーザーが Max patch に対して次のように指示する。

```text
Make this section more tense for 16 bars, then prepare a drop.
```

AI は `bbb.agent.state` から現在の BPM、scene、audio density、parameter を受け取り、次の tool call を返す。

```text
set_filter 1800 0.7
reverb_mix 0.45
delay_feedback 0.38
scene_hint prepare_drop
```

`bbb.agent.guard` が範囲と rate limit を検証し、安全な message のみ Max instrument に通す。

この demo が動けば、プロジェクトの価値が明確に伝わる。
