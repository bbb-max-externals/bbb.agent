import { WebSocketServer, WebSocket } from "ws";
import dotenv from "dotenv";

dotenv.config();

const PORT = parseInt(process.env.PORT ?? "8765", 10);

interface IncomingMessage {
  type: string;
  id: string;
  [key: string]: unknown;
}

function handleMessage(ws: WebSocket, raw: string) {
  let msg: IncomingMessage;
  try {
    msg = JSON.parse(raw);
  } catch {
    ws.send(JSON.stringify({ type: "error", error: "invalid json" }));
    return;
  }

  switch (msg.type) {
    case "ping":
      ws.send(JSON.stringify({ type: "pong", id: msg.id, timestamp: Date.now() }));
      break;
    case "api.keys": {
      const providers = Object.keys(msg).filter((k) => !["type", "id"].includes(k));
      console.log(`received api keys for: ${providers.join(", ")}`);
      ws.send(JSON.stringify({ type: "api.keys.ack", id: msg.id }));
      break;
    }
    case "chat.request":
      ws.send(JSON.stringify({ type: "chat.delta", id: msg.id, text: "[echo] " }));
      ws.send(JSON.stringify({ type: "chat.done", id: msg.id }));
      break;
    default:
      ws.send(JSON.stringify({ type: "echo", id: msg.id, original: msg }));
  }
}

const wss = new WebSocketServer({ port: PORT });

wss.on("listening", () => {
  console.log(`bbb.agent helper service listening on ws://localhost:${PORT}`);
});

wss.on("connection", (ws) => {
  console.log("client connected");

  ws.on("message", (raw) => {
    handleMessage(ws, raw.toString());
  });

  ws.on("close", () => {
    console.log("client disconnected");
  });

  ws.send(JSON.stringify({ type: "connected" }));
});

wss.on("error", (err) => {
  console.error("server error:", err);
  process.exit(1);
});
