#ifndef OPENMM_XTBFORCEIMPL_H_
#define OPENMM_XTBFORCEIMPL_H_

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

#include "XtbForce.h"
#include "openmm/internal/ContextImpl.h"
#include "openmm/internal/CustomCPPForceImpl.h"
#include "xtb.h"

namespace XtbPlugin {

/**
 * This is the internal implementation of XtbForce.
 */

class OPENMM_EXPORT_XTB XtbForceImpl : public OpenMM::CustomCPPForceImpl {
public:
    XtbForceImpl(const XtbForce& owner);
    ~XtbForceImpl();
    void initialize(OpenMM::ContextImpl& context);
    const XtbForce& getOwner() const {
        return owner;
    }
    double computeForce(OpenMM::ContextImpl& context, const std::vector<OpenMM::Vec3>& positions, std::vector<OpenMM::Vec3>& forces);
private:
    void checkErrors();
    const XtbForce& owner;
    xtb_TEnvironment env;
    xtb_TCalculator calc;
    xtb_TResults res;
    xtb_TMolecule mol;
    double charge;
    int multiplicity;
    bool hasInitializedMolecule;
    std::vector<int> indices, numbers;
    std::vector<double> positionVec, forceVec;
};

} // namespace XtbPlugin

#endif /*OPENMM_XTBFORCEIMPL_H_*/
