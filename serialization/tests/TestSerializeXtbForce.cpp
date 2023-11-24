/* -------------------------------------------------------------------------- *
 *                                OpenMMPlumed                                 *
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
#include "openmm/Platform.h"
#include "openmm/internal/AssertionUtilities.h"
#include "openmm/serialization/XmlSerializer.h"
#include <iostream>
#include <sstream>

using namespace XtbPlugin;
using namespace OpenMM;
using namespace std;

extern "C" void registerXtbSerializationProxies();

void testSerialization() {
    // Create a Force.

    XtbForce force(XtbForce::GFN2xTB, 1.0, 3, true, {0, 1, 2}, {8, 1, 1});

    // Serialize and then deserialize it.

    stringstream buffer;
    XmlSerializer::serialize<XtbForce>(&force, "Force", buffer);
    XtbForce* copy = XmlSerializer::deserialize<XtbForce>(buffer);

    // Compare the two forces to see if they are identical.

    XtbForce& force2 = *copy;
    ASSERT_EQUAL(force.getMethod(), force2.getMethod());
    ASSERT_EQUAL(force.getCharge(), force2.getCharge());
    ASSERT_EQUAL(force.getMultiplicity(), force2.getMultiplicity());
    ASSERT_EQUAL(force.usesPeriodicBoundaryConditions(), force2.usesPeriodicBoundaryConditions());
    ASSERT_EQUAL_CONTAINERS(force.getParticleIndices(), force2.getParticleIndices());
    ASSERT_EQUAL_CONTAINERS(force.getAtomicNumbers(), force2.getAtomicNumbers());
}

int main() {
    try {
        registerXtbSerializationProxies();
        testSerialization();
    }
    catch(const exception& e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }
    cout << "Done" << endl;
    return 0;
}
