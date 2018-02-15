#!/bin/bash
while [[ $(squeue --user=$USER | wc -l) != 1 ]]; do sleep 10; done
sleep 60;




