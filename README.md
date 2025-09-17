# markovs-dumb-chatbot
A small markov chain implementing basic token prediction, written in ANSI/POSIX C.

## Building/Getting the Program
### Downloading
To get a copy of the program, simply go to the "Releases" page and download the executable. At the moment, only GNU/Linux x86\_64 is supported.

Then, you can proceed to running the program.

### Building from Source
The program may be built with 3 options: `debug`, `release`, or `profile`.

`debug` will build the program with debugging symbols enabled in case you need to use GDB or similar
`profile` will build the code with the `-pg` option so that gprof can be used to profile the code
`release` will build the code with all optimizations available to maximize performance

Then, the program can be built simply with GNU make:
```
make release
```

If you need to clean up the directory, you can simply run
```
make clean
```

## Running the Program
To run an existing model, you can specify the prompt with `-p`, the model with `-m`, and the number of tokens to be generated with `-n`:
```
./markovs_dumb_chatbot -m model.txt -p "this is a prompt" -n 42
```
Then, the program will echo the prompt and begin printing the autocompleted text to `stdout`, until the set number of words have been generated.

To train the model, the files can be listed with `-t` and optionally, an external vocabulary can be specified with `-v`. Again, a model must be specified with `-m` for output:
```
./markovs_dumb_chatbot -m model.txt -t file1.txt,file2.txt,file3.txt -v vocab.txt
```

Pretrained sample models are available on the "Releases" page.


## Documentation
For a quick reminder of syntax, the program can be run with the `-h` flag:
```
./markovs_dumb_chatbot -h
```
A quick listing of options will be printed, then the program will terminate.

The main source of documentation is through man pages. To build those with groff:
```
groff -mdoc markovs_dumb_chatbot.1 -Thtml > markovs_dumb_chatbot.html
```
Then, the man page can be opened as an html file in the browser of your choice. It will contain some information about flags and parameters, and a few examples to help you get started.


## Credit

- uthash.h (github.com/troydhanson/uthash)

This program uses the uthash.h library, which was written by Troy D. Hanson. Its license is included below:

```
Copyright (c) 2005-2025, Troy D. Hanson  https://troydhanson.github.io/uthash/
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
