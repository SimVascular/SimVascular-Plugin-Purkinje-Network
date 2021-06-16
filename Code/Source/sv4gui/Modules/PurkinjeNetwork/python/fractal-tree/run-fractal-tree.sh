#!/bin/bash

dir=/Users/parkerda/software/SimVascular/SimVascular-fork/SimCardio/Modules/PurkinjeNetwork/python/fractal-tree/

python ./fractal_tree.py  --infile=${dir}/sphere.vtp  \
                          --outfile=/Users/parkerda/tmp/sphere-network \
                          --init_node='[-1.0 ,0.0, 0.0]' \
                          --second_node='[-0.964, 0.00, 0.266]'



