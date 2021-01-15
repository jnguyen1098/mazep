# Mazep

Generates mazes in a multi-threaded manner using the OpenMP library.

## Install

```
make
```

## Run (Single-threaded)

```
./maze -s 234324    # -s is used to set the seed
./maze -n 21        # -n is used to set the size of the maze (length and width). Must be odd.
```

## Run (Multi-threaded)

```
./mazep -s 234324    # -s is used to set the seed
./mazep -n 21        # -n is used to set the size of the maze (length and width). Must be odd.
```

Comes with a viewer (`viewer.c`) that demonstrates the multithreading (see below picture for an example).

![maze](https://i.imgur.com/6Yoouqc.png)
