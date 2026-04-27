{
  "patcher" : {
    "fileversion" : 1,
    "appversion" : {
      "major" : 8,
      "minor" : 6,
      "revision" : 4
    },
    "classnamespace" : "box",
    "rect" : [100.0, 100.0, 850.0, 700.0],
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
    "description" : "Help file for bbb.agent.toolrouter",
    "digest" : "Route validated tool calls from LLM to Max messages",
    "tags" : "ai, tool, routing, help",
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
          "text" : "bbb.agent.toolrouter"
        }
      },
      {
        "box" : {
          "id" : "obj-2",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 55.0, 500.0, 20.0],
          "text" : "Route validated tool calls from LLM to Max messages"
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
          "patching_rect" : [50.0, 110.0, 400.0, 20.0],
          "text" : "inlet 0 (left): tool <name> <args>, schema, loadschema, list, clear"
        }
      },
      {
        "box" : {
          "id" : "obj-5",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 130.0, 400.0, 20.0],
          "text" : "inlet 1 (right): enable <name> <0|1>, allow <name>, deny <name>"
        }
      },
      {
        "box" : {
          "id" : "obj-6",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 165.0, 100.0, 20.0],
          "text" : "--- Outlets ---"
        }
      },
      {
        "box" : {
          "id" : "obj-7",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 185.0, 400.0, 20.0],
          "text" : "outlet 0 (left): <tool_name> <args...> — routed tool messages"
        }
      },
      {
        "box" : {
          "id" : "obj-8",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 205.0, 400.0, 20.0],
          "text" : "outlet 1 (middle): reject <reason> <tool_name>"
        }
      },
      {
        "box" : {
          "id" : "obj-9",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 225.0, 400.0, 20.0],
          "text" : "outlet 2 (right): status messages"
        }
      },
      {
        "box" : {
          "id" : "obj-20",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 260.0, 300.0, 20.0],
          "text" : "--- Example: Allow and route tools ---"
        }
      },
      {
        "box" : {
          "id" : "obj-21",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 290.0, 100.0, 22.0],
          "text" : "allow set_filter"
        }
      },
      {
        "box" : {
          "id" : "obj-22",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [160.0, 290.0, 100.0, 22.0],
          "text" : "allow play_sample"
        }
      },
      {
        "box" : {
          "id" : "obj-23",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [270.0, 290.0, 100.0, 22.0],
          "text" : "deny set_filter"
        }
      },
      {
        "box" : {
          "id" : "obj-30",
          "maxclass" : "newobj",
          "numinlets" : 2,
          "numoutlets" : 3,
          "outlettype" : ["", "", ""],
          "patching_rect" : [50.0, 350.0, 140.0, 22.0],
          "text" : "bbb.agent.toolrouter"
        }
      },
      {
        "box" : {
          "id" : "obj-31",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 410.0, 50.0, 22.0],
          "text" : "print route"
        }
      },
      {
        "box" : {
          "id" : "obj-32",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [170.0, 410.0, 55.0, 22.0],
          "text" : "print reject"
        }
      },
      {
        "box" : {
          "id" : "obj-33",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [300.0, 410.0, 55.0, 22.0],
          "text" : "print status"
        }
      },
      {
        "box" : {
          "id" : "obj-40",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 460.0, 300.0, 20.0],
          "text" : "--- Test tool calls ---"
        }
      },
      {
        "box" : {
          "id" : "obj-41",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 490.0, 250.0, 22.0],
          "text" : "tool set_filter {\"cutoff\":1200,\"resonance\":0.8}"
        }
      },
      {
        "box" : {
          "id" : "obj-42",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [320.0, 490.0, 220.0, 22.0],
          "text" : "tool unknown_tool {\"value\":1}"
        }
      },
      {
        "box" : {
          "id" : "obj-50",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 540.0, 200.0, 20.0],
          "text" : "--- Attributes ---"
        }
      },
      {
        "box" : {
          "id" : "obj-51",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 565.0, 400.0, 20.0],
          "text" : "@strict 0/1 — reject tools not in allowlist (default: 0)"
        }
      },
      {
        "box" : {
          "id" : "obj-60",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 610.0, 500.0, 40.0],
          "text" : "Usage: bbb.agent.chat → bbb.agent.toolrouter → [route tool1 tool2 ...]\nallow tools first, then tool calls from chat are routed or rejected"
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
          "source" : ["obj-41", 0],
          "destination" : ["obj-30", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-42", 0],
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
