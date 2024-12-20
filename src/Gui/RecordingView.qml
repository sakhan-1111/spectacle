/*
 * SPDX-FileCopyrightText: 2023 Aleix Pol i Gonzalez <aleixpol@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC
import QtMultimedia
import org.kde.kirigami as Kirigami
import org.kde.spectacle.private

import "Annotations"

FocusScope {
    id: root
    // BUG with mediaPlayer.playing: https://bugreports.qt.io/browse/QTBUG-117006
    readonly property bool playing: mediaPlayer.playbackState === MediaPlayer.PlayingState

    MouseArea {
        anchors.fill: videoOutput
        cursorShape: enabled ?
            (pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor)
            : undefined
        enabled: mediaPlayer.hasVideo
        onPositionChanged: {
            contextWindow.startDrag()
        }
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectFit
    }

    MediaPlayer {
        id: mediaPlayer
        source: SpectacleCore.currentVideo
        videoOutput: videoOutput
        onHasVideoChanged: if (hasVideo) {
            pause();
        }
        onPlaybackStateChanged: if (playbackState === MediaPlayer.StoppedState) {
            pause()
        }
    }

    Kirigami.Heading {
        anchors.fill: parent
        visible: SpectacleCore.videoPlatform.isRecording
        text: i18n("Recording:\n%1", SpectacleCore.recordedTime)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    HoverHandler {
        id: tbHoverHandler
    }

    component ToolButton: QQC.ToolButton {
        display: QQC.ToolButton.IconOnly
        QQC.ToolTip.text: text
        QQC.ToolTip.visible: (hovered || pressed) && display === QQC.ToolButton.IconOnly
        QQC.ToolTip.delay: Kirigami.Units.toolTipDelay
    }

    FloatingToolBar {
        id: toolBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: padding * 2
        visible: false
        opacity: 0
        enabled: mediaPlayer.hasVideo
        contentItem: RowLayout {
            spacing: parent.spacing
            ToolButton {
                id: playPauseButton
                icon.name: root.playing ?
                    "media-playback-pause" : "media-playback-start"
                text: root.playing ? i18n("Pause") : i18n("Play")
                display: QQC.ToolButton.IconOnly
                onClicked: if (root.playing) {
                    mediaPlayer.pause()
                } else {
                    mediaPlayer.play()
                }
            }
            QQC.Slider {
                id: seekBar
                Layout.fillWidth: true
                enabled: mediaPlayer.seekable
                wheelEnabled: false
                from: 0
                to: Math.max(1, mediaPlayer.duration)
                value: mediaPlayer.position
                onMoved: mediaPlayer.setPosition(Math.round(value))
            }
            QQC.Label {
                leftPadding: parent.spacing
                rightPadding: parent.spacing
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideNone
                wrapMode: Text.NoWrap
                text: {
                    let position = SpectacleCore.timeFromMilliseconds(mediaPlayer.position)
                    let duration = SpectacleCore.timeFromMilliseconds(mediaPlayer.duration)
                    return position + " / " + duration
                }
            }
        }
        layer.enabled: true // makes opacity animations look better
        state: "normal"
        states: [
            State {
                name: "hovered"
                when: tbHoverHandler.hovered && mediaPlayer.hasVideo
                PropertyChanges {
                    target: toolBar
                    opacity: 1
                }
            },
            State {
                name: "normal"
                when: !tbHoverHandler.hovered
                PropertyChanges {
                    target: toolBar
                    opacity: 0
                }
            }
        ]
        transitions: [
            Transition {
                to: "hovered"
                SequentialAnimation {
                    PropertyAction {
                        target: toolBar
                        property: "visible"
                        value: true
                    }
                    OpacityAnimator {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutCubic
                    }
                }
            },
            Transition {
                to: "normal"
                SequentialAnimation {
                    OpacityAnimator {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutCubic
                    }
                    PropertyAction {
                        target: toolBar
                        property: "visible"
                        value: false
                    }
                }
            }
        ]
    }
}
