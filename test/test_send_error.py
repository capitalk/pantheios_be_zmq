import zmq
import time

# Create context and connect
context = zmq.Context()
socket = context.socket(zmq.PUB)

#socket.connect("tcp://127.0.0.1:5556")
socket.bind("tcp://127.0.0.1:212121")

while True:
    socket.send_multipart(["ERROR", "error test"])
    time.sleep(1)

