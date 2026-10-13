@echo off

powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"

echo "Installed Astral UV"
uv sync
uv run raptor setup all

pause
