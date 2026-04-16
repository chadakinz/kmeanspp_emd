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
import matplotlib.pyplot as plt
import ot
from sklearn.decomposition import PCA
import numpy as np

def assign_w2_clusters_ot(pdfs_df: pd.DataFrame, clusters_df: pd.DataFrame):

    pdfs = np.ascontiguousarray(pdfs_df.to_numpy())
    clusters = np.ascontiguousarray(clusters_df.to_numpy())

    n = pdfs.shape[0]
    k = clusters.shape[0]
    d = pdfs.shape[1]

    # 1D support
    x = np.ascontiguousarray(np.arange(d).reshape(-1, 1))

    # cost matrix (must also be contiguous)
    M = np.ascontiguousarray(ot.dist(x, x, metric="euclidean") ** 2)

    labels = np.zeros(n, dtype=int)

    for i in range(n):
        p = np.ascontiguousarray(pdfs[i])

        best_j = 0
        best_cost = np.inf

        for j in range(k):
            q = np.ascontiguousarray(clusters[j])
            p = p / np.sum(p)
            q = q / np.sum(q)

            cost = ot.emd2(p, q, M)

            if cost < best_cost:
                best_cost = cost
                best_j = j

        labels[i] = best_j

    return pd.DataFrame({
        "pdf_index": np.arange(n),
        "cluster": labels
    })
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


def pca_graphs():
    # X: (N, 10) PMFs
    input_file = "../tests/test_files/test_input_1.txt"
    inputs_df = pd.read_csv(input_file, sep=" ", header=None)
    subprocess.run(["../build/bin/main", "-k 100", "-i", input_file, "-o", "./clusters_test_input.txt", "-s","1e-5", "-N 10", "-t 2"],
                   capture_output=True, text=True)
    clusters_df = pd.read_csv("./clusters_test_input.txt", sep=",", header = None)

    cluster_assignments = assign_w2_clusters_ot(inputs_df, clusters_df)

    X = inputs_df.to_numpy()

    pca = PCA(n_components=2)
    X_2d = pca.fit_transform(X)
    print(pca.explained_variance_ratio_)
    out_df = pd.DataFrame({
        "x": X_2d[:, 0],
        "y": X_2d[:, 1]
    }, index=inputs_df.index)  # preserves original indexing

    plt.scatter(
        out_df["x"], out_df["y"],
        c=cluster_assignments["cluster"],
        cmap="tab10",
        s=5,
        alpha=0.6,
        linewidths=0
    )
    plt.title("Wasserstein-2 Clustering of PMFs (PCA Projection)")
    plt.xlabel("PC1")
    plt.ylabel("PC2")

    plt.savefig("./graphs/wasserstein_pca_clusters.png", dpi=300, bbox_inches="tight")

def get_silhouette_score():

if __name__ == "__main__":
    #run_cpp_tests()
    #generate_plots()
    pca_graphs()
    print("\n✅ Report generation complete. Check ./graphs/ for plots")