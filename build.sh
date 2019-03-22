#! /bin/sh

# from build and run tutorial on docker website
# https://docs.docker.com/develop/develop-images/multistage-build/

echo Building base images

docker build --no-cache --rm -t hungrynerds-assignment-2:base . -f "docker/base.Dockerfile"

echo Building hungrynerds-assignment-2:build

docker build --no-cache --rm -t hungrynerds-assignment-2:build . -f "docker/Dockerfile"

echo remove dangling image 

docker rmi $(docker images -f "dangling=true" -q)
