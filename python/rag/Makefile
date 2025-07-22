VENV_DIR = .venv
PYTHON = $(VENV_DIR)/bin/python
PIP = $(VENV_DIR)/bin/pip

.PHONY: setup run clean

setup: $(VENV_DIR)/bin/activate

$(VENV_DIR)/bin/activate:
	@echo "🚀 Creating virtual environment..."
	python3 -m venv $(VENV_DIR)
	@echo "📦 Installing dependencies..."
	$(PIP) install --upgrade pip
	$(PIP) install -r requirements.txt

run: setup
	@echo "🤖 Running bot..."
	$(PYTHON) rag_bot.py

clean:
	rm -rf $(VENV_DIR)

