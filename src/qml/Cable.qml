/*
    SPDX-FileCopyrightText: 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3

CanvasItem {
    spriteKey: "cable" + parent.sprite
    anchors.fill: parent
    rotation: parent.angle
    opacity: (main.state === "paused")? 0 : 1
    Behavior on rotation {
        id: animate
        NumberAnimation {
            duration: main.rotateDuration
            easing.type: Easing.OutQuad
            onRunningChanged: {
                if(!running && parent.angle != 0) {
                    animate.enabled = false
                    main.rotated(parent.index, parent.angle % 360);
                    animate.enabled = true
                }
            }
        }
    }
}
