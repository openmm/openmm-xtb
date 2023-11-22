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

/**
 * This tests the Reference implementation of XtbForce.
 */

#include "XtbForce.h"
#include "openmm/internal/AssertionUtilities.h"
#include "openmm/Context.h"
#include "openmm/CustomExternalForce.h"
#include "openmm/LangevinMiddleIntegrator.h"
#include "openmm/NonbondedForce.h"
#include "openmm/Platform.h"
#include "openmm/System.h"
#include "openmm/reference/SimTKOpenMMRealType.h"
#include <iostream>
#include <string>
#include <vector>

using namespace XtbPlugin;
using namespace OpenMM;
using namespace std;

void testWater(Platform& platform, XtbForce::Method method) {
    // Create a system representing a single water molecule.

    System system;
    system.addParticle(16.0);
    system.addParticle(1.0);
    system.addParticle(1.0);
    vector<Vec3> positions(3);
    positions[0] = Vec3(0.1593, 0.7872, 0.5138);
    positions[1] = Vec3(0.1917, 0.7084, 0.4703);
    positions[2] = Vec3(0.2379, 0.8298, 0.5481);
    XtbForce* force = new XtbForce(method, 0.0, 1, false, {0, 1, 2}, {8, 1, 1});
    system.addForce(force);
    LangevinMiddleIntegrator integrator(300.0, 1.0, 0.001);
    Context context(system, integrator, platform);
    context.setPositions(positions);

    // Simulate it and make sure that the geometry remains reasonable.

    for (int i = 0; i < 100; i++) {
        integrator.step(1);
        State state = context.getState(State::Positions);
        Vec3 d1 = state.getPositions()[0]-state.getPositions()[1];
        Vec3 d2 = state.getPositions()[0]-state.getPositions()[2];
        double r1 = sqrt(d1.dot(d1));
        double r2 = sqrt(d2.dot(d2));
        ASSERT(r1 > 0.09 && r1 < 0.11);
        ASSERT(r2 > 0.09 && r2 < 0.11);
        double theta = acos(d1.dot(d2)/(r1*r2));
        ASSERT(theta > 1.7 && theta < 2.1);
    }

    // Make sure the force is the gradient of the energy.

    State state1 = context.getState(State::Positions | State::Forces);
    positions = state1.getPositions();
    const vector<Vec3>& forces = state1.getForces();
    double norm = 0.0;
    for (int i = 0; i < forces.size(); i++)
        norm += forces[i].dot(forces[i]);
    norm = sqrt(norm);
    const double stepSize = 1e-4;
    double step = 0.5*stepSize/norm;
    vector<Vec3> positions2(3), positions3(3);
    for (int i = 0; i < positions.size(); i++) {
        Vec3 p = positions[i];
        Vec3 f = forces[i];
        positions2[i] = p-f*step;
        positions3[i] = p+f*step;
    }
    context.setPositions(positions2);
    State state2 = context.getState(State::Energy);
    context.setPositions(positions3);
    State state3 = context.getState(State::Energy);
    ASSERT_EQUAL_TOL(norm, (state2.getPotentialEnergy()-state3.getPotentialEnergy())/stepSize, 5e-3);
}

void testPlatform(Platform& platform) {
    testWater(platform, XtbForce::GFN1xTB);
    testWater(platform, XtbForce::GFN2xTB);
    testWater(platform, XtbForce::GFNFF);
}

#include "xtb.h"

int main2() {
    xtb_TEnvironment env = xtb_newEnvironment();
    xtb_TCalculator calc = xtb_newCalculator();
    xtb_TResults res = xtb_newResults();
    xtb_setVerbosity(env, XTB_VERBOSITY_MUTED);
    int atoms = 2;
    double charge = 0;
    int multiplicity = 1;
    int numbers[] = {1, 1};
    double positions[] = {0, 0, 0, 1.9, 0, 0};
    xtb_TMolecule mol = xtb_newMolecule(env, &atoms, numbers, positions, &charge, &multiplicity, NULL, NULL);
    xtb_loadGFNFF(env, mol, calc, NULL);
    xtb_singlepoint(env, mol, calc, res);
    return 0;
}

int main() {
    try {
        Platform::loadPluginsFromDirectory(Platform::getDefaultPluginsDirectory());
        for (int i = 0; i < Platform::getNumPlatforms(); i++) {
            Platform& platform = Platform::getPlatform(i);
            printf("Testing %s\n", platform.getName().c_str());
            testPlatform(platform);
        }
    }
    catch(const std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Done" << std::endl;
    return 0;
}
