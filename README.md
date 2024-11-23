# Uniform Function Call Syntax for C

This preprocessor adds Uniform Function Call Syntax to C, meaning that code like:

```c
    // -b +/- sqrt(b*b - 4ac)/2a
    vec2 inside = vec2_divide(vec2_sqrt(vec2_subtract(vec2_multiply(b, b), vec2_scalar_multiply(vec2_multiply(a, c), 4))), vec2_scalar_multiply(a, 2));
    vec2 positive = vec2_add(vec2_negate(b), inside);
    vec2 negative = vec2_subtract(vec2_negate(b), inside);
```

Can be rewritten as:

```c
    // -b +/- sqrt(b*b - 4ac)/2a
    vec2 inside = a.vec2_multiply(c).vec2_scalar_multiply(4);
	inside = b.vec2_multiply(b).vec2_subtract(inside);
	inside = vec2_sqrt(inside).vec2_divide(~> a.vec2_scalar_multiply(2));
	vec2 positive = vec2_negate(b).vec2_add(inside);
	vec2 negative = vec2_negate(b).vec2_subtract(inside);
```

And the procedure to read an entire file into a string can be written as:

```c
    FILE* file = fopen("file.txt", "r");
    if(!file) {
        perror("Failed to open file");
        return -1;
    }

    file.fseek(0L, SEEK_END);
    size_t size = file.ftell();
    file.rewind();

    char* content = (char*)malloc(size + 1);
    content.fgets(size, file);
    content[size] = '\0';

    file.fclose();
```

## Running

The script is run by providing a input file and optional output file. If the output file is not provided it will take the the name of the input file with the first instance of ".ufcs" removed from the name. If either file is replaced with `.` it will be input/output from standard input/output respectively.

```bash
ufcs example.ufcs.c . # Processes UFCS and print the result to the terminal
```

This allows the result of the preprocessor to be pipped to a compiler.

```bash
ufcs example.ufcs.c . | gcc -x c -
```

## Building

We use a very simple cmake setup, to the build the project simply run:

```bash
mkdir build
cd build
cmake ..
make
```

### Building on Windows

We also support compiling with Visual Studio. First use cmake to generate a solution.

```bash
mkdir build
cd build
cmake ..
```

There will then be a Visual Studio solution file in the build directory, which can be opened.

## Limitations

The preprocessor uses a very simple regular expression matching system. 
In practice this means that it often has trouble with nesting. 
The `~>` (wiggle arrow) operator can often be placed in front of a UFCS call that improperly parsed to mark where it should be begin. However sometimes the results will need to be split and stored in intermediate variables.
