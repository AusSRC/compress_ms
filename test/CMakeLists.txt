
set(config ${CMAKE_SOURCE_DIR}/test/adios_config.yaml)
set(compress_config ${CMAKE_SOURCE_DIR}/test/config.txt)
#test no compression
    #test real/complex
add_test(NAME none_real COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_r.ms test_nr.ms real none ${config} stepsize)
add_test(NAME none_complex COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_c.ms test_nc.ms complex none ${config} stepsize)
#test compression
    #test real/complex
        #test mgard
add_test(NAME mgard_real COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_r.ms test_mr.ms real mgard ${config} stepsize)
add_test(NAME mgard_complex COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_c.ms test_mc.ms complex mgard ${config} stepsize)
        #test zfp
add_test(NAME zfp_real COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_r.ms test_zr.ms real zfp ${config} stepsize)
add_test(NAME zfp_complex COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_c.ms test_zc.ms complex zfp ${config} stepsize)
        #test sz
# add_test(NAME sz_real COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_r.ms test_sr.ms real sz ${config})
# add_test(NAME sz_complex COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_c.ms test_sc.ms complex sz ${config})
        #test bzip2
add_test(NAME bzip2_real COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_r.ms test_br.ms real bzip2 ${config} stepsize)
add_test(NAME bzip2_complex COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_c.ms test_bc.ms complex bzip2 ${config} stepsize)
    #test complex
        #test mgard_complex
add_test(NAME mgard_complex_complex COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_c.ms test_mcc.ms complex mgard_complex ${config} stepsize)

# config test
add_test(NAME config_test COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms_config.py ${compress_config})
# nsteps test
add_test(NAME nsteps_test COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/tcompress_ms.py test_orig_c.ms test_nc.ms complex none ${config} nsteps)

