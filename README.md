# Mishell ⚡

Mishell is an Unix shell.

## Installation

```
cd build
cmake ..
make
```

## Usage

Launch the shell using the following command in the `build` folder: 

```
./mishell
```

## Built-in commands

In addition to your system binaries, you can also run the following built-in commands:

### `jobs` prints the background jobs

Example:

```
mishell ⚡ sleep 10 &
sleep 10 mishell ⚡
mishell ⚡ jobs
Job list :
10904 sleep 10 alive
mishell ⚡
mishell ⚡ Terminated child : 10904
```

### `(scheme expr)` executes the `scheme expr` using a [Scheme](https://en.wikipedia.org/wiki/Scheme_%28programming_language%29) interpreter

Example:

```
mishell ⚡ (display "Hello world!\n")
Hello world!
mishell ⚡
```

## Implemented variants

- Asynchronous termination
- Chained pipes
