import OSC
import alsaseq
import alsamidi

import logging as log

class Aeolus(object):
    def __init__(self, addr='128:0', channel=0):
        self.ctrl_param = 98
        self.dst = tuple([int(i) for i in addr.split(':')])
        self.channel = channel - 1

        alsaseq.client('Aeolus-OSC', 0, 1, True)
        alsaseq.start()
        alsaseq.connectto(0, self.dst[0], self.dst[1])
        self.src = (alsaseq.id(), 0)
        log.info('Output port %s:%s' % (str(self.src[0]), str(self.src[1])))

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
        ev = (alsaseq.SND_SEQ_EVENT_CONTROLLER, 0, 0, 0, (0,0), self.src, self.dst, (self.channel, 0, 0, 0, self.ctrl_param, value))
        self.send_event(ev)

    def send_program_change(self, program):
        ev = (alsaseq.SND_SEQ_EVENT_PGMCHANGE, 0, 0, 0, (0,0), self.src, self.dst, (self.channel, 0, 0, 0, 0, program))
        self.send_event(ev)

    def parse_presets(self, filename):
        # 14 bytes starting with 'PRESET\0'
        # 2 bytes, ((p)[0] + ((p)[1] << 8))) is ngroup
        # 256 bytes of midi channel info
        # for each preset
        #   4 bytes: bank, preset, 0, 0 
        #   for each group
        #       4-byte bitmasks ((p)[0] + ((p)[1] << 8) + ((p)[2] << 16) + ((p)[3] << 24))
        pass

class Server(object):
    def __init__(self, addr=('0.0.0.0', 8080)):
        self.osc = OSC.OSCServer(('192.168.77.7', 8080))
        self.osc.addMsgHandler('default', self.handler)

    def run(self):
        self.osc.serve_forever()

    def handler(self, addr, tags, data, client_addr):
        log.debug('%s %s', addr, data)
        if addr.startswith("/aeolus/button"):
            group, button = [int(x) - 1 for x in addr.split('/')[3:5]]

            if data[0] == 0:
                mode = 0b01
            else:
                mode = 0b10
            val = (0b01 << 6) | (mode << 4) | group
            aeolus.send_control_event(val)
            aeolus.send_control_event(button)

        elif addr.startswith("/aeolus/preset"):
            if data != [0]:
                aeolus.send_program_change(int(addr.split('/')[3]) - 1)

        elif addr == "/aeolus/cancel":
            num_groups = data[0]
            for g in range(0, num_groups):
                val = 0b01 << 6 | g
                aeolus.send_control_event(val)
        
if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-p', '--port', type=int, default=8080,
            help='Port on which to listen for OSC messages')
    parser.add_argument('-A', '--aeolus', default='128:0',
            help='ALSA sequencer client:port on which Aeolus listens')
    parser.add_argument('-c', '--control-channel', type=int, default=1,
            help='MIDI channel on which Aeolus expects control messages')
    parser.add_argument('-v', '--verbose', action='store_true')
    opts = parser.parse_args()

    if opts.verbose:
        log.basicConfig(level=log.DEBUG)
    
    aeolus = Aeolus(opts.aeolus, channel=opts.control_channel)
    osc = Server(('0.0.0.0', opts.port))
    osc.run()

# TODO 
# . autodetect aeolus
# . send to subscriptions
# . better log formatter
