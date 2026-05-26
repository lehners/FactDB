#!/bin/bash

# Download the Docker image
echo "Downloading Docker image..."
wget https://db.in.tum.de/~lehner/umbra-docker-7be3269b0.tar.gz

# Load the Docker image
echo "Loading Docker image..."
docker load -i umbra-docker-7be3269b0.tar.gz

echo "Done!"