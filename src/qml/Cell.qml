/*
    SPDX-FileCopyrightText: 2013 Ashwin Rajeev <ashwin_rajeev@hotmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3

Item{
    property string sprite
    property string type
    property int index
    property int angle
    property bool locked: false

    width: grid.width / grid.columns;
    height: width;

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: (locked && main.state === "running")? 0.5 : 0
    }

    MouseArea {
        anchors.fill: parent
        enabled: main.state === "running"
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        hoverEnabled: true
        onClicked: mouse => {
            main.selected = index
            switch (mouse.button) {
                case Qt.LeftButton:
                    if (main.reverseButtons) {
                        main.rotateClockwise();
                    }
                    else {
                        main.rotateCounterclockwise();
                    }
                    break;
                case Qt.RightButton:
                    if (main.reverseButtons) {
                        main.rotateCounterclockwise();
                    }
                    else {
                        main.rotateClockwise();
                    }
                    break;
                case Qt.MiddleButton:
                    locked = !locked;
                    break;
            }
        }
        onEntered: main.selected = index
        onPositionChanged: main.selected = index
    }
}
