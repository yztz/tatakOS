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
            if not line or line.startswith('#') or line.startswith('//'):
                break
            else:
                fields = line.split()
                ret.append({'num':fields[0], 'name':fields[1], 'sys_func':fields[2]})
    return ret

tbl_formatter = "__SYS_CALL({num}, {name}, {sys_func})"
hdr_formatter = "#define NR_{name:10}{num}"

formatters = {"hdr": hdr_formatter, "tbl": tbl_formatter}
def gen_out(li):
    formatter = formatters[args.type]
    ret = []
    for item in li:
        ret.append(formatter.format(**item))
    
    return wrap("\n".join(ret))

def wrap(content):
    ans = content
    if args.type == 'tbl':
        ans = "#ifndef __SYS_CALL\n#define __SYS_CALL\n#endif\n\n" + ans 
    ans = "// DO NOT MODIFY THE STUFF BLOW!\n\n" + ans
    return ans


if __name__ == "__main__":
    content = gen_out(read_file())
    if args.output:
        with open(args.output, 'w') as f:
            f.write(content)
    else:
        print(content)