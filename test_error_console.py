import zmq

# Create context and connect
context = zmq.Context()
socket = context.socket(zmq.SUB)

#socket.bind("tcp://127.0.0.1:5556")
socket.connect("tcp://127.0.0.1:212121")

socket.setsockopt(zmq.SUBSCRIBE, '')

while True:
    [level, msg] = socket.recv_multipart()
    print level, "-", msg

