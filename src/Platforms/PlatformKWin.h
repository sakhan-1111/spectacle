/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "Platform.h"

#include <QImage>
class QScreen;

#include <memory>

class ScreenShotSource2;
class ScreenShotSourceWorkspace2;

/**
 * The PlatformKWin class uses the org.kde.KWin.ScreenShot2 dbus interface
 * for taking screenshots of screens and windows.
 */
class PlatformKWin final : public Platform
{
    Q_OBJECT

public:
    static std::unique_ptr<PlatformKWin> create();

    enum class ScreenShotFlag : uint {
        IncludeCursor = 0x1,
        IncludeDecoration = 0x2,
        NativeSize = 0x4,
    };
    Q_DECLARE_FLAGS(ScreenShotFlags, ScreenShotFlag)

    enum class InteractiveKind : uint {
        Window = 0,
        Screen = 1,
    };

    GrabModes supportedGrabModes() const override;
    ShutterModes supportedShutterModes() const override;

public Q_SLOTS:
    void doGrab(Platform::ShutterMode shutterMode, Platform::GrabMode grabMode, bool includePointer, bool includeDecorations) override;

private Q_SLOTS:
    void updateSupportedGrabModes();

private:
    explicit PlatformKWin(QObject *parent = nullptr);

    void takeScreenShotInteractive(InteractiveKind kind, ScreenShotFlags flags);
    void takeScreenShotArea(const QRect &area, ScreenShotFlags flags);
    void takeScreenShotActiveWindow(ScreenShotFlags flags);
    void takeScreenShotActiveScreen(ScreenShotFlags flags);
    void takeScreenShotWorkspace(ScreenShotFlags flags);
    void takeScreenShotCroppable(ScreenShotFlags flags);

    void trackSource(ScreenShotSource2 *source);
    void trackCroppableSource(ScreenShotSourceWorkspace2 *source);

    int m_apiVersion = 1;
    GrabModes m_grabModes;
};

/**
 * The ScreenShotSource2 class is the base class for screenshot sources that use the
 * org.kde.KWin.ScreenShot2 dbus interface.
 */
class ScreenShotSource2 : public QObject
{
    Q_OBJECT

public:
    template<typename... ArgType>
    explicit ScreenShotSource2(const QString &methodName, ArgType... arguments);
    ~ScreenShotSource2() override;

    QImage result() const;

Q_SIGNALS:
    void finished(const QImage &image);
    void errorOccurred();

private Q_SLOTS:
    void handleMetaDataReceived(const QVariantMap &metadata);

private:
    QImage m_result;
    int m_pipeFileDescriptor = -1;
};

/**
 * The ScreenShotSourceArea2 class provides a convenient way to take a screenshot of the
 * specified area using the org.kde.KWin.ScreenShot2 dbus interface.
 */
class ScreenShotSourceArea2 final : public ScreenShotSource2
{
    Q_OBJECT

public:
    ScreenShotSourceArea2(const QRect &area, PlatformKWin::ScreenShotFlags flags);
};

/**
 * The ScreenShotSourceInteractive2 class provides a convenient way to take a screenshot
 * of a screen or a window as selected by the user. This uses the org.kde.KWin.ScreenShot2
 * dbus interface.
 */
class ScreenShotSourceInteractive2 final : public ScreenShotSource2
{
    Q_OBJECT

public:
    ScreenShotSourceInteractive2(PlatformKWin::InteractiveKind kind, PlatformKWin::ScreenShotFlags flags);
};

/**
 * The ScreenShotSourceScreen2 class provides a convenient way to take a screenshot of
 * the specified screen using the org.kde.KWin.ScreenShot2 dbus interface.
 */
class ScreenShotSourceScreen2 final : public ScreenShotSource2
{
    Q_OBJECT

public:
    ScreenShotSourceScreen2(const QScreen *screen, PlatformKWin::ScreenShotFlags flags);
};

/**
 * The ScreenShotSourceActiveWindow2 class provides a convenient way to take a screenshot
 * of the active window. This uses the org.kde.KWin.ScreenShot2 dbus interface.
 */
class ScreenShotSourceActiveWindow2 final : public ScreenShotSource2
{
    Q_OBJECT

public:
    ScreenShotSourceActiveWindow2(PlatformKWin::ScreenShotFlags flags);
};

/**
 * The ScreenShotSourceActiveScreen2 class provides a convenient way to take a screenshot
 * of the active screen. This uses the org.kde.KWin.ScreenShot2 dbus interface.
 */
class ScreenShotSourceActiveScreen2 final : public ScreenShotSource2
{
    Q_OBJECT

public:
    ScreenShotSourceActiveScreen2(PlatformKWin::ScreenShotFlags flags);
};

/**
 * The ScreenShotSourceWorkspace2 class provides a convenient way to take a screenshot
 * of the whole workspace. This uses the org.kde.KWin.ScreenShot2 dbus interface.
 */
class ScreenShotSourceWorkspace2 final : public ScreenShotSource2
{
    Q_OBJECT

public:
    ScreenShotSourceWorkspace2(PlatformKWin::ScreenShotFlags flags);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PlatformKWin::ScreenShotFlags)
