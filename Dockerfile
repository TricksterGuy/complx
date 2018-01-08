FROM ubuntu:16.04

# Need software-properties-common for add-apt-repository.
# gnome-themes-standard prevents it from looking hideous.
# libcanberra-gtk-module prevents the message `Gtk-Message: Failed to
# load module "canberra-gtk-module"' which might confuse students.

RUN apt-get update && \
    apt-get install -y software-properties-common gnome-themes-standard libcanberra-gtk-module && \
    add-apt-repository -y ppa:tricksterguy87/ppa-gt-cs2110 && \
    apt-get update && \
    apt-get install -y complx-tools && \
    apt-get clean

# Create a user. Running X11 apps as root is A Bad Thing(TM)
RUN useradd --user-group --create-home --shell /bin/bash developer

USER developer
# Create ~/.local/share/ so GTK won't generate a bunch of stderr noise
# when trying to write to ~/.local/share/recently-used.xbel
RUN mkdir -p /home/developer/workdir /home/developer/.local/share

WORKDIR /home/developer/workdir
CMD complx
