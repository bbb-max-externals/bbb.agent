export class ConversationHistory {
  private messages: Array<{ role: "system" | "user" | "assistant"; content: string }> = [];
  private maxTurns: number;

  constructor(maxTurns = 20) {
    this.maxTurns = maxTurns;
  }

  add(role: "system" | "user" | "assistant", content: string) {
    this.messages.push({ role, content });
    this.trim();
  }

  getMessages(): Array<{ role: "system" | "user" | "assistant"; content: string }> {
    return [...this.messages];
  }

  clear() {
    this.messages = [];
  }

  setMaxTurns(n: number) {
    this.maxTurns = n;
    this.trim();
  }

  private trim() {
    const systemMsgs = this.messages.filter((m) => m.role === "system");
    const otherMsgs = this.messages.filter((m) => m.role !== "system");
    const maxOther = this.maxTurns * 2;
    if (otherMsgs.length > maxOther) {
      this.messages = [...systemMsgs, ...otherMsgs.slice(otherMsgs.length - maxOther)];
    }
  }
}
