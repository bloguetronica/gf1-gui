This package contains all the necessary files and scripts to install GF1 GUI
(which is a graphical user interface that invokes commands to control the GF1
Function Generator). The scripts included here are compatible with most 32-bit
and 64-bit Debian based operating systems (e.g. Kubuntu, etc). Prior to
installation, you must certify that your system is Debian based (or at least
uses apt-get) and that you have an active Internet connection. It is important
to note that you should also have the control commands for the function
generator already installed, but you can always install them later (please
refer to the package "gf1-3.0.tar.gz" to do so).

A list of the included scripts follows:
– install.sh;
– uninstall.sh.

In order to compile and install GF1 GUI for the first time, all you have to do
is to run "install.sh" by invoking "sudo ./install.sh" on a terminal window,
after changing your working directory to the current one. This script will
first obtain and install the required "build-essential" and
"qt5-default"/"qtbase5-dev" packages (if they are not installed yet). Then it
will compile the application and move it to "/usr/local/bin/".

Conversely, to uninstall, you should run "uninstall.sh" by invoking "sudo
./uninstall.sh", again on a terminal window after making sure that your
working directory is this one. This will delete the previously installed
application and source code. However it won't remove the packages mentioned
above, since they could be already installed before the first installation.

P.S.:
Since the GUI was designed having KDE in mind, you should get the best
rendering on Kubuntu and other Debian based systems that employ the above
desktop environment. However, the application can be perfectly used on systems
that have other desktop environments (e.g. Gnome, Xfce).
