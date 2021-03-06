## Basic Descriptions of this package
Name:       SAMPLE
Summary:    SAMPLE
Version:		1.0
Release:    1
Group:      Framework/system
License:    Apache License, Version 2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    SAMPLE.service
Source2:    SAMPLE.socket

# Required packages
# Pkgconfig tool helps to find libraries that have already been installed
BuildRequires:  cmake
BuildRequires:  libattr-devel
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(capi-system-runtime-info)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(libprivilege-control)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(sysman)
BuildRequires:  pkgconfig(tapi)
BuildRequires:  pkgconfig(pmapi)
BuildRequires:  pkgconfig(edbus)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(usbutils)
BuildRequires:  pkgconfig(udev)
BuildRequires:  pkgconfig(device-node)
BuildRequires:  pkgconfig(libsmack)
BuildRequires:  pkgconfig(libsystemd-daemon)
BuildRequires:	gettext

## Description string that this package's human users can understand
%description
SAMPLE


## Preprocess script
%prep
# setup: to unpack the original sources / -q: quiet
# patch: to apply patches to the original sources
%setup -q

## Build script
%build
# 'cmake' does all of setting the Makefile, then 'make' builds the binary.
cmake . -DCMAKE_INSTALL_PREFIX=/usr
make %{?jobs:-j%jobs}

## Install script
%install
# make_install: equivalent to... make install DESTDIR="%(?buildroot)"
%make_install

# install license file
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

# install systemd service
mkdir -p %{buildroot}%{_libdir}/systemd/system/graphical.target.wants
mkdir -p %{buildroot}%{_libdir}/systemd/system/sockets.target.wants
install -m 0644 %SOURCE1 %{buildroot}%{_libdir}/systemd/system/
install -m 0644 %SOURCE2 %{buildroot}%{_libdir}/systemd/system/
ln -s ../SAMPLE.service %{buildroot}%{_libdir}/systemd/system/graphical.target.wants/SAMPLE.service
ln -s ../SAMPLE.socket %{buildroot}%{_libdir}/systemd/system/sockets.target.wants/SAMPLE.socket


## Postprocess script
%post 

## Binary Package: File list
%files
%manifest SAMPLE.manifest
%{_bindir}/SAMPLE_server
%{_libdir}/systemd/system/SAMPLE.service
%{_libdir}/systemd/system/SAMPLE.socket
%{_libdir}/systemd/system/graphical.target.wants/SAMPLE.service
%{_libdir}/systemd/system/sockets.target.wants/SAMPLE.socket
/usr/share/license/%{name}
