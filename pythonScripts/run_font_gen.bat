@echo off
echo Starting virtual environment setup on Windows...

:: Create the virtual environment named "venv" if it doesn't exist
if not exist venv\Scripts\activate (
    python3 -m venv venv
)

:: Activate the environment
call venv\Scripts\activate

:: Install or update dependencies
echo Installing dependencies...
pip install --upgrade pip --quiet
pip install freetype-py --quiet

:: Execute the script passing all command-line arguments using %*
echo Running font generator...
python3 font_generator.py %*

:: Deactivate the environment when finished
deactivate
echo Process completed.