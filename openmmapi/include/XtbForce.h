#ifndef OPENMM_XTBFORCE_H_
#define OPENMM_XTBFORCE_H_

/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2023 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "openmm/Context.h"
#include "openmm/Force.h"
#include "internal/windowsExportXtb.h"
#include <vector>

namespace XtbPlugin {

/**
 * This class implements a connection between OpenMM and XTB (https://xtb-docs.readthedocs.io/en/latest).  It is a Force
 * object that you add to the System with addForce().  It computes forces on a subset (or potentially all) of the
 * particles in the System using one of the methods supported by XTB.
 */

class OPENMM_EXPORT_XTB XtbForce : public OpenMM::Force {
public:
    /**
     * This is an enumeration of methods that can be used to compute forces and energies.
     */
    enum Method {
        GFN1xTB = 0,
        GFN2xTB = 1,
        GFNFF = 2
    };
    /**
     * Create a XtbForce.
     *
     * @param method           the method to use for computing forces and energy
     * @param charge           the total charge
     * @param multiplicity     the spin multiplicity
     * @param particleIndices  the indices of the particles within the System to which this force should be applied
     * @param atomicNumbers    the atomic numbers of the particles to which this force should be applied.  This must
     *                         have the same length as particleIndices.  Element i of this vector is the atomic number
     *                         of the particle specified by element i of particleIndices.
     */
    XtbForce(Method method, double charge, int multiplicity, bool periodic, const std::vector<int>& particleIndices, const std::vector<int>& atomicNumbers);
    /**
     * Get the method to use for computing forces and energy.
     */
    Method getMethod() const;
    /**
     * Set the method to use for computing forces and energy.
     */
    void setMethod(Method method);
    /**
     * Get the total charge of the XTB system.
     */
    double getCharge() const;
    /**
     * Set the total charge of the XTB system.
     */
    void setCharge(double charge);
    /**
     * Get the spin multiplicity of the XTB system.
     */
    int getMultiplicity() const;
    /**
     * Set the spin multiplicity of the XTB system.
     */
    void setMultiplicity(int multiplicity);
    /**
     * Get the indices of the particles this force is applied to.
     */
    const std::vector<int>& getParticleIndices() const;
    /**
     * Set the indices of the particles this force is applied to.
     */
    void setParticleIndices(const std::vector<int>& indices);
    /**
     * Get the atomic numbers of the particles this force is applied to.
     */
    const std::vector<int>& getAtomicNumbers() const;
    /**
     * Set the atomic numbers of the particles this force is applied to.
     */
    void setAtomicNumbers(const std::vector<int>& numbers);
    /**
     * Get whether this force uses periodic boundary conditions.
     */
    bool usesPeriodicBoundaryConditions() const;
    /**
     * Set whether this force uses periodic boundary conditions.
     */
    void setUsesPeriodicBoundaryConditions(bool periodic);
protected:
    OpenMM::ForceImpl* createImpl() const;
private:
    Method method;
    double charge;
    int multiplicity;
    bool periodic;
    std::vector<int> particleIndices, atomicNumbers;
};

} // namespace XtbPlugin

#endif /*OPENMM_XTBFORCE_H_*/
