# mishell ⚡

Mishell is a Unix shell

## Installation

```
cd build
cmake ..
make
```

## Usage
```
./mishell
```

## Commands

### `jobs` : print the background jobs

Example :

```
mishell ⚡ sleep 10 &
sleep 10 mishell ⚡
mishell ⚡ jobs
Job list :
10904 sleep 10 alive
mishell ⚡
mishell ⚡ Terminated child : 10904

```


### `(scheme expr)` : execute the "`scheme expr`" via an interpreter [Scheme](https://en.wikipedia.org/wiki/Scheme_%28programming_language%29)

Example :

```
mishell ⚡ (display "Hello world!\n")
Hello world!
mishell ⚡
```

## Variantes implemented

- Asynchronous termination
- Multiple Pipes
