cmake_minimum_required(VERSION 2.6)

# Force the list of configurations that will appear in Visual Studio
# as described in http://www.cmake.org/Bug/view.php?id=6788
set(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo CACHE STRING "" FORCE)

set(NEWWOLF_PROJECT_NAME NewWolfLives)
project(${NEWWOLF_PROJECT_NAME})

if(NOT MSVC80)
    message(SEND_ERROR "
        Compilation of this project has currently only been tested
        with Microsoft Visual C++ 2005, so generating build
        systems for other versions or compilers is disabled.  Feel
        free to remove this check from CMakeLists.txt if you want
        to experiment.")
endif()

# Vendor (third-party) sources, possibly with local modifications, are
# under this directory.
set(VENDOR_ROOT  libs)

set(JPEGLIB_ROOT ${VENDOR_ROOT}/jpeglib)

include_directories(msvc_compat ${JPEGLIB_ROOT})

###############################################################################
# NewWolf sources
###############################################################################

set(NEWWOLF_SOURCES 
    actor_ai.c
    actors.c
    ai_com.c
    areas.c
    automap.c
    cl_input.c
    cl_main.c
    console.c
    doors.c
    fileio.c
    fmopl.c
    fontman.c
    game.c
    gl_draw.c
    keys.c
    level.c
    map.c
    math.c
    menu.c
    opengl.c
    particles.c
    player.c
    powerups.c
    pwalls.c
    q_fns.c
    r_models.c
    raycast.c
    render.c
    saveload.c
    sound.c
    sprites.c
    texman.c
    video.c
    wl_text.c
    wolfgl.c
)

set(NEWWOLF_STABLE_SOURCES
    common/common.h
    common/keys.h
    scripts/scripts.h
)
source_group("Source Files\\Stable" FILES ${NEWWOLF_STABLE_SOURCES})

set(NEWWOLF_STABLE_COMMON_SOURCES
    common/cmd.c
    common/common.c
    common/cvar.c
    common/files.c
    common/parselib.c
)
source_group("Source Files\\Stable\\Common" FILES ${NEWWOLF_STABLE_COMMON_SOURCES})

set(NEWWOLF_STABLE_FILE_IO_SOURCES
    common/io_images.c
)
source_group("Source Files\\Stable\\File I/O" FILES ${NEWWOLF_STABLE_FILE_IO_SOURCES})

set(NEWWOLF_STABLE_NONPORTABLE_WIN32_SOURCES
    win32/gl_win.c
    win32/in_win.c
    win32/sys_info.c
    win32/sys_win.c
    win32/vid_win.c
    win32/win32.h
)
source_group("Source Files\\Stable\\Non-portable\\win32" FILES ${NEWWOLF_STABLE_NONPORTABLE_WIN32_SOURCES})

set(NEWWOLF_STABLE_REFRESH_SOURCES
    r_gl/r_gl.c
    r_gl/r_gl.h
)
source_group("Source Files\\Stable\\Refresh" FILES ${NEWWOLF_STABLE_REFRESH_SOURCES})

set(NEWWOLF_STABLE_SCRIPTS_SOURCES
    scripts/scr_models.c
)
source_group("Source Files\\Stable\\Scripts" FILES ${NEWWOLF_STABLE_SCRIPTS_SOURCES})

set(NEWWOLF_STABLE_WOLF_LOGIC_SOURCES
    version.c
)
source_group("Source Files\\Stable\\Wolf Logic" FILES ${NEWWOLF_STABLE_WOLF_LOGIC_SOURCES})

set(NEWWOLF_HEADER_FILES
    act_stat.h
    actor_ai.h
    actors.h
    ai_com.h
    anorms.h
    areas.h
    audiowl6.h
    automap.h
    client.h
    doors.h
    fileio.h
    fmopl.h
    fontman.h
    game.h
    keys.h
    level.h
    map.h
    math.h
    menu.h
    menudef.h
    opengl.h
    particles.h
    player.h
    powerups.h
    pwalls.h
    q_fns.h
    r_models.h
    raycast.h
    render.h
    saveload.h
    server.h
    sound.h
    sprites.h
    sprt_def.h
    texman.h
    version.h
    vgapics.h
    video.h
    wl_text.h
    wolfdef.h
)
source_group("Header Files" FILES ${NEWWOLF_HEADER_FILES})

set(NEWWOLF_OLDHEADERS_FILES
    fileioo.h
    wl_ai.h
    wl_game.h
    wolfgl.h
)
source_group("OldHeaders (still used)" FILES ${NEWWOLF_OLDHEADERS_FILES})

# @todo Investigate using CMake scripting to avoid needing to
# duplicate the version number in the .rc file and source code.
set(NEWWOLF_WINDOWS_RESOURCE_FILES
    win32/newwolf.rc
)
source_group("Resource Files" FILES ${NEWWOLF_WINDOWS_RESOURCE_FILES})

# These are explicitly unused sources, i.e. newwolf.vcproj listed them
# as ExcludedFromBuild: sv_main.c
#
# As above, but in the Stable\Caching group/filter: common/models.c,
# src/textures.c (the entire content of that group)
#
# As above, but in the Stable\File I/O group/filter: common/io_wolf.c

###############################################################################
# Vendor sources
###############################################################################

set(NEWWOLF_LIBS_JPEGLIB_SOURCES
    ${JPEGLIB_ROOT}/jcapimin.c
    ${JPEGLIB_ROOT}/jcapistd.c
    ${JPEGLIB_ROOT}/jccoefct.c
    ${JPEGLIB_ROOT}/jccolor.c
    ${JPEGLIB_ROOT}/jcdctmgr.c
    ${JPEGLIB_ROOT}/jchuff.c
    ${JPEGLIB_ROOT}/jcinit.c
    ${JPEGLIB_ROOT}/jcmainct.c
    ${JPEGLIB_ROOT}/jcmarker.c
    ${JPEGLIB_ROOT}/jcmaster.c
    ${JPEGLIB_ROOT}/jcomapi.c
    ${JPEGLIB_ROOT}/jcparam.c
    ${JPEGLIB_ROOT}/jcphuff.c
    ${JPEGLIB_ROOT}/jcprepct.c
    ${JPEGLIB_ROOT}/jcsample.c
    ${JPEGLIB_ROOT}/jctrans.c
    ${JPEGLIB_ROOT}/jdapimin.c
    ${JPEGLIB_ROOT}/jdapistd.c
    ${JPEGLIB_ROOT}/jdatadst.c
    ${JPEGLIB_ROOT}/jdatasrc.c
    ${JPEGLIB_ROOT}/jdcoefct.c
    ${JPEGLIB_ROOT}/jdcolor.c
    ${JPEGLIB_ROOT}/jddctmgr.c
    ${JPEGLIB_ROOT}/jdhuff.c
    ${JPEGLIB_ROOT}/jdinput.c
    ${JPEGLIB_ROOT}/jdmainct.c
    ${JPEGLIB_ROOT}/jdmarker.c
    ${JPEGLIB_ROOT}/jdmaster.c
    ${JPEGLIB_ROOT}/jdmerge.c
    ${JPEGLIB_ROOT}/jdphuff.c
    ${JPEGLIB_ROOT}/jdpostct.c
    ${JPEGLIB_ROOT}/jdsample.c
    ${JPEGLIB_ROOT}/jdtrans.c
    ${JPEGLIB_ROOT}/jerror.c
    ${JPEGLIB_ROOT}/jfdctflt.c
    ${JPEGLIB_ROOT}/jfdctfst.c
    ${JPEGLIB_ROOT}/jfdctint.c
    ${JPEGLIB_ROOT}/jidctflt.c
    ${JPEGLIB_ROOT}/jidctfst.c
    ${JPEGLIB_ROOT}/jidctint.c
    ${JPEGLIB_ROOT}/jidctred.c
    ${JPEGLIB_ROOT}/jmemmgr.c
    ${JPEGLIB_ROOT}/jmemnobs.c
    ${JPEGLIB_ROOT}/jquant1.c
    ${JPEGLIB_ROOT}/jquant2.c
    ${JPEGLIB_ROOT}/jutils.c
)
source_group("Source Files\\libs\\jpeglib" FILES ${NEWWOLF_LIBS_JPEGLIB_SOURCES})

set(NEWWOLF_LIBS_JPEGLIB_HEADERS_SOURCES
    ${JPEGLIB_ROOT}/jchuff.h
    ${JPEGLIB_ROOT}/jconfig.h
    ${JPEGLIB_ROOT}/jdct.h
    ${JPEGLIB_ROOT}/jdhuff.h
    ${JPEGLIB_ROOT}/jerror.h
    ${JPEGLIB_ROOT}/jinclude.h
    ${JPEGLIB_ROOT}/jmemsys.h
    ${JPEGLIB_ROOT}/jmorecfg.h
    ${JPEGLIB_ROOT}/jpegint.h
    ${JPEGLIB_ROOT}/jpeglib.h
    ${JPEGLIB_ROOT}/jversion.h
)
source_group("Source Files\\libs\\jpeglib\\headers" FILES ${NEWWOLF_LIBS_JPEGLIB_HEADERS_SOURCES})

###############################################################################
# Targets and compile/link flags
###############################################################################

set(NEWWOLF_ALL_SOURCES
    ${NEWWOLF_SOURCES}
    ${NEWWOLF_STABLE_SOURCES}
    ${NEWWOLF_STABLE_COMMON_SOURCES}
    ${NEWWOLF_STABLE_FILE_IO_SOURCES}
    ${NEWWOLF_STABLE_NONPORTABLE_WIN32_SOURCES}
    ${NEWWOLF_STABLE_REFRESH_SOURCES}
    ${NEWWOLF_STABLE_SCRIPTS_SOURCES}
    ${NEWWOLF_STABLE_WOLF_LOGIC_SOURCES}
    ${NEWWOLF_LIBS_JPEGLIB_SOURCES}
    ${NEWWOLF_LIBS_JPEGLIB_HEADERS_SOURCES}

    # These groups only have headers, but we need to include them if
    # we want them to appear in the IDE.
    ${NEWWOLF_HEADER_FILES}
    ${NEWWOLF_OLDHEADERS_FILES}
)

add_executable(${NEWWOLF_PROJECT_NAME} WIN32
               ${NEWWOLF_ALL_SOURCES}
               ${NEWWOLF_WINDOWS_RESOURCE_FILES})

# @todo newwolf.vcproj set the compile flag "/ZI" (Program Database
# for Edit & Continue) for Debug builds, consider doing that here.

# /MTd: Runtime Library: Multi-threaded Debug (CMake default is the
#       DLL version of this).  Statically linking the runtime
#       increases the size of the executable, e.g. from 956K to 1548K.
#       This means we don't need to worry about installing the runtime
#       library on old machines.
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")

# /GL: Whole Program Optimization.
# /MT: Runtime Library: Multi-threaded (CMake default is the DLL
#      version of this).  See analysis for /MTd above.
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /GL /MT")

# /INCREMENTAL:NO: "YES" appears to be the CMake default for this
#                  configuration, but it is not compatible with /LTCG
#                  or /OPT:REF.
# /LTCG: Should pass this since we pass /GL in C_FLAGS.  Adding
#        ":STATUS" would be useful, but is ugly in the VS2005 IDE.
# /OPT:REF: Eliminate Unreferenced Data: This is the default, but is
#           disabled when /DEBUG is used, so this forces it on.

set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
    "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO /LTCG /OPT:REF")

set_target_properties(${NEWWOLF_PROJECT_NAME}
    PROPERTIES
        COMPILE_DEFINITIONS
            # @todo Address the warnings about unsafe library
            # functions, but in a platform-independent way.  Note that
            # by setting this, rather than disabling warning #4996 as
            # was done in newwolf.proj, we do expose a few new
            # warnings.
            _CRT_SECURE_NO_DEPRECATE

        # @todo Add "COMPILE_FLAGS /Wp64" (Detect 64-Bit Portability
        # Issues) here.  newwolf.vcproj set this, then un-set it for
        # NEWWOLF_LIBS_JPEGLIB_SOURCES, although the sources outside
        # of jpeglib contain more warnings due to this flag than those
        # in it.

        # Add a suffix to the debug executable basename as was done by
        # newwolf.vcproj.  ".exe" is still added to this.
        DEBUG_OUTPUT_NAME ${NEWWOLF_PROJECT_NAME}_d
)

target_link_libraries(${NEWWOLF_PROJECT_NAME} OpenGL32 Glu32 Winmm)

# @todo Set the generated executable as the default thing to be
# run/debugged as per http://stackoverflow.com/a/12758553
