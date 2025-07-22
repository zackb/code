#!/bin/bash
set -e  # Exit on error

VENV_DIR=".venv"

# 1. Create virtual environment if missing
if [ ! -d "$VENV_DIR" ]; then
  echo "🚀 Creating virtual environment..."
  python3 -m venv "$VENV_DIR"
fi

# 2. Activate virtual environment
source "$VENV_DIR/bin/activate"

# 3. Upgrade pip and install dependencies
echo "📦 Installing dependencies..."
pip install --upgrade pip
pip install -r requirements.txt

# 4. Run the bot
echo "🤖 Running rag_bot.py..."
python rag_bot.py

# 5. Deactivate venv after script ends
deactivate

