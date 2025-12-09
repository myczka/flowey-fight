#### Copy Audio to Build

cmake_minimum_required(VERSION 3.8)
project(flowey-fight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_executable(floweyfight ./src/flowey.cpp)

#target_include_directories(floweyfight PRIVATE ./lib/cppLibrary/)
target_include_directories(floweyfight SYSTEM PRIVATE ./lib/cppLibrary)

# user32 and gdi32 are linked automatically by Windows toolchain normally, but ensure:
target_link_libraries(floweyfight PRIVATE user32 gdi32)

# Blocks *conversion from size_t to int potential loss of data* compiler warning
target_compile_options(floweyfight PRIVATE /wd4267) 

add_custom_command(TARGET floweyfight POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/audio $<TARGET_FILE_DIR:floweyfight>/audio
    COMMENT "Copying audio files to build directory"
)


#### Bake Audio Into EXE

# cmake_minimum_required(VERSION 3.8)
# project(flowey-fight LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)

# # Find xxd executable - try multiple methods
# find_program(XXD_EXECUTABLE 
#     NAMES xxd xxd.exe
#     PATHS 
#         "C:/Program Files/Git/usr/bin"
#         "C:/Program Files (x86)/Git/usr/bin"
#         ENV PATH
# )

# if(NOT XXD_EXECUTABLE)
#     message(STATUS "xxd not found, using Python fallback")
#     set(USE_PYTHON TRUE)
# else()
#     message(STATUS "Found xxd: ${XXD_EXECUTABLE}")
#     set(USE_PYTHON FALSE)
# endif()

# # Get all audio files from audio directory
# file(GLOB AUDIO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/audio/*.wav")
# message(STATUS "Found audio files: ${AUDIO_FILES}")

# # Generate header files for each audio file
# set(AUDIO_HEADERS "")
# foreach(AUDIO_FILE ${AUDIO_FILES})
#     get_filename_component(AUDIO_NAME ${AUDIO_FILE} NAME_WE)
#     set(OUTPUT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/audio_${AUDIO_NAME}.h")
    
#     if(USE_PYTHON)
#         add_custom_command(
#             OUTPUT ${OUTPUT_HEADER}
#             COMMAND python -c "data=open(r'${AUDIO_FILE}','rb').read(); open(r'${OUTPUT_HEADER}','w').write('const unsigned char audio_${AUDIO_NAME}_data[]={'+','.join('0x%02x'%b for b in data)+'};\\nconst unsigned int audio_${AUDIO_NAME}_len='+str(len(data))+';\\n')"
#             DEPENDS ${AUDIO_FILE}
#             COMMENT "Embedding audio file: ${AUDIO_NAME}"
#         )
#     else()
#         add_custom_command(
#             OUTPUT ${OUTPUT_HEADER}
#             COMMAND "${XXD_EXECUTABLE}" -i "${AUDIO_FILE}" > "${OUTPUT_HEADER}"
#             DEPENDS ${AUDIO_FILE}
#             COMMENT "Embedding audio file: ${AUDIO_NAME}"
#         )
#     endif()
    
#     list(APPEND AUDIO_HEADERS ${OUTPUT_HEADER})
# endforeach()

# # Create executable with audio headers as dependencies
# add_executable(floweyfight ./src/flowey.cpp ${AUDIO_HEADERS})

# # Include the binary directory so generated headers can be found
# target_include_directories(floweyfight PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# #target_include_directories(floweyfight PRIVATE ./lib/cppLibrary/)
# target_include_directories(floweyfight SYSTEM PRIVATE ./lib/cppLibrary)

# # user32 and gdi32 are linked automatically by Windows toolchain normally, but ensure:
# target_link_libraries(floweyfight PRIVATE user32 gdi32)

# # Blocks *conversion from size_t to int potential loss of data* compiler warning
# target_compile_options(floweyfight PRIVATE /wd4267)