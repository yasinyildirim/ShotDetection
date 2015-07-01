android{
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
ANDROID_PACKAGE_SOURCE_DIR=$$_PRO_FILE_PWD_/android
LIBS        += -lopencv_flann -lopencv_androidcamera -lopencv_video -lopencv_highgui -lopencv_imgproc -lopencv_core  -llibtiff
 LIBS += \
        -L"$$_PRO_FILE_PWD_/OpenCV-2.3.1-android-bin/OpenCV-2.3.1/libs/armeabi-v7a" \
        -L"$$_PRO_FILE_PWD_/OpenCV-2.3.1-android-bin/OpenCV-2.3.1/share/OpenCV/3rdparty/libs/armeabi-v7a" \
INCLUDEPATH += "$$_PRO_FILE_PWD_/OpenCV-2.3.1-android-bin/OpenCV-2.3.1/include"

LIBS += \
    -llibtiff\
    -llibjpeg\
    -llibjasper\
    -llibpng\
    -lzlib\
    -lopencv_core\
    -lopencv_androidcamera\
    -lopencv_flann\
    -lopencv_imgproc\
    -lopencv_highgui\
    -lopencv_features2d\
    -lopencv_calib3d\
    -lopencv_ml\
    -lopencv_objdetect\
    -lopencv_video\
    -lopencv_contrib\
    -lopencv_java\
    -lopencv_legacy\
    -lopencv_ts

# - setup the correct location to install to and load from
android {
    # android platform
    # From: http://community.kde.org/Necessitas/Assets
    SAMPLES_INSTALL_PATH=/assets/


# - setup the 'make install' step
samples.path = $$SAMPLES_INSTALL_PATH
samples.files += $$SAMPLE_FILES
samples.depends += FORCE

INSTALLS += samples

}

OTHER_FILES += \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/version.xml \
    android/res/values-id/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-et/strings.xml \
    android/res/drawable/logo.png \
    android/res/drawable/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/res/values-nb/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-nl/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-el/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-ru/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/AndroidManifest.xml
}
else{
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
INCLUDEPATH += /usr/include/opencv2 /usr/include
LIBS        += -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc -lopencv_flann

}

HEADERS += \
    shotdetector.h

SOURCES += \
    shotdetector.cpp
