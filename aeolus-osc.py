import OSC
import alsaseq
import alsamidi


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
# This API is nuts. Might as well use ctypes.


def handler(addr, tags, data, client_addr):
    print addr
    if addr.startswith("/aeolus/button"):
        group, button = [int(x) - 1 for x in addr.split('/')[3:5]]

        if data[0] == 0:
            mode = 0b01
        else:
            mode = 0b10
        val = (0b01 << 6) | (mode << 4) | group
        ev = (alsaseq.SND_SEQ_EVENT_CONTROLLER, 0, 0, 0, (0,0), (129,0), (128, 0), (1, 0, 0, 0, 98, val))
        alsaseq.output(ev)
        ev = (alsaseq.SND_SEQ_EVENT_CONTROLLER, 0, 0, 0, (0,0), (129,0), (128, 0), (1, 0,  0, 0,98, button))
        alsaseq.output(ev)

    elif addr == "/aeolus/cancel":
        num_groups = data[0]
        for g in range(0, num_groups):
            print g
            val = 0b01 << 6 | g
            ev = (alsaseq.SND_SEQ_EVENT_CONTROLLER, 0, 0, 0, (0,0), (129,0), (128, 0), (1, 0, 0, 0, 98, val))
            alsaseq.output(ev)
        

alsaseq.client('Aeolus-OSC', 0, 1, True)
alsaseq.start()
alsaseq.connectto(0, 128, 0)

s = OSC.OSCServer(('192.168.77.7', 8080))
s.addMsgHandler('default', handler)
s.serve_forever()

# TODO 
# . fix general cancel to not require a refresh
# . command-line args
# . autodetect aeolus
# . send to subscriptions
# . send using own client:port programmatically (instead of 129:0)
