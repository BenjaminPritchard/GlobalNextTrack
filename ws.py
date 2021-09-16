# ws.py
# This little script is a websocket server that listens on port 9899
# The tamperMonkey script connects to this server.
# And listens for commands from this script.
# This script reads from Stdin, and sends commands to the tamperMonkey script.
# Finally, the C code executes this script, listens for global hotkeys, and writes to our
# stdin.
# Benjamin Pritchard / Kundalini Software
# Sept. 2021

from simple_websocket_server import WebSocketServer, WebSocket
from threading import Thread
import time
import select
import sys

class SimpleEcho(WebSocket):

    # just read from stdin, looking for simple commands
    # (these will come from globalHotKey.c, which spawns this program... and then sends it commands via stdin when a hotkey is pressed)
    def readStdin(self):
        while True:
            # this will just block forever (unit there is some input to read from stdin)
            if select.select([sys.stdin, ], [], [])[0]:
                stdInInput = sys.stdin.readline().strip()
                if stdInInput == "next":
                    # hack: send over some junk data??
                    # i don't know why this would be needed, but it is to flush the buffer or something?? i don't get it.
                    # if this is not here, the other side doesn't get every other message??
                    self.send_message("junk??")
                    # send over real message
                    self.send_message("next")

                elif stdInInput == "prev":
                    # hack: send over some junk data??
                    # i don't know why this would be needed, but it is to flush the buffer or something?? i don't get it.
                    # if this is not here, the other side doesn't get every other message??
                    self.send_message("junk??")
                    # send over real message
                    self.send_message("prev")

    def connected(self):
        print(self.address, 'connected')
        Thread(target=self.readStdin).start()

    def handle_close(self):
        print(self.address, 'closed')


# setup a websocker server on port 9899
# listen for incoming connection requests from tampermonkey
server = WebSocketServer('localhost', 9899, SimpleEcho)
server.serve_forever()
