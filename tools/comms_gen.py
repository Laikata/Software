#!/bin/python3
import sys
import tomli

file = open(sys.argv[1], "rb")
data = tomli.load(file)
file.close()


def sizeof(type) -> int:
    if type == "double":
        return 8
    elif type == "uint32_t":
        return 4
    elif type == "uint8_t":
        return 1

def send_header(data):
    print(f'void comms_{data["name"]}(', end="")
    for idx, field in enumerate(data["field"], start=1):
        print(field["type"], field["name"], end="")
        if idx < len(data["field"]):
            print(", ", end="")
    print(");")

def send_code(data):
    print(f"void comms_{data['name']}(", end="")
    for idx, field in enumerate(data["field"], start=1):
        print(field["type"], field["name"], end="")
        if idx < len(data["field"]):
            print(", ", end="")
    print(") {")

    print(f"static const int data_size = ", end="")
    for idx, field in enumerate(data["field"], start=1):
        print(f"sizeof({field['type']})", end="")
        if idx < len(data["field"]):
            print(" + ", end="")
    print(";")
    
    print("uint8_t data[data_size];")
    packet_pointer = 0
    for field in data["field"]:
        print(f"memcpy(&data[{packet_pointer}], &{field['name']}, sizeof({field['type']}));")
        packet_pointer += sizeof(field["type"])
    
    print("comms_send(data, data_size);")
    print("}")

def recv_header(data):
    for field in data["field"]:
        print(f"static {field['type']} {data['name']}_{field['name']};")


def recv_unpack_code(data):
    print(f"void comms_unpack_{data['name']}(uint8_t data[], ", end="")
    for idx, field in enumerate(data["field"], start=1):
        print(field["type"], f'*{field["name"]}', end="")
        if idx < len(data["field"]):
            print(", ", end="")
    print(") {")

    packet_pointer = 0
    for field in data["field"]:
        print(f"memcpy({field['name']}, &data[{packet_pointer}], sizeof({field['type']}));")
        packet_pointer += sizeof(field["type"])
    
    print("}")



# C Code
send_header(data)
send_code(data)
recv_header(data)
recv_unpack_code(data)