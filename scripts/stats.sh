#!/bin/bash

find ../ -name '*.c' -or -name '*.h' | xargs wc -l
