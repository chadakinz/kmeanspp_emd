from eval7 import equity, Card, handrange
import os
import numpy as np
import matplotlib.pyplot as plt

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


if __name__ == "__main__":
    generate_equity_distributions()