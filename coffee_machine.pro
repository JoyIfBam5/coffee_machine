TEMPLATE = app
CONFIG += console c++11 thread
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    coffee_machine.cpp \
    coffee_machine_manager.cpp \
    ut_coffee_machine.cpp \
    ut_coffee_machine_manager.cpp

HEADERS += \
    abstract_coffee_machine_manager.h \
    abstract_coffee_machnie.h \
    abstract_storage.h \
    coffee_machine.h \
    coffee_machine_manager.h \
    coin.h \
    exceptions.h \
    ingredient.h \
    mock_coffee_machine.h \
    mock_coffee_machine_manager.h \
    mock_storage.h \
    storage.h

DISTFILES += \
    README.md


unix:!macx: LIBS += -L$$PWD/../googletest/googlemock/ -lgmock

INCLUDEPATH += $$PWD/../googletest/googlemock/include
DEPENDPATH += $$PWD/../googletest/googlemock/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../googletest/googlemock/libgmock.a

unix:!macx: LIBS += -L$$PWD/../googletest/googlemock/gtest/ -lgtest

INCLUDEPATH += $$PWD/../googletest/googletest/include
DEPENDPATH += $$PWD/../googletest/googletest/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../googletest/googlemock/gtest/libgtest.a
