#!/usr/bin/env python

from __future__ import print_function
import os 
import sys
import numpy as np

def load_matrix_map(file_path):
    buf = []
    with open(file_path, 'r') as f:
        buf = f.readlines()

    matrix_map = {}
    i = 0
    while i < len(buf):
        if buf[i].startswith("% name"):
            name = buf[i].strip().split(' ')[-1]
            row = int(buf[i + 2].strip().split(' ')[-1])
            col = int(buf[i + 3].strip().split(' ')[-1])
            mat_list = []
            for line in buf[i + 4: i + 4 + row]:
                row_vec = [float(x) for x in line.strip().split(' ')]
                mat_list.append(row_vec)
            matrix_map[name] = np.array(mat_list, dtype = np.float64)
            i = i + 4 + row
    return matrix_map

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

    mat_1_map = load_matrix_map(argv[0])
    mat_2_map = load_matrix_map(argv[1])

    key_list_1 = mat_1_map.keys()
    key_list_2 = mat_2_map.keys()

    if len(mat_1_map) != len(mat_2_map):
        print("matrices number dont agree!")
        return 1

    for i in range(len(mat_1_map)):
        print("mat 1 : {0}, mat 2 : {1}".format(key_list_1[i], key_list_2[i]))
        cal_diff(mat_1_map[key_list_1[i]], mat_2_map[key_list_2[i]])
    
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1: ]))

