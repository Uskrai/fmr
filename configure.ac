# Process this file with autoconf to produce a configure script.

AC_INIT(fmr,1.1.0-alpha,github.com/Uskrai/fmr)

AC_PREREQ(2.69)

AC_CONFIG_SRCDIR([src/main/app.cpp])

AC_CONFIG_AUX_DIR(aux-build)

# sets build, host variables and the same with _alias
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

#Check for compiler
AC_PROG_CC
AC_PROG_CXX


m4_include([aux-build/wxwin.m4])
m4_include([aux-build/ax_cxx_compile_stdcxx.m4])
m4_include([aux-build/ax_check_compile_flag.m4])
# notice that if --host was given but --build was not, cross_compiling is
# only set to "maybe" and not "yes" and will be either set to "yes" later or
# configure will exit with error in AC_PROG_CC so don't test for = "yes" here

if test "$cross_compiling" != "no"; then
    HOST_PREFIX="${host_alias}-"
    HOST_SUFFIX="-$host_alias"
else
    HOST_PREFIX=
    HOST_SUFFIX=
fi

AC_CONFIG_FILES([Makefile])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AM_PROG_AR([act-if-fail])
LT_INIT([shared disable-static])

# check for os
build_linux=no
build_windows=no
build_mac=no

# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

AM_CONDITIONAL([SHARED], test "x$enable_shared" = "xyes")

AX_CXX_COMPILE_STDCXX([17],[noext],[mandatory])

if test "$HAVE_CXX17" != 1; then
	AC_MSG_ERROR([This Application need c++17 support])
fi
CXXFLAGS="$CXXFLAGS -std=c++17"

# check for libraries
AM_OPTIONS_WXCONFIG
reqwx=3.0.4
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi
CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

AC_OUTPUT


