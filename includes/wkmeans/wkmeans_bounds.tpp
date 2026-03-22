#include <thread>
#include <vector>
#include <algorithm>
#include <iostream>

namespace kmeans {
    template <typename T>
    void WKmeans<T>::update_bounds() {
        // Precompute cluster deltas (same for both approaches)
        std::vector<T> cluster_deltas(n_clusters);
        for (int k = 0; k < n_clusters; k++) {
            cluster_deltas[k] = wasserstein_2(clusters[k], new_clusters[k]);
        }

        int num_threads = std::thread::hardware_concurrency();
        // Cap at a reasonable number to avoid oversubscription

        std::vector<std::thread> threads;
        size_t chunk_size = (d_size + num_threads - 1) / num_threads;

        // Store only the deltas per point, recompute lower bounds during merge
        // This reduces memory usage
        struct ThreadResult {
            std::vector<T> deltas;  // Only store the computed deltas per point
            size_t start;
            size_t end;
        };

        std::vector<ThreadResult> results(num_threads);

        // Phase 1: Compute all deltas in parallel
        for (int t = 0; t < num_threads; t++) {
            size_t start = t * chunk_size;
            size_t end = std::min(start + chunk_size, static_cast<size_t>(d_size));
            size_t range = end - start;

            results[t].start = start;
            results[t].end = end;
            results[t].deltas.resize(range);

            threads.emplace_back([this, t, start, end, &results]() {
                auto& res = results[t];
                for (size_t local_i = 0; local_i < res.deltas.size(); local_i++) {
                    size_t i = start + local_i;
                    int cluster_id = cluster_assignments[i];
                    res.deltas[local_i] = wasserstein_2(
                            clusters[cluster_id],
                            new_clusters[cluster_id]
                    );
                }
            });
        }

        for (auto& th : threads) {
            th.join();
        }
        threads.clear();

        // Phase 2: Apply updates sequentially (no race conditions)
        // This is fast because it's just addition and max operations
        for (int t = 0; t < num_threads; t++) {
            auto& res = results[t];
            for (size_t local_i = 0; local_i < res.deltas.size(); local_i++) {
                size_t i = res.start + local_i;
                T delta = res.deltas[local_i];

                upper_bounds[i] += delta;
                r[i] = true;

                // Update lower bounds for this point
                size_t base_idx = i * n_clusters;
                for (int k = 0; k < n_clusters; k++) {
                    lower_bounds[base_idx + k] = std::max(
                            lower_bounds[base_idx + k] - cluster_deltas[k],
                            T{}
                    );
                }
            }
        }
    }

        template <typename T>
        void WKmeans<T>::old_update_bounds(){
            for(int i = 0; i < d_size; i++){
                upper_bounds[i] += wasserstein_2(clusters[cluster_assignments[i]], new_clusters[cluster_assignments[i]]);
                r[i] = true;
                for(int k = 0; k < n_clusters; k++){
                    int index = i * n_clusters + k;
                    lower_bounds[index] = std::max(lower_bounds[index] - wasserstein_2(clusters[k], new_clusters[k]), T{});
                }
            }
        }



}
