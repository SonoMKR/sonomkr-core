AC_PREREQ([2.69])
AC_INIT([SONOMKR-CORE], [1.0.0], [valentin.lebescond@sonomkr.fr])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CXX
dnl AX_CXX_COMPILE_STDCXX_14([noext], [mandatory])

# Checks for libraries.
LIBS="-pthread $LIBS"

dnl PKG_CHECK_MODULES([ZMQ], [libzmq], [], [
dnl    AC_MSG_ERROR([ZMQ package not found])
dnl ])
dnl LIBS="$ZMQ_LIBS $LIBS"
dnl CFLAGS="$CFLAGS $ZMQ_CFLAGS"

PKG_CHECK_MODULES([ZMQPP], [libzmqpp], [], [
    AC_MSG_ERROR([ZMQPP package not found])
])
LIBS="$ZMQPP_LIBS $LIBS"
CFLAGS="$CFLAGS $ZMQPP_CFLAGS"

PKG_CHECK_MODULES([CONFIGPP], [libconfig++], [], [
    AC_MSG_ERROR([CONFIG++ package not found])
])
LIBS="$CONFIGPP_LIBS $LIBS"
CFLAGS="$CFLAGS $CONFIGPP_CFLAGS"

PKG_CHECK_MODULES([ALSA], [alsa], [], [
    AC_MSG_ERROR([ALSA package not found])
])
LIBS="$ALSA_LIBS $LIBS"
CFLAGS="$CFLAGS $ALSA_CFLAGS"

# Checks for header files.
dnl AC_CHECK_HEADERS([zmq.h libconfig.h++ alsa/asoundlib.h zmqpp/zmqpp.hpp], [], [
dnl     AC_MSG_ERROR([some headers were not found])
dnl ])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
