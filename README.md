# Movie Recommender System

## Quick Setup Instructions

### 1. Required Files
- Download all files from the GitHub repository
- Ensure `ml-25m/` folder is in the project root (DOWNLOAD: https://www.kaggle.com/datasets/garymk/movielens-25m-dataset)
- Copy `libcurl-x64.dll` to `cmake-build-debug/` folder

### 2. Running the Program
After building, run the executable. You'll see:

Movie Recommender System
========================

Build and Run Graph Recommender

Benchmark Algorithms (Graph vs Heap)

Exit
Choose option:

### 3. Demo Options

**For Fast Demo:**
- Choose **Option 1** -> **1** (TMDb API, small dataset)
- Enter `100` for movie pool size
- Search for movies like "The Dark Knight"

**For Large Dataset Demo:**
- Choose **Option 1** -> **2** (MovieLens 25M, 62K+ movies)
- This might be a a couple of minutes

### 4. Performance Note
If the large dataset is too slow, edit `RunGraph.h`:
- Change `100000` to `1000` for faster performance
- Located in the MovieLens loader section

### 5. Algorithm Comparison
Use **Option 2** to compare:
- Graph-based recommendations (Dijkstra)
- Heap-based recommendations (Top-K)
- Performance metrics and recommendation quality
- thank you for reading
