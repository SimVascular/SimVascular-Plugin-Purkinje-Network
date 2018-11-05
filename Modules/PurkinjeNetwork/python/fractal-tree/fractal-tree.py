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

def main():
    init_logging()
    logger = logging.getLogger('fractal-tree')
    error_flag = False

    # Create a Parameters object to store parameters.
    param = Parameters()

    ## Set parameters from command-line arguments.
    #
    args, print_help = parse_args()

    if args.infile == None:
        logger.error("No input file name given.")
        error_flag = True
    else:
        logger.info("Input file name %s" % args.infile)
        param.input_file_name = args.infile

    if args.outfile == None:
        logger.error("No output file name given.")
        error_flag = True
    else:
        logger.info("Output file name %s" % args.outfile)
        param.output_file_name = args.outfile

    ## Calculate the fractal tree.
    #
    branches, nodes = Fractal_Tree_3D(param)


if __name__ == '__main__':
    main()


