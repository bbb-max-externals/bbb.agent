{
  "patcher" : {
    "fileversion" : 1,
    "appversion" : {
      "major" : 8,
      "minor" : 6,
      "revision" : 4
    },
    "classnamespace" : "box",
    "rect" : [100.0, 100.0, 900.0, 750.0],
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
    "description" : "Help file for bbb.agent.hub",
    "digest" : "WebSocket connection manager for bbb.agent helper service",
    "tags" : "ai, network, connection, help",
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
          "text" : "bbb.agent.hub"
        }
      },
      {
        "box" : {
          "id" : "obj-2",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 55.0, 600.0, 20.0],
          "text" : "WebSocket connection manager for bbb.agent helper service"
        }
      },
      {
        "box" : {
          "id" : "obj-3",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 100.0, 150.0, 20.0],
          "text" : "--- Inlets ---"
        }
      },
      {
        "box" : {
          "id" : "obj-4",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 120.0, 500.0, 20.0],
          "text" : "inlet 0: commands (connect, disconnect, restart, status, apikey, send)"
        }
      },
      {
        "box" : {
          "id" : "obj-5",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 155.0, 150.0, 20.0],
          "text" : "--- Outlets ---"
        }
      },
      {
        "box" : {
          "id" : "obj-6",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 175.0, 400.0, 20.0],
          "text" : "outlet 0 (left): status events (status connected/disconnected/error)"
        }
      },
      {
        "box" : {
          "id" : "obj-7",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 195.0, 400.0, 20.0],
          "text" : "outlet 1 (right): response data (data <json_string>)"
        }
      },
      {
        "box" : {
          "id" : "obj-8",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 240.0, 200.0, 20.0],
          "text" : "--- Connection Control ---"
        }
      },
      {
        "box" : {
          "id" : "obj-10",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 280.0, 70.0, 22.0],
          "text" : "connect"
        }
      },
      {
        "box" : {
          "id" : "obj-11",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [130.0, 280.0, 80.0, 22.0],
          "text" : "disconnect"
        }
      },
      {
        "box" : {
          "id" : "obj-12",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [220.0, 280.0, 70.0, 22.0],
          "text" : "restart"
        }
      },
      {
        "box" : {
          "id" : "obj-13",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [300.0, 280.0, 60.0, 22.0],
          "text" : "status"
        }
      },
      {
        "box" : {
          "id" : "obj-20",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 2,
          "outlettype" : ["", ""],
          "patching_rect" : [170.0, 340.0, 100.0, 22.0],
          "text" : "bbb.agent.hub"
        }
      },
      {
        "box" : {
          "id" : "obj-21",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [100.0, 420.0, 34.0, 22.0],
          "text" : "print"
        }
      },
      {
        "box" : {
          "id" : "obj-22",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [20.0, 420.0, 70.0, 20.0],
          "text" : "status:"
        }
      },
      {
        "box" : {
          "id" : "obj-23",
          "maxclass" : "newobj",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [280.0, 420.0, 34.0, 22.0],
          "text" : "print"
        }
      },
      {
        "box" : {
          "id" : "obj-24",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [240.0, 420.0, 40.0, 20.0],
          "text" : "data:"
        }
      },
      {
        "box" : {
          "id" : "obj-30",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 480.0, 200.0, 20.0],
          "text" : "--- Send JSON Example ---"
        }
      },
      {
        "box" : {
          "id" : "obj-31",
          "maxclass" : "message",
          "numinlets" : 2,
          "numoutlets" : 1,
          "outlettype" : [""],
          "patching_rect" : [50.0, 510.0, 300.0, 22.0],
          "text" : "send {\"type\":\"ping\",\"id\":\"test\"}"
        }
      },
      {
        "box" : {
          "id" : "obj-40",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 570.0, 200.0, 20.0],
          "text" : "--- Attributes ---"
        }
      },
      {
        "box" : {
          "id" : "obj-41",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 595.0, 300.0, 20.0],
          "text" : "@host (symbol) — helper hostname (default: localhost)"
        }
      },
      {
        "box" : {
          "id" : "obj-42",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 615.0, 300.0, 20.0],
          "text" : "@port (int) — helper port (default: 8765)"
        }
      },
      {
        "box" : {
          "id" : "obj-43",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 635.0, 300.0, 20.0],
          "text" : "@reconnect (int 0/1) — auto-reconnect (default: 1)"
        }
      },
      {
        "box" : {
          "id" : "obj-44",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 655.0, 400.0, 20.0],
          "text" : "@reconnect_interval (int ms) — reconnect delay (default: 3000)"
        }
      },
      {
        "box" : {
          "id" : "obj-50",
          "maxclass" : "comment",
          "numinlets" : 1,
          "numoutlets" : 0,
          "patching_rect" : [50.0, 690.0, 400.0, 20.0],
          "text" : "Start helper: cd helper && npm run dev"
        }
      }
    ],
    "lines" : [
      {
        "patchline" : {
          "source" : ["obj-10", 0],
          "destination" : ["obj-20", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-11", 0],
          "destination" : ["obj-20", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-12", 0],
          "destination" : ["obj-20", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-13", 0],
          "destination" : ["obj-20", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-20", 0],
          "destination" : ["obj-21", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-20", 1],
          "destination" : ["obj-23", 0]
        }
      },
      {
        "patchline" : {
          "source" : ["obj-31", 0],
          "destination" : ["obj-20", 0]
        }
      }
    ]
  }
}
