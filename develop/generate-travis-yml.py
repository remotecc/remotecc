#!/usr/bin/env python3

from __future__ import print_function
from __future__ import with_statement

import copy
import yaml


class MyDumper(yaml.Dumper):
    """
    Specialized for indent of "list" elements
    See: https://stackoverflow.com/questions/25108581/python-yaml-dump-bad-indentation
    """
    def increase_indent(self, flow=False, indentless=False):
        return super(MyDumper, self).increase_indent(flow, False)


def main():
    root = {
        "language": "cpp",
        "branches": {
            "except": [
                "/^t(e?)mp(/.+)?$/",  # ignore branches: t[e]mp, t[e]mp/xxx
            ]
        },
        "matrix": {
            "include": [],
        },
    }

    def generate_linux(cmake_version, compiler_type, compiler_version, build_type):
        return {
            "os": "linux",
            "dist": "xenial",
            "services": [
                "docker"
            ],
            "env": [
                "CI_COMPILER_TYPE=%s" % compiler_type,
                "CI_COMPILER_VERSION=%s" % compiler_version,
                "CI_BUILD_TYPE=%s" % build_type,
                "CI_CMAKE_VERSION=%s" % cmake_version,
            ],
            "script": [
                "./ci/linux/build.sh"
            ],
        }

    CMAKE_VERSIONS = [
        "3.5.0",
        #"3.6.0",
        #"3.7.0",
        #"3.8.0",
        #"3.9.0",
        #"3.10.0",
        #"3.11.0",
        #"3.12.0",
        #"3.13.0",
        "3.13.3"  # latest version
        ]
    GCC_VERSIONS = [
        "5.3", "5.4", "5.5",
        #"6.1", "6.2", "6.3", "6.4", "6.5",
        "6.2", "6.5",
        #"7.1", "7.2", "7.3", "7.4",
        "7.3", "7.4",
        "8.1", "8.2",
        ]
    CLANG_VERSIONS = [
        #"3.5", "3.6", "3.7", "3.8", "3.9",
        "3.5", "3.9",
        "4.0",
        "5.0",
        "6.0",
        "7",  # not "7.0"
        "8",  # not "8.0"
        ]
    for cmake_version in CMAKE_VERSIONS:
        for gcc_version in GCC_VERSIONS:
            root["matrix"]["include"].append(generate_linux(cmake_version, "gcc", gcc_version, "Release"))
    for cmake_version in CMAKE_VERSIONS:
        for clang_version in CLANG_VERSIONS:
            root["matrix"]["include"].append(generate_linux(cmake_version, "clang", clang_version, "Release"))

    travis_yml = yaml.dump(root, Dumper=MyDumper, default_flow_style=False)
    print(travis_yml)


if __name__ == '__main__':
    main()

