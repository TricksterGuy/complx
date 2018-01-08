#!/bin/bash
#
# A simple script to manage running complx in docker.

# USAGE: ./run_docker [link-directory]

# Error out when anything goes wrong
set -e

IMAGE_NAME="ausbin/complx"

# get location of script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if ! command -v docker >/dev/null 2>&1; then
	echo "[ERR] Docker has not been installed! Go to http://get.docker.com" >&2
	exit 2
fi

# Standardize calling (work no matter where we call from)
cd "$DIR"

if ! docker images | grep -q "$IMAGE_NAME"; then
	# To trigger a rebuild, simply do a `docker rmi $IMAGE_NAME`
	docker build -t "$IMAGE_NAME" .
fi

if [ -n "$1" ] && [ -d "$1" ]; then
	echo "Linking user specified dir '$1' on your system to '/home/developer/workdir' in the container"
	LINK="-v $1:/home/developer/workdir"
fi

# Docker image has been built, lets run it.
# If this fails, run `xhost +` to disable x socket security
docker run \
	-e DISPLAY=$DISPLAY \
	-v /tmp/.X11-unix:/tmp/.X11-unix $LINK \
	"$IMAGE_NAME"
