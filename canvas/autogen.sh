#! /bin/sh

cat <<EOF

Creating build environment ...

 * Building configure.in
EOF
cat autoconf/configure.begin.in > configure.in

if test -f configure.in.in; then
    cat >> configure.in <<EOF
dnl
dnl ------- USER PART (configure.in.in) -----------
dnl
dnl
EOF

    cat configure.in.in >> configure.in
    cat >> configure.in <<EOF
dnl
dnl ------- END OF USER PART (configure.in.in) -----------
dnl
dnl



EOF
fi
cat autoconf/configure.end.in >> configure.in

# Get the version and email
version=`grep 'VERSION[[:space:]]*=[[:space:]]*' configure.in.in | sed 's/.*=[[:space:]]*\(.*\)/\1/'`
email=`grep 'EMAIL[[:space:]]*=[[:space:]]*'   configure.in.in | sed 's/.*=[[:space:]]*\(.*\)/\1/'`

sed -i "s/\\\$version\\\$/${version+1.0}/" configure.in
sed -i "s/\\\$email\\\$/${email+noemail@nowhere}/"     configure.in

echo " * Running aclocal"
aclocal

echo " * Adding GenoM specific macros"
cat autoconf/acinclude.m4 >> aclocal.m4

echo " * Running autoconf"
autoconf

cat <<EOF

If you already have a build of this module, do not forget to
reconfigure (for instance by running ./config.status --recheck)

EOF

