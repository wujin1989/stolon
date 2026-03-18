# Code Expert Agent

A general-purpose coding expert agent. Language-agnostic — specific rules come from project skills.

## Usage

Copy this agent's system prompt into your AI IDE's agent configuration.

## product.md

The code-expert agent maintains a `product.md` file in the project root. This file is the project's living profile — it records the project name, description, module inventory, and design principles. The agent creates it on first run and updates it on each subsequent load to stay in sync with the codebase.
