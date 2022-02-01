AC_INIT([wpaf], [0.1], [davidb@230ruedubac.fr])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC
AC_GNU_SOURCE
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS([stdlib.h stdio.h])
AC_CHECK_LIB(bcm2835, bcm2835_init)
AC_CHECK_LIB(pthread, sem_init)
AC_CHECK_LIB(m, pow)
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

