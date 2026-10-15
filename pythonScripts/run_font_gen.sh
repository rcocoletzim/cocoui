#!/bin/bash

echo "Starting virtual environment setup on macOS/Linux..."

# Create the virtual environment named "venv" if it doesn't exist
if [ ! -d "venv" ]; then
    python3 -m venv venv
fi

# Activate the environment
source venv/bin/activate

# Install or update dependencies
echo "Installing dependencies..."
pip install --upgrade pip --quiet
pip install freetype-py --quiet

# Execute the script passing all command-line arguments using "$@"
echo "Running font generator..."
python3 pythonScripts/font_generator.py "$@"

# Deactivate the environment when finished
deactivate
echo "Process completed."