import openmm as mm
import openmm.app as app
import openmm.unit as unit
from openmmxtb import XtbForce
import unittest


class TestXtbForce(unittest.TestCase):

    def testForce(self):
        # Create a XtbForce and check its fields.

        force = XtbForce(XtbForce.GFNFF, 0.0, 1, False, [0, 1, 2], [8, 1, 1])
        self.assertEqual(XtbForce.GFNFF, force.getMethod())
        self.assertEqual(0.0, force.getCharge())
        self.assertEqual(1, force.getMultiplicity())
        self.assertEqual((0, 1, 2), force.getParticleIndices())
        self.assertEqual((8, 1, 1), force.getAtomicNumbers())

        # Create a System and verify that we can compute forces and energy.

        system = mm.System()
        system.addParticle(16.0)
        system.addParticle(1.0)
        system.addParticle(1.0)
        system.addForce(force)
        positions = [mm.Vec3(0.1593, 0.7872, 0.5138),
                     mm.Vec3(0.1917, 0.7084, 0.4703),
                     mm.Vec3(0.2379, 0.8298, 0.5481)]
        integ = mm.LangevinIntegrator(300.0, 1.0, 0.001)
        context = mm.Context(system, integ, mm.Platform.getPlatformByName('Reference'))
        context.setPositions(positions)
        state = context.getState(getEnergy=True, getForces=True)
        self.assertAlmostEqual(-859.209, state.getPotentialEnergy().value_in_unit(unit.kilojoules_per_mole), places=3)

    def testForceFields(self):
        """Test using a ForceField to create an XtbForce."""
        pdb = app.PDBFile('alanine-dipeptide.pdb')
        top = pdb.topology
        tests = [
            ('gfnff.xml', XtbForce.GFNFF, 0, 1, False),
            ('gfn1xtb.xml', XtbForce.GFN1xTB, 1, 3, False),
            ('gfn2xtb.xml', XtbForce.GFN2xTB, 0, 1, True)
        ]
        for forcefield, method, charge, multiplicity, periodic in tests:
            ff = app.ForceField(f'xtb/{forcefield}')
            system = ff.createSystem(top, charge=charge, multiplicity=multiplicity, periodic=periodic)
            xtb = [f for f in system.getForces() if XtbForce.isinstance(f)]
            self.assertEqual(1, len(xtb))
            force = XtbForce.cast(xtb[0])
            self.assertEqual(method, force.getMethod())
            self.assertEqual(charge, force.getCharge())
            self.assertEqual(multiplicity, force.getMultiplicity())
            self.assertEqual(tuple(range(top.getNumAtoms())), force.getParticleIndices())
            self.assertEqual(tuple(a.element.atomic_number for a in top.atoms()), force.getAtomicNumbers())

if __name__ == '__main__':
    unittest.main()