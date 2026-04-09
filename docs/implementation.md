# Implementation


This algorithm is consists of 7 steps, all of which are executed in ./includes/wkmeans/wkmeans_run_procedure.tpp file.

## Initialization Procedure
The initialization phase in `run_restart()` consists of two steps: `init_clusters()` and `init_bounds()`.

**init_clusters():**  
This function initializes the cluster centers using a k-means++ style approach. It first selects an initial cluster uniformly
at random from the data (`ppfs`). For each subsequent cluster, it computes the squared Wasserstein-2 distance from each data point to its nearest existing cluster center.
These distances are used to define a discrete probability distribution, and a new cluster center is sampled with probability proportional to distance.
Distance computations are parallelized using multiple threads. This results in well-separated initial cluster centers.

**init_bounds():**  
This function assigns each data point to its nearest cluster center and initializes distance bounds. For every point, it
computes the distance to all cluster centers, assigns the point to the closest one, and stores the corresponding distance
as an upper bound. Distances to all clusters are stored as lower bounds. Cluster sizes are computed using thread-local
counts and aggregated after parallel execution.
## Update Procedure

The update phase in `run_restart()` primarily consists of `update_clusters()` and `update_bounds()`. These steps recompute cluster centers and adjust distance bounds efficiently.

**update_clusters():**  
This function recomputes cluster centers based on current assignments. It first initializes an accumulator (`sum_cluster`) for each cluster. Then, for each data point, it adds its PPF to the sum of its assigned cluster. After aggregating all points, each new cluster center is computed as the average of its assigned points:
```math
\text{new\_clusters}[k] = \frac{\sum_{\{i: c_i = k\}} \text{ppfs}[i]}{\text{cluster\_size}[k]}
```
This is the standard centroid update step adapted to PPFs.

**update_bounds():**  
The ```update_bounds()``` function updates upper and lower distance bounds after clusters move, reducing unnecessary distance computations.


1. Compute movement of each cluster:

```cpp
std::vector<T> cluster_deltas(n_clusters);
for (int k = 0; k < n_clusters; k++){
cluster_deltas[k] = wasserstein_2(clusters[k], new_clusters[k]);
}
```

2. Set up parallel computation for all points:

```cpp
int num_threads = std::thread::hardware_concurrency();
size_t chunk_size = (d_size + num_threads - 1) / num_threads;
std::vector<std::thread> threads;
struct ThreadResult {
std::vector<T> deltas;
size_t start, end;
};
std::vector<ThreadResult> results(num_threads);
```

3. Phase 1: compute movement for each point’s assigned cluster in parallel:

```cpp
for (int t = 0; t < num_threads; t++){
size_t start = t * chunk_size;
size_t end = std::min(start + chunk_size, static_cast<size_t>(d_size));
results[t].start = start;
results[t].end = end;
results[t].deltas.resize(end - start);

    threads.emplace_back([this, t, start, end, &results](){
        auto& res = results[t];
        for (size_t i = 0; i < res.deltas.size(); i++){
            int cluster_id = cluster_assignments[start + i];
            res.deltas[i] = wasserstein_2(clusters[cluster_id], new_clusters[cluster_id]);
        }
    });
}

for (auto& th : threads) th.join();
threads.clear();
```

4. Phase 2: update upper and lower bounds sequentially:

```cpp
for (int t = 0; t < num_threads; t++){
auto& res = results[t];
for (size_t i = 0; i < res.deltas.size(); i++){
size_t idx = res.start + i;
T delta = res.deltas[i];

        upper_bounds[idx] += delta;
        r[idx] = true;

        size_t base_idx = idx * n_clusters;
        for (int k = 0; k < n_clusters; k++){
            lower_bounds[base_idx + k] = std::max(lower_bounds[base_idx + k] - cluster_deltas[k], T{});
        }
    }
}
```
This avoids recomputing all distances by adjusting bounds based on how much cluster centers moved.
## Assign New Clusters

The ```assign_new_clusters()``` function reassigns points to clusters using bounds to minimize distance calculations.

1. Get threshold vector:

```cpp
std::vector<T> s = get_vector_s();
```

2. Loop over points:

```cpp
for(int i = 0; i < d_size; i++){
if(upper_bounds[i] <= s[cluster_assignments[i]]) continue;
```

3. Loop over clusters and skip those that cannot improve the assignment:

```cpp
for(int k = 0; k < n_clusters; k++){
if(k == cluster_assignments[i] || upper_bounds[i] <= lower_bounds[i * n_clusters + k]) continue;
```

4. Preliminary triangle inequality check:

```cpp
distance_placeholder1 = 0.5 * W_2(clusters[k], clusters[c_i])
```

5. Recompute distance if ```r[i]``` is true:

```cpp
distance_placeholder2 = W_2(clusters[c_i], ppfs[i])
```

6. Compute candidate cluster distance and reassign if smaller:

```cpp
distance_placeholder3 = W_2(clusters[k], ppfs[i])
cluster_size[c_i] -= 1
c_i = k
upper_bounds[i] = distance_placeholder3
cluster_size[c_i] += 1
```
## Metrics

Metric functions are used to see how close our algorithm is to a local minimum for the kmeans objective. There are two 
objective functions which we can utilize to produce a correct algorithm. `delta_clusters()` and `get_objective()`.

`delta_clusters()`: This function takes the clusters from generated from step (t-1) and finds the average difference between
the clusters generated in step t.
```cpp
T sum = T{};
for (std::size_t i = 0; i < n_clusters; i++) {
    sum += wasserstein_2(old_clusters[i], new_clusters[i]);
}
return sum / static_cast<T>(n_clusters);
```
Using our distance metric, if this average sum is less than our parameter `epsilon` then we stop the algorithm and return 
the objective recieved from the current restart.

`get_objective()`: This function returns the objective defined by the standard kmeans algorithm (see `wasserstein_kmeans.pdf` for more detail).
```cpp
for(int t = 0; t < num_threads; t++){
    int start = t * chunk;
    int end = std::min(start + chunk, d_size);
    threads.emplace_back([=, this, &thread_local_sum](){
        for(int i = start; i < end; i++){
            thread_local_sum[t] += wasserstein_2(ppfs[i], clusters[cluster_assignments[i]]);
        }
    });
}
```
We use standard threading procedure to speed up calculations. 
