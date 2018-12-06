#!/usr/bin/env python

import argparse
import logging

from FractalTree import *
from parameters import Parameters

def parse_args():
    """ Parse command-line arguments."""
    parser = argparse.ArgumentParser()
    parser.add_argument("-i",   "--infile",      help="input file")
    parser.add_argument("-o",   "--outfile",     help="output file")
    return parser.parse_args(), parser.print_help

def init_logging():
    import logging
    logger = logging.getLogger('fractal-tree')
    logger.setLevel(logging.INFO)
    console_handler = logging.StreamHandler()
    formatter = logging.Formatter('[%(name)s] %(levelname)s - %(message)s')
    console_handler.setFormatter(formatter)
    logger.addHandler(console_handler)

def run(infile=None, outfile=None):
    init_logging()
    logger = logging.getLogger('fractal-tree')

    # Create a Parameters object to store parameters.
    param = Parameters()

    ## Set parameters.
    #
    if infile == None:
        logger.error("No input file name given.")
        return 
    else:
        logger.info("Input file name %s" % infile)
        param.input_file_name = infile

    if outfile == None:
        logger.error("No output file name given.")
        return 
    else:
        logger.info("Output file name %s" % outfile)
        param.output_file_name = outfile

    ## Calculate the fractal tree.
    #
    branches, nodes = Fractal_Tree_3D(param)

if __name__ == '__main__':
    args, print_help = parse_args()
    run(infile=args.infile, outfile=args.outfile)


