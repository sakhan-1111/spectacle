/* This file is part of Spectacle, the KDE screenshot utility
 * SPDX-FileCopyrightText: 2015 Boudhayan Gupta <bgupta@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <SpectacleCommon.h>

#include <KLocalizedString>
#include <QDateTime>
#include <QIODevice>
#include <QMap>
#include <QObject>
#include <QPixmap>
#include <QPrinter>
#include <QUrl>

class QTemporaryDir;

class ExportManager : public QObject
{
    Q_OBJECT

    // singleton-ize the class

public:
    static ExportManager *instance();

private:
    explicit ExportManager(QObject *parent = nullptr);
    ~ExportManager() override;

    ExportManager(ExportManager const &) = delete;
    void operator=(ExportManager const &) = delete;

    // now the usual stuff

public:
    QString defaultSaveLocation() const;
    bool isFileExists(const QUrl &url) const;
    bool isImageSavedNotInTemp() const;
    void setPixmap(const QPixmap &pixmap);
    QPixmap pixmap() const;
    void updatePixmapTimestamp();
    void setTimestamp(const QDateTime &timestamp);
    QString windowTitle() const;
    Spectacle::CaptureMode captureMode() const;
    void setCaptureMode(Spectacle::CaptureMode theCaptureMode);
    QString formatFilename(const QString &nameTemplate);

    static const QMap<QString, KLocalizedString> filenamePlaceholders;

Q_SIGNALS:

    void errorMessage(const QString &str);
    void imageSaved(const QUrl &savedAt);
    void imageCopied();
    void imageLocationCopied(const QUrl &savedAt);
    void imageSavedAndCopied(const QUrl &savedAt);
    void forceNotify(const QUrl &savedAt);

public Q_SLOTS:

    QUrl getAutosaveFilename();
    QUrl tempSave();

    void setWindowTitle(const QString &windowTitle);
    void doSave(const QUrl &url = QUrl(), bool notify = false);
    bool doSaveAs(QWidget *parentWindow = nullptr, bool notify = false);
    void doSaveAndCopy(const QUrl &url = QUrl());
    void doCopyToClipboard(bool notify = false);
    void doCopyLocationToClipboard(bool notify = false);
    void doPrint(QPrinter *printer);

private:
    QString truncatedFilename(const QString &filename);
    QString makeAutosaveFilename();
    using FileNameAlreadyUsedCheck = bool (ExportManager::*)(const QUrl &) const;
    QString autoIncrementFilename(const QString &baseName, const QString &extension, FileNameAlreadyUsedCheck isFileNameUsed);
    QString makeSaveMimetype(const QUrl &url);
    bool writeImage(QIODevice *device, const QByteArray &format);
    bool save(const QUrl &url);
    bool localSave(const QUrl &url, const QString &mimetype);
    bool remoteSave(const QUrl &url, const QString &mimetype);
    bool isTempFileAlreadyUsed(const QUrl &url) const;

    bool mImageSavedNotInTemp;
    QPixmap mSavePixmap;
    QDateTime mPixmapTimestamp;
    QUrl mTempFile;
    QTemporaryDir *mTempDir = nullptr;
    QList<QUrl> mUsedTempFileNames;
    QString mWindowTitle;
    Spectacle::CaptureMode mCaptureMode{Spectacle::CaptureMode::AllScreens};
};
