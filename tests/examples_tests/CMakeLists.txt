cmake_minimum_required(VERSION 3.14)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(EXAMPLES_TESTS_DIR ${CMAKE_CURRENT_LIST_DIR})
set(EXAMPLES_TESTS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(GLOBAL_RUNNER_POOLS ${CMAKE_SOURCE_DIR}/scripts/runner/runner_pools.json)
file(MAKE_DIRECTORY ${EXAMPLES_TESTS_OUTPUT_DIR})

set(EXAMPLES_MANIFEST ${EXAMPLES_TESTS_OUTPUT_DIR}/examples_manifest.json)
set(EXAMPLES_MATRIX ${EXAMPLES_TESTS_OUTPUT_DIR}/examples_matrix.json)
set(EXAMPLES_UNASSIGNED ${EXAMPLES_TESTS_OUTPUT_DIR}/examples_unassigned.json)

add_custom_target(ob_examples_tests_manifest
    COMMAND ${Python3_EXECUTABLE}
            ${EXAMPLES_TESTS_DIR}/generate_examples_manifest.py
            --workspace-root ${CMAKE_SOURCE_DIR}
            --output ${EXAMPLES_MANIFEST}
            --overrides ${EXAMPLES_TESTS_DIR}/config/example_overrides.json
    BYPRODUCTS ${EXAMPLES_MANIFEST}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generate examples test manifest"
)

add_custom_target(ob_examples_tests_matrix
    COMMAND ${Python3_EXECUTABLE}
            ${EXAMPLES_TESTS_DIR}/generate_examples_manifest.py
            --workspace-root ${CMAKE_SOURCE_DIR}
            --output ${EXAMPLES_MANIFEST}
            --overrides ${EXAMPLES_TESTS_DIR}/config/example_overrides.json
    COMMAND ${Python3_EXECUTABLE}
            ${EXAMPLES_TESTS_DIR}/generate_ci_matrix.py
            --manifest ${EXAMPLES_MANIFEST}
            --runner-pools ${GLOBAL_RUNNER_POOLS}
            --output-matrix ${EXAMPLES_MATRIX}
            --output-unassigned ${EXAMPLES_UNASSIGNED}
    BYPRODUCTS ${EXAMPLES_MANIFEST} ${EXAMPLES_MATRIX} ${EXAMPLES_UNASSIGNED}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generate examples CI matrix"
)

set_target_properties(ob_examples_tests_manifest PROPERTIES FOLDER "tests")
set_target_properties(ob_examples_tests_matrix PROPERTIES FOLDER "tests")