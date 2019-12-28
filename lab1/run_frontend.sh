#!/bin/bash

# only check add window result, compare with p1win.dat
# ./lab1_frontend --audio_file data/p1in.dat --feat_file data/win.dat \
#   --frontend.window true --window.hamming true \
#   --frontend.fft false \
#   --frontend.melbin false \
#   --frontend.dct false

# only check add window + mel bin result, compare with p1bin.dat
 ./lab1_frontend --audio_file data/p1in.dat --feat_file data/mel_bin.dat \
   --frontend.window true --window.hamming true \
   --frontend.fft true \
   --frontend.melbin true --melbin.bins 26 --melbin.log true \
   --frontend.dct false

# only check extracted mfcc feature, compare with p1all.dat
# ./lab1_frontend --audio_file data/p1test.dat --feat_file data/mfcc.dat \
#   --frontend.window true --window.hamming true \
#   --frontend.fft true \
#   --frontend.melbin true --melbin.bins 26 --melbin.log true \
#   --frontend.dct true --dct.coeffs 12

# submit this mfcc.dat
#./lab1_frontend --audio_file data/p1test.dat --feat_file data/mfcc.dat \
#  --frontend.window true --window.hamming true \
#  --frontend.fft true \
#  --frontend.melbin true --melbin.bins 26 --melbin.log true \
#  --frontend.dct true --dct.coeffs 12
