Name: system-tools
Version: 0.0.1
Release: 0
Summary: Tizen kernel and system tools
Group: libs
License: Qualcomm Proprietary License
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake

%if "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

%description
System tools includes all application related with Tizen kernel

%prep
%setup -q

%build
export LDFLAGS+="-Wl,--rpath=%{_prefix}/lib -Wl,--as-needed"
LDFLAGS="$LDFLAGS" cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

make

%install

%make_install

%files
%defattr(-,root,root,-)
%{_bindir}/dtbtool
%{_bindir}/mkdzimage
