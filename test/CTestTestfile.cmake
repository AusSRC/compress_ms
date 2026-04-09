# CMake generated Testfile for 
# Source directory: /mnt/compress_ms/test
# Build directory: /mnt/compress_ms/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[none_real]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_r.ms" "test_nr.ms" "real" "none" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[none_real]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;6;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[none_complex]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_c.ms" "test_nc.ms" "complex" "none" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[none_complex]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;7;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[mgard_real]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_r.ms" "test_mr.ms" "real" "mgard" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[mgard_real]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;11;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[mgard_complex]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_c.ms" "test_mc.ms" "complex" "mgard" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[mgard_complex]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;12;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[zfp_real]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_r.ms" "test_zr.ms" "real" "zfp" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[zfp_real]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;14;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[zfp_complex]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_c.ms" "test_zc.ms" "complex" "zfp" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[zfp_complex]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;15;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[bzip2_real]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_r.ms" "test_br.ms" "real" "bzip2" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[bzip2_real]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;20;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[bzip2_complex]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_c.ms" "test_bc.ms" "complex" "bzip2" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[bzip2_complex]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;21;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[mgard_complex_complex]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_c.ms" "test_mcc.ms" "complex" "mgard_complex" "/mnt/compress_ms/test/adios_config.yaml" "stepsize")
set_tests_properties([=[mgard_complex_complex]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;24;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[config_test]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms_config.py" "/mnt/compress_ms/test/config.txt")
set_tests_properties([=[config_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;27;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
add_test([=[nsteps_test]=] "/opt/spack/var/spack/environments/spack-env/.spack-env/view/bin/python3.14" "/mnt/compress_ms/test/tcompress_ms.py" "test_orig_c.ms" "test_nc.ms" "complex" "none" "/mnt/compress_ms/test/adios_config.yaml" "nsteps")
set_tests_properties([=[nsteps_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/compress_ms/test/CMakeLists.txt;29;add_test;/mnt/compress_ms/test/CMakeLists.txt;0;")
