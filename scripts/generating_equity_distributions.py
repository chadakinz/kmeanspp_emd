from eval7 import equity, Card, handrange
import os
import numpy as np
import matplotlib.pyplot as plt
import json
import ast
import pandas as pd
import ot
from generate_metrics_graphs import assign_w2_clusters_ot
from sklearn.decomposition import PCA
import matplotlib.patches as mpatches

def remove_cards(deck, cards):
    new_deck = []
    for c in deck:
        if c in cards:
            continue
        else:
            new_deck.append(c)
    return new_deck

def generate_equity_distributions():
    villain_dist =  ("AA, KK, QQ, AKs, JJ, AQs, KQs, AJs, KJs, TT, AKo, ATs, QJs, KTs, QTs, JTs, 99, AQo, A9s, KQo, 88, "
                     "K9s, T9s, A8s, Q9s, J9s, AJo, A5s, 77, A7s, KJo, A4s, A3s, A6s, QJo, 66, K8s, T8s, A2s, 98s, J8s, "
                     "ATo, Q8s, K7s, KTo, 55, JTo, 87s, QTo, 44, 33, 22, K6s, 97s, K5s, 76s, T7s, K4s, K3s, K2s, Q7s, 86s, "
                     "65s, J7s, 54s, Q6s, 75s, 96s, Q5s, 64s, Q4s, Q3s, T9o, T6s, Q2s, A9o, 53s, 85s, J6s, J9o, K9o, J5s, Q9o, "
                     "43s, 74s, J4s, J3s, 95s, J2s, 63s, A8o, 52s, T5s, 84s, T4s, T3s, 42s, T2s, 98o, T8o, A5o, A7o, 73s, A4o, 32s, "
                     "94s, 93s, J8o, A3o, 62s, 92s, K8o, A6o, 87o, Q8o, 83s, A2o, 82s, 97o, 72s, 76o, K7o, 65o, T7o, K6o, "
                     "86o, 54o, K5o, J7o, 75o, Q7o, K4o, K3o, 96o, K2o, 64o, Q6o, 53o, 85o, T6o, Q5o, 43o, Q4o, Q3o, 74o, "
                     "Q2o, J6o, 63o, J5o, 95o, 52o, J4o, J3o, 42o, J2o, 84o, T5o, T4o, 32o, T3o, 73o, T2o, 62o, 94o, 93o, 92o, 83o, 82o, 72o")
    villain_dist = handrange.HandRange(villain_dist)
    data = dict()
    private_cards = [
        (Card("Ad"), Card("Ac")),
        (Card("Ks"), Card("Kc")),
        (Card("5d"), Card("6c")),
        (Card("2d"), Card("7c")),
        (Card("2h"), Card("7c")),
        (Card("5h"), Card("6h"))
    ]
    public_cards = list(map(Card, ("3h", "4h", "Ah", "Kd")))
    _deck = []
    suits = ["c", "h", "d", "s"]
    ranks = ["2", "3", "4", "5", "6", "7", "8" , "9", "T", "J", "Q", "K", "A"]
    for suit in suits:
        for rank in ranks:
            _deck.append(Card(rank+suit))
    k = 0
    for private_card in private_cards:
        histogram = [0] * 10
        deck = remove_cards(_deck, private_card)
        hero_dist = handrange.HandRange(f"{str(private_card[0])}{str(private_card[1])}")
        for i in range(len(deck)):
            card1 = deck[i]
            public_hand = public_cards + [card1]
            try:
                equity1 = equity.py_all_hands_vs_range(hero_dist, villain_dist, public_hand, 10000000)[(private_card[0], private_card[1])]
            except:

                equity1 = equity.py_all_hands_vs_range(hero_dist, villain_dist, public_hand, 10000000)[(private_card[1], private_card[0])]

            index = min(int((equity1*10)), 9)
            histogram[index] += 1
        data[str(private_card)] = histogram
        N = len(histogram)
        x = np.arange(N) / N  # 0, 1/N, ..., (N-1)/N

        plt.bar(x, histogram, width=1/N)

        plt.text(
            0,
            max(histogram),
            f"Public cards: {public_cards}",
            fontsize=6
        )

        plt.xlabel("Equity")
        plt.ylabel("Count")
        plt.title(f"Equity histogram for private cards: {private_card}")

        # Ensure output directory exists
        os.makedirs("graphs", exist_ok=True)

        # Create a safe filename
        filename = f"graphs/equity_hist_{k}.png"
        plt.savefig(filename, dpi=300, bbox_inches="tight")
        plt.close()
        k += 1
    with open("equity_histograms.json", "w") as f:
        json.dump(data, f)


def assign_equity_distributions():
    with open("equity_histograms.json", "r") as f:
        data = json.load(f)

    pmfs = []
    keys = []

    for k, hist in data.items():
        hist = np.array(hist, dtype=float)

        total = hist.sum()
        if total == 0:
            pmf = hist  # avoid divide-by-zero (edge case)
        else:
            pmf = hist / total

        pmfs.append(pmf)
        keys.append(k)

    pmfs = np.array(pmfs)  # shape: (num_hands, num_bins)
    data_df = pd.DataFrame(pmfs, columns=[f"bin_{i}" for i in range(pmfs.shape[1])])
    keys = np.array(keys)

    df = pd.read_csv("clusters_test_input.txt", header=None)


    l = assign_w2_clusters_ot(data_df, df)
    for i, row in l.iterrows():
        print(f"{keys[i]:<35} → Cluster {row['cluster']}")

    input_file = "../tests/test_files/test_input_1.txt"
    inputs_df = pd.read_csv(input_file, sep=" ", header=None)
    cluster_assignments = assign_w2_clusters_ot(inputs_df, df)
    X = inputs_df.to_numpy()
    pca = PCA(n_components=2)
    X_2d = pca.fit_transform(X)
    out_df = pd.DataFrame({
        "x": X_2d[:, 0],
        "y": X_2d[:, 1]
    }, index=inputs_df.index)  # preserves original indexing

    pmfs_new = pca.transform(pmfs)
    # original data
    plt.scatter(
        out_df["x"], out_df["y"],
        c=cluster_assignments["cluster"],
        cmap="tab10",
        s=5,
        alpha=0.6,
        linewidths=0
    )

    # new points (highlighted)
    plt.scatter(
        pmfs_new[:, 0],
        pmfs_new[:, 1],
        color="red",
        s=80,
        label="new points"
    )
    unique_keys = np.unique(keys)

# assign colors for new keys
    cmap = plt.get_cmap("tab10")
    key_to_color = {k: cmap(i % 10) for i, k in enumerate(unique_keys)}

# plot NEW points grouped by key
    for k in unique_keys:
        idx = np.where(keys == k)[0]

        plt.scatter(
            pmfs_new[idx, 0],
            pmfs_new[idx, 1],
            color=key_to_color[k],
            s=80,
            edgecolors="black"
        )

# 3. legend ONLY for new points
    handles = [
        mpatches.Patch(color=key_to_color[k], label=str(k))
        for k in unique_keys
    ]
    plt.title("2D PCA Embedding of PMF Clusters with Overlaid Known Points")
    plt.legend(handles=handles, title="Private cards", fontsize=6)
    plt.savefig("./graphs/qualitative_tests.png", dpi=300, bbox_inches="tight")
    plt.close()
if __name__ == "__main__":
    #generate_equity_distributions()
    assign_equity_distributions()