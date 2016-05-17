#!/bin/bash
mkdir -p ../output
mkdir -p ../reports
../generator/x86_linux64bit/tst2db  \
../Templates/Instance_data/global.def \
../Templates/Instance_data/automotiveobject.def \
../Templates/Instance_data/odometer.def \
../Templates/Instance_data/organisationalperson.def \
../Templates/Instance_data/physicalperson.def \
../Templates/Instance_data/revenue.def \
../Templates/Master_data/product.def \
../Templates/Master_data/productcoverage.def \
../Templates/Master_data/productindustry.def \
../Templates/Master_data/productusage.def \
../Templates/Master_data/productpaymentinterval.def \
../Templates/Master_data/productlimits.def \
../Templates/Master_data/ruleconfiguration.def \
../Templates/Master_data/delayedpayment.def \
../Templates/Master_data/serviceproductoffering.def \
../Templates/Master_data/coveragepricing.def \
../Templates/Master_data/contractproperty.def \
../Templates/Master_data/productcosting.def \
../Templates/Master_data/productoptioncosting.def \
../Templates/Master_data/productpricing.def \
../Templates/Master_data/productoptionpricing.def \
../Templates/Master_data/financialfactor.def \
../Templates/Instance_data/customercontract.def \
../Templates/Instance_data/cost.def \
../Templates/Relations/base.def \
../Templates/Relations/dependencies.def \
../sys_properties/db2.properties.def \
../sys_properties/xml.properties.def \
../Test_data_specification \
--output_directory../output --report_directory../reports



