#!/bin/bash

set -x
set -e

# Get REPO_DIR
if [ ! -z "$TRAVIS_BUILD_DIR" ]; then
    REPO_DIR="$TRAVIS_BUILD_DIR"
elif [ ! -z "$BUILD_REPOSITORY_LOCALPATH" ]; then
    REPO_DIR="$BUILD_REPOSITORY_LOCALPATH"
else
    REPO_DIR="$(dirname "$0")/../.."
fi

if [ ! -d "$REPO_DIR" ]; then
    echo "ERROR: REPO_DIR not exist: $REPO_DIR"
    exit 1
fi

cd "$REPO_DIR"
REPO_DIR="$(pwd)"
echo "INFO: REPO_DIR: $REPO_DIR"


# Get target CMake version
if [ -z "$CI_CMAKE_VERSION" ]; then
    echo "ERROR: CI_CMAKE_VERSION not defined or empty"
    exit 1
else
    echo "INFO: CI_CMAKE_VERSION: $CI_CMAKE_VERSION"
fi


# Get target compiler and version
if [ -z "$CI_COMPILER_TYPE" ]; then
    echo "ERROR: CI_COMPILER_TYPE not defined or empty"
    exit 1
else
    echo "INFO: CI_COMPILER_TYPE: $CI_COMPILER_TYPE"
fi

if [ -z "$CI_COMPILER_VERSION" ]; then
    echo "ERROR: CI_COMPILER_VERSION not defined or empty"
    exit 1
else
    echo "INFO: CI_COMPILER_VERSION: $CI_COMPILER_VERSION"
fi


# Detect CI_BUILD_TYPE
if [ -z "$CI_BUILD_TYPE" ]; then
    echo "WARN: CI_BUILD_TYPE not defined or empty; set CI_BUILD_TYPE=Release"
    CI_BUILD_TYPE=Release
else
    echo "INFO: CI_BUILD_TYPE: $CI_BUILD_TYPE"
fi


# Detect NCPU
if hash nproc 2>/dev/null; then
    NCPU="$(nproc)"
else
    echo "WARN: nproc not found; set NCPU=1"
    NCPU=1
fi
echo "INFO: NCPU: $NCPU"


IMAGE="remotecc/ci:${CI_COMPILER_TYPE}-${CI_COMPILER_VERSION}-cmake-${CI_CMAKE_VERSION}"
CONTAINER="${CI_COMPILER_TYPE}-${CI_COMPILER_VERSION}-cmake-${CI_CMAKE_VERSION}_$(date +'%Y%m%d%H%M%S%Z')"

__exit_cleanup() {
    docker stop "$CONTAINER" || true
    #sleep 1 || true
    #docker rm "$CONTAINER" || true
}
trap __exit_cleanup EXIT


docker pull "$IMAGE"
docker run \
    --rm \
    --detach \
    --tty \
    --network host \
    --env CI_BUILD_TYPE="$CI_BUILD_TYPE" \
    --env NCPU="$NCPU" \
    --name "$CONTAINER" \
    --volume "$REPO_DIR":/remotecc:ro \
    "$IMAGE"

docker exec --tty "$CONTAINER" bash -c '
    set -xe
    mkdir /build
    cd /build
    cmake /remotecc \
        -DCMAKE_BUILD_TYPE="$CI_BUILD_TYPE" \
        -DREMOTECC_TREAT_WARNINGS_AS_ERRORS=ON
    make -j"$[NCPU+2]" VERBOSE=1
    ctest -VV
    '
