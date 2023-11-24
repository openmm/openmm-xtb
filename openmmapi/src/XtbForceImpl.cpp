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

#include "internal/XtbForceImpl.h"
#include "openmm/OpenMMException.h"
#include "openmm/internal/ContextImpl.h"

using namespace XtbPlugin;
using namespace OpenMM;
using namespace std;

XtbForceImpl::XtbForceImpl(const XtbForce& owner) : CustomCPPForceImpl(owner), owner(owner), env(nullptr), calc(nullptr), res(nullptr), mol(nullptr), hasInitializedMolecule(false) {
}

XtbForceImpl::~XtbForceImpl() {
    if (res != nullptr)
        xtb_delResults(&res);
    if (calc != nullptr)
        xtb_delCalculator(&calc);
    if (mol != nullptr)
        xtb_delMolecule(&mol);
    if (env != nullptr)
        xtb_delEnvironment(&env);
}

void XtbForceImpl::initialize(ContextImpl& context) {
    CustomCPPForceImpl::initialize(context);
    indices = owner.getParticleIndices();
    numbers = owner.getAtomicNumbers();
    if (indices.size() != numbers.size())
        throw OpenMMException("Different numbers of particle indices and atomic numbers are specified");
    charge = owner.getCharge();
    multiplicity = owner.getMultiplicity();
    env = xtb_newEnvironment();
    calc = xtb_newCalculator();
    res = xtb_newResults();
    xtb_setVerbosity(env, XTB_VERBOSITY_MUTED);
    checkErrors();
    int numParticles = indices.size();
    positionVec.resize(3*numParticles, 0.0);
    forceVec.resize(3*numParticles);
  }

double XtbForceImpl::computeForce(ContextImpl& context, const vector<Vec3>& positions, vector<Vec3>& forces) {
    const double distanceScale = 18.897261246257703; // Convert nm to bohr
    const double energyScale = 2625.4996394798254; // Convert Hartree to kJ/mol
    const double forceScale = 49614.75258920568; // Convert Hartree/bohr to kJ/mol/nm

    // Pass the current state to XTB.

    for (int i = 0; i < positions.size(); i++) {
        positionVec[3*i] = distanceScale*positions[i][0];
        positionVec[3*i+1] = distanceScale*positions[i][1];
        positionVec[3*i+2] = distanceScale*positions[i][2];
    }
    Vec3 box[3];
    context.getPeriodicBoxVectors(box[0], box[1], box[2]);
    double boxVectors[9];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            boxVectors[3*i+j] = distanceScale*box[i][j];
    if (hasInitializedMolecule)
        xtb_updateMolecule(env, mol, positionVec.data(), boxVectors);
    else {
        bool periodic[3] = {owner.usesPeriodicBoundaryConditions(), owner.usesPeriodicBoundaryConditions(), owner.usesPeriodicBoundaryConditions()};
        int numParticles = indices.size();
        mol = xtb_newMolecule(env, &numParticles, numbers.data(), positionVec.data(), &charge, &multiplicity, boxVectors, periodic);
        checkErrors();
        if (owner.getMethod() == XtbForce::GFN1xTB)
            xtb_loadGFN1xTB(env, mol, calc, NULL);
        else if (owner.getMethod() == XtbForce::GFN2xTB)
            xtb_loadGFN2xTB(env, mol, calc, NULL);
        else if (owner.getMethod() == XtbForce::GFNFF)
            xtb_loadGFNFF(env, mol, calc, NULL);
        checkErrors();
    }
    checkErrors();

    // Perform the computation.

    xtb_singlepoint(env, mol, calc, res);
    checkErrors();
    double energy;
    xtb_getEnergy(env, res, &energy);
    checkErrors();
    xtb_getGradient(env, res, forceVec.data());
    for (int i = 0; i < positions.size(); i++)
        forces[i] = -forceScale*Vec3(forceVec[3*i], forceVec[3*i+1], forceVec[3*i+2]);
    return energyScale*energy;
}

void XtbForceImpl::checkErrors() {
    if (xtb_checkEnvironment(env)) {
        vector<char> buffer(1000);
        int maxLength = buffer.size();
        xtb_getError(env, buffer.data(), &maxLength);
        throw OpenMMException(string(buffer.data()));
    }
}