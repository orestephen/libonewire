# libonewire
A one wire abstraction library

*Library is intended to be used on small applications.

## Config
`cmake -S . -B build`

## Build
`cmake --build build`

### options
enable compile commands output
`-DCMAKE_EXPORT_COMPILE_COMMANDS=1`

enable ctest testing
`-DONEWIRE_TEST=1`

enable debug printf's
`-DONEWIRE_DBG=1`

## Tests
run tests

`ctest --test-dir build`

### Unit Tests
 - test_master_tx: test master transmit API
 - test_master_rx: test master receive API
 - test_master_tx: test master transmit with infinite timeout API
