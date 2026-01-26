#!/bin/bash

# Demo script to show the setup wizard
# This will automatically respond to the wizard prompts

echo "=== Easy RSH Server - Setup Wizard Demo ==="
echo ""
echo "Starting server for the first time..."
echo ""
echo "The wizard will:"
echo "1. ✓ Create admin account"
echo "2. ✓ Configure server port"
echo "3. ✓ Select server mode (with interactive arrows!)"
echo "4. ✓ Choose multi-client support (with interactive arrows!)"
echo "5. ✓ Save configuration"
echo ""
echo "Press Ctrl+C to exit the server when demo is complete."
echo ""
sleep 2

./server
