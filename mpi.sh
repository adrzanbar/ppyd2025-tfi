#!/bin/bash

# Exit on error
set -e

# Create mpiuser if it doesn't exist
if ! id "mpiuser" &>/dev/null; then
  echo "Creating user 'mpiuser'..."
  useradd -m -s /bin/bash mpiuser
  echo "mpiuser:mpipass" | chpasswd
else
  echo "User 'mpiuser' already exists."
fi

# Add mpiuser to the sudo group
usermod -aG sudo mpiuser

# Create working directory
mkdir -p /home/mpiuser/ppyd
chown mpiuser:mpiuser /home/mpiuser/ppyd

# Install OpenMPI and ZeroTier
echo "Installing OpenMPI and ZeroTier..."
apt update
apt install -y openmpi-bin openmpi-common libopenmpi-dev zerotier-one

echo "Setup complete."
