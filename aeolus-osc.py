import OSC

s = OSC.OSCServer(('192.168.77.7', 8080))
s.addMsgHandler('default', s.msgPrinter_handler)
s.serve_forever()
