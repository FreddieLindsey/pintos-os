# Pintos
[![wercker status](https://app.wercker.com/status/4a46f89a31f7efcf05306928f28d9ef6/m "wercker status")](https://app.wercker.com/project/bykey/4a46f89a31f7efcf05306928f28d9ef6)

An implementation of an operating system in C

### Coding Style Conventions
This is a list of coding conventions followed by the members of the group during this project.
- In pointers declaration the asterisk should be close to the variable name rather than the type. For example:

  `int *a, *b, c;`

- When declaring any variable other than primitives use `malloc` even if they are only used in the current scope.

### Local testing

Since we are using `wercker`, you can find the installation instructions for the `wercker` cli [here](http://devcenter.wercker.com/cli/index.html). You will also need Docker installed and ready for use, [instructions here](https://docs.docker.com/engine/installation/).

```zsh
$ wercker build
```

This will test using the most recent testing file (wercker.yml).

To add to the test file, let's say you have a sequence of commands to test a part of the codebase, you need to append them to the appropriate place in the build steps as below:

```
build:
  steps:
    ...
    - script:
      name: <TESTING COMMENT>
      code: |
        <COMMAND 1>
        <COMMAND 2>
        <COMMAND 3>
    ...
```

### Team

Freddie Lindsey, Oliver Norton, James Lane, Elias Benussi
