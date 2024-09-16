#!/usr/bin/env python
import OSC
import alsaseq
import alsamidi
import os
import re

import logging as log

def backticks(cmd):
    # I know I should use subprocess, but I rage against the fact that it
    # doesn't have a simple way to do this.
    return os.popen(cmd).read()

class Aeolus(object):
    SUBSCRIBERS = (254, 0)
    def __init__(self, addr=None, channel=1):
        self.ctrl_param = 98
        self.channel = channel - 1

        alsaseq.client('Aeolus-OSC', 0, 1, True)
        alsaseq.start()
        self.src = (alsaseq.id(), 0)
        log.info('I am %s:%s' % (str(self.src[0]), str(self.src[1])))

        self.update_connection(addr)

        # from help(alsaseq):
        #
        #     ALSA events are returned as a tuple with 8 elements:
        #         (type, flags, tag, queue, time stamp, source, destination, data)
        #
        #     Some elements are also tuples:
        #         time = (seconds, nanoseconds)
        #         source, destination = (client, port)
        #         data = ( varies depending on type )
        #
        #     See DATA section below for event type constants.
        #
        # This API is nuts.

        # TODO should send events to subscribed ports not directly to aeolus

        self.presets = None

    def send_event(self, ev):
        log.debug(ev)
        alsaseq.output(ev)

    def send_control_event(self, value):
        ev = (alsaseq.SND_SEQ_EVENT_CONTROLLER, 0, 0, 0, (0,0), self.src, Aeolus.SUBSCRIBERS, (self.channel, 0, 0, 0, self.ctrl_param, value))
        self.send_event(ev)

    def send_program_change(self, program):
        ev = (alsaseq.SND_SEQ_EVENT_PGMCHANGE, 0, 0, 0, (0,0), self.src, Aeolus.SUBSCRIBERS, (self.channel, 0, 0, 0, 0, program))
        self.send_event(ev)

    # on startup: subscribe to first Aeolus (unless param)
    # on client start event: if client is Aeolus and port is 0, subscribe
    def update_connection(self, addr=None):
        def do_connect(addr):
            log.info('Connecting to %d:%d' % addr)
            alsaseq.connectto(0, addr[0], addr[1])

        # The alsaseq API doesn't expose a way to get the client name, and I
        # don't want to extend it because its API is nuts anyway and I'd rather
        # do a fresh ctypes interface. But for now, I'll just use a pipeline
        #
        # XXX I think this is slow enough to produce noticeable delay in
        # assigning all the stops at once, e.g. on presets. If it's annoying
        # I'll have to do something more performant. Maybe reading the file
        # directly and processing it in python is fast enough. Especially if we
        # keep the file open and seek(0) every time.
        connected = ('' != backticks('grep -B2 "Connected From: .*%d:0" /proc/asound/seq/clients | head -n1 | grep \'"aeolus"\'' % self.src[0]))
        if not connected:
            if addr:
                do_connect(addr)
            else:
                client_id = backticks('grep aeolus /proc/asound/seq/clients | head -n1 | cut -d" " -f2')
                if client_id != '':
                    do_connect((int(client_id), 0))


class Server(object):
    def __init__(self, aeolus, addr=('0.0.0.0', 8080)):
        self.aeolus = aeolus
        self.osc = OSC.OSCServer(('192.168.77.7', 8080))
        self.osc.addMsgHandler('default', self.handler)

    def run(self):
        self.osc.serve_forever()

    def handler(self, addr, tags, data, client_addr):
        self.aeolus.update_connection()

        log.debug('%s %s', addr, data)
        if addr.startswith("/aeolus/button"):
            group, button = [int(x) - 1 for x in addr.split('/')[3:5]]

            if data[0] == 0:
                mode = 0b01
            else:
                mode = 0b10
            val = (0b01 << 6) | (mode << 4) | group
            self.aeolus.send_control_event(val)
            self.aeolus.send_control_event(button)

        elif addr.startswith("/aeolus/preset"):
            if data != [0]:
                self.aeolus.send_program_change(int(addr.split('/')[3]) - 1)

        elif addr == "/aeolus/cancel":
            num_groups = data[0]
            for g in range(0, num_groups):
                val = 0b01 << 6 | g
                self.aeolus.send_control_event(val)
        
if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-p', '--port', type=int, default=8080,
            help='Port on which to listen for OSC messages')
    parser.add_argument('-A', '--aeolus', default=None,
            help='Explicit ALSA sequencer client:port on which Aeolus listens')
    parser.add_argument('-c', '--control-channel', type=int, default=1,
            help='MIDI channel on which Aeolus expects control messages')
    parser.add_argument('-v', '--verbose', action='store_true')
    opts = parser.parse_args()

    if opts.verbose:
        log.basicConfig(level=log.DEBUG)
    
    aeolus = Aeolus(opts.aeolus, channel=opts.control_channel)
    osc = Server(aeolus, ('0.0.0.0', opts.port))
    osc.run()

# TODO 
# . better log formatter
