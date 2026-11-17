# Unit tests (C++, no framework)
add_executable( test_wavreader test_wavreader.cpp )
target_link_libraries( test_wavreader sonomkr )
add_test( NAME wavreader COMMAND test_wavreader )

# End-to-end tests: synthetic WAV -> sonomkr-core -> ZeroMQ spectra
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    set(E2E_SCENARIOS
        sine_1k_s16
        sine_1k_s24
        sine_1k_s32
        sine_1k_float
        stereo_two_channels
        multitone_48k
        low_bands_48k
        multitone_44k1
        band_20k_44k1
        step_sync_44k1
        paced_x4
        short_integration
        long_file_60s
        channel2_only
        channel2_only_mono
    )
    foreach(scenario ${E2E_SCENARIOS})
        add_test(
            NAME e2e_${scenario}
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/e2e_wav.py
                    --core $<TARGET_FILE:sonomkr-core>
                    --filters ${PROJECT_SOURCE_DIR}/conf/filters.conf
                    --scenario ${scenario}
        )
        set_tests_properties(e2e_${scenario} PROPERTIES TIMEOUT 120)
    endforeach()
else()
    message(WARNING "Python 3 not found: end-to-end WAV tests disabled")
endif()
