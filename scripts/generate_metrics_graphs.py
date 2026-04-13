#!/usr/bin/env python3
"""
Run this script to regenerate convergence plots from scratch.
Usage: python scripts/generate_report.py
"""

import subprocess
import sys
import matplotlib.pyplot as plt
import pandas as pd
from pathlib import Path
import os

def run_cpp_tests():
    """Run the C++ test suite to generate CSV data"""
    print("Running C++ tests to generate convergence data...")
    result = subprocess.run(["../build/tests", "test_get_objective"],
                            capture_output=True, text=True)
    if result.returncode != 0:
        print("Tests failed!")
        print(result.stderr)
        sys.exit(1)
    print("Tests completed successfully")

def generate_plots():
    """Read CSVs from tests/output/ and generate plots"""
    PLOT_PATH = "../tests/inertia_metrics/"
    csv_files = os.listdir(PLOT_PATH)
    count = 5
    if not (os.path.isdir("./graphs")):
        os.mkdir("./graphs")
    for csv_file in csv_files:
        if csv_file[:9] != "test_input": continue
        data = pd.read_csv(PLOT_PATH + csv_file)
        # ... plotting code ...
        plt.plot(data.index, data.values, label=f"sample {count}")
        count += 1


    plt.xlim(0, 25)
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)

    plt.xlabel("Time (iterations)")        # x-axis label
    plt.ylabel("Inertia")     # y-axis label
    plt.title("K-Means Convergence Over Time")  # plot title
    plt.savefig(f"./graphs/plot_test_file_{count}.png")


    print(f"Plots saved to ./scripts/graphs")

if __name__ == "__main__":
    #run_cpp_tests()
    generate_plots()
    print("\n✅ Report generation complete. Check ./graphs/ for plots")