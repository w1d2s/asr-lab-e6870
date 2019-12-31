#!/usr/bin/env python

from __future__ import print_function
import os 
import sys
import numpy as np

def load_matrix(file_path):
    buf = []
    with open(file_path, 'r') as f:
        buf = f.readlines()
    
    row = int(buf[2].strip().split(' ')[-1])
    col = int(buf[3].strip().split(' ')[-1])
    
    mat_list = []
    for line in buf[4: ]:
        row_vec = [float(x) for x in line.strip().split(' ')]
        mat_list.append(row_vec)

    return np.array(mat_list, dtype = np.float64)

def cal_diff(mat_1, mat_2):
    if mat_1.shape != mat_2.shape:
        print("size mismatch : mat_1 : {0}, mat_2 : {1}".format(mat_1.shape, mat_2.shape))
        return 1
    mat1_norm = np.sum(np.square(mat_1))
    total_diff_mat = mat_1 - mat_2
    total_diff_norm = np.sum(np.square(total_diff_mat))
    print("total rel err (norm2) : {0}".format(total_diff_norm / mat1_norm))
    return 0

def main(argv):
    if len(argv) != 2:
        print("Usage .py mat-1.dat mat-2.dat")
        return 1

    mat_1 = load_matrix(argv[0])
    mat_2 = load_matrix(argv[1])

    return cal_diff(mat_1, mat_2)

if __name__ == '__main__':
    sys.exit(main(sys.argv[1: ]))

