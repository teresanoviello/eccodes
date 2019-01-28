ecCodes
=======

[![Windows: master](https://img.shields.io/appveyor/ci/ecmwf/eccodes/master.svg?label=windows-master)](https://ci.appveyor.com/project/ecmwf/eccodes/branch/master)
[![Windows: develop](https://img.shields.io/appveyor/ci/ecmwf/eccodes/develop.svg?label=windows-dev)](https://ci.appveyor.com/project/ecmwf/eccodes/branch/develop)

ecCodes is a package developed by ECMWF which provides an application programming interface
and a set of tools for decoding and encoding messages in the following formats:

   * WMO FM-92 GRIB edition 1 and edition 2
   * WMO FM-94 BUFR edition 3 and edition 4
   * WMO GTS abbreviated header (only decoding)

A useful set of command line tools provide quick access to the messages.
C, Fortran 90 and Python interfaces provide access to the main ecCodes functionality.

ecCodes is an evolution of GRIB API.
It is designed to provide the user with a simple set of functions to access data from
several formats with a key/value approach.

Documentation can be found here:
   https://confluence.ecmwf.int/display/ECC/ecCodes+Home

INSTALLATION
------------

1. Download ecCodes from https://confluence.ecmwf.int/display/ECC/Releases

2. Unpack distribution:
   > tar -xzf eccodes-x.y.z-Source.tar.gz

3. Create a separate directory where to build ecCodes:
   > mkdir build
   > cd build

4. Run cmake pointing to the source and specify the installation location:
   > cmake  ../eccodes-x.y.z-Source -DCMAKE_INSTALL_PREFIX=/path/to/where/you/install/eccodes
   
   It is strongly recommended that you install into a clean directory

5. Compile, test and install:
   > make
   > ctest
   > make install


For more details, please see:
https://confluence.ecmwf.int/display/ECC/ecCodes+installation

If you encounter any problems please send an e-mail with your problem to:

   Software.Support@ecmwf.int



COPYRIGHT AND LICENSE
----------------------

Copyright 2005-2018 ECMWF.

This software is licensed under the terms of the Apache Licence Version 2.0
which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

