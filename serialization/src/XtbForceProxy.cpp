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

#include "XtbForceProxy.h"
#include "XtbForce.h"
#include "openmm/serialization/SerializationNode.h"

using namespace XtbPlugin;
using namespace OpenMM;
using namespace std;

XtbForceProxy::XtbForceProxy() : SerializationProxy("XtbForce") {
}

void XtbForceProxy::serialize(const void* object, SerializationNode& node) const {
    node.setIntProperty("version", 0);
    const XtbForce& force = *reinterpret_cast<const XtbForce*>(object);
    node.setIntProperty("method", (int) force.getMethod());
    node.setDoubleProperty("charge", force.getCharge());
    node.setIntProperty("multiplicity", force.getMultiplicity());
    node.setBoolProperty("periodic", force.usesPeriodicBoundaryConditions());
    const vector<int>& indices = force.getParticleIndices();
    auto& indicesNode = node.createChildNode("indices");
    for (int i = 0; i < indices.size(); i++)
        indicesNode.createChildNode("particle").setIntProperty("index", indices[i]);
    const vector<int>& numbers = force.getAtomicNumbers();
    auto& numbersNode = node.createChildNode("numbers");
    for (int i = 0; i < indices.size(); i++)
        numbersNode.createChildNode("particle").setIntProperty("number", numbers[i]);
}

void* XtbForceProxy::deserialize(const SerializationNode& node) const {
    const int version = node.getIntProperty("version");
    if (version != 0)
        throw OpenMMException("Unsupported version number");
    vector<int> indices, numbers;
    for (const auto& particle: node.getChildNode("indices").getChildren())
        indices.push_back(particle.getIntProperty("index"));
    for (const auto& particle: node.getChildNode("numbers").getChildren())
        numbers.push_back(particle.getIntProperty("number"));
    XtbForce* force = new XtbForce((XtbForce::Method) node.getIntProperty("method"), node.getDoubleProperty("charge"),
            node.getIntProperty("multiplicity"), node.getBoolProperty("periodic"), indices, numbers);
    return force;
}
