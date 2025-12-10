#!/bin/bash

# Exit immediately if any command fails
set -e

# Check if the modules directory exists
if [ -d "modules/anira" ]; then
  echo "Directory 'modules/anira' already exists. Skipping cloning."
  exit 0
fi

# Clone the anira repository
echo "Cloning anira repository..."
git clone git@github.com:Andonvr/anira.git modules/anira