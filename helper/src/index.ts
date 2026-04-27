import { WebSocketServer, WebSocket } from "ws";
import dotenv from "dotenv";
import { handlePing, handleChatRequest, handleChatCancel, handleApiKeys } from "./handlers.js";

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
      handlePing(ws, msg);
      break;
    case "chat.request":
      handleChatRequest(ws, msg as unknown as Parameters<typeof handleChatRequest>[1]);
      break;
    case "chat.cancel":
      handleChatCancel(ws, msg);
      break;
    case "api.keys":
      handleApiKeys(ws, msg);
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
