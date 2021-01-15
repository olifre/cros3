#
# spec file for package cros3
#

Name:			cros3
BuildRequires:	%kernel_module_package_buildreqs
License:		GPL-2.0+
Group:			System/Kernel
Summary:		cros3 kernel module
Version:		0.1
Release:		0
Url:			https://github.com/BGO-OD/cros3
Source0:		%{name}-%{version}.tar.bz2
BuildRoot:		%{_tmppath}/%{name}-%{version}-build
BuildRequires:	udev

# %suse_kernel_module_package -p %_sourcedir/preamble

%description
Kernel driver for the cros3 module

%prep
%setup
set -- *
mkdir source
cp "$@" source/
mkdir obj

%build
for flavor in %flavors_to_build; do
        rm -rf obj/$flavor
        cp -r source obj/$flavor
        make -C %{kernel_source $flavor} modules M=$PWD/obj/$flavor
done

%install
%if 0%{?suse_version} >= 1230
%define udevdir /usr/lib/udev
%else
%define udevdir /lib/udev
%endif
export INSTALL_MOD_PATH=%{buildroot}
export INSTALL_MOD_DIR=updates
for flavor in %flavors_to_build; do
        make -C %{kernel_source $flavor} modules_install M=$PWD/obj/$flavor
done

%files
%defattr(-,root,root,-)

%changelog
