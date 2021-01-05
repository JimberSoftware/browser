#!/usr/bin/env python3

import yaml
import os

JS_PATH= [ "../client/js/Proto.js" ]
CPP_PATH= [ "../qpa/src/proto.h", "../browser/src/proto.h" ]

def generatecpp():
    print("Generating C++ library")
    with open(r'commands.yaml') as file:
        # The FullLoader parameter handles the conversion from YAML
        # scalar values to Python the dictionary format
        enums_to_generate = yaml.load(file, Loader=yaml.FullLoader)
        out = ""
        for enum in enums_to_generate:
            values = enums_to_generate[enum]
            out += "enum class {} : int {{".format(enum)
            out += "\n"
            counter = 0
            for value in values:
                counter += 1
                out += "{} = {},".format(value, counter)
                out += "\n"
            out += "};"
            out += "\n"
        for file in CPP_PATH:
            code_file = open(file, "w")
            n = code_file.write(out)
            code_file.close()
            print("Wrote proto.h in "+file)

def generatejs():
    print("Generating JS library")
    with open(r'commands.yaml') as file:
        # The FullLoader parameter handles the conversion from YAML
        # scalar values to Python the dictionary format
        enums_to_generate = yaml.load(file, Loader=yaml.FullLoader)
        out = ""
        for enum in enums_to_generate:
            values = enums_to_generate[enum]
            out += "export const {} = {{".format(enum)
            out += "\n"
            counter = 0
            for value in values:
                counter += 1
                out += "\t{}: {},".format(value, counter)
                out += "\n"
            out += "}"
            out += "\n"
        for file in JS_PATH:
            code_file = open(file, "w")
            n = code_file.write(out)
            code_file.close()

generatejs()
generatecpp()
