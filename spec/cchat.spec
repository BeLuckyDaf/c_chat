Name: cchat
Version: 1.0
Release: 1
Summary: Simple chat written in C
Group: Applications/Social
License: MIT
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/cchat-root

%description
This application allows communicating to people all over the world
by sending text messages whilst using the app.

%prep
%setup

%build
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
install build/chat $RPM_BUILD_ROOT/usr/local/bin/cchat

%files
%defattr(-,root,root)
/usr/local/bin/cchat

%clean
make clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Thu May 16 2019 Vladislav Smirnov
- Initial build

