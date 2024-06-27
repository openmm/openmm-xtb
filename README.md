OpenMM XTB Plugin
=================

This project provides a connection between [OpenMM](http://openmm.org) and [XTB](https://xtb-docs.readthedocs.io/en/latest).
It lets you compute forces and energies with the semi-empirical quantum chemistry methods provided by XTB.

This plugin requires XTB and OpenMM 8.1 or later.

Installation
============

The easiest way to install this package along with all dependencies is with the conda or mamba package manager.

```
mamba install -c conda-forge openmm-xtb
```

Building From Source
====================

This project uses [CMake](http://www.cmake.org) for its build system.  To build it, follow these
steps:

1. Create a directory in which to build the plugin.

2. Run the CMake GUI or ccmake, specifying your new directory as the build directory and the top
level directory of this project as the source directory.

3. Press "Configure".

4. Set OPENMM_DIR to point to the directory where OpenMM is installed.  This is needed to locate
the OpenMM header files and libraries.  If OpenMM was installed with conda, this should be the root
directory of the conda environment.

6. Set XTB_INCLUDE_DIR and XTB_LIBRARY_DIR to point to the directories where the XTB header
files and libraries are installed.

7. Set CMAKE_INSTALL_PREFIX to the directory where the plugin should be installed.  Usually,
this will be the same as OPENMM_DIR, so the plugin will be added to your OpenMM installation.

8. Press "Configure" again if necessary, then press "Generate".

9. Use the build system you selected to build and install the plugin.  For example, if you
selected Unix Makefiles, type `make install` to install the plugin, and `make PythonInstall` to
install the Python wrapper.

Using The Plugin
================

Simply create a `XtbForce` object and add it to your `System`.  For example,

```Python
from openmmxtb import XtbForce
system.addForce(XtbForce(XtbForce.GFN2xTB, 0.0, 1, False, particleIndices, atomicNumbers))
```

The arguments are as follows.

- `method`: the method to use for computing forces and energy.  The currently supported options are `GFN1xTB`, `GFN2xTB`, and `GFNFF`.
- `charge`: the total charge of the system
- `multiplicity`: the spin multiplicity
- `periodic`: whether to apply periodic boundary conditions
- `particleIndices`: a list containing the indices of the particles within the System to which the force should be applied.
   This allows you to use XTB for only part of the full system.
- `atomicNumbers`: a list containing the atomic numbers of the particles to which the force should be applied.  This must
   have the same length as `particleIndices`.  Element `i` is the atomic number of the particle specified by element
  `i` of `particleIndices`.

License
=======

Portions copyright (c) 2023 Stanford University and the Authors.

Authors: Peter Eastman

Contributors:

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.
