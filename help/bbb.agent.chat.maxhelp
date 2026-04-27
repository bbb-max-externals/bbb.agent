{
  "patcher" : {
    "fileversion" : 1,
    "appversion" : {
      "major" : 8,
      "minor" : 6,
      "revision" : 4
    },
    "classnamespace" : "box",
    "rect" : [100.0, 100.0, 950.0, 800.0],
    "bglocked" : 1,
    "openrect" : [0.0, 0.0, 0.0, 0.0],
    "openinpresentation" : 0,
    "default_fontsize" : 12.0,
    "default_fontface" : 0,
    "default_fontname" : "Arial",
    "gridonopen" : 2,
    "gridsize" : [15.0, 15.0],
    "gridsnaponopen" : 0,
    "objectsnaponopen" : 1,
    "statusbarvisible" : 2,
    "toolbarvisible" : 2,
    "lefttoolbarpinned" : 0,
    "toptoolbarpinned" : 0,
    "righttoolbarpinned" : 0,
    "bottomtoolbarpinned" : 0,
    "toolbars_unpinned_last_save" : 0,
    "tallnewobj" : 0,
    "boxanimatetime" : 200,
    "enablehscroll" : 1,
    "enablevscroll" : 1,
    "devicewidth" : 0.0,
    "description" : "Help file for bbb.agent.chat",
    "digest" : "LLM chat with streaming response via bbb.agent.hub",
    "tags" : "ai, chat, streaming, help",
    "style" : "",
    "subpatcher_template" : "",
    "assistshowspatchername" : 0,
    "boxes" : [
      {
        "box" : {
          "id" : "obj-1",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 30.0, 300.0, 20.0],
          "text" : "bbb.agent.chat"
        }
      },
      {
        "box" : {
          "id" : "obj-2",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 55.0, 500.0, 20.0],
          "text" : "LLM chat with streaming response. Requires bbb.agent.hub in the same patch."
        }
      },
      {
        "box" : {
          "id" : "obj-3",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 90.0, 100.0, 20.0],
          "text" : "--- Inlets ---"
        }
      },
      {
        "box" : {
          "id" : "obj-4",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 110.0, 500.0, 20.0],
          "text" : "inlet 0: prompt <text>, system <text>, send, clear, cancel, delta, done, tool, error"
        }
      },
      {
        "box" : {
          "id" : "obj-5",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 145.0, 100.0, 20.0],
          "text" : "--- Outlets ---"
        }
      },
      {
        "box" : {
          "id" : "obj-6",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 165.0, 400.0, 20.0],
          "text" : "outlet 0 (left): delta <text>, done <full_text>"
        }
      },
      {
        "box" : {
          "id" : "obj-7",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 185.0, 400.0, 20.0],
          "text" : "outlet 1 (middle): tool <name> <args>"
        }
      },
      {
        "box" : {
          "id" : "obj-8",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 205.0, 400.0, 20.0],
          "text" : "outlet 2 (right): status <msg>, error <code> <msg>"
        }
      },
      {
        "box" : {
          "id" : "obj-20",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 250.0, 300.0, 20.0],
          "text" : "--- Chat Example (requires hub connected) ---"
        }
      },
      {
        "box" : {
          "id" : "obj-21",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 280.0, 350.0, 22.0],
          "text" : "system You are a Max/MSP performance assistant."
        }
      },
      {
        "box" : {
          "id" : "obj-22",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 310.0, 350.0, 22.0],
          "text" : "prompt Suggest 3 creative ways to use random in a live set."
        }
      },
      {
        "box" : {
          "id" : "obj-23",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 340.0, 60.0, 22.0],
          "text" : "send"
        }
      },
      {
        "box" : {
          "id" : "obj-24",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [120.0, 340.0, 60.0, 22.0],
          "text" : "cancel"
        }
      },
      {
        "box" : {
          "id" : "obj-25",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [190.0, 340.0, 60.0, 22.0],
          "text" : "clear"
        }
      },
      {
        "box" : {
          "id" : "obj-30",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 3,
          "outlettype" : ["", "", ""],
          "patching_rect" : [50.0, 400.0, 110.0, 22.0],
          "text" : "bbb.agent.chat"
        }
      },
      {
        "box" : {
          "id" : "obj-31",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 460.0, 80.0, 22.0],
          "text" : "print response"
        }
      },
      {
        "box" : {
          "id" : "obj-32",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [200.0, 460.0, 60.0, 22.0],
          "text" : "print tool"
        }
      },
      {
        "box" : {
          "id" : "obj-33",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [320.0, 460.0, 70.0, 22.0],
          "text" : "print status"
        }
      },
      {
        "box" : {
          "id" : "obj-34",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 500.0, 600.0, 40.0],
          "text" : "Signal flow: chat sends chat.request JSON via hub → helper calls OpenAI →\nhelper sends chat.delta (streaming) → hub outputs data → patch routes to chat"
        }
      },
      {
        "box" : {
          "id" : "obj-40",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 560.0, 200.0, 20.0],
          "text" : "--- Attributes ---"
        }
      },
      {
        "box" : {
          "id" : "obj-41",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 585.0, 300.0, 20.0],
          "text" : "@model gpt-4.1 — LLM model name"
        }
      },
      {
        "box" : {
          "id" : "obj-42",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 605.0, 300.0, 20.0],
          "text" : "@temperature 0.7 — sampling temperature"
        }
      },
      {
        "box" : {
          "id" : "obj-43",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 625.0, 300.0, 20.0],
          "text" : "@stream 1 — enable streaming (0 for single response)"
        }
      },
      {
        "box" : {
          "id" : "obj-44",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 645.0, 400.0, 20.0],
          "text" : "@history 1 — keep conversation history on helper side"
        }
      },
      {
        "box" : {
          "id" : "obj-45",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 665.0, 400.0, 20.0],
          "text" : "@max_history 20 — max conversation turns to keep"
        }
      },
      {
        "box" : {
          "id" : "obj-46",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 685.0, 300.0, 20.0],
          "text" : "@timeout 30 — request timeout in seconds"
        }
      },
      {
        "box" : {
          "id" : "obj-50",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 730.0, 400.0, 20.0],
          "text" : "Set OPENAI_API_KEY in helper/.env before starting"
        }
      }
    ],
    "lines" : [
      {
        "patchline" : {
          "source" : ["obj-21", 0],
          "destination" : ["obj-30", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-22", 0],
          "destination" : ["obj-30", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-23", 0],
          "destination" : ["obj-30", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-24", 0],
          "destination" : ["obj-30", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-25", 0],
          "destination" : ["obj-30", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-30", 0],
          "destination" : ["obj-31", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-30", 1],
          "destination" : ["obj-32", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-30", 2],
          "destination" : ["obj-33", 0]
        }
      }
    ]
  }
}
