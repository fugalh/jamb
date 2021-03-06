#!/usr/bin/env python

# Generate a Control interface for an Aeolus instrument
# http://charlie-roberts.com/Control/

# Aeolus instrument definitions have 
# Keyboards, Divisions, Interface Groups
# The primary concern is the Interface Groups

# Stuff in the definition file:
# /divis/new $label $keyboard $section
# /rank $pan $delay $filename
# /swell (make a slider)
#
# /group/new $label
# /stop $keyboard $division $rank (xref with /rank for filename, and extract the label from there)
# /tremul $division $mnemonic $label
# /coupler $keyboard $division $mnemonic $label
#
# So, interface groups are not necessarily the same as divisions, but I think
# as long as the names are the same we could get away with grouping a swell
# slider with that interface group. (TODO)

# layout:
# basic unit is roughly 1em. Stop buttons are 4x2. There's a 1 unit gap between
# divisions (the division name goes in here too). The combination action at the
# bottom is 1 unit tall, and its buttons are 1x1.

import json
import os
import struct
import time

class AeolusInstrument(object):
    def __init__(self, instrument_dir):
        self.label = os.path.basename(instrument_dir)
        self.keyboards = []
        self.divisions = []
        self.groups = []
        self.dirname = instrument_dir

        # Aeolus indices are 1-based
        ki = 1
        di = 1
        ri = None
        gi = 1
        bi = None

        d = None
        g = None

        for line in open(instrument_dir + '/definition'):
            line = line.strip()
            if line == "":
                continue

            tokens = line.split()
            t = tokens[0]
            if t == '/tuning':
                self.tuning = {
                    'base': float(tokens[1]),
                    'itemp': int(tokens[2])
                    }

            elif t == '/manual/new' or t == '/pedal/new':
                self.keyboards.append({
                    'type': t.split("/")[1],
                    'label': tokens[1],
                    'index': ki,
                    })
                ki += 1

            elif t == '/divis/new':
                d = {
                    'label': tokens[1],
                    'index': di,
                    'ranks': [],
                    'swell': False,
                    }
                di += 1
                ri = 1
                self.divisions.append(d)

            elif t == '/rank':
                d['ranks'].append({
                    'index': ri,
                    'pan': tokens[1],
                    'delay': int(tokens[2]),
                    'filename': tokens[3]
                    })
                ri += 1

            elif t == '/swell':
                d['swell'] = True

            elif t == '/divis/end':
                d = None
                ri = None

            elif t == '/group/new':
                g = {
                    'label': tokens[1],
                    'index': gi,
                    'buttons': [],
                    }
                gi += 1
                bi = 1
                self.groups.append(g)

            elif t == '/stop':
                division = int(tokens[2])
                rank = int(tokens[3])

                rank_filename = self.divisions[division - 1]['ranks'][rank - 1]['filename']
                label = None
                mnemonic = None
                with open(self.dirname + '/../' + rank_filename, 'r') as f:
                    f.seek(32)
                    label = f.read(32)
                    label = label[0:label.find('\0')]
                    label = label.replace('$', '\n')

                    f.seek(32+32+56)
                    mnemonic = f.read(8)
                    mnemonic = mnemonic[0:mnemonic.find('\0')]
                    mnemonic = mnemonic.replace('$', '\n')

                g['buttons'].append({
                    'type': 'stop',
                    'index': bi,
                    'keyboard': int(tokens[1]),
                    'division': division,
                    'rank': rank,
                    'label': label,
                    'mnemonic': mnemonic,
                    })
                bi += 1

            elif t == '/tremul':
                if g is not None:
                    g['buttons'].append({
                        'type': 'tremul',
                        'index': bi,
                        'division': int(tokens[1]),
                        'mnemonic': tokens[2],
                        'label': tokens[3],
                        })
                    bi += 1

            elif t == '/coupler':
                g['buttons'].append({
                    'type': 'coupler',
                    'index': bi,
                    'keyboard': int(tokens[1]),
                    'division': int(tokens[2]),
                    'mnemonic': tokens[3],
                    'label': tokens[4],
                    })
                bi += 1

            elif t == '/group/end':
                g = None
                bi = None

            elif t == '/instr/end':
                break

def emit_control(instrument_dir):
    aeolus = AeolusInstrument(instrument_dir)
    playpage = []
    groups = []


    ux = 1.0 / 8
    uy = 0.9 / (5 + 9*2)
    x = 0
    y = 0

    # template
    playpage.append({
        "name": "refresh",
        "type": "Button",
        "bounds": [0, 0, 0.5, uy],
        "startingValue": 0,
        "isLocal": True,
        "mode": "contact",
        "ontouchstart": "interfaceManager.refreshInterface()",
        "stroke": "#aaa",
        "label": "refresh",
        })
    playpage.append({
        "name": "tabButton",
        "type": "Button",
        "bounds": [0.5, 0, 0.5, uy],
        "mode": "toggle",
        "stroke": "#aaa",
        "isLocal": True,
        "ontouchstart": "if(this.value == this.max) { control.showToolbar(); } else { control.hideToolbar(); }",
        "label": "menu",
        })

    y += uy

    palette = {
            'stop': {
                'fill': '#202040',
                'stroke': '#eee'
                },
            'tremul': {
                'fill': '#204020',
                'stroke': '#5f5'
                },
            'coupler': {
                'fill': '#403020',
                'stroke': '#fa5'
                },
            'piston': {
                'fill': '#202040',
                'stroke': '#eee'
                },
            }

    for g in aeolus.groups:
        group = []
        playpage.append({
            'name': 'g%s' % g['index'],
            'type': 'Label',
            'x': 0, 'y': y,
            'width': 1, 'height': uy,
            'align': 'left',
            'value': g['label'],
            })
        y += uy
        for b in g['buttons']:
            name = 'g%sb%s' % (g['index'], b['index'])
            group.append(name)
            playpage.append({
                'name': name,
                'type': 'Button',
                'x': x, 'y': y,
                'width': ux, 'height': 2 * uy,
                'label': b['label'].replace("\n", " ").replace("- ", ""),
                'color': palette[b['type']]['fill'],
                'stroke': palette[b['type']]['stroke'],
                'address': '/aeolus/button/%d/%d' % (g['index'], b['index']),
                })
            x += ux
            if x > 1:
                x = 0
                y += 2 * uy
        groups.append(group)
        y += 2 * uy
        x = 0

    playpage.append({
        'type': 'Label',
        'name': 'statusBar',
        'value': 'Build time: ' + time.strftime("%Y-%m-%d %H:%M:%S"),
        'bounds': [x, y, 1, uy],
        })

    y += uy

    playpage.append({
        "name": "generalCancel",
        "type": "Button",
        "bounds": [x, y, ux/2, 2 * uy],
        "mode": "momentary",
        'color': palette['piston']['fill'],
        'stroke': palette['piston']['stroke'],
        "address": "/aeolus/cancel",
        "max": len(aeolus.groups),
        "label": "0",
        "ontouchend": "general_cancel()",
        # until we can get info from aeolus directly this is the easiest way
        # to maintain some semblance of synchronization.
        'oninit': 'general_cancel()',
        })
    x += ux/2

    for i in range(1, 15):
        playpage.append({
            'name': 'preset%d' % i,
            'label': str(i),
            'type': 'Button',
            'bounds': [x, y, ux/2, 2*uy],
            'mode': 'momentary',
            'color': palette['piston']['fill'],
            'stroke': palette['piston']['stroke'],
            'isLocal': True,
            'ontouchstart': 'preset_action(%d)' % i,
            })
        x += ux/2

    playpage.append({
        'name': 'setButton',
        'type': 'Button',
        'label': 'S',
        'bounds': [x, y, ux/2, 2*uy],
        'mode': 'toggle',
        'color': palette['piston']['fill'],
        'stroke': palette['piston']['stroke'],
        'isLocal': True,
        })

    pages = [playpage]
    json.dumps(pages, indent=2)
    s = open('aeolus.js.template', 'r').read().format(
            name = aeolus.label,
            pages = json.dumps(pages, indent=2),
            groups = json.dumps(groups, indent=2),
            )
    return s

if __name__ == "__main__":
    import sys
    print emit_control(sys.argv[1])

# TODO
# . refactor javascript to store objects in preamble and make references to
#   those in pages array, so it's easier to find only the groups/buttons
# . store presets in lawnchair.
#   var aeolus = Lawnchair('Aeolus')
#   obj = {
#     # a preset is a list of groups, which are lists of buttons, which are just on or off (or maybe 1/0 - whatever the value of the buttons is)
#     'Aeolus': {'presets': [[[true, false, ...]], ...], ...},
#     'Another Instrument': {...},
#   aeolus.save(obj, callback)
