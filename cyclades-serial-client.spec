Summary: Serial Port Interface for Cyclades Terminal Servers
Name: cyclades-serial-client
Version: 0.93
Release: 1
Copyright: GPL
Group: Network
#URL: 
#Source: 
BuildRoot: /tmp/%{name}-buildroot
Prefixes: %{_prefix} %{_datadir}
Requires: glibc >= 2.1
Requires: perl >= 5.0
BuildRequires: glibc-devel >= 2.1

source0: cyclades-serial-client_0.93.tar.gz

%description
This package connects a pseudo-tty on a local system to a remote serial port.

%prep
%setup -q

%build
./configure --prefix=$RPM_BUILD_ROOT%{_prefix} --mandir=$RPM_BUILD_ROOT%{_mandir}
make

%install
DESTDIR=$RPM_BUILD_ROOT; export DESTDIR
[ -n "`echo $DESTDIR | sed -n 's:^/tmp/[^.].*$:OK:p'`" ] && rm -rf $DESTDIR ||
(echo "Invalid BuildRoot: '$DESTDIR'! Check the spec ..."; exit 1) || exit 1

mkdir -p $DESTDIR/usr/sbin
mkdir -p $DESTDIR%{_mandir}/man{5,8}
mkdir -p $DESTDIR/etc

install -c -m 644 cyclades-devices $DESTDIR/etc
install -c -m 755 cyclades-ser-cli cyclades-serial-client $DESTDIR/usr/sbin
install -c -m 644 man/cyclades-ser-cli.8 man/cyclades-serial-client.8 $DESTDIR%{_mandir}/man8
install -c -m 644 man/cyclades-devices.5 $DESTDIR%{_mandir}/man5

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/etc/cyclades-devices
/usr/sbin/cyclades-ser-cli
/usr/sbin/cyclades-serial-client
%{_mandir}/man8/cyclades-ser-cli.8.gz
%{_mandir}/man8/cyclades-serial-client.8.gz
%{_mandir}/man5/cyclades-devices.5.gz

%changelog
* Wed Oct 30 2002 Russell Coker <russell@coker.com.au>
- first packaging
