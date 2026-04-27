{
  "patcher" : {
    "fileversion" : 1,
    "appversion" : {
      "major" : 8,
      "minor" : 6,
      "revision" : 4
    },
    "classnamespace" : "box",
    "rect" : [100.0, 100.0, 800.0, 700.0],
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
    "description" : "Help file for bbb.agent.ping",
    "digest" : "Ping helper service to measure round-trip latency",
    "tags" : "ai, network, diagnostic, help",
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
          "text" : "bbb.agent.ping"
        }
      },
      {
        "box" : {
          "id" : "obj-2",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 55.0, 500.0, 20.0],
          "text" : "Ping the helper service to measure round-trip latency via bbb.agent.hub"
        }
      },
      {
        "box" : {
          "id" : "obj-3",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 95.0, 150.0, 20.0],
          "text" : "--- Inlets ---"
        }
      },
      {
        "box" : {
          "id" : "obj-4",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 115.0, 400.0, 20.0],
          "text" : "inlet 0: ping, bang, pong (receive pong from hub)"
        }
      },
      {
        "box" : {
          "id" : "obj-5",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 150.0, 150.0, 20.0],
          "text" : "--- Outlets ---"
        }
      },
      {
        "box" : {
          "id" : "obj-6",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 170.0, 400.0, 20.0],
          "text" : "outlet 0: pong <ms>, latency <ms>, status <msg>"
        }
      },
      {
        "box" : {
          "id" : "obj-10",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 220.0, 300.0, 20.0],
          "text" : "--- Example: Round-trip ping via hub ---"
        }
      },
      {
        "box" : {
          "id" : "obj-11",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 245.0, 500.0, 20.0],
          "text" : "Requires bbb.agent.hub connected to helper service in the same patch"
        }
      },
      {
        "box" : {
          "id" : "obj-12",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 2,
          "outlettype" : ["", ""],
          "patching_rect" : [350.0, 280.0, 100.0, 22.0],
          "text" : "bbb.agent.hub"
        }
      },
      {
        "box" : {
          "id" : "obj-13",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [350.0, 250.0, 70.0, 22.0],
          "text" : "connect"
        }
      },
      {
        "box" : {
          "id" : "obj-14",
          "maxclass" : "button",
          "numinlets" : 1,
          "numoutlets" : 1,
          "outlettype" : ["bang"],
          "patching_rect" : [50.0, 280.0, 24.0, 24.0]
        }
      },
      {
        "box" : {
          "id" : "obj-15",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 320.0, 100.0, 22.0],
          "text" : "bbb.agent.ping"
        }
      },
      {
        "box" : {
          "id" : "obj-16",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 370.0, 34.0, 22.0],
          "text" : "print"
        }
      },
      {
        "box" : {
          "id" : "obj-17",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [90.0, 372.0, 200.0, 20.0],
          "text" : "← outputs: pong <ms>, latency <ms>"
        }
      },
      {
        "box" : {
          "id" : "obj-18",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [350.0, 370.0, 34.0, 22.0],
          "text" : "print"
        }
      },
      {
        "box" : {
          "id" : "obj-19",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 430.0, 300.0, 20.0],
          "text" : "--- Manual pong trigger (for testing) ---"
        }
      },
      {
        "box" : {
          "id" : "obj-20",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 455.0, 50.0, 22.0],
          "text" : "pong"
        }
      },
      {
        "box" : {
          "id" : "obj-30",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 510.0, 200.0, 20.0],
          "text" : "--- Attributes ---"
        }
      },
      {
        "box" : {
          "id" : "obj-31",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 535.0, 400.0, 20.0],
          "text" : "@interval (int ms) — auto-ping interval (default: 0 = manual only)"
        }
      },
      {
        "box" : {
          "id" : "obj-40",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 580.0, 400.0, 20.0],
          "text" : "Signal flow: bang → ping → hub.send(JSON) → helper → hub.data_out"
        }
      },
      {
        "box" : {
          "id" : "obj-41",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 600.0, 400.0, 20.0],
          "text" : "Route hub's data_out → pong message to get latency measurement"
        }
      },
      {
        "box" : {
          "id" : "obj-42",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 640.0, 400.0, 20.0],
          "text" : "Start helper: cd helper && npm run dev"
        }
      }
    ],
    "lines" : [
      {
        "patchline" : {
          "source" : ["obj-13", 0],
          "destination" : ["obj-12", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-14", 0],
          "destination" : ["obj-15", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-15", 0],
          "destination" : ["obj-16", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-12", 1],
          "destination" : ["obj-18", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-20", 0],
          "destination" : ["obj-15", 0]
        }
      }
    ]
  }
}
