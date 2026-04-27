import OpenAI from "openai";
import { ConversationHistory } from "./history.js";

interface ToolCallAccumulator {
  id: string;
  name: string;
  arguments: string;
}

function accumulateToolCalls(
  acc: Map<number, ToolCallAccumulator>,
  deltas: Array<{ index: number; id?: string; function?: { name?: string; arguments?: string } }>
) {
  for (const d of deltas) {
    const existing = acc.get(d.index) ?? { id: "", name: "", arguments: "" };
    if (d.id) existing.id = d.id;
    if (d.function?.name) existing.name += d.function.name;
    if (d.function?.arguments) existing.arguments += d.function.arguments;
    acc.set(d.index, existing);
  }
}

interface ClientState {
  history: ConversationHistory;
  abortController: AbortController | null;
  openai: OpenAI | null;
}

const clients = new WeakMap<import("ws").WebSocket, ClientState>();

function getState(ws: import("ws").WebSocket): ClientState {
  if (!clients.has(ws)) {
    clients.set(ws, {
      history: new ConversationHistory(),
      abortController: null,
      openai: null,
    });
  }
  return clients.get(ws)!;
}

function getOpenAI(ws: import("ws").WebSocket): OpenAI | null {
  const state = getState(ws);
  if (!state.openai) {
    const key = process.env.OPENAI_API_KEY;
    if (!key) return null;
    state.openai = new OpenAI({ apiKey: key });
  }
  return state.openai;
}

export function handlePing(ws: import("ws").WebSocket, msg: { id: string }) {
  ws.send(JSON.stringify({ type: "pong", id: msg.id, timestamp: Date.now() }));
}

export async function handleChatRequest(
  ws: import("ws").WebSocket,
  msg: {
    id: string;
    model?: string;
    system?: string;
    prompt: string;
    temperature?: number;
    stream?: number;
    history?: number;
    max_history?: number;
    context?: Record<string, unknown>;
    tools?: Array<{ type: "function"; function: { name: string; description?: string; parameters?: Record<string, unknown> } }>;
  }
) {
  const openai = getOpenAI(ws);
  if (!openai) {
    ws.send(JSON.stringify({ type: "error", id: msg.id, code: "no_api_key", message: "OPENAI_API_KEY not set" }));
    return;
  }

  const state = getState(ws);
  state.abortController = new AbortController();

  if (msg.max_history) state.history.setMaxTurns(msg.max_history);

  const useHistory = msg.history !== 0;
  if (msg.system) state.history.add("system", msg.system);
  if (useHistory) state.history.add("user", msg.prompt);

  const messages = useHistory
    ? state.history.getMessages()
    : [
        ...(msg.system ? [{ role: "system" as const, content: msg.system }] : []),
        { role: "user" as const, content: msg.prompt },
      ];

  try {
    const useStream = (msg.stream ?? 1) === 1;

    if (useStream) {
      const stream = await openai.chat.completions.create(
        {
          model: msg.model ?? "gpt-4.1",
          messages,
          temperature: msg.temperature ?? 0.7,
          stream: true,
          ...(msg.tools && msg.tools.length > 0 ? { tools: msg.tools } : {}),
        },
        { signal: state.abortController.signal }
      );

      let fullText = "";
      const toolCalls = new Map<number, ToolCallAccumulator>();
      for await (const chunk of stream) {
        const choice = chunk.choices[0];
        if (!choice) continue;

        const delta = choice.delta;
        if (delta?.content) {
          fullText += delta.content;
          ws.send(JSON.stringify({ type: "chat.delta", id: msg.id, text: delta.content }));
        }

        if (delta?.tool_calls) {
          accumulateToolCalls(toolCalls, delta.tool_calls as Array<{
            index: number; id?: string; function?: { name?: string; arguments?: string };
          }>);
        }
      }

      if (toolCalls.size > 0) {
        for (const [, tc] of toolCalls) {
          let args: Record<string, unknown> = {};
          try { args = JSON.parse(tc.arguments); } catch { args = { _raw: tc.arguments } }
          ws.send(JSON.stringify({
            type: "tool.call", id: msg.id,
            tool_call_id: tc.id, tool: tc.name, arguments: args,
          }));
        }
      }

      if (useHistory) state.history.add("assistant", fullText);
    } else {
      const completion = await openai.chat.completions.create(
        {
          model: msg.model ?? "gpt-4.1",
          messages,
          temperature: msg.temperature ?? 0.7,
          ...(msg.tools && msg.tools.length > 0 ? { tools: msg.tools } : {}),
        },
        { signal: state.abortController.signal }
      );

      const choice = completion.choices[0];
      const text = choice?.message?.content ?? "";
      if (text) {
        ws.send(JSON.stringify({ type: "chat.delta", id: msg.id, text }));
      }

      if (choice?.message?.tool_calls) {
        for (const tc of choice.message.tool_calls) {
          let args: Record<string, unknown> = {};
          try { args = JSON.parse(tc.function.arguments); } catch { args = { _raw: tc.function.arguments } }
          ws.send(JSON.stringify({
            type: "tool.call", id: msg.id,
            tool_call_id: tc.id, tool: tc.function.name, arguments: args,
          }));
        }
      }

      if (useHistory) state.history.add("assistant", text);
    }

    ws.send(JSON.stringify({ type: "chat.done", id: msg.id }));
  } catch (err: unknown) {
    if (err instanceof Error && err.name === "AbortError") return;
    const message = err instanceof Error ? err.message : String(err);
    ws.send(JSON.stringify({ type: "error", id: msg.id, code: "api_error", message }));
    ws.send(JSON.stringify({ type: "chat.done", id: msg.id }));
  } finally {
    state.abortController = null;
  }
}

export function handleChatCancel(ws: import("ws").WebSocket, msg: { id: string }) {
  const state = getState(ws);
  if (state.abortController) {
    state.abortController.abort();
    state.abortController = null;
  }
}

export function handleApiKeys(
  ws: import("ws").WebSocket,
  msg: { id: string; [key: string]: unknown }
) {
  const state = getState(ws);
  const providers = Object.keys(msg).filter((k) => !["type", "id"].includes(k));
  if (msg.openai && typeof msg.openai === "string") {
    state.openai = new OpenAI({ apiKey: msg.openai });
  }
  console.log(`received api keys for: ${providers.join(", ")}`);
  ws.send(JSON.stringify({ type: "api.keys.ack", id: msg.id }));
}
