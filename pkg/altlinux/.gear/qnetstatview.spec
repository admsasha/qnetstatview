Name:		qnetstatview
Version:	1.5.8
Release:	alt1
Summary:	Shows detailed listings of all TCP and UDP endpoints
Group:		Networking/Other
License:	GPLv3+
URL:		https://dansoft.ru/more.html?id=1016
Source0:	%{name}-%{version}.tar

BuildRequires:	qt5-tools
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
%make_build

%install
%makeinstall INSTALL_ROOT=%{buildroot}

%files
%doc README.md
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/polkit-1/actions/org.pkexec.qnetstatview.policy

%changelog
* Mon Jun 27 2022 Alexander Danilov <admsasha@altlinux.org> 1.5.8-alt1
- release 1.5.8

* Wed Aug 25 2021 Alexander Danilov <admsasha@altlinux.org> 1.5.7-alt1
- add icons
- add Ukrainian translation
- change url homepage

* Mon Feb 3 2020 Alexander Danilov <admsasha@altlinux.org> 1.5.6-alt1
- release 1.5.6

* Thu Dec 26 2019 Alexander Danilov <admsasha@altlinux.org> 1.5.5-alt1
- release 1.5.5

* Thu Dec 26 2019 Alexander Danilov <admsasha@altlinux.org> 1.5.4-alt1
- release 1.5.4

* Thu Aug 15 2019 Alexander Danilov <admsasha@altlinux.org> 1.5.3-alt1
- release 1.5.3

* Fri Aug  2 2019 Alexander Danilov <admsasha@altlinux.org> 1.5.2-alt1
- release 1.5.2
