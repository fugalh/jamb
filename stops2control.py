#!/usr/bin/env python

# given a stops directory
# generate a Control interface for each instrument (stops/*/definition).
# http://charlie-roberts.com/Control/

# Aeolus instrument definitions have 
# Keyboards, Divisions, Interface Groups
# The primary concern is the Interface Groups

# Aeolus accepts MIDI control messages on controller #98. Summary of things
# that can be done (see Aeolus README section 6):
# . group select+reset (01mm0ggg where mm = 00)
# . group select+set on/off/toggle mode (01mm0ggg for mm = 01/10/11)
# . button select (nothing/on/off/toggle depending on group mode) (000bbbbb)

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
# In addition
# . instrument
# - MIDICTL_BANK (which bank)
# - program change (which preset)
# - MIDICTL_HOLD 
# - MIDICTL_ASOFF (All sound off) 
# - MIDICTL_ANOFF (All notes off)
#
# . division
# - swell
# - tremul frequency
# - tremul amplitude
#
# MIDI messages that are understood are code-documented in model.cc
# Model::proc_qmidi(). See also global.h
#
# So, interface groups are not necessarily the same as divisions, but I think
# as long as the names are the same we could get away with grouping a swell
# slider with that interface group. The "combination action" bits should also
# be on the main screen. The rest can be on a separate page (if anywhere).
# General cancel should exist of course.

# combination action
# http://www.organ.byu.edu/orpipe.html
# S for set
# 0 for general cancel
# M1-32 for bank (other page)
# 1-32 for presets. (all 32 on other page, just what's convenient on playing page)
#
# aeolus only has general presets, but it has multiple banks. of course if we
# feel inclined we could store preset info locally and do full combination
# action (or, hack aeolus to have per-division presets) 

# layout:
# basic unit is roughly 1em. Stop buttons are 4x2. There's a 1 unit gap between
# divisions (the division name goes in here too). The combination action at the
# bottom is 1 unit tall, and its buttons are 1x1.

import json
import os
import struct

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


    ux = 1.0 / 8
    uy = 0.9 / (5 + 9*2)
    x = 0
    y = 0

    # template
    playpage.append({
        "name": "generalCancel",
        "type": "Button",
        "bounds": [0, 0, 1, uy],
        "mode": "contact",
        "stroke": "#aaa",
        "address": "/aeolus/cancel",
        "max": 4,
        "label": "general cancel (and refresh interface)",
        # TODO clear all buttons on our screen, hack now is to just refresh interface
        "ontouchstart": "interfaceManager.refreshInterface()",
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
            }

    for g in aeolus.groups:
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
            playpage.append({
                'name': 'g%sb%s' % (g['index'], b['index']),
                'type': 'Button',
                'x': x, 'y': y,
                'width': ux, 'height': 2 * uy,
                'label': b['label'].replace("\n", " ").replace("- ", ""),
                'color': palette[b['type']]['fill'],
                'stroke': palette[b['type']]['stroke'],
                'address': '/aeolus/button/%d/%d' % (g['index'], b['index']),
                })
            x += ux
            if x >= 1:
                x = 0
                y += 2 * uy
        y += 2 * uy
        x = 0

    pages = [playpage]
    s = """
loadedInterfaceName = "%s";
interfaceOrientation = "landscape";

pages = %s;
    """ % (aeolus.label, json.dumps(pages, indent=2))
    print s

if __name__ == "__main__":
    import sys
    emit_control(sys.argv[1])
