import OSC
import alsaseq
import alsamidi
import logging as log


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


src = (129,0)
dst = (128,0)
channel = 1
control_parameter = 98

def send_event(ev):
    log.debug(ev)
    alsaseq.output(ev)

def send_control_event(value):
    ev = (alsaseq.SND_SEQ_EVENT_CONTROLLER, 0, 0, 0, (0,0), src, dst, (channel, 0, 0, 0, control_parameter, value))
    send_event(ev)

def send_program_change(program):
    ev = (alsaseq.SND_SEQ_EVENT_PGMCHANGE, 0, 0, 0, (0,0), src, dst, (channel, 0, 0, 0, 0, program))

    # Frustrating, that doesn't work either. TODO fix aeolus
    ## apparently aeolus is not listening to program change, though it is in the
    ## case statement in proc_qmidi(). Luckily it's also listening to low notes
    ## to change programs.
    #note = program + 24
    #ev = (alsaseq.SND_SEQ_EVENT_NOTEON, 0, 0, 0, (0,0), src, dst, (channel, note, 1, 0, 0))

    send_event(ev)

def handler(addr, tags, data, client_addr):
    log.info((addr, data))
    if addr.startswith("/aeolus/button"):
        group, button = [int(x) - 1 for x in addr.split('/')[3:5]]

        if data[0] == 0:
            mode = 0b01
        else:
            mode = 0b10
        val = (0b01 << 6) | (mode << 4) | group
        send_control_event(val)
        send_control_event(button)

    elif addr.startswith("/aeolus/preset"):
        if data != [0]:
            send_program_change(int(addr.split('/')[3]) - 1)

    elif addr == "/aeolus/cancel":
        num_groups = data[0]
        for g in range(0, num_groups):
            val = 0b01 << 6 | g
            send_control_event(val)
        

log.basicConfig(level=log.DEBUG)

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
