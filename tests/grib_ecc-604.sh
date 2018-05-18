#!/bin/sh
# Copyright 2005-2018 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

label="grib_ecc-604"
temp_dir=tempdir.${label}

NUM_THREADS=6
NUM_ITER=100
OUTPUT=output

validate()
{
    echo "Checking every output file is identical..."
    # Get checksum of first file
    ck1=`cksum $OUTPUT/output_file_0-0.grib | awk '{print $1}'`
    set +x
    # Get checksum of all of them and sort unique
    res=`cksum $OUTPUT/output_file_* | awk '{print $1}' | sort -u`
    set -x
    # Should be the same as the first
    [ "$res" = "$ck1" ]
}
process()
{
    input=$1 # The input GRIB file

    # Test 01: Clone + output
    # ------------------------
    rm -fr $OUTPUT;   mkdir -p $OUTPUT
    time ${test_dir}/grib_ecc-604-1 par $input $NUM_THREADS $NUM_ITER
    validate

    # Test 02: No clone + output
    # --------------------------
    rm -fr $OUTPUT;   mkdir -p $OUTPUT
    time ${test_dir}/grib_ecc-604-2 par $input $NUM_THREADS $NUM_ITER
    validate

    # Test 03: Clone + no output
    # ---------------------------
    rm -fr $OUTPUT
    time ${test_dir}/grib_ecc-604-3 par $input $NUM_THREADS $NUM_ITER
    # Nothing to validate as there is no output
}
###################################################
rm -fr $temp_dir
mkdir -p $temp_dir
cd $temp_dir

# GRIB1 inputs
process $ECCODES_SAMPLES_PATH/gg_sfc_grib1.tmpl
process ${data_dir}/gen_bitmap.grib
process ${data_dir}/spectral_complex.grib1
#process ${data_dir}/gen.grib

# GRIB2 inputs
process $ECCODES_SAMPLES_PATH/gg_sfc_grib2.tmpl
process ${data_dir}/sample.grib2


# Clean up
cd $test_dir
rm -fr $temp_dir
