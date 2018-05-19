#!/bin/bash
for i in ../tests/grammar/*; do python testrunner.py `which ana` "$i"; done
