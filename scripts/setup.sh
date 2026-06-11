#!/bin/bash

set -e

xcode-select --install || true

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

eval "$(/opt/homebrew/bin/brew shellenv)"

brew install sfml@2

echo "SFML 2 installed at:"
brew --prefix sfml@2