# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
message("You are running qmake on a generated .pro file. This may not work!")


HEADERS += ./file_functions.h \
    ./functions/functions.h \
    ./functions/world_init.h \
    ./FXNTNBT/fxnt_get_nbt.h \
    ./FXNTNBT/fxnt_nbt_types.h \
    ./globals.h \
    ./leveldb.h \
    ./NBTParser/parse_nbt.h \
    ./mcbeadmin.h \
    ./worldchooser.h \
    ./worldwindow.h \
    ./prunewindow.h \
    ./prune.h
SOURCES += ./main.cpp \
    ./file_functions.cpp \
    ./globals.cpp \
    ./mcbeadmin.cpp \
    ./prunewindow.cpp \
    ./worldchooser.cpp \
    ./worldwindow.cpp
FORMS += ./dialog_worldChooser.ui \
    ./mcbeadmin.ui \
    ./prunewindow.ui \
    ./worldwindow.ui
RESOURCES += mcbeadmin.qrc
