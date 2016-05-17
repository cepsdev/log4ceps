@echo off
start  /i /b ..\windows\tst2db\Release\tst2db.exe ^
../Templates/Instance_data/global.def ^
../Templates/Instance_data/automotiveobject.def ^
../Templates/Instance_data/odometer.def ^
../Templates/Instance_data/organisationalperson.def ^
../Templates/Instance_data/physicalperson.def ^
../Templates/Instance_data/revenue.def ^
../Templates/Master_data/product.def ^
../Templates/Master_data/productcoverage.def ^
../Templates/Master_data/productlimits.def ^
../Templates/Master_data/ruleconfiguration.def ^
../Templates/Master_data/delayedpayment.def ^
../Templates/Master_data/serviceproductoffering.def ^
../Templates/Master_data/coveragepricing.def ^
../Templates/Instance_data/costumercontract.def ^
../Templates/Relations/base.def ^
../Templates/Relations/dependencies.def ^
../sys_properties/db2.properties.def ^
../sys_properties/xml.properties.def ^
../Test_data_specification ^
../Templates/report/overview.def ^
--output_directory../output --print-no-header-in-sql --report_directory../reports

