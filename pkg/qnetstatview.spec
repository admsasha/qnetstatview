Name:		qnetstatview
Version:	1.5.3
Release:	1
Summary:	Shows detailed listings of all TCP and UDP endpoints
Group:		Networking/Other
License:	GPLv3+
URL:		http://dansoft.krasnokamensk.ru/more.html?id=1016
Source0:	%{name}-%{version}.tar.gz

BuildRequires:	qt5-linguist-tools
BuildRequires:	pkgconfig(Qt5Core)
BuildRequires:	pkgconfig(Qt5Gui)
BuildRequires:	pkgconfig(Qt5Widgets)
BuildRequires:	pkgconfig(Qt5Network)


%description
Shows detailed listings of all TCP and UDP endpoints.

%prep
%setup -q

%build
%qmake_qt5
%make

%install
%make_install INSTALL_ROOT=%{buildroot}

%files
%doc README.md
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png

