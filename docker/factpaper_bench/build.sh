
# Build docker image
DOCKER_BUILDKIT=1 docker build \
    --ssh default \
    --build-arg date=$BUILDDATE \
    --tag=factdb/factdb:latest \
    --ulimit nofile=2048 \
    --no-cache \
    --progress=plain \
    -f docker/factpaper_bench/Dockerfile .
