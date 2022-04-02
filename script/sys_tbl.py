#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(description='tbl-parser.')
parser.add_argument("filename", help="file")
parser.add_argument("-o", "--output", help="output")
parser.add_argument("-t", "--type", default="hdr",help="output type: hdr|tbl")
args = parser.parse_args()


def read_file():
    ret = []
    with open(args.filename, 'r') as f:
        while True:
            line = f.readline()
            if not line:
                break
            else:
                fields = line.split()
                ret.append({'num':fields[0], 'name':fields[1], 'sys_func':fields[2]})
    return ret

tbl_formatter = "__SYS_CALL({num}, {sys_func})"
hdr_formatter = "#define {sys_func:12}{num}"

def gen_out(li):
    formatter = tbl_formatter if args.type == 'tbl' else hdr_formatter
    ret = []
    for item in li:
        ret.append(formatter.format(**item))
    return "\n".join(ret)



if __name__ == "__main__":
    content = gen_out(read_file())
    if args.output:
        with open(args.output, 'w') as f:
            f.write(content)
    else:
        print(content)