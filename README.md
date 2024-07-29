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

The plugin can be used in two ways: by directly creating a force and adding it to your `System`,
or by using a `ForceField`.

Creating a Force Directly
-------------------------

This is the most general and flexible approach, giving complete control over all aspects.  For example, you can use
XTB to compute forces on some parts of the system but a classical force field to compute the forces on other parts.
To use this method, simply create a `XtbForce` object and add it to your `System`.  For example,

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

Using a ForceField
------------------

The above method requires you to create the `System` yourself, add particles to it, and build the lists of particle indices
and atomic numbers.  In simple cases where you want to use XTB as the only force for the entire system, it can be easier
to let a `ForceField` create the `System` for you.

```python
ff = app.ForceField('xtb/gfn2xtb.xml')
system = ff.createSystem(topology, charge=2, multiplicity=1, periodic=False)
```

The plugin provides three force field files for the available methods: `'xtb/gfn1xtb.xml'`, `'xtb/gfn2xtb.xml'` and `'xtb/gfnff.xml'`.

License
=======

Portions copyright (c) 2023-2024 Stanford University and the Authors.

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
