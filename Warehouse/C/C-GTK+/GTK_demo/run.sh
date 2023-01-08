#!/bin/bash

gcc -o Helloworld Helloworld.c `pkg-config --cflags --libs gtk+-2.0`
./Helloworld


