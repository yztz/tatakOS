#!/usr/bin/env python3

import argparse
import os

parser = argparse.ArgumentParser(description='tbl-parser.')
parser.add_argument("filename", help="file")
parser.add_argument("-o", "--output", help="output")
args = parser.parse_args()


def read_file():
    ret = []
    with open(args.filename, 'r') as f:
        while True:
            line = f.readline()
            if not line:
                break
            elif line.startswith('#') or line.startswith('//'):
                continue
            else:
                fields = line.split()
                ret.append({"type":fields[0], "name":fields[1]})
    return ret

def gen_out(li):
    ans = "// DO NOT MODIFY THE STUFF BLOW!\n\n"
    ret = []

    # for define
    ans += "\n\n#ifdef PROFILE_DEF\n\n"
    for item in li:
        ret.append("#ifdef {name}\n#undef {name}\n#endif".format(**item))
        if item['type'] == 'void':
            ret.append("#define {name}(...) profile_void({name}, ##__VA_ARGS__)".format(**item))
        else:
            ret.append("#define {name}(...) profile({name}, ##__VA_ARGS__)".format(**item))
    ans += "\n".join(ret)
    ans += "\n\n#endif // PROFILE_DEF\n"

    # for call
    ans += "\n\n#ifdef PROFILE_CALL\n\n"
    ret.clear()
    for item in li:
        ret.append("{name}()".format(**item))
    ans += "\n".join(ret)
    ans += "\n\n#endif // PROFILE_CALL\n"

    return ans

if __name__ == "__main__":
    content = gen_out(read_file())
    if args.output:
        with open(args.output, 'w') as f:
            f.write(content)
    else:
        print(content)