# SurfRemesh

<p align="center">
  <a href="https://github.com/csv610/SurfRemesh/releases">
    <img src="https://img.shields.io/github/v/release/csv610/SurfRemesh?include_prereleases&style=flat&label=release" alt="GitHub release">
  </a>
  <a href="https://github.com/csv610/SurfRemesh/blob/master/LICENSE">
    <img src="https://img.shields.io/github/license/csv610/SurfRemesh?style=flat" alt="License">
  </a>
  <a href="https://github.com/csv610/SurfRemesh/issues">
    <img src="https://img.shields.io/github/issues/csv610/SurfRemesh?style=flat" alt="Issues">
  </a>
  <a href="https://cmake.org/">
    <img src="https://img.shields.io/badge/C%2B%2B-20-blue?style=flat" alt="C++20">
  </a>
</p>

SurfRemesh is a C++20 library and command-line tool for refining triangular surface meshes using a planar Delaunay triangulation approach. It projects each triangular face to 2D, applies Delaunay refinement using Jonathan Shewchuk's Triangle algorithm, and maps the refined result back to 3D.

## Algorithm

The refinement works by processing each triangular face independently:

1. **Project to 2D**: Map each triangular face onto its own 2D plane using local coordinate system
2. **Delaunay Triangulation**: Apply Shewchuk's Triangle algorithm to generate optimal 2D triangulation
3. **Map back to 3D**: Convert refined 2D triangles back to 3D coordinates

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Input 3D Mesh  │────▶│  Per-Face 2D    │────▶│ Delaunay        │
│  (Triangles)    │     │  Projection     │     │ Triangulation   │
└─────────────────┘     └─────────────────┘     └─────────────────┘
                                                        │
                                                        ▼
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Output 3D Mesh │◀────│  Map Back to 3D │◀────│  Refined 2D     │
│  (Refined)       │     │                 │     │  Triangulation  │
└─────────────────┘     └─────────────────┘     └─────────────────┘
```

**Why Planar Delaunay?**

Since each original triangle is a flat planar surface:
- **No curvature constraints**: Delaunay optimization works on ideal flat geometry
- **No crease preservation**: Each face treated independently preserves boundaries
- **Quality optimization**: Triangle quality metric (aspect ratio) is globally optimized
- **Better downstream processing**: Refined mesh enables:
  - Smoothing algorithms converge faster
  - Fairing/optimization has more degrees of freedom
  - FEM mesh quality improved for simulation

## Features

- **Planar Delaunay Refinement**: Each triangle processed independently in 2D projection
- **Jonathan Shewchuk's Triangle**: Uses industry-standard Delaunay triangulation
- **High-Quality Meshes**: Optimized triangle quality independent of surface curvature
- **Multi-Format Support**: Load meshes in various formats (OFF, OBJ, STL, PLY, FBX, glTF, GLB, 3DS, DAE)
- **Edge Analysis**: Analyze edge length distribution with histogram visualization
- **Modern C++**: Built with C++20 and standard libraries

## Installation

### Prerequisites

- CMake 3.16+
- C++20 compatible compiler (GCC 10+, Clang 14+, MSVC 2019+)
- [ASSIMP](https://github.com/assimp/assimp) (fetched automatically)

### Build

```bash
# Clone the repository
git clone https://github.com/csv610/SurfRemesh.git
cd SurfRemesh

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)
```

## Usage

### Mesh Refinement

```bash
./surfremesh <input.mesh> <max_edge_length> <output.mesh>
```

**Example:**
```bash
./surfremesh models/bunny.obj 0.5 models/bunny_remeshed.obj
```

### Edge Statistics

```bash
./edge_stats <input.mesh> [num_bins]
```

**Example:**
```bash
./edge_stats models/bunny.obj 20
```

**Output:**
```
Edge length statistics:
  Min:    0.0234567
  Mean:   0.0345678
  Max:    0.0567890

Edge length distribution (20 bins):
  [  0.0235,   0.0252):   1234 (25.6789%)
  [  0.0252,   0.0269):    890 (18.4321%)
  ...
```

## Supported Formats

| Format | Extension | Read | Write |
|--------|-----------|------|--------|
| Wavefront OBJ | .obj | Yes | Yes |
| Object File Format | .off | Yes | Yes |
| Stanford PLY | .ply | Yes | No |
| STereo Lithography | .stl | Yes | No |
| Filmbox FBX | .fbx | Yes | No |
| glTF | .gltf, .glb | Yes | No |
| 3D Studio | .3ds | Yes | No |
| COLLADA | .dae | Yes | No |

## API Usage

```cpp
#include "SurfRemesh.h"

int main() {
    SurfRemesh remesh;
    remesh.setMesh("input.obj");
    remesh.setMaxEdgeLength(0.5);
    remesh.refine();
    remesh.saveAs("output.obj");

    // Get edge statistics
    auto lengths = remesh.getEdgeLengths();
    std::cout << "Min: " << lengths[0] << "\n";
    std::cout << "Mean: " << lengths[1] << "\n";
    std::cout << "Max: " << lengths[2] << "\n";

    return 0;
}
```

## Project Structure

```
SurfRemesh/
├── app/
│   ├── main.cpp          # Main application entry
│   └── edge_stats.cpp   # Edge statistics tool
├── src/
│   ├── SurfRemesh.h    # Library header
│   ├── SurfRemesh.cpp  # Library implementation
│   ├── veclib.hpp    # Vector mathematics
│   ├── trilib.hpp    # Triangle geometry
│   ├── triangle.c    # Delaunay triangulation
│   └── triangle.h    # Triangle header
├── CMakeLists.txt    # Build configuration
└── README.md
```

## Dependencies

- [ASSIMP](https://github.com/assimp/assimp) - 3D mesh import library
- [Triangle](https://www.cs.cmu.edu/~quake/triangle.html) - Delaunay triangulation (bundled)

## License

SurfRemesh is distributed under the MIT License. See [LICENSE](LICENSE) for details.

Triangle is copyrighted by Jonathan Richard Shewchuk and distributed under its own license.

## Acknowledgments

- [Jonathan Shewchuk](https://www.cs.berkeley.edu/~jrs/) for the Triangle library
- [ASSIMP](https://github.com/assimp/assimp) team for mesh I/O support

## Contributing

Contributions are welcome! Please read our [contributing guidelines](CONTRIBUTING.md) before submitting PRs.

## Related Papers

If you use this software in academic work, please cite the original Delaunay triangulation papers:

> Shewchuk, J. R. (2002). Delaunay Refinement Algorithms for Triangular Mesh Generation. *Computational Geometry: Theory and Applications*, 22(1-3), 21-74.

> Shewchuk, J. R. (1997). Adaptive Precision Floating-Point Arithmetic and Fast Robust Geometric Predicates. *Discrete & Computational Geometry*, 18(3), 305-363.

> Ruppert, J. (1995). A Delaunay Refinement Algorithm for Quality 2-Dimensional Mesh Generation. *Journal of Algorithms*, 18(3), 548-585.

---

<p align="center">
  Built with C++20 • CMake • ASSIMP
</p>