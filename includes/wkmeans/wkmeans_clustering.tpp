#include <algorithm>
#include <cstdint>
#include <limits>
#include <thread>
#include <utility>

namespace kmeans {
// todo: find better name
    template<typename T>
    inline std::vector <T> WKmeans<T>::get_vector_s() {
        std::vector <T> s(n_clusters, std::numeric_limits<T>::max());
        T distance;
        for (int k1 = 0; k1 < n_clusters - 1; k1++) {
            for (int k2 = k1 + 1; k2 < n_clusters; k2++) {
                distance = .5 * wasserstein_2(clusters[k1], clusters[k2]);
                if (distance < s[k1])
                    s[k1] = distance;
                if (distance < s[k2])
                    s[k2] = distance;
            }
        }
        return s;
    }

    template<typename T>
    void WKmeans<T>::update_clusters() {
        std::vector <PPF<T>> sum_cluster(n_clusters, PPF<T>(PPF_SIZE));

        for (int i = 0; i < d_size; i++) {
            sum_cluster[cluster_assignments[i]] += ppfs[i];
        }
        for (int k = 0; k < n_clusters; k++) {
            new_clusters[k] = sum_cluster[k] / cluster_size[k];
        }
    }

    template<typename T>
    void WKmeans<T>::mini_batch_update_clusters(
            const std::vector <uint32_t> &sizes) {
        std::vector <PPF<T>> sum_cluster(n_clusters, PPF<T>(PPF_SIZE));

        for (int i = 0; i < d_size; i++) {
            sum_cluster[cluster_assignments[i]] += ppfs[i];
        }
        for (int k = 0; k < n_clusters; k++) {
            PPF <T> cluster_k = clusters[k] * (double) sizes[k];
            clusters[k] =
                    (sum_cluster[k] + cluster_k) / (double) (cluster_size[k] + sizes[k]);
        }
    }

// Function that updates the cluster assignments of data using the upper and
// lower bound vectors
// TODO: to improve runtime, instead of computing the distance between the
// clusters around line 40, I can do it in the helper function while I am
// computing the vector s
// TODO: rename distance_placeholder vectors
    template<typename T>
    void WKmeans<T>::assign_new_clusters() {
        T distance_placeholder1, distance_placeholder2, distance_placeholder3;
        std::vector <T> s = get_vector_s();
        for (int i = 0; i < d_size; i++) {
            if (upper_bounds[i] <= s[cluster_assignments[i]])
                continue;
            for (int k = 0; k < n_clusters; k++) {
                if (k == cluster_assignments[i] ||
                    upper_bounds[i] <= lower_bounds[i * n_clusters + k])
                    continue;
                distance_placeholder1 =
                        .5 * wasserstein_2(clusters[k], clusters[cluster_assignments[i]]);
                if (upper_bounds[i] <= distance_placeholder1)
                    continue;
                if (r[i]) {
                    distance_placeholder2 =
                            wasserstein_2(clusters[cluster_assignments[i]], ppfs[i]);
                    upper_bounds[i] = distance_placeholder2;
                    lower_bounds[i * n_clusters + cluster_assignments[i]] =
                            distance_placeholder2;
                    r[i] = false;
                } else {
                    distance_placeholder2 = upper_bounds[i];
                }
                if (distance_placeholder2 > lower_bounds[i * n_clusters + k] ||
                    distance_placeholder2 > distance_placeholder1) {
                    distance_placeholder3 = wasserstein_2(clusters[k], ppfs[i]);
                    lower_bounds[i * n_clusters + k] = distance_placeholder3;
                    if (distance_placeholder3 < distance_placeholder2) {
                        cluster_size[cluster_assignments[i]] -= 1;
                        cluster_assignments[i] = k;
                        upper_bounds[i] = distance_placeholder3;
                        cluster_size[cluster_assignments[i]] += 1;
                    }
                }
            }
        }
    }

    template<typename T>
    void WKmeans<T>::mini_batch_assign_clusters() {
        std::fill(cluster_size.begin(), cluster_size.end(), 0);
        int n_threads = std::thread::hardware_concurrency();
        std::vector <std::thread> threads;
        int chunk = (d_size + n_threads - 1) / n_threads;
        std::vector <std::vector<int>> local_counts(num_threads,
                                                    std::vector<int>(n_clusters, 0));
        for (int t = 0; t < n_threads; t++) {
            int start = chunk * t;
            int end = std::min(start + chunk, d_size);
            threads.emplace_back([=, this, &local_counts]() {
                for (int i = start; i < end; i++) {
                    int min = wasserstein_2(ppfs[i], clusters[i]);
                    local_counts[t][0] += 1;
                    cluster_assignments[i] = 0;
                    for (int k = 1; k < n_clusters; k++) {
                        if(wasserstein_2(ppfs[i], clusters[k]) < min){
                            local_counts[t][cluster_assignments[i]] -=1;
                            cluster_assignments[i] = k;
                            local_counts[t][k] +=1;
                        }
                    }
                }
            });
        }
        for(auto& thread: threads){
            thread.join();
        }
        for(int t = 0; t < n_threads; t++){
            for(int k = 0; k < n_clusters; k ++){
                cluster_size[k] = local_counts[t][k];
            }
        }
    }

    template<typename T>
    inline void WKmeans<T>::swap_clusters() {
        clusters = new_clusters;
    }

    template<typename T>
    std::vector <PDF<T>> WKmeans<T>::convert_clusters() {
        std::vector <PDF<T>> clusters_as_pdfs(n_clusters);
        for (int k = 0; k < n_clusters; k++) {
            CDF <T> temp_cdf = clusters[k].get_cdf(features);
            PDF <T> temp_pdf = temp_cdf.get_pdf();
            clusters_as_pdfs[k] = temp_pdf;
        }
        return clusters_as_pdfs;
    }

} // namespace kmeans