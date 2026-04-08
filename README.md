# Parallelized kmeans++ in Wasserstein space
In this repository I implemented a kmeans++ algorithm in Wasserstein space. For the mathematical
insight on this algorithm, see wasserstein_kmeans.pdf, along with any relevant sources.

## Overview
- This project allows the user to cluster 1 dimensional probability mass functions using the second Wasserstein distance function.
- The clustering algorithm was implemented using a standard KMeans procedure.
- The algorithm utilizes the kmeans++ initialization procedure to produce better starting clusters and improve local minimum values.
- It also incorporates Charles Elkan's acceleration algorithm, using the triangle inequality to skip computations during cluster reassignments.
- Additionally, the code utilizes all available system threads to run processes in parallel.

## File Structure  
<small> (Code might change without updating, general structure should remain) </small>


```
wasserstein_kmeanspp/
├── build/                      # Build artifacts (generated)
├── includes/                   # Core library headers
│   ├── containers/             # Data structures and abstractions
│   │   ├── cdf.h               # Cumulative distribution functions
│   │   ├── numericArray.h      # Base class for numeric array containers
│   │   ├── pdf.h               # Probability density functions
│   │   └── ppf.h               # Percent point functions (inverse CDF)
│   │
│   ├── wkmeans/                # Wasserstein K-means implementation
│   │   ├── wkmeans.h                   # Main class definition
│   │   ├── wkmeans_bounds.tpp          # Update bounds
│   │   ├── wkmeans_clustering.tpp      # Assign clusters, update clusters...
│   │   ├── wkmeans_constructor.tpp     
│   │   ├── wkmeans_init_procedure.tpp  # Initialization (k-means++)
│   │   ├── wkmeans_metrics.tpp         # Performance metrics
│   │   └── wkmeans_run_procedure.tpp   # Main algorithm loop
│   │
│   ├── config.h               # Global configuration
│   └── distributions.hpp      # Distribution utilities
│
├── tests/                     # Test files (see Tests section for more detail)
│
├── CMakeLists.txt             # CMake build configuration
├── Makefile                   # Alternative build system
├── main.cpp                   # Entry point
├── mini_batch_kmeans.cpp      # Mini-batch variant implementation
│
├── README.md                  # Project documentation
├── wasserstein_kmeans.pdf     # Paper / reference material
└── wasserstein_kmeans.qmd     # Quarto source for documentation
```
## Build
To build this project, git clone the repository into your local environemnt

```
git clone git@github.com:chadakinz/kmeanspp_emd.git
```
Then inside the repository run:
```
cmake -B build
```
This code is run from main where the parameters of the algorithm are intialized and used to begin the run procedure.

An example of how to run the algorithm (after building it using cmake):

```
./build/bin/main -k 1000 -i sample.txt -u 500 -s 5e-4 -t 5 -N 10 -o ./output.txt
```
Each flag, its default initialization and its description can be seen in this format:
```
(flag, default): description
(-k, 10): number of clusters
(-i, NONE): name of the input file (must be a csv)
(-o, output_999.csv): name of the output file
(-s, 1e-4): epsilon parameter used to specify when program should terminate
(-u, 20): size of percent point function bins
(-N, 50): Number of restarts to run algorithm
(-t, 1): number of threads to run the algorithm (cannot exceed Number of restarts)
(S, 0): choose the specify the seed for the random number generator

```
You can view this if you run the program without any flags.

## Implementation


This algorithm is consists of 7 steps, all of which are executed in ./includes/wkmeans/wkmeans_run_procedure.tpp file.

### Initialization Procedure
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
### Update Procedure

The update phase in `run_restart()` primarily consists of `update_clusters()` and `update_bounds()`. These steps recompute cluster centers and adjust distance bounds efficiently.

**update_clusters():**  
This function recomputes cluster centers based on current assignments. It first initializes an accumulator (`sum_cluster`) for each cluster. Then, for each data point, it adds its PPF to the sum of its assigned cluster. After aggregating all points, each new cluster center is computed as the average of its assigned points:
```math
\text{new\_clusters}[k] = \frac{\sum_{\{i: c_i = k\}} \text{ppfs}[i]}{\text{cluster\_size}[k]}
```
This is the standard centroid update step adapted to PPFs.

**update_bounds():**  
This function updates upper and lower distance bounds after cluster centers move.

- First, it computes **cluster deltas**, i.e., the movement of each cluster:
  ```math
  \delta_k = W_2(\text{clusters}[k], \text{new\_clusters}[k])
  ```

- Then, in parallel, it computes for each point the movement of its assigned cluster and stores these deltas.

- In a sequential phase:
    - The **upper bound** for each point is increased by the movement of its assigned cluster:
      ```math
      \text{upper\_bounds}[i] \mathrel{+}= \delta_{c_i}
      ```
    - A flag `r[i]` is set to indicate the bound needs reconsideration.
    - The **lower bounds** to all clusters are decreased using the triangle inequality:
      ```math
      \text{lower\_bounds}[i,k] = \max\big(\text{lower\_bounds}[i,k] - \delta_k,\ 0\big)
      ```

This avoids recomputing all distances by adjusting bounds based on how much cluster centers moved.
# Assign New Clusters

The ```assign_new_clusters()``` function reassigns points to clusters using bounds to minimize distance calculations.

## Procedure

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
