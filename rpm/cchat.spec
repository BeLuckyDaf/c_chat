Name: cchat
Version: 1.0
Release: 2
Summary: Simple chat written in C
Group: Applications/Social
License: MIT
Source0: %{name}-%{version}.tar.gz

%description
This application allows communicating to people all over the world
by sending text messages whilst using the app.

%prep
%setup

%build
make

%install
mkdir -p %{buildroot}%{_bindir}
install build/chat %{buildroot}%{_bindir}/cchat

%files
%{_bindir}/cchat

%clean
make clean
rm -rf %{buildroot}

%changelog
* Fri May 17 2019 Vladislav Smirnov
- Fixed macros in files and install
* Thu May 16 2019 Vladislav Smirnov
- Initial build
