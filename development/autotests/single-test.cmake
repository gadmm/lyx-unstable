# This script invokes the keytest.py script with the simple set-up needed
# to re-run deterministic regression tests that one would like to have.
#
# AUTOTEST_ROOT   = ${LYX_ROOT}/development/autotests
# KEYTEST_INFILE  = xxx-in.txt
# KEYTEST_OUTFILE = xxx-out.txt
# BINDIR          = ${BUILD_DIR}/bin
# WORKDIR         = ${BUILD_DIR}/autotests/out-home
# LYX_USERDIR_VER = ${LYX_USERDIR_VER}
# LOCALE_DIR      = ${BUILD_DIR}/autotests/locale
# PO_BUILD_DIR    = ${BUILD_DIR}/po
# PACKAGE         = lyx2.1
#
# Script should be called like:
# cmake -DAUTOTEST_ROOT=xxxx \
#       -DLYX_TESTS_USERDIR=${LYX_TESTS_USERDIR}
#       -DKEYTEST_INFILE=xxxx \
#       -DKEYTEST_OUTFILE=xxx \
#       -DBINDIR=xxx \
#       -DLYX=xxx \
#       -DWORKDIR=xxx \
#       -DLYX_USERDIR_VER=${LYX_USERDIR_VER} \
#       -DLOCALE_DIR=xxx \
#       -DPO_BUILD_DIR=xxx \
#       -DPACKAGE=xxx \
#       -DFRONTEND=Qt? \
#       -P ${AUTOTEST_ROOT}/single-test.cmake

set(KEYTEST "${AUTOTEST_ROOT}/keytest.py")

if(0)
  # This thread works, but the sollution in keytest.py is faster
  #-------------------------------------------------------------
  # Try to wait some seconds until the previous lyx stopped
  # but not more than 30 seconds
  message(STATUS "Wait for maximal 30 seconds until the previous lyx finishes")
  set(lcount 0)
  set(pidstat 1)
  while (NOT lcount GREATER 30)
    MATH(EXPR lcount "${lcount} + 1")
    execute_process(COMMAND pidof ${LYX} OUTPUT_VARIABLE LYX_PID RESULT_VARIABLE pidstat OUTPUT_VARIABLE pidres)
    if (pidstat)
      break()
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 1)
  endwhile()

  if (lcount GREATER 1)
    # We had to wait for some seconds
    message(STATUS "There was still some lyx in way(${lcount})")
  endif()
  if (NOT pidstat)
    # lyx still running, remove trailing '\n' from pid
    string(REGEX REPLACE "\n" "" pidres ${pidres})
    message(FATAL_ERROR "Old lyx with pid ${pidres} still running")
  endif()
endif()

#check for plausible DISPLAY environment (needed bei keytests)
set(DISPLAY_VAR $ENV{DISPLAY})
if(NOT DISPLAY_VAR MATCHES "^[a-zA-Z\\.]*:[0-9]+\(\\.[0-9]+\)?$")
  message(FATAL_ERROR "Invalid DISPLAY environment value (== '${DISPLAY_VAR}')")
endif()

set(LYX_EXE "${BINDIR}/${LYX}")

set(ENV{QT_FRONTEND} ${FRONTEND})

if(EXISTS "${LYX_TESTS_USERDIR}/session")
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove -f "${LYX_TESTS_USERDIR}/session")
endif()
# Environments needed by keytest.py
set(ENV{PACKAGE} ${PACKAGE})
set(ENV{LOCALE_DIR} ${LOCALE_DIR})
set(ENV{LYX_LOCALEDIR} "${LOCALE_DIR}")
set(ENV{${LYX_USERDIR_VER}} "${LYX_TESTS_USERDIR}")
set(ENV{LYX_PID} ${pidres})
set(ENV{LYX_WINDOW_NAME} ${LYX_WINDOW_NAME})
set(ENV{LYX_EXE} ${LYX_EXE})
set(ENV{LYX} ${LYX})
set(ENV{XVKBD_EXE} ${XVKBD_EXE})
set(ENV{KEYTEST_INFILE} "${AUTOTEST_ROOT}/${KEYTEST_INFILE}")
set(ENV{KEYTEST_OUTFILE} "${WORKDIR}/${KEYTEST_OUTFILE}")
set(ENV{PO_BUILD_DIR} "${PO_BUILD_DIR}")
set(ENV{MAX_DROP} 0)
set(ENV{MAX_LOOPS} 1)
string(REGEX REPLACE "-in\\.(txt|sh)$" "" _jj ${KEYTEST_INFILE})
if(EXISTS "${WORKDIR}/../${_jj}.lyx.emergency")
  execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove -f "${WORKDIR}/../${_jj}.lyx.emergency" )
endif()
file(GLOB _testfiles RELATIVE "${WORKDIR}" "test*.*" "#test*.*")
if(_testfiles)
  # remove some leftover files from previous test
  execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove -f ${_testfiles} )
endif()
if(EXISTS "${AUTOTEST_ROOT}/${_jj}.lyx")
  configure_file("${AUTOTEST_ROOT}/${_jj}.lyx" "${WORKDIR}/../${_jj}.lyx" COPYONLY)
endif()

execute_process(
  COMMAND python ${KEYTEST}
  RESULT_VARIABLE KEYTEST_RES)
if(KEYTEST_RES)
  message(FATAL_ERROR "KEYTEST failed: KEYTEST_RES=${KEYTEST_RES}")
endif()

