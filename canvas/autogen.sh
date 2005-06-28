#! /bin/sh

cat <<EOF

Creating build environment ...

 * Building configure.ac
EOF
cat autoconf/configure.ac.begin > configure.ac

if test -f configure.ac.user; then
    cat >> configure.ac <<EOF
dnl
dnl ------- USER PART (configure.ac.user) -----------
dnl
dnl
EOF

    cat configure.ac.user >> configure.ac
    cat >> configure.ac <<EOF
dnl
dnl ------- END OF USER PART (configure.ac.user) -----------
dnl
dnl



EOF
fi
cat autoconf/configure.ac.end >> configure.ac

# Get the version and email
version=`grep 'VERSION[[:space:]]*=[[:space:]]*' configure.ac.user | sed 's/.*=[[:space:]]*\(.*\)/\1/'`
email=`grep 'EMAIL[[:space:]]*=[[:space:]]*'   configure.ac.user | sed 's/.*=[[:space:]]*\(.*\)/\1/'`
use_cxx=`grep 'USE_CXX[[:space:]]*=[[:space:]]*'   configure.ac.user | sed 's/.*=[[:space:]]*\(.*\)/\1/'`

sed -i "s/\\\$version\\\$/${version:-1.0}/" configure.ac
sed -i "s/\\\$email\\\$/${email:-noemail@nowhere}/"     configure.ac
sed -i "s/\\\$use_cxx\\\$/${use_cxx:-0}/" configure.ac

echo " * Running aclocal"
aclocal -I autoconf/

echo " * Running autoconf"
autoconf

cat <<EOF

If you already have a build of this module, do not forget to
reconfigure (for instance by running ./config.status --recheck)

EOF

