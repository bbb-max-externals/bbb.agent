import OpenAI from "openai";
import { ConversationHistory } from "./history.js";

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
        },
        { signal: state.abortController.signal }
      );

      let fullText = "";
      for await (const chunk of stream) {
        const delta = chunk.choices[0]?.delta?.content;
        if (delta) {
          fullText += delta;
          ws.send(JSON.stringify({ type: "chat.delta", id: msg.id, text: delta }));
        }
      }
      if (useHistory) state.history.add("assistant", fullText);
    } else {
      const completion = await openai.chat.completions.create(
        {
          model: msg.model ?? "gpt-4.1",
          messages,
          temperature: msg.temperature ?? 0.7,
        },
        { signal: state.abortController.signal }
      );

      const text = completion.choices[0]?.message?.content ?? "";
      if (text) {
        ws.send(JSON.stringify({ type: "chat.delta", id: msg.id, text }));
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
