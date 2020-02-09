# Woody Woodpacker

This 42 project aims to create an `ELF64 packer`.

## Packer

This `packer` will encrypt data located inside the `first loadable segment`
with `RX` or `RWX` rights of targeted binary.

Then it will add a `loader` after that segment to decrypt it.

Then a new binary named `woody` will be generated.

This binary `entrypoint` will point toward the `loader`.

`Multiple loaders` can be added until there is no space left in that segment.

## Compiling

You may compile `woody_woodpacker` by running `cmake`.

## Usage

Usage: `./woody_woodpacker` elf64_binary \[encryption key\]

User custom encryption key size needs to be between 16 and 32