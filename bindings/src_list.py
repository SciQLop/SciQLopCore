#!/bin/env python3
# taken from https://github.com/radareorg/cutter/blob/master/src/bindings/src_list.py


import os
import re
import sys
import xml.etree.ElementTree as Et

script_path = os.path.dirname(os.path.realpath(__file__))

def fix_name(name):
    return name.replace('<','_').replace('>','_')

def find_all_objects(node, element, found=None):
    found = found or []
    found += list(map(lambda t:fix_name(t.attrib['name']), node.findall('object-type')+node.findall('interface-type')))
    smart_ptrs = node.findall('smart-pointer-type')
    for sp in smart_ptrs:
        classes = sp.attrib.get('instantiations', '').split(',')
        for c in classes:
            found.append(f"{sp.attrib['name']}_{c}")
    for item in node.findall('namespace-type'):
        if item.attrib.get('visible', 'true') in ['true', 'auto']:
            found += [item.attrib['name']]
        found += list(map(lambda name:f"{item.attrib['name']}_{name}",find_all_objects(item, element)))
    return found


def get_cpp_files_gen(args, include_package=True):
    with open(os.path.join(script_path, "bindings.xml"),'r') as f:
        xml=Et.fromstring(f.read())
        types = find_all_objects(xml,'object-type')
        package = xml.attrib['package']

    cpp_files_gen = [f"{package.lower()}_module_wrapper.cpp"]+[f"{typename.lower()}_wrapper.cpp" for typename in types]

    if include_package:
        cpp_files_gen = [os.path.join(package, f) for f in cpp_files_gen]

    if len(args) > 0:
        cpp_files_gen = [os.path.join(args[0], f) for f in cpp_files_gen]

    return cpp_files_gen


def cmd_cmake(args):
    sys.stdout.write(";".join(get_cpp_files_gen(args)))


def cmd_qmake(args):
    sys.stdout.write("\n".join(get_cpp_files_gen(args)) + "\n")


def cmd_meson(args):
    sys.stdout.write(";".join(get_cpp_files_gen(args, include_package=False)))


cmds = {"cmake": cmd_cmake, "qmake": cmd_qmake, "meson": cmd_meson}

if len(sys.argv) < 2 or sys.argv[1] not in cmds:
    print(f"""usage: {sys.argv[0]} [{"/".join(cmds.keys())}] [base path]""")
    exit(1)
cmds[sys.argv[1]](sys.argv[2:])
