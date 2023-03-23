#!/bin/python3
import sys
import tomli

datas = []
for arg in sys.argv[1:]:
    file = open(arg, "rb")
    datas.append(tomli.load(file))
    file.close()


def sizeof(type) -> int:
    if type == "double":
        return 8
    elif type == "vec3_t":
        return 12
    elif type == "float":
        return 4
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

    print(f"\tstatic const int data_size = ", end="")
    for idx, field in enumerate(data["field"], start=1):
        print(f"sizeof({field['type']})", end="")
        if idx < len(data["field"]):
            print(" + ", end="")
    print(";")
    
    print("\tuint8_t data[data_size];")
    packet_pointer = 0
    for field in data["field"]:
        print(f"\tmemcpy(&data[{packet_pointer}], &{field['name']}, sizeof({field['type']}));")
        packet_pointer += sizeof(field["type"])
    
    print("\tcomms_send(data, data_size);")
    print("}")

def recv_unpack_code(data):
    print(f"void comms_unpack_{data['name']}(uint8_t data[], ", end="")
    for idx, field in enumerate(data["field"], start=1):
        print(field["type"], f'*{field["name"]}', end="")
        if idx < len(data["field"]):
            print(", ", end="")
    print(") {")

    packet_pointer = 0
    for field in data["field"]:
        print(f"\tmemcpy({field['name']}, data + {packet_pointer}, sizeof({field['type']}));")
        packet_pointer += sizeof(field["type"])
    
    print("}")

def recv_header(datas):
    print("typedef enum {none = 0, ", end="")
    for idx, data in enumerate(datas, start=1):
        print(f"{data['name']} = {data['header']}", end="")
        if idx < len(datas):
            print(", ", end="")
    print("} PacketType;")

    print("PacketType comms_recv();")

    for data in datas:
        for field in data["field"]:
            print(f"static {field['type']} {data['name']}_{field['name']};")

def recv_code(datas):
    print("""PacketType comms_recv() { \n\tuint8_t start = 0x00;\n\tread(&start, 1);
\tif(start == 0x16) {
\t\tuint8_t buffer[2];
\t\tread(buffer, 2);
\t\tuint8_t header = buffer[1];""")
    first_if = True
    for data in datas:
        if first_if:
            print(f"\t\tif (header == {data['header']}) {{")
            first_if = False
        else:
            print(f"\t\telse if (header == {data['header']}) {{")
        size = 0
        for field in data['field']:
            size += sizeof(field['type'])
        print(f"\t\t\tuint8_t data[{size} + 4];\n\t\t\tread(data, {size} + 4);\n\t\t\tif(check_crc(data, {size}, data + {size})){{ return none; }}")
        print(f"\t\t\tcomms_unpack_{data['name']}(data, ", end="")
        for idx, field in enumerate(data['field'], start=1):
            print(f"&{data['name']}_{field['name']}", end="")
            if idx < len(data['field']):
                print(", ", end="")
        print(");")
        print("\t\t}")
    print("\t}\n\treturn none;\n}")




# C Code
recv_header(datas)
recv_code(datas)
for data in datas:
    send_header(data)
    send_code(data)
    recv_unpack_code(data)