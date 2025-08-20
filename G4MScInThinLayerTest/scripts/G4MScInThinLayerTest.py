#!/usr/bin/env python
# -*- coding: utf-8 -*-

###############################################################################
# (c) Copyright 2000-2020 CERN for the benefit of the LHCb Collaboration      #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

"""
    Stirring application for the Multiple Scattering in Thin Layer test (aka TestEm5).
"""

import argparse
import os
import subprocess


def create_cfg_file(n_runs=1000, name='G4MScInThinLayerTest.mac'):
    """
        Create the stirring file with Geant4 directives for
        all runs, energies and limits.
    """
    with open('G4MScInThinLayerTest.mac', 'w') as outfile:
        outfile.write('/control/cout/ignoreThreadsExcept 0\n' +
                      '/control/verbose 1\n' +
                      '/run/verbose 1\n' +
                      '/testem/det/setAbsMat Silicon\n' +
                      '/testem/det/setAbsThick 300 um\n' +
                      '/testem/det/setAbsYZ    42 mm\n' +
                      '/testem/phys/addPhysics  emstandard_opt2\n' +
                      '/run/setCut 5 mm\n')

        energies = [1, 2, 3, 4, 5, 7, 9, 12, 15, 20, 25, 30, 40]
        limits = [4.0, 3.0, 2.0, 1.5, 1.0, 0.6,
                  0.5, 0.5, 0.4, 0.2, 0.2, 0.15, 0.15]

        for en in energies:
            lim = 0
            for i in range(0, n_runs):
                outfile.write(('/run/initialize\n' +
                               '/testem/gun/setDefault\n' +
                               '/gun/particle e-\n' +
                               '/gun/energy {:.2f} GeV\n' +
                               '/analysis/setFileName Output_{:.2f}\n' +
                               '/analysis/h1/set  1 100 0 100 keV       #energy depostied in absorber\n' +
                               '/analysis/h1/set  10 100 0 100 keV      #K.E at exit of world\n' +
                               '/analysis/h1/set  11 100 0 100 keV      #Energy fluence dE/dOmega\n' +
                               '/analysis/h1/set  12 100 0.0 0.09 mrad          #space angle dN/dOmega\n' +
                               '/analysis/h1/set  13 100 -{:.2f} {:.2f} mrad        #projected angle at exit of world\n' +
                               '/analysis/h1/set  14 100 -14 14 nm      #projected positon at exit of world\n' +
                               '/analysis/h1/set  15 100 0 44 mm        #radius at exit of world\n' +
                               '/run/beamOn 10000\n').format(en, en, limits[lim], limits[lim]))
            lim += 1


def main():
    args_parser = argparse.ArgumentParser(
        description='Stirring application for the Multiple Scattering in Thin Layer test (aka TestEm5).')

    args_parser.add_argument('n_runs',
                             help='Number of runs for determining uncertainties (int)',
                             nargs='?',
                             action='store',
                             type=int,
                             default=1000)

    parsed_args, leftovers = args_parser.parse_known_args()

    print('Starting MSc in Thin Layer test with ' +
          str(parsed_args.n_runs) + ' run(s)...')

    workspace = os.path.join(os.sep, os.getcwd(), 'G4MScInThinLayerTestOutput')
    if not os.path.exists(workspace):
        os.makedirs(workspace)

    os.chdir(workspace)
    print('Working directory:', os.getcwd())

    cfg_file_name = 'G4MScInThinLayerTest.mac'

    print('Creating the Geant4 directives file...')
    create_cfg_file(n_runs=parsed_args.n_runs, name=cfg_file_name)

    print('Executing the test...')
    cmd = ' '.join(['G4MScInThinLayerTest',
                    cfg_file_name, str(parsed_args.n_runs)])
    subprocess.call(cmd, shell=True)


if __name__ == '__main__':
    main()

# EOF
