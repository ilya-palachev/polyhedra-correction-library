#!/usr/bin/env python

import argparse
import subprocess
import sys
import re
import os

def parse_args():
    parser = argparse.ArgumentParser("Performance testing script for recoverer")
    parser.add_argument("contours_name", type=str, help="Name of shadow "
                        "contours data")
    parser.add_argument("estimator", type=str, help="Estimator")
    parser.add_argument("problem_type", type=str, help="Problem type")
    parser.add_argument("step", type=int, help="Step of launching")
    parser.add_argument("max_number", type=int, help="Maximum number of "
                        "contours")
    parser.add_argument("-u", "--unreduced", action="store_true",
                        default=False, help="")
    args = parser.parse_args()
    return args

if __name__ == '__main__':
    args = parse_args()
    contours_file = os.path.join("poly-data-in", args.contours_name,
                                 "shadow_contours.dat")
    if not os.path.exists(contours_file):
        raise Exception("Cannot find {0}".format(contours_file))

    out_file = open("performance_report.{0}.{1}.{2}.txt".format(
                    args.contours_name, args.estimator, args.problem_type), "w")

    for num_contours in range(args.step, args.max_number + 1, args.step):
        os.environ['OPENBLAS_NUM_THREADS'] = "4"
        command = ["buildRelease/Recoverer_test", "-f", contours_file, "-N",
                   str(num_contours), "-b", "-x", "-s", "-r", args.estimator,
                   "-p", args.problem_type]
        if args.unreduced:
            command.extend(["-t", "gk"])
        run = subprocess.Popen(command, stderr=subprocess.STDOUT,
                               stdout=subprocess.PIPE)
        problem_size = 0
        preparation_time = 0
        estimation_time = 0
        l1_error = 0
        l1_error_average = 0
        l2_error_squared = 0
        l2_error = 0
        l2_error_average = 0
        linf_error = 0
        while True:
            line = run.stdout.readline()
            if line != '':
                if "Number of support function items: " in line:
                    problem_size = re.findall(r"Number of support function "
                                              "items: ([\.0-9]*)", line)[0]
                if "Estimation data preparation: " in line:
                    preparation_time = re.findall(r"Estimation data "
                                                  "preparation: ([\.0-9]*)",
                                                  line)[0]
                if "Estimation: " in line:
                    estimation_time = re.findall(r"Estimation: ([\.0-9]*)",
                                                 line)[0]
                if "L1 = " in line:
                    l1_error = re.findall(r"L1 = ([\.0-9]*)", line)[0]
                if "L1 / " in line:
                    l1_error_average = re.findall(r"L1 / [0-9]* = ([\.0-9]*)",
                                                  line)[0]
                if "Sum of squares = " in line:
                    l2_error_squared = re.findall(r"Sum of squares = "
                                                  "([\.0-9]*)", line)[0]
                if "L2 = " in line:
                    l2_error = re.findall(r"L2 = ([\.0-9]*)", line)[0]
                if "L2 / sqrt" in line:
                    l2_error_average = re.findall(r"L2 / sqrt\([0-9]*\) = "
                                                  "([\.0-9]*)", line)[0]
                if "Linf = " in line:
                    linf_error = re.findall(r"Linf = ([\.0-9]*)", line)[0]

            else:
                break
        res = "{0} {1} {2} {3} {4} {5} {6} {7} {8} {9}".format(num_contours,
                                                               problem_size,
                                                               preparation_time,
                                                               estimation_time,
                                                               l1_error,
                                                               l1_error_average,
                                                               l2_error_squared,
                                                               l2_error,
                                                               l2_error_average,
                                                               linf_error)
        print("{0}".format(res))
        out_file.write("{0}\n".format(res))
