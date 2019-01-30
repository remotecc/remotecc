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
                "CI_CMAKE_VERSION=%s" % cmake_version,
                "CI_COMPILER_TYPE=%s" % compiler_type,
                "CI_COMPILER_VERSION=%s" % compiler_version,
                "CI_BUILD_TYPE=%s" % build_type,
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
    for cmake_version in CMAKE_VERSIONS:
        for gcc_version in GCC_VERSIONS:
            root["matrix"]["include"].append(generate_linux(cmake_version, "gcc", gcc_version, "Release"))

    travis_yml = yaml.dump(root, Dumper=MyDumper, default_flow_style=False)
    print(travis_yml)


if __name__ == '__main__':
    main()

