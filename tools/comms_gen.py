#!/bin/python3
import sys
import tomli
import cgen as c

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

def to_value(field):
    return c.Value(field["type"], field["name"])

def to_pointer(field):
    return c.Pointer(c.Value(field["type"], field["name"]))

def send_header(data):
    args = map(to_value, data["field"])
    print(c.FunctionDeclaration(c.Value("void", f"comms_{data['name']}"), args)) 

def send_code(data):
    func = c.FunctionBody(
        c.FunctionDeclaration(c.Value("void", f"comms_{data['name']}"), map(to_value, data["field"])),
        c.Block()
        )

    data_size = "static const int data_size = "
    for idx, field in enumerate(data["field"], start=1):
        data_size += f"sizeof({field['type']})"
        if idx < len(data["field"]):
            data_size += " + "
    func.body.contents.append(c.Statement(data_size))
    
    func.body.contents.append(c.ArrayOf(c.Value("uint8_t", "data"), count="data_size"))
    packet_pointer = 0
    for field in data["field"]:
        func.body.contents.append(c.Statement(f"memcpy(&data[{packet_pointer}], &{field['name']}, sizeof({field['type']}))"))
        packet_pointer += sizeof(field["type"])
    
    func.body.contents.append(c.Statement(f"comms_send(data, data_size, {data['header']})"))
    print(func)

def recv_unpack_code(data):
    args = list(map(to_pointer, data["field"]))
    func = c.FunctionBody(
        c.FunctionDeclaration(c.Value("void", f"comms_unpack_{data['name']}"), [c.ArrayOf(c.Value("uint8_t", "data"))] + args), 
        c.Block()
        )
    
    packet_pointer = 0
    for field in data["field"]:
        func.body.contents.append(c.Statement(f"memcpy({field['name']}, data + {packet_pointer}, sizeof({field['type']}))"))
        packet_pointer += sizeof(field["type"])
    
    print(func)

def recv_header(datas):
    packet_enum = c.Enum()
    print("typedef enum {packet_error_crc = -1, packet_none = 0, ", end="")
    for idx, data in enumerate(datas, start=1):
        print(f"packet_{data['name']} = {data['header']}", end="")
        if idx < len(datas):
            print(", ", end="")
    print("} packet_t;")

    print("packet_t comms_recv();")

    for data in datas:
        for field in data["field"]:
            print(f"extern {field['type']} {data['name']}_{field['name']};")

def recv_code(datas):
    func = c.FunctionBody(
        c.FunctionDeclaration(c.Value("packet_t", "comms_recv"), []), 
        c.Block([
            c.Initializer(c.Value("uint8_t", "start"), 0),
            c.Statement("read(&start, 1)"),
            c.If("start == 0x16",
                c.Block([
                    c.ArrayOf(c.Value("uint8_t", "buffer"), count=2),
                    c.Statement("read(buffer, 2)"),
                    c.Initializer(c.Value("uint8_t", "header"), "buffer[1]")
                ]))
        ]))

    first_if = True
    for data in datas:
        working_if = None
        if first_if:
            func.body.contents[2].then_.append(
                c.If(f"header == {data['header']}", c.Block())
            )
            first_if = False
            working_if = func.body.contents[2].then_.contents[-1]
        else:
            deepest_if = func.body.contents[2].then_.contents[-1]
            while not (deepest_if.else_ is None):
                deepest_if = deepest_if.else_
            deepest_if.else_ = c.If(f"header == {data['header']}", then_=c.Block())
            working_if = deepest_if.else_

        size = 0
        for field in data['field']:
            size += sizeof(field['type'])

        unpack_call = f"comms_unpack_{data['name']}(data, "
        for idx, field in enumerate(data['field'], start=1):
            unpack_call += f"&{data['name']}_{field['name']}"
            if idx < len(data['field']):
                unpack_call += ", "
        unpack_call += (")")


        working_if.then_ = c.Block([
        c.ArrayOf(c.Value("uint8_t", "data"), count=f"{size} + 4"),
        c.Statement(f"read(data, {size} + 4)"),
        c.If(f"!check_crc(data, {size}, data + {size})", 
            c.Statement("return packet_error_crc")),
        c.Statement(unpack_call),
        c.Statement(f"return packet_{data['name']}")
        ])

    func.body.contents.append(c.Statement("return packet_none"))
    print(func)



print("comms.h:\n")
recv_header(datas)
for data in datas:
    send_header(data)
print("comms.cpp:\n")
for data in datas:
    send_code(data)
    recv_unpack_code(data)
recv_code(datas)
